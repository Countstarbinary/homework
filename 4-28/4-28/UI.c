#pragma once
#include "4-28.h"
#include <windows.h>
#include <wchar.h>
#include <CommCtrl.h>
#include "framework.h"
#include"UI.h"
#include "pe.h"



void AddListMoudle(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化								
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_MOUDLE);
	//设置整行选中								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("模块名");				//列标题				
	lv.cx = 200;								//列宽
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
	//第二列

	lv.pszText = (LPWSTR)TEXT("镜像基址");
	lv.cx = 150;
	lv.iSubItem = 1;
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
	//第三列								
	lv.pszText = (LPWSTR)TEXT("镜像大小");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//第四列								
	lv.pszText = (LPWSTR)TEXT("入口点");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

};

void AddListProcessMoudle(HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess;
	DWORD dwRowid = 0;

	HWND ID = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	TCHAR szPid[0x20] = { 0 };
	LV_ITEM lv;
	memset(&lv, 0, sizeof(LV_ITEM));
	dwRowid = SendMessage(ID, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

	lv.iSubItem = 1;
	lv.pszText = szPid;
	lv.cchTextMax = 0x20;
	SendMessage(ID, LVM_GETITEMTEXT, dwRowid, &lv);
	//MessageBox(NULL,szPid, TEXT("pid"),MB_OK);

	DWORD d = 0;
	// %lu 对应 unsigned long
	swscanf_s(szPid, L"%lu", &d);

	memset(processModule, 0, sizeof(processModule) / sizeof(MoudleInfo));
	EnumMoudles((DWORD64)processModule, d);

	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_MOUDLE);
	//初始化								
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	for (int i = 0; i < 0x128; i++)
	{
		if (!processModule[i].Imagebase)continue;

		vitem.pszText = (LPWSTR)(processModule[i].name);
		vitem.iItem = i;
		vitem.iSubItem = 0;
		//ListView_InsertItem(hListProcess, &vitem);								
		SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&vitem);

		WCHAR buf[32];
		swprintf(buf, _countof(buf), L"0x%016llX", processModule[i].Imagebase);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 1;
		ListView_SetItem(hListProcess, &vitem);


		// 如果你想十六进制显示，加上 0x 前缀：L"0x%016llX"
		swprintf(buf, _countof(buf), L"0x%llX", processModule[i].Imagesize);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 2;
		ListView_SetItem(hListProcess, &vitem);

		swprintf(buf, _countof(buf), L"%u", processModule[i].MoudleId);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 3;
		ListView_SetItem(hListProcess, &vitem);
	};

};

void AddListProcess(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化								
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//设置整行选中								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("进程");				//列标题				
	lv.cx = 200;								//列宽
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
	//第二列

	lv.pszText = (LPWSTR)TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
	//第三列								
	lv.pszText = (LPWSTR)TEXT("线程数量");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//第四列								
	lv.pszText = (LPWSTR)TEXT("父进程id");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

};

void AddListProcessItem(HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess;

	EnumProcessesNew((DWORD64)processInfo);

	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//初始化								
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	for (int i = 0; i < 0x128; i++)
	{
		if (!processInfo[i].pid)continue;

		vitem.pszText = (LPWSTR)(processInfo[i].name);
		vitem.iItem = i;
		vitem.iSubItem = 0;
		//ListView_InsertItem(hListProcess, &vitem);								
		SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&vitem);

		WCHAR buf[32];
		swprintf(buf, _countof(buf), L"%u", processInfo[i].pid);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 1;
		ListView_SetItem(hListProcess, &vitem);


		// 如果你想十六进制显示，加上 0x 前缀：
		swprintf(buf, _countof(buf), L"%llu", processInfo[i].NumberOfThreads);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 2;
		ListView_SetItem(hListProcess, &vitem);

		swprintf(buf, _countof(buf), L"%llu", processInfo[i].Parentpid);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 3;
		ListView_SetItem(hListProcess, &vitem);
	};

};

void AddListSection(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化								
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//设置整行选中								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("节名");				//列标题				
	lv.cx = 150;								//列宽
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
	//第二列

	lv.pszText = (LPWSTR)TEXT("文件偏移");
	lv.cx = 100;
	lv.iSubItem = 1;
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
	//第三列								
	lv.pszText = (LPWSTR)TEXT("文件大小");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//第四列								
	lv.pszText = (LPWSTR)TEXT("内存偏移");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

	//第五列								
	lv.pszText = (LPWSTR)TEXT("内存大小");
	lv.cx = 100;
	lv.iSubItem = 4;
	ListView_InsertColumn(hListProcess, 4, &lv);

	//第6列								
	lv.pszText = (LPWSTR)TEXT("节区属性");
	lv.cx = 100;
	lv.iSubItem = 5;
	ListView_InsertColumn(hListProcess, 5, &lv);

};

