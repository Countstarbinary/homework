#include "pch.h"
#include "Mytest.h"
#include <Windows.h>
#include <stdio.h>

#include <malloc.h>
PPeAllHeaders GetPeAllHeaders(char* FileBuffer)
{
	PPeAllHeaders PtrToPeHeader = NULL;
	PIMAGE_DOS_HEADER FileDosHeaderPtr = (PIMAGE_DOS_HEADER)FileBuffer;

	if (FileDosHeaderPtr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeadersPtr = (PIMAGE_NT_HEADERS)(FileBuffer + FileDosHeaderPtr->e_lfanew);

	if (NtHeadersPtr->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_FILE_HEADER FileHeaderPtr = &NtHeadersPtr->FileHeader;



	PVOID OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;
	PIMAGE_SECTION_HEADER SectionHeaderPtr = (PIMAGE_SECTION_HEADER)(FileBuffer + FileDosHeaderPtr->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER + FileHeaderPtr->SizeOfOptionalHeader);


	PPeAllHeaders HeadersPtr = (PPeAllHeaders)malloc(sizeof(PeAllHeaders));
	HeadersPtr->FileDosHeaderPtr = FileDosHeaderPtr;
	HeadersPtr->NtHeadersPtr = NtHeadersPtr;
	HeadersPtr->FileHeaderPtr = FileHeaderPtr;
	HeadersPtr->u.OptionHeaderPtr = (PIMAGE_OPTIONAL_HEADER)OptionHeaderPtr;
	HeadersPtr->SectionHeaderPtr = SectionHeaderPtr;

	return HeadersPtr;
};

wchar_t * singlestring_to_wide(const char * pastring)
{
	int wchar_count = MultiByteToWideChar(CP_ACP, 0, pastring, -1,
		NULL, 0);
	wchar_t * pwstring = (wchar_t*)malloc(wchar_count * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, pastring, -1,
		pwstring, wchar_count * sizeof(wchar_t));
	return pwstring;
}

//修复导入表，需要拉伸后的FileBuffer
BOOL UpdataImportDirtoryX86(char* FileBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导入表入口
	IMAGE_DATA_DIRECTORY ImportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	};

	//导入表入口 RvaToFoa。拉伸后不需要这行
	//DWORD ImportEntryFoa = RvaToFoa(ImportEntry.VirtualAddress, FileBuffer);

	PIMAGE_IMPORT_DESCRIPTOR ImportDirtory = (PIMAGE_IMPORT_DESCRIPTOR)(FileBuffer + ImportEntry.VirtualAddress);

	DWORD OFT = NULL;
	int* OFTitem;
	char* name;
	while (1)
	{
		printf("*******************************************************\n");
		if (ImportDirtory->Characteristics == 0 && ImportDirtory->FirstThunk == 0
			&& ImportDirtory->ForwarderChain == 0 && ImportDirtory->Name == 0 &&
			ImportDirtory->OriginalFirstThunk == 0 && ImportDirtory->TimeDateStamp == 0)
		{
			break;
		};

		//导入地址表的Rva
		//OFT = ImportDirtory->FirstThunk;
		//OFTitem = (int*)(FileBuffer + RvaToFoa(OFT, FileBuffer));

		//导入地址表
		OFTitem = (int*)(FileBuffer + ImportDirtory->FirstThunk);

		//name = FileBuffer + RvaToFoa(ImportDirtory->Name, FileBuffer);

		name = FileBuffer + ImportDirtory->Name;
		wchar_t * wName = singlestring_to_wide(name);
		printf("动态链接库：%s\n", name);
		HMODULE hModule = LoadLibrary((LPCWSTR)wName);
		if (!hModule)
		{
			ImportDirtory++;
			continue;
		}
		while (1)
		{

			if (!(*OFTitem)) break;

			if ((*OFTitem & 0x80000000) != 0x80000000)
			{

				//name = FileBuffer + RvaToFoa(*OFTitem, FileBuffer) + 2;
				name = FileBuffer + *OFTitem + 2;
				char* FunctionAdd = (char*)GetProcAddress(hModule, name);
				*OFTitem = (int)FunctionAdd;
				printf("函数名导出 %x %x %s\n", OFT, *OFTitem, name);
			}
			else
			{
				int FunctionNumber = *OFTitem & 0x7FFFFFFF;
				char* FunctionAdd = (char*)GetProcAddress(hModule, (LPCSTR)FunctionNumber);
				*OFTitem = (int)FunctionAdd;
				printf("序号导出 %x %x\n", OFT, *OFTitem & 0x7FFFFFFF);
			};
			OFTitem++;
		};

		ImportDirtory++;
	};
	return 1;

};

void Init()
{
	MessageBox(NULL, L"Init", L"Init", MB_OK);
	HMODULE hMainModule = GetModuleHandle(NULL);
	if (!hMainModule)return ;


};

void Destory()
{
	MessageBox(NULL, L"Destory", L"Destory", MB_OK);

	HMODULE hModule;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)L"inject.dll", &hModule);

	if (hModule)FreeLibrary(hModule);
};


void ExportFunction()
{
	MessageBox(NULL, L"ExportFunction", L"ExportFunction", MB_OK);
};
