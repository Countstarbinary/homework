// 
//	本项目在写完之后，本机(win11)可以运行，x86不能运行，还找不到原因。
//  后来在x86环境下重写梳理，有许多vs2017可以通过的语法，在vc6++通过不了，修改后可以运行。
//  主要的问题是字符宽度的不同，导致读文件报2号错误
//	

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "17.h"

#define TargetFunciton 0x7754ea29


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


	PPeAllHeaders HeadersPtr=(PPeAllHeaders)malloc(sizeof(PeAllHeaders));
	HeadersPtr->FileDosHeaderPtr= FileDosHeaderPtr;
	HeadersPtr->NtHeadersPtr = NtHeadersPtr;
	HeadersPtr->FileHeaderPtr= FileHeaderPtr;
	HeadersPtr->OptionHeaderPtr = OptionHeaderPtr;
	HeadersPtr->SectionHeaderPtr= SectionHeaderPtr;
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

	//分配内存空间，用于存放文件中的数据
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

	if (!hFile) printf("文件写回内存失败！\n");

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;
	
	DWORD FileSize = PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].SizeOfRawData;
	DWORD lpNumberOfBytesRead;
	BOOL flag = WriteFile(hFile, FileBuffer, FileSize, &lpNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	free(PtrToPeHeader);
	return flag;
};

//文件拉伸：把文件从文件中的格式展开成内存中的格式
void FileBufferToImageBuffer(char* FileBuffer, char* ImageBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//头部复制
	memcpy(ImageBuffer, FileBuffer, PtrToPeHeader->OptionHeaderPtr->SizeOfHeaders);
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
};

//文件收缩：把文件从内存中的格式收缩成文件中的格式
void ImageBufferToFileBuffer(char* FileBuffer, char* ImageBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//头部复制
	memcpy(FileBuffer, ImageBuffer, PtrToPeHeader->OptionHeaderPtr->SizeOfHeaders);
	printf("文件头转移成功！\n");

	//节区收缩
	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		char* TargetAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr->PointerToRawData;

		char* SourceAddress = ImageBuffer + PtrToPeHeader->SectionHeaderPtr->VirtualAddress;

		unsigned int Size = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData;

		memcpy(TargetAddress, SourceAddress, Size);

		printf("%x 节收缩成功成功！\n", PtrToPeHeader->SectionHeaderPtr);

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
};

//把Rva转换成Foa。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int RvaToFoa(unsigned int Rva, char* ImageBuffer)
{
	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
	if (!PtrToPeHeader)return 0;

	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		if ((Rva >= PtrToPeHeader->SectionHeaderPtr->VirtualAddress) && (Rva < PtrToPeHeader->SectionHeaderPtr->SizeOfRawData))
		{
			Offset = Rva - PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
			return PtrToPeHeader->SectionHeaderPtr->PointerToRawData + Offset;
		};

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 0;
};

//call和jmp偏移计算
void Shellcode_E8_E9_calculate(char* shellcode, int shellcodelength, int IndexOfSection, char* FileBuffer)
{
	int Offset = 0;
	char* AddressOfCurrent = NULL;
	char* AddressOfNextLine = NULL;

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	for (int i = 0; i < shellcodelength; i++)
	{
		if (shellcode[i] == (char)0xe8)
		{
			AddressOfCurrent = shellcode + i + 1;
			AddressOfNextLine = (char *)(PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].VirtualAddress + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize + i + 5);
			Offset = TargetFunciton - (int)AddressOfNextLine;
			*(int*)AddressOfCurrent = Offset;
		};

		if (shellcode[i] == (char)0xe9)
		{
			AddressOfCurrent = shellcode + i + 1;
			AddressOfNextLine = (char *)(PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].VirtualAddress + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize + i + 5);
			Offset = PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->OptionHeaderPtr->AddressOfEntryPoint - (int)AddressOfNextLine;
			*(int*)AddressOfCurrent = Offset;
		};
	};
	free(PtrToPeHeader);
};


//添加shellcode到目标节当中
BOOL AddShellcodeToTargetSection(int shellcodelength, int IndexOfSection, char*shellcode, char* FileBuffer)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	if (PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData < PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize)
	{
		printf("该节长度不够，不能添加shellcode！\n");
		return flag;
	};

	char* TargetAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize;

	//修正shellcode中的call和jmp偏移
	Shellcode_E8_E9_calculate(shellcode, shellcodelength, IndexOfSection, FileBuffer);

	//修改节属性
	PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Characteristics |= IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;

	//拷贝shellcode数据
	memcpy(TargetAddress, shellcode, shellcodelength);

	//修改.exe文件入口点
	*(unsigned long*)&(PtrToPeHeader->OptionHeaderPtr->AddressOfEntryPoint) = TargetAddress - FileBuffer;


	if (TargetAddress[0] == shellcode[0])
	{
		flag = 1;
	}
	free(PtrToPeHeader);
	return flag;
};

//添加shellcode到目标节当中,并形成exe文件
int AddCodeToExe(char* Filepath,int IndexOfSection)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);

	if (flag)printf("文件读取完成 %x\n", FileBuffer);

	flag=AddShellcodeToTargetSection(sizeof(shellcode) / sizeof(shellcode[0]), IndexOfSection, (char *)shellcode, FileBuffer);
	
	if (flag)printf("shellcode写入成功!\n");
	
	flag= FileBufferToFile(NULL, FileBuffer);

	//释放FileBuffer内存
	free(FileBuffer);

	if(flag)printf("文件写入成功!\n");

	return 0;
}