void AddListSectionItem(TCHAR* FilePath, HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess;

	char* FileBuffer = NULL;

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;
	PIMAGE_SECTION_HEADER Sectionpointer = PtrToPeHeader->SectionHeaderPtr;
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//初始化								
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{


		WCHAR buf[32];
		swprintf(buf, _countof(buf), L"%S", Sectionpointer->Name);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 0;
		//ListView_InsertItem(hListProcess, &vitem);								
		SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&vitem);

		// 如果你想十六进制显示，加上 0x 前缀：
		swprintf(buf, _countof(buf), L"0x%llX", Sectionpointer->PointerToRawData);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 1;
		ListView_SetItem(hListProcess, &vitem);

		swprintf(buf, _countof(buf), L"0x%llX", Sectionpointer->SizeOfRawData);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 2;
		ListView_SetItem(hListProcess, &vitem);
		// 如果你想十六进制显示，加上 0x 前缀：
		swprintf(buf, _countof(buf), L"0x%llX", Sectionpointer->VirtualAddress);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 3;
		ListView_SetItem(hListProcess, &vitem);

		// 如果你想十六进制显示，加上 0x 前缀：
		swprintf(buf, _countof(buf), L"0x%llX", Sectionpointer->Misc.VirtualSize);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 4;
		ListView_SetItem(hListProcess, &vitem);

		// 如果你想十六进制显示，加上 0x 前缀：
		swprintf(buf, _countof(buf), L"0x%llX", Sectionpointer->Characteristics);
		vitem.pszText = buf;
		vitem.iItem = i;
		vitem.iSubItem = 5;
		ListView_SetItem(hListProcess, &vitem);

		Sectionpointer++;
	};


	if (FileBuffer)free(FileBuffer);
	if (PtrToPeHeader)free(PtrToPeHeader);
};

void GetPeItem(TCHAR* FilePath, HWND hWnd)
{
	char* FileBuffer = NULL;
	printf("%d\n", sizeof(char*));
	///HMODULE mod=LoadLibrary(L"Dll1.dll");

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);



	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	TCHAR szBuffer[128] = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->AddressOfEntryPoint);
		SendDlgItemMessage(hWnd, IDC_EDIT_ENTYRPOINT, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->ImageBase);
		SendDlgItemMessage(hWnd, IDC_EDIT_IMAGEBASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->SizeOfImage);
		SendDlgItemMessage(hWnd, IDC_EDIT_IMAGESIZE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->BaseOfCode);
		SendDlgItemMessage(hWnd, IDC_EDIT_CODEBASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->BaseOfData);
		SendDlgItemMessage(hWnd, IDC_EDIT_DATABASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->SectionAlignment);
		SendDlgItemMessage(hWnd, IDC_EDIT_SECTIONALIG, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->FileAlignment);
		SendDlgItemMessage(hWnd, IDC_EDIT_FILEALIG, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->NtHeadersPtr->Signature);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIGNATURE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->Subsystem);
		SendDlgItemMessage(hWnd, IDC_EDIT_SUBSYSTEM, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->NumberOfSections);
		SendDlgItemMessage(hWnd, IDC_EDIT_SECTIONNUMBER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->SizeOfHeaders);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZEOFHEADER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->TimeDateStamp);
		SendDlgItemMessage(hWnd, IDC_EDIT1_TIMESTEAP, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->Magic);
		SendDlgItemMessage(hWnd, IDC_EDIT_MAGIC, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->CheckSum);
		SendDlgItemMessage(hWnd, IDC_EDIT_CHECKSUM, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader);
		SendDlgItemMessage(hWnd, IDC_EDIT_OPTIONLHEADER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr32->NumberOfRvaAndSizes);
		SendDlgItemMessage(hWnd, IDC_EDIT_NUMBEROFDIR, WM_SETTEXT, 0, szBuffer);
	}
	else
	{
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->AddressOfEntryPoint);
		SendDlgItemMessage(hWnd, IDC_EDIT_ENTYRPOINT, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->ImageBase);
		SendDlgItemMessage(hWnd, IDC_EDIT_IMAGEBASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->SizeOfImage);
		SendDlgItemMessage(hWnd, IDC_EDIT_IMAGESIZE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->BaseOfCode);
		SendDlgItemMessage(hWnd, IDC_EDIT_CODEBASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->BaseOfCode);
		SendDlgItemMessage(hWnd, IDC_EDIT_DATABASE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->SectionAlignment);
		SendDlgItemMessage(hWnd, IDC_EDIT_SECTIONALIG, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->FileAlignment);
		SendDlgItemMessage(hWnd, IDC_EDIT_FILEALIG, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->NtHeadersPtr->Signature);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIGNATURE, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->Subsystem);
		SendDlgItemMessage(hWnd, IDC_EDIT_SUBSYSTEM, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->NumberOfSections);
		SendDlgItemMessage(hWnd, IDC_EDIT_SECTIONNUMBER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->SizeOfHeaders);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZEOFHEADER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->TimeDateStamp);
		SendDlgItemMessage(hWnd, IDC_EDIT1_TIMESTEAP, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->Magic);
		SendDlgItemMessage(hWnd, IDC_EDIT_MAGIC, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->CheckSum);
		SendDlgItemMessage(hWnd, IDC_EDIT_CHECKSUM, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader);
		SendDlgItemMessage(hWnd, IDC_EDIT_OPTIONLHEADER, WM_SETTEXT, 0, szBuffer);

		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", PtrToPeHeader->u.OptionHeaderPtr64->NumberOfRvaAndSizes);
		SendDlgItemMessage(hWnd, IDC_EDIT_NUMBEROFDIR, WM_SETTEXT, 0, szBuffer);
	};



	if (FileBuffer)free(FileBuffer);
	if (PtrToPeHeader)free(PtrToPeHeader);
};

