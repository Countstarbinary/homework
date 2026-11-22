
#pragma once
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "6-11 Project.h"

#define OpenMonitor 1
#define CloseMonitor 2
#define RemoteCall 3

BOOL IsSelf = TRUE;
HANDLE EventNotify = NULL;
HANDLE EventNotifyInject = NULL;
DWORD OldImageBase = 0;
DWORD NewImageBase = 0;
Message MSG_NEW[256] = { 0 };
BYTE  gOrigBytes[1024][16]; //保存inlinehook原始函数中的数据

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

int strlen_new(char * pastring)
{
	int len = 0;
	char* temp = pastring;
	while (1)
	{
		if (*temp)
		{
			len++;
			temp++;
		}
		else
		{
			break;
		};
	};
	return len;
};

int wcstrlen_new(wchar_t * pastring)
{
	int len = 0;
	wchar_t* temp = pastring;
	while (1)
	{
		if (*temp)
		{
			len++;
			temp++;
		}
		else
		{
			break;
		};
	};
	return len;
};

wchar_t * Asci_to_wchar(const char * pastring, wchar_t * pwstring)
{
	int len = strlen_new((char * )pastring) + 1;

	while (1)
	{
		if (len <= 1)
		{
			//;在修复导入表前，wchar_t wName[256] = {0}清空不了内存，人为加上后缀
			*pwstring = 0x00;
			break;
		};
		*pwstring = *pastring;
		pwstring++;
		pastring++;
		len--;
	};

	return pwstring;
}

//区分大小写
BOOL wcscmp_NEW(TCHAR* DES, TCHAR* SRC)
{
	int len = wcstrlen_new(DES);
	BOOL flag = 1;
	TCHAR* DEStemp = DES;
	TCHAR* SRCtemp = SRC;
	for (int i = 0; i < len; i++)
	{
		if (*DEStemp != *SRCtemp)
		{
			flag = 0;
			break;
		};
		DEStemp++;
		SRCtemp++;
	};
	return flag;
};
BOOL strcmp_NEW(char* DES, char* SRC)
{
	int len = strlen_new(DES);
	BOOL flag = 1;
	char* DEStemp = DES;
	char* SRCtemp = SRC;
	for (int i = 0; i < len; i++)
	{
		if (*DEStemp != *SRCtemp)
		{
			flag = 0;
			break;
		};
		DEStemp++;
		SRCtemp++;
	};
	return flag;
};

//查询ImageBuffer中的函数地址，返回的是virtualaddress，完整的地址。考虑到只有在ImageBuffer中才能调用函数，因此这么设计.
void* GetFuncAddrByName(char* ImageBuffer, char * FunctionName)
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
	PIMAGE_DOS_HEADER FileDosHeaderPtr = (PIMAGE_DOS_HEADER)ImageBuffer;

	if (FileDosHeaderPtr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeadersPtr = (PIMAGE_NT_HEADERS)(ImageBuffer + FileDosHeaderPtr->e_lfanew);

	if (NtHeadersPtr->Signature != IMAGE_NT_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_FILE_HEADER FileHeaderPtr = &NtHeadersPtr->FileHeader;

	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];


	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(ImageBuffer + ExportEntry.VirtualAddress);


	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称表
		AddressOfNamesFileBuffer = (PDWORD)(ImageBuffer + ExportDirtory->AddressOfNames);

		//获取地址表
		AddressOfFunctionsFileBuffer = (PDWORD)(ImageBuffer + ExportDirtory->AddressOfFunctions);

		//获取序号表
		AddressOfNameOrdinalsFileBuffer = (PWORD)(ImageBuffer + ExportDirtory->AddressOfNameOrdinals);


		//获取名称
		AddressOfFuncitonName = (PDWORD)(ImageBuffer + AddressOfNamesFileBuffer[i]);

		//获取函数地址
		FunctionVirtualAddress = AddressOfFunctionsFileBuffer[AddressOfNameOrdinalsFileBuffer[i]];


		if (strcmp_NEW((char*)AddressOfFuncitonName, (char*)FunctionName))
		{
			FunctionAddress = ImageBuffer + FunctionVirtualAddress;
			break;
		};

		AddressOfNamesFileBuffer++;
	};


	

	return FunctionAddress;
};

