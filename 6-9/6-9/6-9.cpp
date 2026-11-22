// 6-9.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <stdio.h>
#include "6-9.h"
DWORD OldAddress=0;
DWORD NewAddress=0;

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

//IATHook
BOOL SetIATHook(char* OldAddress, char* NewAddress)
{
	char* ImageBuffer = NULL;

	//1、获取自身句柄
	HMODULE hMainModule = GetModuleHandle(NULL);
	if (!hMainModule)return 0;
	ImageBuffer = (char*)hMainModule;

	//2、获取导入表入口
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
	if (!PtrToPeHeader)return 0;


	IMAGE_DATA_DIRECTORY ImportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	};


	PIMAGE_IMPORT_DESCRIPTOR ImportDirtory = (PIMAGE_IMPORT_DESCRIPTOR)(ImageBuffer + ImportEntry.VirtualAddress);

	//3、遍历导入表找到目标函数
	int* OFTitem;

	while (1)
	{
		printf("*******************************************************\n");
		if (ImportDirtory->Characteristics == 0 && ImportDirtory->FirstThunk == 0
			&& ImportDirtory->ForwarderChain == 0 && ImportDirtory->Name == 0 &&
			ImportDirtory->OriginalFirstThunk == 0 && ImportDirtory->TimeDateStamp == 0)
		{
			break;
		};

		//导入地址表
		OFTitem = (int*)(ImageBuffer + ImportDirtory->FirstThunk);

		while (1)
		{

			if (!(*OFTitem)) break;


			if (*OFTitem == (int)OldAddress)
			{
				DWORD OldProct = 0;
				VirtualProtect(OFTitem,4,PAGE_EXECUTE_READWRITE, &OldProct);
				memcpy(OFTitem, &NewAddress, 4);
				VirtualProtect(OFTitem, 4, OldProct, &OldProct);
				break;
			};

			OFTitem++;
		};

		ImportDirtory++;
	};
};

//解除IATHook
BOOL ResetIATHook(char* OldAddress, char* NewAddress)
{
	char* ImageBuffer = NULL;

	//1、获取自身句柄
	HMODULE hMainModule = GetModuleHandle(NULL);
	if (!hMainModule)return 0;
	ImageBuffer = (char*)hMainModule;

	//2、获取导入表入口
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
	if (!PtrToPeHeader)return 0;


	IMAGE_DATA_DIRECTORY ImportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	};


	PIMAGE_IMPORT_DESCRIPTOR ImportDirtory = (PIMAGE_IMPORT_DESCRIPTOR)(ImageBuffer + ImportEntry.VirtualAddress);

	//3、遍历导入表找到目标函数
	int* OFTitem;

	while (1)
	{
		printf("*******************************************************\n");
		if (ImportDirtory->Characteristics == 0 && ImportDirtory->FirstThunk == 0
			&& ImportDirtory->ForwarderChain == 0 && ImportDirtory->Name == 0 &&
			ImportDirtory->OriginalFirstThunk == 0 && ImportDirtory->TimeDateStamp == 0)
		{
			break;
		};

		//导入地址表
		OFTitem = (int*)(ImageBuffer + ImportDirtory->FirstThunk);

		while (1)
		{
			if (!(*OFTitem)) break;

			if (*OFTitem == (int)NewAddress)
			{
				DWORD OldProct = 0;
				VirtualProtect(OFTitem, 4, PAGE_EXECUTE_READWRITE, &OldProct);
				memcpy(OFTitem, &OldAddress, 4);
				VirtualProtect(OFTitem, 4, OldProct, &OldProct);
				break;
			};

			OFTitem++;
		};

		ImportDirtory++;
	};
};


//WINAPI要加上，默认是_cdecl.与WINAPI即stdcall不一样，会导致堆栈不平衡
int WINAPI MyMessagebox(_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	int ret = 0;
	printf("hWnd %x lpText %ws lpCaption %ws uType %x\n", hWnd, lpText, lpCaption, uType);
	typedef int (WINAPI *MessageBoxWProc)(HWND ,LPCWSTR ,LPCWSTR ,UINT );
	MessageBoxWProc Func = (MessageBoxWProc)OldAddress;//IATHOOK不能直接用MessageBoxW调用，此时MessageBoxW已经被改成MyMessagebox，会没完没了的调用自己
	ret= Func(hWnd, lpText, lpCaption, uType);
	printf("ret %x \n",ret);
	return ret;
};

