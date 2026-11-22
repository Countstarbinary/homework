#include <windows.h>
#pragma once
#include <windows.h>
#include <stdio.h>
#include "6-4.h"

BOOL IsSelf = TRUE;

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

int strlen_new(const char * pastring)
{
	int len = 0;
	while (1) 
	{
		if (*pastring) 
		{
			len++;
			pastring++;
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
	int len = strlen_new(pastring) + 1;
	
	while (1) 
	{
		if (len <=1)
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
	int* OFTitem;
	char* name;
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

		//name = FileBuffer + RvaToFoa(ImportDirtory->Name, FileBuffer);
		name = FileBuffer + ImportDirtory->Name;
		//wchar_t * wName = singlestring_to_wide(name);
		
		//wchar_t wName[256] = {0};本质上用的是memset，这个函数在注入时不能用，所以不初始化，人为在字符串后补终止符
		wchar_t wName[256];
		Asci_to_wchar(name, wName);
		
		//printf("动态链接库：%s\n", name);
		typedef HMODULE(WINAPI* LoadLibraryproc)(_In_ LPCWSTR lpLibFileName);
		LoadLibraryproc LoadLib = LoadLibrary;
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
				char* FunctionAdd = (char*)GetProcAddress(hModule, FuncName);
				*OFTitem = (int)FunctionAdd;//这里不论是上面用的是名称表还是地址表，修改的都是地址表
				//printf("函数名导出 %x %x %s\n", OFT, *OFTitem, name);
			}
			else
			{
				int FunctionNumber = *OFTitem & 0x7FFFFFFF;
				char* FunctionAdd = (char*)GetProcAddress(hModule, (LPCSTR)FunctionNumber);
				*OFTitem = (int)FunctionAdd;
				//printf("序号导出 %x %x\n", OFT, *OFTitem & 0x7FFFFFFF);
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
	if (!PtrToPeHeader)return;
	PIMAGE_DATA_DIRECTORY tlsDir = &PtrToPeHeader->NtHeadersPtr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
	if (tlsDir->Size == 0) return;

	PIMAGE_TLS_DIRECTORY tls = (PIMAGE_TLS_DIRECTORY)(imageBase + tlsDir->VirtualAddress);
	PIMAGE_TLS_CALLBACK* callback = (PIMAGE_TLS_CALLBACK*)tls->AddressOfCallBacks;
	while (callback && *callback) {
		(*callback)(imageBase, DLL_PROCESS_ATTACH, NULL);
		callback++;
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

	if (!RelocationEntry.Size|| !RelocationEntry.VirtualAddress)
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

				*virtadd = *virtadd - ImageBase + NewImagebase;

			};

			//重定位项更新
			RelocationEntryItem++;
		};
		//更新重定位表  
		RelocationDirtory = (PIMAGE_BASE_RELOCATION)((DWORD)RelocationDirtory + RelocationDirtory->SizeOfBlock);
	}

	free(PtrToPeHeader);
	return FileBuffer;
};


DWORD RemoteThread(LPVOID lpThreadParameter) 
{
	DWORD64 Hmodule = (DWORD64)lpThreadParameter;
	//10、更新导入表，之后就可以为所欲为。不能有需要重定位的函数，全部手写
	UpdataImportDirtoryX86((char*)Hmodule);

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders((char*)Hmodule);
	if (!PtrToPeHeader)return 0;
	
	MessageBox(NULL, L"测试", L"不是本进程", NULL);

	//这里不能转到入口点，程序直接退出了。
	typedef void(*AddressOfEntryPointProc)(void);
	typedef int(*WinMainProc)(HINSTANCE, HINSTANCE, LPSTR, int);
	AddressOfEntryPointProc OPE = (AddressOfEntryPointProc)(Hmodule+ PtrToPeHeader->u.OptionHeaderPtr32->AddressOfEntryPoint);
	//OPE();


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

		if ((DWORD)pHandle == -1)
		{

			goto ReleaseResource;
		};

		//6、在远程进程申请空间
		NewProcessImageBase = (char*)VirtualAllocEx(pHandle, NULL, SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);


		//7、对模块中的代码进行重定位
		Address = UpdataRelocationDirtory(ImageBuffer, (int)NewProcessImageBase);
		if ((int)Address == 0)
		{
			goto ReleaseResource;
		};

		//8、将修复后的数据，复制到A的内存中
		IsSuccess = WriteProcessMemory(pHandle, NewProcessImageBase, ImageBuffer, SizeOfImage, (SIZE_T*)&writelen);
		if (!IsSuccess)return 0;

		
		//9、创建一个远程线程，执行Entry
		ThreadFuntion = (int)RemoteThread - ImageBase + (int)NewProcessImageBase;
		tHandle = CreateRemoteThread(pHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)(ThreadFuntion), (LPVOID)NewProcessImageBase, NULL, NULL);
		if ((DWORD)tHandle == -1)
		{
			goto ReleaseResource;
		};

	ReleaseResource:
		if (PtrToPeHeader)free(PtrToPeHeader);
		if (ImageBuffer)VirtualFree(ImageBuffer, SizeOfImage, MEM_RELEASE);
		CloseHandle(pHandle);
		WaitForSingleObject(tHandle, INFINITE);
		CloseHandle(tHandle);
		return 0;

	End:
		//这里换成事件通知更好
		WaitForSingleObject(tHandle, INFINITE);
		if(NewProcessImageBase)VirtualFreeEx(pHandle, NewProcessImageBase, SizeOfImage,MEM_RELEASE);
		return 1;
	}
};