//查询FileBuffer中的函数地址，返回的是virtualaddress，完整的地址。
void* GetFuncAddrByOrdinals(char* ImageBuffer, int FunctionOrdinals)
{
	PVOID FunctionAddress = NULL;

	DWORD AddressOfFunctionsFoa = 0;

	PDWORD AddressOfFunctionsFileBuffer = NULL;

	DWORD FunctionVirtualAddress = NULL;

	int Offset = 0;
	PIMAGE_DOS_HEADER FileDosHeaderPtr = (PIMAGE_DOS_HEADER)ImageBuffer;

	if (FileDosHeaderPtr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeadersPtr = (PIMAGE_NT_HEADERS)(ImageBuffer + FileDosHeaderPtr->e_lfanew);

	if (NtHeadersPtr->Signature != IMAGE_NT_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_FILE_HEADER FileHeaderPtr = &NtHeadersPtr->FileHeader;

	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];


	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(ImageBuffer + ExportEntry.VirtualAddress);


	//获取地址表
	AddressOfFunctionsFileBuffer = (PDWORD)(ImageBuffer + ExportDirtory->AddressOfFunctions);


	//获取函数地址
	FunctionVirtualAddress = AddressOfFunctionsFileBuffer[FunctionOrdinals - ExportDirtory->Base];


	if (FunctionVirtualAddress)
	{
		FunctionAddress = ImageBuffer + FunctionVirtualAddress;
	};

	return FunctionAddress;
};

#include <Windows.h>
#include <winternl.h>
#include <wchar.h>

// 确保必要的结构体定义
#ifndef _PEB_DEFINED
//typedef struct _PEB_LDR_DATA {
//	BYTE Reserved1[8];
//	PVOID Reserved2[3];
//	LIST_ENTRY InMemoryOrderModuleList;
//} PEB_LDR_DATA, *PPEB_LDR_DATA;

//typedef struct _LDR_DATA_TABLE_ENTRY {
//	PVOID Reserved1[2];
//	LIST_ENTRY InMemoryOrderLinks;
//	PVOID Reserved2[2];
//	PVOID DllBase;
//	PVOID EntryPoint;
//	PVOID Reserved3;
//	UNICODE_STRING FullDllName;
//	BYTE Reserved4[8];
//	PVOID Reserved5[3];
//	union {
//		ULONG CheckSum;
//		PVOID Reserved6;
//	};
//	ULONG TimeDateStamp;
//} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

//typedef struct _PEB {
//	BYTE Reserved1[2];
//	BYTE BeingDebugged;
//	BYTE Reserved2[1];
//	PVOID Reserved3[2];
//	PPEB_LDR_DATA Ldr;
//	// 简化定义，只保留必要字段
//} PEB, *PPEB;
#endif

//获取Kernel32.dll.更新导入表，不能有系统函数调用，自写函数。后来发现是重定位表没有更新全。想用系统的和自写的都行
HMODULE GetKernel32Base()
{
	PPEB peb = NULL;

	// 安全获取 PEB 指针（兼容 x86/x64）
#if defined(_WIN64)
	peb = (PPEB)__readgsqword(0x60);  // GS:[0x60] for x64
#else
	peb = (PPEB)__readfsdword(0x30);  // FS:[0x30] for x86
#endif

	// 验证 PEB 有效性
	if (!peb ) {
		return NULL;
	}

	// 验证 LDR 指针有效性
	PPEB_LDR_DATA ldr = peb->Ldr;
	if (!ldr ) {
		return NULL;
	}

	// 获取模块链表
	LIST_ENTRY *listHead = &ldr->InMemoryOrderModuleList;
	LIST_ENTRY *listEntry = listHead->Flink;

	// 遍历模块链表
	while (listEntry != listHead)
	{
		// 获取 LDR 条目
		PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(
			listEntry,
			LDR_DATA_TABLE_ENTRY,
			InMemoryOrderLinks
		);

		// 验证条目指针有效性
		if (!entry ) {
			break;
		}

		// 检查模块名称
		if (entry->FullDllName.Buffer)
		{
			// 检查 kernel32.dll 或 KernelBase.dll
			if (wcscmp_NEW(entry->FullDllName.Buffer, (TCHAR*)L"C:\\WINDOWS\\System32\\KERNEL32.DLL"))
			{
				
				return (HMODULE)entry->DllBase;
			}
		}

		// 移动到下一个条目
		listEntry = listEntry->Flink;

	}
	return 0;


	
}


//修复导入表，需要拉伸后的FileBuffer
BOOL UpdataImportDirtoryX86(char* FileBuffer)
{

	PIMAGE_DOS_HEADER FileDosHeaderPtr = (PIMAGE_DOS_HEADER)FileBuffer;

	if (FileDosHeaderPtr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeadersPtr = (PIMAGE_NT_HEADERS)(FileBuffer + FileDosHeaderPtr->e_lfanew);

	if (NtHeadersPtr->Signature != IMAGE_NT_SIGNATURE)
	{
		//printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_FILE_HEADER FileHeaderPtr = &NtHeadersPtr->FileHeader;



	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;
	//PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	//if (!PtrToPeHeader)return 0;

	//获取导入表入口
	IMAGE_DATA_DIRECTORY ImportEntry = { 0 };
	if (FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImportEntry = OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		ImportEntry = OptionHeaderPtr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	};

	//导入表入口 RvaToFoa。拉伸后不需要转换
	//DWORD ImportEntryFoa = RvaToFoa(ImportEntry.VirtualAddress, FileBuffer);

	PIMAGE_IMPORT_DESCRIPTOR ImportDirtory = (PIMAGE_IMPORT_DESCRIPTOR)(FileBuffer + ImportEntry.VirtualAddress);

	DWORD OFT = NULL;
	int* OFTitem;     //名称表
	int* OFTAddress;  //地址表
	char* name;

	HMODULE kernel32 = GetKernel32Base();
	if (!kernel32) { return 0; };
	typedef HMODULE(WINAPI* LoadLibraryproc)(_In_ LPCWSTR lpLibFileName);
	LoadLibraryproc LoadLib = (LoadLibraryproc)GetFuncAddrByName((char*)kernel32, (char*)"LoadLibraryW");

	while (1)
	{
		//printf("*******************************************************\n");
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
		//OFTitem = (int*)(FileBuffer + ImportDirtory->FirstThunk);

		//导入名称表。这里不能用地址表，本进程已经打开，模块已经加载，地址表已经加载了本进程的函数地址，不能用了。所以用名称表
		OFTitem = (int*)(FileBuffer + ImportDirtory->OriginalFirstThunk);
		OFTAddress= (int*)(FileBuffer + ImportDirtory->FirstThunk);
		//name = FileBuffer + RvaToFoa(ImportDirtory->Name, FileBuffer);
		name = FileBuffer + ImportDirtory->Name;
		//wchar_t * wName = singlestring_to_wide(name);

		//wchar_t wName[256] = {0};本质上用的是memset，这个函数在注入时不能用，所以不初始化，人为在字符串后补终止符
		wchar_t wName[256];
		Asci_to_wchar(name, wName);

		//printf("动态链接库：%s\n", name);

		HMODULE hModule = LoadLib((LPCWSTR)wName);
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
				char* FuncName = FileBuffer + *OFTitem + 2;
				//if (strcmp_NEW(FuncName, (char*)"WaitForSingleObject"))
				//{
				//	int test = 0xffff8989;
				//};
				char* FunctionAdd = (char*)GetFuncAddrByName((char*)hModule, FuncName);
				*OFTAddress = (int)FunctionAdd;//这里不论是上面用的是名称表还是地址表，修改的都是地址表
				//printf("函数名导出 %x %x %s\n", OFT, *OFTitem, name);
			}
			else
			{
				int FunctionNumber = *OFTitem & 0x7FFFFFFF;
				char* FunctionAdd = (char*)GetFuncAddrByOrdinals((char*)hModule, FunctionNumber);
				*OFTAddress = (int)FunctionAdd;
				//printf("序号导出 %x %x\n", OFT, *OFTitem & 0x7FFFFFFF);
			};
			OFTitem++;
			OFTAddress++;
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

bool IsReadable(void* ptr, size_t size)
{
	__try {
		volatile char temp;
		for (size_t i = 0; i < size; ++i) {
			temp = ((char*)ptr)[i];  // 触发读访问
		}
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}



//修复重定位表
char* UpdataRelocationDirtory(char* FileBuffer, int NewImagebase)
{
	PWORD RelocationEntryItem = 0;


	//获取pe文件头

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//修复重定位项,若有必要
	DWORD ImageBase = 0;
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr32->ImageBase;
		//PtrToPeHeader->u.OptionHeaderPtr->ImageBase = NewImagebase;
	}
	else
	{
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr64->ImageBase;
		//PtrToPeHeader->u.OptionHeaderPtr->ImageBase = NewImagebase;
	};



	//获取重定位表入口。x64的头有些不同，堆和栈的保留宽度宏变，如果解析的exe是32位就编译成32位程序，64就编译成64位
	IMAGE_DATA_DIRECTORY RelocationEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	}
	else
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	};

	if (!RelocationEntry.Size || !RelocationEntry.VirtualAddress)
	{
		return (char*)1;
	};
	//重定位表入口 RvaToFoa
	//DWORD RelocationEntryFoa = RvaToFoa(RelocationEntry.VirtualAddress, FileBuffer);

	//重定位表
	PIMAGE_BASE_RELOCATION RelocationDirtory = (PIMAGE_BASE_RELOCATION)(FileBuffer + RelocationEntry.VirtualAddress);



	//移动导出表
	int* virtadd = 0;
	RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
	//修复重定位表,若有必要
	while (1)
	{
		if (RelocationDirtory->VirtualAddress == 0 && RelocationDirtory->SizeOfBlock == 0)
		{
			break;
		}
		for (int i = 0; i < (RelocationDirtory->SizeOfBlock - 8) / 2; i++)
		{
			//判断是否是需要重定位的项 
			if (((*RelocationEntryItem) & 0x3000) == 0x3000)
			{
				//计算rva。注意数据类型要进行强转，否则计算失败
				virtadd = (int*)(FileBuffer + RelocationDirtory->VirtualAddress + (DWORD)((*RelocationEntryItem) & 0xFFF));
				int test = (int)virtadd & 0x0000FFFF;
				//0X00203C35
				//if (i == 0x3A)
				//{

				//	MessageBox(NULL, L"找到了", NULL, NULL);

				//};
				*virtadd = *virtadd - ImageBase + NewImagebase;

			};

			//重定位项更新
			RelocationEntryItem++;
		};
		//更新重定位表  
		RelocationDirtory = (PIMAGE_BASE_RELOCATION)((DWORD)RelocationDirtory + RelocationDirtory->SizeOfBlock);
		//更新重定位项，之前的都没加这行，就会少更新几个函数，运气好可以运行
		RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
	}

	free(PtrToPeHeader);
	return FileBuffer;
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
				VirtualProtect(OFTitem, 4, PAGE_EXECUTE_READWRITE, &OldProct);
				memcpy(OFTitem, &NewAddress, 4);
				VirtualProtect(OFTitem, 4, OldProct, &OldProct);
				return 1;
			};

			OFTitem++;
		};

		ImportDirtory++;
	};
	return 0;
};

