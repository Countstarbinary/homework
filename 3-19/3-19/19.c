// 
//	本项目在写完之后，本机(win11)可以运行，x86不能运行，还找不到原因。
//  后来在x86环境下重写梳理，有许多vs2017可以通过的语法，在vc6++通过不了，修改后可以运行。
//  主要的问题是字符宽度的不同，导致读文件报2号错误
//	

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "19.h"

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

	if (hFile==INVALID_HANDLE_VALUE) printf("创建句柄失败！\n");

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
void Shellcode_E8_E9_calculateToTargetSection(char* shellcode, int shellcodelength, int IndexOfSection, char* FileBuffer)
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

//call和jmp偏移计算,注意索引IndexOfSection，不要超过节表数
void Shellcode_E8_E9_calculateToNewSection(char* shellcode, int shellcodelength, int IndexOfSection, char* FileBuffer)
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
			AddressOfNextLine = (char *)(PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].VirtualAddress + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData + i + 5);
			Offset = TargetFunciton - (int)AddressOfNextLine;
			*(int*)AddressOfCurrent = Offset;
		};

		if (shellcode[i] == (char)0xe9)
		{
			AddressOfCurrent = shellcode + i + 1;
			AddressOfNextLine = (char *)(PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].VirtualAddress + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData + i + 5);
			Offset = PtrToPeHeader->OptionHeaderPtr->ImageBase + PtrToPeHeader->OptionHeaderPtr->AddressOfEntryPoint - (int)AddressOfNextLine;
			*(int*)AddressOfCurrent = Offset;
		};
	};
	free(PtrToPeHeader);
};

//添加shellcode到目标节末尾当中
BOOL AddShellcodeToTargetSection(int shellcodelength, int IndexOfSection, char*shellcode, char* FileBuffer)
{
	char* TargetAddress = NULL;
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	if (PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData < PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize)
	{
		printf("该节长度不够，不能添加shellcode！\n");
		return flag;
	}

	TargetAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize;

	//修正shellcode中的call和jmp偏移
	Shellcode_E8_E9_calculateToTargetSection(shellcode, shellcodelength, IndexOfSection, FileBuffer);

	//修改节属性
	PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Characteristics |= IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE| IMAGE_SCN_CNT_CODE;

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

//添加shellcode到新增节当中
BOOL AddShellcodeToNewSection(int shellcodelength, int IndexOfSection, char*shellcode, char* FileBuffer)
{
	char* TargetAddress = NULL;
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	if (PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData < PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize)
	{
		printf("该节长度不够，不能添加shellcode！\n");
		TargetAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Misc.VirtualSize;

	}
	else
	{
		TargetAddress = FileBuffer + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[IndexOfSection].SizeOfRawData;
	};


	//修正shellcode中的call和jmp偏移
	Shellcode_E8_E9_calculateToNewSection(shellcode, shellcodelength, IndexOfSection, FileBuffer);

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
	if (PtrToPeHeader != NULL) 
	{
		free(PtrToPeHeader);
	}
	return flag;
};

//添加shellcode到目标节当中,并形成exe文件
int AddCodeToExe(char* Filepath, int IndexOfSection)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);

	if (flag)printf("文件读取完成 %x\n", FileBuffer);

	flag = AddShellcodeToTargetSection(sizeof(shellcode) / sizeof(shellcode[0]), IndexOfSection, (char *)shellcode, FileBuffer);

	if (flag)printf("shellcode写入成功!\n");

	flag = FileBufferToFile(NULL, FileBuffer);

	//释放FileBuffer内存
	free(FileBuffer);

	if (flag)printf("文件写入成功!\n");

	return 0;
}

//扩展最后一个节
BOOL ExpandLastSection(char* FileBuffer,int ExpandLength)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return flag;


	//获取最后一个节，并进行修改
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	LastSection->SizeOfRawData += ExpandLength;
	//这条必须要加上，不能只加SizeOfRawData，SizeOfRawData反而是次要的。在这里花了2个小时，老是在莫名奇妙的地方浪费大量时间
	LastSection->Misc.VirtualSize += ExpandLength;  

	//修改sizeofimage大小
	PtrToPeHeader->OptionHeaderPtr->SizeOfImage += ExpandLength;

	//在FileBuffer中新增加0x1000的长度
	char* NewFileBuffer = malloc(PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	memcpy(NewFileBuffer, FileBuffer, PtrToPeHeader->OptionHeaderPtr->SizeOfImage - ExpandLength);


	//这里容易错，注意与新增节有所不同
	int numberofsection = PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//在新节中加入数据
	flag = AddShellcodeToTargetSection(sizeof(shellcode) / sizeof(shellcode[0]), numberofsection, (char *)shellcode, NewFileBuffer);
	if (!flag)printf("shellcode写入失败!\n");

	//新NewFileBuffer写入文件
	flag=FileBufferToFile(NULL, NewFileBuffer);
	if (!flag)printf("文件生成失败!\n");
	
	//释放内存
	free(FileBuffer);
	free(NewFileBuffer);
	free(PtrToPeHeader);
	return flag;
};

