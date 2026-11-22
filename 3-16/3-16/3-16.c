// 3-10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "16.h"


//文件拉伸：把文件从文件中的格式展开成内存中的格式
void FileToMemoryFunciton(char* FileBuffer,char* ImageBuffer)
{
	PIMAGE_DOS_HEADER FileDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;

	if (FileDosHeader->e_magic != 0x5A4D)
	{
		printf("该文件不是PE文件，请确认！\n");
		return NULL;
	};

	PIMAGE_NT_HEADERS  NtHeaders = (PIMAGE_NT_HEADERS)(FileBuffer + FileDosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x4550)
	{
		printf("该文件不是PE文件，请确认！\n");
		return NULL;
	};

	IMAGE_FILE_HEADER FileHeader = NtHeaders->FileHeader;

	IMAGE_OPTIONAL_HEADER OptionHeader = NtHeaders->OptionalHeader;


	PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)(FileBuffer + FileDosHeader->e_lfanew + 4 + 20 + FileHeader.SizeOfOptionalHeader);

	//头部复制
	memcpy(ImageBuffer, FileBuffer, OptionHeader.SizeOfHeaders);
	printf("文件头转移成功！\n");

	//节区拉伸
	for (int i = 0; i < FileHeader.NumberOfSections; i++)
	{
		
		char* TargetAddress = ImageBuffer + SectionHeader->VirtualAddress;
		
		char* SourceAddress = FileBuffer + SectionHeader->PointerToRawData;

		unsigned int Size = SectionHeader->SizeOfRawData;

		memcpy(TargetAddress, SourceAddress, Size);

		printf("%x 节拉伸成功成功！\n", SectionHeader);

		SectionHeader++;
		
	};
};

//文件收缩：把文件从内存中的格式收缩成文件中的格式
void MemoryToFileFunciton(char* FileBuffer, char* ImageBuffer)
{
	PIMAGE_DOS_HEADER FileDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;

	if (FileDosHeader->e_magic != 0x5A4D)
	{
		printf("该文件不是PE文件，请确认！\n");
		return NULL;
	};

	PIMAGE_NT_HEADERS  NtHeaders = (PIMAGE_NT_HEADERS)(ImageBuffer + FileDosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x4550)
	{
		printf("该文件不是PE文件，请确认！\n");
		return NULL;
	};

	IMAGE_FILE_HEADER FileHeader = NtHeaders->FileHeader;

	IMAGE_OPTIONAL_HEADER OptionHeader = NtHeaders->OptionalHeader;


	PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)(ImageBuffer + FileDosHeader->e_lfanew + 4 + 20 + FileHeader.SizeOfOptionalHeader);

	//头部复制
	memcpy(FileBuffer, ImageBuffer, OptionHeader.SizeOfHeaders);
	printf("文件头转移成功！\n");

	//节区收缩
	for (int i = 0; i < FileHeader.NumberOfSections; i++)
	{
		char* TargetAddress = FileBuffer + SectionHeader->PointerToRawData;

		char* SourceAddress = ImageBuffer + SectionHeader->VirtualAddress;
		
		unsigned int Size = SectionHeader->SizeOfRawData;

		memcpy(TargetAddress, SourceAddress, Size);

		printf("%x 节收缩成功成功！\n", SectionHeader);

		SectionHeader++;
	};
};

//把Rva转换成Foa。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int RvaToFoa(int Rva, char* ImageBuffer)
{
	int Offset = 0;
	PIMAGE_DOS_HEADER FileDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;

	if (FileDosHeader->e_magic != 0x5A4D)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeaders = (PIMAGE_NT_HEADERS)(ImageBuffer + FileDosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x4550)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	IMAGE_FILE_HEADER FileHeader = NtHeaders->FileHeader;

	IMAGE_OPTIONAL_HEADER OptionHeader = NtHeaders->OptionalHeader;

	PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)(ImageBuffer + FileDosHeader->e_lfanew + 4 + 20 + FileHeader.SizeOfOptionalHeader);

	for (int i = 0; i < FileHeader.NumberOfSections; i++)
	{
		if ((Rva>= SectionHeader->VirtualAddress)&& (Rva < SectionHeader->SizeOfRawData))
		{
			Offset = Rva - SectionHeader->VirtualAddress;
			return SectionHeader->PointerToRawData + Offset;
		};

		SectionHeader++;
	};

	return 0;
};

int GetPeHeaders(char* Filepath)
{
	//读取文件
	HANDLE hFile = CreateFile(Filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open file! Error: %lu\n", GetLastError());
		return 1;
	}

	//获取文件大小，其内部还是NtQueryInformationFile函数实现的
	DWORD fileSize = GetFileSize(hFile, NULL);

	if (fileSize == INVALID_FILE_SIZE)
	{
		printf("Failed to get file size! Error: %lu\n", GetLastError());
	}
	else
	{
		printf("File size: %lu bytes\n", fileSize);
	}

	//分配内存空间，用于存放文件中的数据
	char* Address = malloc(fileSize);
	memset(Address, 0, fileSize);
	int bytesRead = 0;

	//读取文件中的数据到我们分配的内存空间中
	ReadFile(hFile, Address, fileSize, &bytesRead, NULL);

	PIMAGE_DOS_HEADER FileDosHeader = (PIMAGE_DOS_HEADER)Address;

	if (FileDosHeader->e_magic != 0x5A4D)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeaders = (PIMAGE_NT_HEADERS)(Address + FileDosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x4550)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	IMAGE_FILE_HEADER FileHeader = NtHeaders->FileHeader;

	IMAGE_OPTIONAL_HEADER OptionHeader = NtHeaders->OptionalHeader;

	PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)(Address + FileDosHeader->e_lfanew + 4 + 20 + FileHeader.SizeOfOptionalHeader);

	//文件拉伸
	char* ImageBuffer = malloc(OptionHeader.SizeOfImage);
	memset(ImageBuffer, 0, OptionHeader.SizeOfImage);
	FileToMemoryFunciton(Address, ImageBuffer);

	//文件保存
	char* FileBuffer = malloc(fileSize);
	memset(FileBuffer, 0, fileSize);
	MemoryToFileFunciton(FileBuffer, ImageBuffer);
	HANDLE hFile1 = CreateFile(L"C:\\MyResetnotepad.exe", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile1, FileBuffer, fileSize, &bytesRead, NULL);

	//关闭文件句柄
	CloseHandle(hFile);
	CloseHandle(hFile1);

	return 0;
}