//解除IATHook
BOOL UnIATHook(char* OldAddress, char* NewAddress)
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
				return 1;
			};

			OFTitem++;
		};

		ImportDirtory++;
	};
	return 0;
};

// Inline Hook
//   target     - 要 Hook 的函数地址
//   detour     - 跳转到的目标函数（你的替身函数）
//   origBytes  - 缓存原始字节，大小至少为 length
//   length     - 要覆盖的字节数（最少 5）
// 返回 TRUE 表示成功
BOOL InlineHook(void* target, void* detour, BYTE* origBytes, SIZE_T length)
{
	DWORD oldProtect;
	BYTE  patch[5];
	SIZE_T i;

	assert(length >= 5);

	// 1. 备份原始字节
	memcpy(origBytes, target, length);

	// 2. 修改页面保护，确保可写
	if (!VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &oldProtect))
		return FALSE;

	// 3. 构造 JMP 指令 (0xE9 + 相对偏移)
	//    rel = detour - target - 5
	*(BYTE*)(patch + 0) = 0xE9;
	*(DWORD*)(patch + 1) = (DWORD)((BYTE*)detour - (BYTE*)target - 5);

	// 4. 写入 JMP 指令
	memcpy(target, patch, sizeof(patch));

	// 5. 用 NOP 填充剩余字节（如果 length>5）
	for (i = 5; i < length; ++i) {
		*((BYTE*)target + i) = 0x90;
	}

	// 6. 刷新指令缓存
	FlushInstructionCache(GetCurrentProcess(), target, length);

	// 7. 恢复原保护
	VirtualProtect(target, length, oldProtect, &oldProtect);
	return TRUE;
}

