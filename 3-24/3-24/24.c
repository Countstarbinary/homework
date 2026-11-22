// 3-24.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

/*
注意编译版本是x64还是x86，对于win7x64以及更高的操作系统来说，无关紧要，高操作系统可以向下兼容。两种程序都可以运行

但是如win7x86等32位操作系统，不能向上兼容，只能运行32位程序。所以测试的时候注意别拿错了
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "24.h"

#define TargetFunciton 0x7747ea29


BYTE shellcode[] = {
	0x6A,0x00,0x6A,0x00,0x6A,0x00,0x6A,0x00,
	0xE8,0x00, 0x00, 0x00, 0x00,
	0xE9,0x00, 0x00, 0x00, 0x00, };

//每个函数里面都有一套获取PE头的代码，过于繁琐，索性直接实现一个函数用于获取这些头。注意释放内存
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

	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;

	PIMAGE_SECTION_HEADER SectionHeaderPtr = (PIMAGE_SECTION_HEADER)(FileBuffer + FileDosHeaderPtr->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER + FileHeaderPtr->SizeOfOptionalHeader);


	PPeAllHeaders HeadersPtr = (PPeAllHeaders)malloc(sizeof(PeAllHeaders));
	HeadersPtr->FileDosHeaderPtr = FileDosHeaderPtr;
	HeadersPtr->NtHeadersPtr = NtHeadersPtr;
	HeadersPtr->FileHeaderPtr = FileHeaderPtr;
	HeadersPtr->OptionHeaderPtr = OptionHeaderPtr;
	HeadersPtr->SectionHeaderPtr = SectionHeaderPtr;
	return HeadersPtr;
};

//把硬盘文件读取到内存FileBuffer。注意内存释放
BOOL FileToFileBuffer(char* FilePath, char* FileBuffer)
{
	//读取文件
	printf("开始读取文件\n");
	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open file! Error: %lu\n", GetLastError());
		return 0;
	}

	//获取文件大小，其内部还是NtQueryInformationFile函数实现的
	unsigned int FileSize = GetFileSize(hFile, NULL);

	if (FileSize == INVALID_FILE_SIZE)
	{
		printf("获取文件大小失败! Error: %lu\n", GetLastError());
		return 0;
	}

	//分配内存空间，用于存放文件中的数据.注意此处x64可能溢出
	char* Address = (char*)malloc(FileSize);
	memset(Address, 0, FileSize);

	*(int*)FileBuffer = (int)Address;
	DWORD lpNumberOfBytesRead;
	//读取文件中的数据到我们分配的内存空间中
	BOOL flag = ReadFile(hFile, Address, FileSize, &lpNumberOfBytesRead, NULL);

	//关闭句柄
	CloseHandle(hFile);

	return flag;
};

//把FileBuffer写回硬盘
BOOL FileBufferToFile(char* FilePath, char* FileBuffer)
{
	HANDLE hFile = CreateFile(L"C:\\Myfg.exe", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) printf("创建句柄失败！\n");

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	DWORD FileSize = PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].SizeOfRawData;
	DWORD lpNumberOfBytesRead;
	BOOL flag = WriteFile(hFile, FileBuffer, FileSize, &lpNumberOfBytesRead, NULL);
	if (!flag) printf("文件生成失败！\n");
	CloseHandle(hFile);
	free(PtrToPeHeader);
	return flag;
};


//把Rva转换成Foa。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int RvaToFoa(unsigned int Rva, char* FileBuffer)
{
	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;


	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		DWORD MAX = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData > PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize ? PtrToPeHeader->SectionHeaderPtr->SizeOfRawData : PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize;
		DWORD VirtualAddress = PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
		if ((Rva >= VirtualAddress) && (Rva < VirtualAddress + MAX))
		{
			Offset = Rva - PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
			return PtrToPeHeader->SectionHeaderPtr->PointerToRawData + Offset;
		};

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 0;
};


//返回对齐后的长度，第一个参数是没对齐的长度，如0x123，第二个参数是对齐长度，如0x200。
int Align(int length, int align)
{
	int x = length / align;
	int y = length % align;
	if (!y)
	{
		return x * align;
	};
	return (x + 1) * align;
};

typedef int(__stdcall *DivProc)(int x, int y);

DivProc di = NULL;

//输出FileBuffer的导出表信息
void OutputExportDirtory(char* FileBuffer)
{
	DWORD AddressOfNamesFoa = 0;
	DWORD AddressOfFunctionsFoa = 0;
	DWORD AddressOfNameOrdinalsFoa = 0;

	PDWORD AddressOfNamesFileBuffer = NULL;
	PDWORD AddressOfFunctionsFileBuffer = NULL;
	PWORD AddressOfNameOrdinalsFileBuffer = NULL;

	PDWORD AddressOfFuncitonName = NULL;
	DWORD FunctionVirtualAddress = NULL;

	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = PtrToPeHeader->OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, FileBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(FileBuffer + ExportDirtoryFoa);

	printf("************************************************************\n");
	printf("*****NumberOfFunciton:%d\n", ExportDirtory->NumberOfFunctions);
	printf("*****NumberOfNames:%d\n", ExportDirtory->NumberOfNames);
	printf("*****AddressOfFunctions:%d\n", ExportDirtory->AddressOfFunctions);
	printf("*****AddressOfNames:%d\n", ExportDirtory->AddressOfNames);
	printf("*****AddressOfNameOrdinals:%d\n", ExportDirtory->AddressOfNameOrdinals);
	printf("************************************************************\n");


	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称表在FileBuffer中的地址
		AddressOfNamesFoa = RvaToFoa(ExportDirtory->AddressOfNames, FileBuffer);
		AddressOfNamesFileBuffer = FileBuffer + AddressOfNamesFoa;

		//获取地址表在FileBuffer中的地址
		AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, FileBuffer);
		AddressOfFunctionsFileBuffer = FileBuffer + AddressOfFunctionsFoa;

		//获取序号表在FileBuffer中的地址
		AddressOfNameOrdinalsFoa = RvaToFoa(ExportDirtory->AddressOfNameOrdinals, FileBuffer);
		AddressOfNameOrdinalsFileBuffer = FileBuffer + AddressOfNameOrdinalsFoa;


		//获取名称在FileBuffer中的地址
		AddressOfFuncitonName = FileBuffer + RvaToFoa(AddressOfNamesFileBuffer[i], FileBuffer);

		//获取函数地址
		FunctionVirtualAddress = AddressOfFunctionsFileBuffer[AddressOfNameOrdinalsFileBuffer[i]];

		Offset = RvaToFoa(FunctionVirtualAddress, FileBuffer);

		BYTE jmp = *(PBYTE)(FileBuffer + Offset); //增量链接

		if (jmp == (BYTE)0xe9)
		{
			//有增量链接
			DWORD Off = *(PDWORD)(FileBuffer + Offset + 1);
			printf("*****函数:%s，地址：%x\n", AddressOfFuncitonName, FileBuffer + Offset + Off + 5);
		}
		else
		{
			//无增量链接
			printf("*****函数:%s，地址：%x\n", AddressOfFuncitonName, FileBuffer + RvaToFoa(FunctionVirtualAddress, FileBuffer));
		};


		AddressOfNamesFileBuffer++;
	};


	free(PtrToPeHeader);
};

//查询ImageBuffer中的函数地址，返回的是virtualaddress，完整的地址。考虑到只有在ImageBuffer中才能调用函数，因此这么设计.
//也因此这两个函数没有调用过，不知对错。哪天遍历模块的时候调用一下。
void* GetFunctionAddrByName(char* ImageBuffer, char * FunctionName)
{
	PVOID FunctionAddress = NULL;
	DWORD AddressOfNamesFoa = 0;
	DWORD AddressOfFunctionsFoa = 0;
	DWORD AddressOfNameOrdinalsFoa = 0;

	PDWORD AddressOfNamesFileBuffer = NULL;
	PDWORD AddressOfFunctionsFileBuffer = NULL;
	PWORD AddressOfNameOrdinalsFileBuffer = NULL;

	PDWORD AddressOfFuncitonName = NULL;
	DWORD FunctionVirtualAddress = NULL;

	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = PtrToPeHeader->OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, ImageBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(ImageBuffer + ExportDirtoryFoa);


	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称表在FileBuffer中的地址
		AddressOfNamesFoa = RvaToFoa(ExportDirtory->AddressOfNames, ImageBuffer);
		AddressOfNamesFileBuffer = ImageBuffer + AddressOfNamesFoa;

		//获取地址表在FileBuffer中的地址
		AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, ImageBuffer);
		AddressOfFunctionsFileBuffer = ImageBuffer + AddressOfFunctionsFoa;

		//获取序号表在FileBuffer中的地址
		AddressOfNameOrdinalsFoa = RvaToFoa(ExportDirtory->AddressOfNameOrdinals, ImageBuffer);
		AddressOfNameOrdinalsFileBuffer = ImageBuffer + AddressOfNameOrdinalsFoa;


		//获取名称在FileBuffer中的地址
		AddressOfFuncitonName = ImageBuffer + RvaToFoa(AddressOfNamesFileBuffer[i], ImageBuffer);

		//获取函数地址
		FunctionVirtualAddress = AddressOfFunctionsFileBuffer[AddressOfNameOrdinalsFileBuffer[i]];

		Offset = RvaToFoa(FunctionVirtualAddress, ImageBuffer);

		BYTE jmp = *(PBYTE)(ImageBuffer + Offset); //增量链接

		if (strcmp(AddressOfFuncitonName, FunctionName) == 0)
		{
			FunctionAddress = ImageBuffer + FunctionVirtualAddress;
			break;
		};

		AddressOfNamesFileBuffer++;
	};


	free(PtrToPeHeader);

	return FunctionAddress;
};

//查询FileBuffer中的函数地址，返回的是virtualaddress，完整的地址。
void* GetFunctionAddrByOrdinals(char* ImageBuffer, int FunctionOrdinals)
{
	PVOID FunctionAddress = NULL;

	DWORD AddressOfFunctionsFoa = 0;

	PDWORD AddressOfFunctionsFileBuffer = NULL;

	DWORD FunctionVirtualAddress = NULL;

	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = PtrToPeHeader->OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, ImageBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(ImageBuffer + ExportDirtoryFoa);


	//获取地址表在FileBuffer中的地址
	AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, ImageBuffer);
	AddressOfFunctionsFileBuffer = ImageBuffer + AddressOfFunctionsFoa;


	//获取函数地址
	FunctionVirtualAddress = AddressOfFunctionsFileBuffer[FunctionOrdinals - ExportDirtory->Base];


	if (FunctionVirtualAddress)
	{
		FunctionAddress = ImageBuffer + FunctionVirtualAddress;
	};


	free(PtrToPeHeader);

	return FunctionAddress;
};
