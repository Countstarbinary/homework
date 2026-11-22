#pragma once
#include <windows.h>
#include <stdio.h>
#include "6-4.h"

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

BOOL FileToFileBuffer(TCHAR* FilePath, char* FileBuffer)
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

	//分配内存空间，用于存放文件中的数据.注意此处x64可能溢出。已修复
	char* Address = (char*)malloc(FileSize);
	memset(Address, 0, FileSize);

	if (sizeof(char*) == 8)
	{
		*(unsigned __int64*)FileBuffer = (unsigned __int64)Address;
	}
	else
	{
		*(int*)FileBuffer = (int)Address;
	};



	DWORD lpNumberOfBytesRead;
	//读取文件中的数据到我们分配的内存空间中
	BOOL flag = ReadFile(hFile, Address, FileSize, &lpNumberOfBytesRead, NULL);

	//关闭句柄
	CloseHandle(hFile);

	return flag;
};

//文件拉伸：把文件从文件中的格式展开成内存中的格式
BOOL FileBufferToImageBuffer(char* FileBuffer, char* ImageBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//头部复制
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		memcpy(ImageBuffer, FileBuffer, PtrToPeHeader->u.OptionHeaderPtr32->SizeOfHeaders);
	}
	else
	{
		memcpy(ImageBuffer, FileBuffer, PtrToPeHeader->u.OptionHeaderPtr64->SizeOfHeaders);
	};
	printf("文件头转移成功！\n");

	//节区拉伸
	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{

		char* TargetAddress = ImageBuffer + PtrToPeHeader->SectionHeaderPtr->VirtualAddress;

		char* SourceAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr->PointerToRawData;

		unsigned int Size = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData;

		memcpy(TargetAddress, SourceAddress, Size);

		printf("%x 节拉伸成功成功！\n", PtrToPeHeader->SectionHeaderPtr);

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 1;
};

//把Rva转换成Foa。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int RvaToFoa(unsigned int Rva, char* FileBuffer)
{
	int Offset = 0;
	DWORD Foa = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	if (PtrToPeHeader->u.OptionHeaderPtr->FileAlignment == PtrToPeHeader->u.OptionHeaderPtr->SectionAlignment)
	{
		Foa = Rva;
		free(PtrToPeHeader);
		return Foa;
	};

	if (Rva <= PtrToPeHeader->u.OptionHeaderPtr->SizeOfHeaders)
	{
		Foa = Rva;
		free(PtrToPeHeader);
		return Foa;
	};


	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		DWORD MAX = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData > PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize ? PtrToPeHeader->SectionHeaderPtr->SizeOfRawData : PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize;
		DWORD VirtualAddress = PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
		if ((Rva >= VirtualAddress) && (Rva < VirtualAddress + MAX))
		{
			Offset = Rva - PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
			Foa = PtrToPeHeader->SectionHeaderPtr->PointerToRawData + Offset;
			free(PtrToPeHeader);
			return Foa;
		};

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 0;
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
		wchar_t * wName=singlestring_to_wide(name);
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
				char* FunctionAdd=(char*)GetProcAddress(hModule, name);
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

//修复导入表，需要拉伸后的FileBuffer
BOOL UpdataImportDirtoryX64(char* FileBuffer)
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
	DWORD64* OFTitem;
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
		OFTitem = (DWORD64*)(FileBuffer + ImportDirtory->FirstThunk);

		//name = FileBuffer + RvaToFoa(ImportDirtory->Name, FileBuffer);

		name = FileBuffer + ImportDirtory->Name;
		wchar_t * wName = singlestring_to_wide(name);
		printf("动态链接库：%s\n", name);
		HMODULE hModule = LoadLibrary((LPCWSTR)wName);
		if (!hModule)
		{
			ImportDirtory++;
			printf("dll加载失败，跳过\n");
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
				*OFTitem = (DWORD64)FunctionAdd;
				printf("函数名导出 %x %x %s\n", OFT, *OFTitem, name);
			}
			else
			{
				int FunctionNumber = *OFTitem & 0x7FFFFFFF;
				char* FunctionAdd = (char*)GetProcAddress(hModule, (LPCSTR)FunctionNumber);
				*OFTitem = (DWORD64)FunctionAdd;
				printf("序号导出 %x %x\n", OFT, *OFTitem & 0x7FFFFFFF);
			};
			OFTitem++;
		};

		ImportDirtory++;
	};
	return 1;

};

VOID ExecuteTLSCallbacks(char* imageBase) 
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(imageBase);
	if (!PtrToPeHeader)return ;
	PIMAGE_DATA_DIRECTORY tlsDir = &PtrToPeHeader->NtHeadersPtr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
	if (tlsDir->Size == 0) return;

	PIMAGE_TLS_DIRECTORY tls = (PIMAGE_TLS_DIRECTORY)(imageBase + tlsDir->VirtualAddress);
	PIMAGE_TLS_CALLBACK* callback = (PIMAGE_TLS_CALLBACK*)tls->AddressOfCallBacks;
	while (callback && *callback) {
		(*callback)(imageBase, DLL_PROCESS_ATTACH, NULL);
		callback++;
	}
}
//加载进程，这个加载出来可以运行。但是没有界面，fg.exe直接点不出来。
//1、将自己进程的ImageBase设置一个较大的值，让自己的程序在高空运行.设置就行
BOOL LoadProcess(TCHAR* FilePath)
{
	BOOL IsSuccess = 0;
	char* FileBuffer = NULL;
	char* ImageBuffer = NULL;
	DWORD64 SizeOfImage = 0;
	DWORD64 ImageBase = 0;
	//2、将要执行的进程读取进来，按照进程的ImageBase和SizeOfImage分配空间
	FileToFileBuffer(FilePath, (char *)&FileBuffer);

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		SizeOfImage = PtrToPeHeader->u.OptionHeaderPtr32->SizeOfImage;
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr32->ImageBase;
	}
	else 
	{
		SizeOfImage = PtrToPeHeader->u.OptionHeaderPtr64->SizeOfImage;
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr64->ImageBase;
	};
	ImageBuffer = (char*)VirtualAlloc((LPVOID)ImageBase, SizeOfImage, MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!ImageBuffer)return 0;
	memset(ImageBuffer,0, SizeOfImage);

	//3、拉伸进程
	IsSuccess = FileBufferToImageBuffer(FileBuffer, ImageBuffer);
	if (!IsSuccess)return 0;

	//4、修复IAT表
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		IsSuccess = UpdataImportDirtoryX86(ImageBuffer);
	}
	else
	{
		IsSuccess = UpdataImportDirtoryX64(ImageBuffer);
	};
	
	if (!IsSuccess)return 0;

	//5、跳转到入口执行
	typedef void(*AddressOfEntryPointProc)(void);
	typedef int(*WinMainProc)(HINSTANCE, HINSTANCE, LPSTR, int);
	AddressOfEntryPointProc OPE=(AddressOfEntryPointProc)(ImageBase+PtrToPeHeader->NtHeadersPtr->OptionalHeader.AddressOfEntryPoint);

	OPE();
};