// Unhook：恢复原始字节
BOOL InlineUnhook(void* target, const BYTE* origBytes, SIZE_T length)
{
	DWORD oldProtect;

	assert(length >= 5);

	// 1. 解除写保护
	if (!VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &oldProtect))
		return FALSE;

	// 2. 恢复原始字节
	memcpy(target, origBytes, length);

	// 3. 刷新指令缓存
	FlushInstructionCache(GetCurrentProcess(), target, length);

	// 4. 恢复保护
	VirtualProtect(target, length, oldProtect, &oldProtect);
	return TRUE;
}



//创建共享缓冲区
LPVOID CreateMappingAndMapView(DWORD* Handle)
{
	Sleep(100);

	// 创建一个文件映射
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024 * 2, L"Global\MySharedFileMapping");

	if (!hMap)
	{
		return 0;
	}


	//把文件映射映射到本进程地址空间
	LPVOID pBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 1024 * 2);
	if (!pBuf)
	{
		CloseHandle(hMap);
		return 0;
	}
	*Handle = (DWORD)hMap;
	return pBuf;
};

void End(HANDLE hMap, LPVOID pBuf)
{
	// Clean up
	UnmapViewOfFile(pBuf);
	CloseHandle(hMap);
}

//写换共享缓冲区
char* WriterThread(PMessage message, LPVOID pBuf)
{


	WaitForSingleObject(EventNotifyInject, INFINITE);
	// 写消息
	//const char* message = "Hello from writer thread via shared file mapping!";
	memcpy(pBuf, message, sizeof(Message));


	//通知
	SetEvent(EventNotify);
	return (char*)message;
}