void GetDirItem(TCHAR* FilePath, HWND hWnd)
{
	char* FileBuffer = NULL;
	printf("%d\n", sizeof(char*));
	///HMODULE mod=LoadLibrary(L"Dll1.dll");

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);



	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	TCHAR szBuffer[128] = { 0 };
	IMAGE_DATA_DIRECTORY Entry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_EXPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_EXPORT, WM_SETTEXT, 0, szBuffer);
		
		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_IMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_IMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RESOURCE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RESOURCE, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_SEH, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_SEH, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_SAFE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_SAFE, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RELOCA, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RELOCA, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_DEBUG, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_DEBUG, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[7];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_COPYRIGHT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_COPYRIGHT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_GLOBALPTR];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_GLOBELPOINTER, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_GLOBELPOINTER, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_TLs, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_TLs, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_CONFIG, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_CONFIG, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_BOUNDIMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_BOUNDIMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_IAT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_IAT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_DELAYIMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_DELAYIMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_COM, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_COM, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[15];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RESERVE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RESERVE, WM_SETTEXT, 0, szBuffer);
	}
	else
	{
		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_EXPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_EXPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_IMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_IMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RESOURCE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RESOURCE, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_SEH, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_SEH, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_SAFE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_SAFE, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RELOCA, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RELOCA, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_DEBUG, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_DEBUG, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[7];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_COPYRIGHT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_COPYRIGHT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_GLOBALPTR];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_GLOBELPOINTER, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_GLOBELPOINTER, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_TLs, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_TLs, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_CONFIG, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_CONFIG, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_BOUNDIMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_BOUNDIMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_IAT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_IAT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_DELAYIMPORT, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_DELAYIMPORT, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_COM, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_COM, WM_SETTEXT, 0, szBuffer);

		Entry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[15];
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.VirtualAddress);
		SendDlgItemMessage(hWnd, IDC_EDIT_RVA_RESERVE, WM_SETTEXT, 0, szBuffer);
		swprintf(szBuffer, _countof(szBuffer), L"0x%llX", Entry.Size);
		SendDlgItemMessage(hWnd, IDC_EDIT_SIZE_RESERVE, WM_SETTEXT, 0, szBuffer);
	};



	if (FileBuffer)free(FileBuffer);
	if (PtrToPeHeader)free(PtrToPeHeader);
};



void AppendTextToEdit(TCHAR* newText, HWND hWnd)
{
	if (!hWnd || !newText) return;

	int len = GetWindowTextLength(hWnd);
	SendMessage(hWnd, EM_SETSEL, (WPARAM)len, (LPARAM)len); // 移动光标到末尾
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)newText);   // 追加文本
	SendMessage(hWnd, EM_SCROLLCARET, 0, 0);                // 滚动到新内容

}