//在最后一个节后新增一个节
BOOL AddNewSectionToFileEnd(char* FileBuffer, int NewSectionLength)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return flag;
	
	//计算所有头占有的
	DWORD SizeofAllheader=PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections;
	//计算头剩下的字节
	DWORD ByteofSurplus= (DWORD)FileBuffer+PtrToPeHeader->OptionHeaderPtr->SizeOfHeaders - SizeofAllheader;
	
	//如果头的剩余字节小于两个节表的长度，或者最后一个节后有数据，如notepad，就扩张最后一个节
	if (ByteofSurplus<=80|| (*(int*)SizeofAllheader != 0))
	{
		printf("header空间不足，进行节扩展！\n");
		ExpandLastSection(FileBuffer, NewSectionLength);
	};

	//获取最后一个节，用于初始化新的节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections-1;
	
	//计算最后一个节的实际大小
	DWORD SizeofLastSection = 0;
	if (LastSection->SizeOfRawData > LastSection->Misc.VirtualSize) 
	{
		SizeofLastSection = LastSection->SizeOfRawData;
	}
	else
	{
		SizeofLastSection = LastSection->Misc.VirtualSize;
	};
	

	//初始化新的节
	PIMAGE_SECTION_HEADER NewSection = malloc(sizeof(IMAGE_SECTION_HEADER));
	
	if (!NewSection)return flag;

	memset(NewSection,0, sizeof(IMAGE_SECTION_HEADER));
	memcpy(NewSection->Name, "NewSection", 8);
	NewSection->Misc.VirtualSize = NewSectionLength;
	NewSection->VirtualAddress = LastSection->VirtualAddress + SizeofLastSection;
	NewSection->SizeOfRawData = NewSectionLength;
	NewSection->PointerToRawData = LastSection->PointerToRawData + LastSection->SizeOfRawData;
	NewSection->PointerToRelocations = 0;
	NewSection->PointerToLinenumbers = 0;
	NewSection->NumberOfRelocations = 0;
	NewSection->NumberOfLinenumbers = 0;
	NewSection->Characteristics = 0x60000020;

	//新的节写入节表
	memcpy((PIMAGE_SECTION_HEADER)SizeofAllheader, NewSection, sizeof(IMAGE_SECTION_HEADER));

	//修改pe头中节数量
	PtrToPeHeader->FileHeaderPtr->NumberOfSections += 1;

	//修改sizeofimage大小
	PtrToPeHeader->OptionHeaderPtr->SizeOfImage += NewSectionLength;

	//在FileBuffer中新增加0x1000的长度
	char* NewFileBuffer=malloc(PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	memcpy(NewFileBuffer, FileBuffer, PtrToPeHeader->OptionHeaderPtr->SizeOfImage- NewSectionLength);

	//这里容易错，注意
	DWORD numberofsection = PtrToPeHeader->FileHeaderPtr->NumberOfSections - 2;

	//在新节中加入数据
	flag=AddShellcodeToNewSection(sizeof(shellcode) / sizeof(shellcode[0]), numberofsection, (char *)shellcode, NewFileBuffer);
	if (!flag)return flag;

	//新NewFileBuffer写入文件
	flag=FileBufferToFile(NULL, NewFileBuffer);
	
	//释放旧FileBuffer内存
	free(FileBuffer);
	free(NewFileBuffer);
	free(PtrToPeHeader);
	free(NewSection);

	return flag;
};

//添加一个节后到最后一个节,并形成exe文件
BOOL AddNewSectionToExe(char* Filepath, int NewSectionLength)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);
	flag = ExpandLastSection(FileBuffer, NewSectionLength);
	//flag = AddNewSectionToFileEnd(FileBuffer, NewSectionLength);
	if (flag)printf("新增节完成！\n");
	return flag;
};