//读换共享缓冲区
char* ReaderThread(LPVOID pBuf)
{
	char* pNewBuf = (char*)malloc(sizeof(Message));
	if (!pNewBuf)return 0;
	WaitForSingleObject(EventNotify, INFINITE);
	printf("test %s\n", pBuf);
	memcpy(pNewBuf, pBuf, sizeof(Message));
	memset(pBuf, 0, 1024 * 2);
	SetEvent(EventNotifyInject);

	return pNewBuf;
}

BOOL OpenMonitorFunc(PMessage pNewBuf, int NumberOfSize)
{


	BOOL IsSuccess = SetIATHook(pNewBuf->OldAddress, pNewBuf->NewAddress);

	if (!IsSuccess)
	{
		IsSuccess = InlineHook(pNewBuf->OldAddress, pNewBuf->NewAddress, gOrigBytes[pNewBuf->MonitorNumer], NumberOfSize);
	};
	return IsSuccess;
};

BOOL CloseMonitorFunc(PMessage pNewBuf, int NumberOfSize)
{


	BOOL IsSuccess = UnIATHook(pNewBuf->OldAddress, pNewBuf->NewAddress);

	if (!IsSuccess)
	{
		IsSuccess = InlineUnhook(pNewBuf->OldAddress, gOrigBytes[pNewBuf->MonitorNumer], NumberOfSize);
	};
	return IsSuccess;
};

BOOL RemoteFunc(PMessage pNewBuf)
{
	printf("%x\n", pNewBuf->FuncPara[0]);
	printf("%x\n", pNewBuf->FuncPara[1]);
	printf("%x\n", pNewBuf->FuncPara[2]);
	printf("%x\n", pNewBuf->FuncPara[3]);
	TCHAR szBuffer1[128] = { 0 };
	memcpy(szBuffer1, pNewBuf->FuncPara[1], 128);
	TCHAR szBuffer2[128] = { 0 };
	memcpy(szBuffer2, pNewBuf->FuncPara[2], 128);
	MessageBox(NULL, szBuffer1, szBuffer2, NULL);
	return 1;
};

//项目->属性->c/c++->所有选择->仅支持我的代码调试，选择否，去掉__CheckForDebuggerJustMyCode函数
DWORD RemoteThread(LPVOID lpThreadParameter)
{
	DWORD64 Hmodule = (DWORD64)lpThreadParameter;
	//10、更新导入表，之后就可以为所欲为。不能有需要重定位的函数，全部手写
	UpdataImportDirtoryX86((char*)Hmodule);
	MessageBox(NULL, L"导入表修复完毕", NULL, NULL);
	EventNotifyInject = CreateEvent(NULL, FALSE, TRUE, L"EventNotifyInject");
	EventNotify = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventNotify");
	HANDLE hMap;
	LPVOID pBuf = CreateMappingAndMapView((DWORD*)&hMap);

	while (1)
	{
		PMessage pNewBuf = (PMessage)ReaderThread(pBuf);
		if (!pNewBuf)
		{
			continue;
		};
		switch (pNewBuf->type)
		{
		case OpenMonitor:
		{
			if (pNewBuf->NumberOfChar == 5)
			{
				OpenMonitorFunc(pNewBuf, 5);
				MessageBox(NULL, L"开启监控", L"开启监控", NULL);
			}
			else if (pNewBuf->NumberOfChar == 6)
			{
				OpenMonitorFunc(pNewBuf, 5);
				wchar_t * name=wcscat((wchar_t *)L"开启监控",pNewBuf->FuncName);
				MessageBox(NULL, name, L"开启监控", NULL);
			};

			break;
		}
		case CloseMonitor:
		{
			if (pNewBuf->NumberOfChar == 5)
			{
				CloseMonitorFunc(pNewBuf, 5);

				MessageBox(NULL, L"关闭监控", L"关闭监控", NULL);
			}
			else if (pNewBuf->NumberOfChar == 6)
			{
				CloseMonitorFunc(pNewBuf, 6);
				MessageBox(NULL, L"关闭监控", L"关闭监控", NULL);
			};
			break;
		}
		case RemoteCall:
		{
			RemoteFunc(pNewBuf);
			break;
		};

		};
		//注意释放内存，否则程序自动退出
		free(pNewBuf);
	};


	//End(hMap, pBuf);


	return 0;
};

//内存写入，传入被注入进程的pid
BOOL LoadMemory(DWORD64 pid)
{
	HANDLE tHandle = NULL;
	HANDLE pHandle = NULL;

	char* ImageBuffer = NULL;
	char* Address = NULL;
	char* NewProcessImageBase = NULL;

	DWORD64 SizeOfImage = 0;
	DWORD64 ImageBase = 0;
	BOOL IsSuccess = 0;
	int writelen = 0;
	DWORD OPE = 0;
	DWORD ThreadFuntion = 0;

	//判断是否是本进程和注入进程，如果是本进程，就进行注入操作
	if (IsSelf)
	{
		IsSelf = FALSE;

		//1、获取自身句柄
		HMODULE hMainModule = GetModuleHandle(NULL);
		if (!hMainModule)return 0;
		OldImageBase = (DWORD)hMainModule;
		//2、得到ImageSize的大小,得到模块的ImageBuffer
		PPeAllHeaders PtrToPeHeader = GetPeAllHeaders((char*)hMainModule);
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

		//3、在当前空间申请空间存放自身代码
		ImageBuffer = (char*)VirtualAlloc(NULL, SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!ImageBuffer)
		{
			goto ReleaseResource;
		}
		memset(ImageBuffer, 0, SizeOfImage);

		//4、拷贝自身到缓存
		memcpy(ImageBuffer, hMainModule, SizeOfImage);

		//5、打开要注入的进程
		pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		if (pHandle == INVALID_HANDLE_VALUE || pHandle == 0)
		{
			goto ReleaseResource;
		};

		//6、在远程进程申请空间
		NewProcessImageBase = (char*)VirtualAllocEx(pHandle, NULL, SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!NewProcessImageBase)
		{
			goto ReleaseResource;
		};
		
		NewImageBase = (DWORD)NewProcessImageBase;
		//7、对模块中的代码进行重定位
		Address = UpdataRelocationDirtory(ImageBuffer, (int)NewProcessImageBase);
		if ((int)Address == 0)
		{
			goto ReleaseResource;
		};

		//8、将修复后的数据，复制到A的内存中
		IsSuccess = WriteProcessMemory(pHandle, NewProcessImageBase, ImageBuffer, SizeOfImage, (SIZE_T*)&writelen);
		if (!IsSuccess)
		{
			goto ReleaseResource;
		}


		//9、创建一个远程线程，执行Entry
		ThreadFuntion = (int)RemoteThread - ImageBase + (int)NewProcessImageBase;
		tHandle = CreateRemoteThread(pHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)(ThreadFuntion), (LPVOID)NewProcessImageBase, NULL, NULL);
		if ((DWORD)tHandle == -1)
		{
			goto ReleaseResource;
		};
		goto End;
	ReleaseResource:
		if (PtrToPeHeader)free(PtrToPeHeader);
		if (ImageBuffer)VirtualFree(ImageBuffer, SizeOfImage, MEM_RELEASE);
		CloseHandle(pHandle);
		CloseHandle(tHandle);
		return 0;

	End:

		CloseHandle(pHandle);
		CloseHandle(tHandle);
		//if (NewProcessImageBase)VirtualFreeEx(pHandle, NewProcessImageBase, SizeOfImage, MEM_RELEASE);//资源释放没写
		return 1;
	}
};

