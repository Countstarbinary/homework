#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"


#define TargetFunciton 0x7754ea29


BYTE shellcode[] = {
	0x6A,0x00,0x6A,0x00,0x6A,0x00,0x6A,0x00,
	0xE8,0x00, 0x00, 0x00, 0x00,
	0xE9,0x00, 0x00, 0x00, 0x00, };


void OutputDosHeader(PIMAGE_DOS_HEADER FileDosHeader)
{
	printf("---------------FileDosHeader_start-------------------\n");
	printf("---------------e_magic:%x-------------------\n", FileDosHeader->e_magic);
	printf("---------------e_cblp:%x-------------------\n", FileDosHeader->e_cblp);
	printf("---------------e_cp:%x-------------------\n", FileDosHeader->e_cp);
	printf("---------------e_crlc:%x-------------------\n", FileDosHeader->e_crlc);
	printf("---------------e_cparhdr:%x-------------------\n", FileDosHeader->e_cparhdr);
	printf("---------------e_minalloc:%x-------------------\n", FileDosHeader->e_minalloc);
	printf("---------------e_maxalloc:%x-------------------\n", FileDosHeader->e_maxalloc);
	printf("---------------e_cs:%x-------------------\n", FileDosHeader->e_cs);
	printf("---------------e_sp:%x-------------------\n", FileDosHeader->e_sp);
	printf("---------------e_csum:%x-------------------\n", FileDosHeader->e_csum);
	printf("---------------e_ip:%x-------------------\n", FileDosHeader->e_ip);
	printf("---------------e_cs:%x-------------------\n", FileDosHeader->e_cs);
	printf("---------------e_lfarlc:%x-------------------\n", FileDosHeader->e_lfarlc);
	printf("---------------e_ovno:%x-------------------\n", FileDosHeader->e_ovno);
	printf("---------------e_res:%hx %hx %hx %hx-------------------\n", FileDosHeader->e_res[0], FileDosHeader->e_res[1], FileDosHeader->e_res[2], FileDosHeader->e_res[3]);
	printf("---------------e_oemid:%x-------------------\n", FileDosHeader->e_oemid);
	printf("---------------e_oeminfo:%x-------------------\n", FileDosHeader->e_oeminfo);
	printf("---------------e_res2:%hx %hx %hx %hx %hx %hx %hx %hx %hx %hx -------------------\n",
		FileDosHeader->e_res2[0], FileDosHeader->e_res2[1], FileDosHeader->e_res2[2],
		FileDosHeader->e_res2[3], FileDosHeader->e_res2[4], FileDosHeader->e_res2[5],
		FileDosHeader->e_res2[6], FileDosHeader->e_res2[7], FileDosHeader->e_res2[8], FileDosHeader->e_res2[9]);
	printf("---------------e_lfanew:%x-------------------\n", FileDosHeader->e_lfanew);
	printf("---------------FileDosHeader_end-------------------\n");
	printf("\n");
};

void OutputFileHeader(PIMAGE_FILE_HEADER FileHeader)
{
	printf("---------------FileHeader_start-------------------\n");
	printf("---------------Machine:%x-------------------\n", FileHeader->Machine);
	printf("---------------NumberOfSections:%x-------------------\n", FileHeader->NumberOfSections);
	printf("---------------TimeDateStamp:%x-------------------\n", FileHeader->TimeDateStamp);
	printf("---------------PointerToSymbolTable:%x-------------------\n", FileHeader->PointerToSymbolTable);
	printf("---------------NumberOfSymbols:%x-------------------\n", FileHeader->NumberOfSymbols);
	printf("---------------SizeOfOptionalHeader:%x-------------------\n", FileHeader->SizeOfOptionalHeader);
	printf("---------------Characteristics:%x-------------------\n", FileHeader->Characteristics);
	printf("---------------FileHeader_end-------------------\n");
	printf("\n");
};

void OutputOptionHeader(PIMAGE_OPTIONAL_HEADER OptionHeader)
{
	printf("---------------OptionHeader_start-------------------\n");
	printf("---------------Magic:%x-------------------\n", OptionHeader->Magic);
	printf("---------------MajorLinkerVersion:%x-------------------\n", OptionHeader->MajorLinkerVersion);
	printf("---------------MinorLinkerVersion:%x-------------------\n", OptionHeader->MinorLinkerVersion);
	printf("---------------SizeOfCode:%x-------------------\n", OptionHeader->SizeOfCode);
	printf("---------------SizeOfInitializedData:%x-------------------\n", OptionHeader->SizeOfInitializedData);
	printf("---------------SizeOfUninitializedData:%x-------------------\n", OptionHeader->SizeOfUninitializedData);
	printf("---------------SizeOfUninitializedData:%x-------------------\n", OptionHeader->AddressOfEntryPoint);
	printf("---------------BaseOfCode:%x-------------------\n", OptionHeader->BaseOfCode);
	//printf("---------------BaseOfData:%x-------------------\n", OptionHeader->BaseOfData);
	printf("---------------ImageBase:%x-------------------\n", OptionHeader->ImageBase);
	printf("---------------SectionAlignment:%x-------------------\n", OptionHeader->SectionAlignment);
	printf("---------------FileAlignment:%x-------------------\n", OptionHeader->FileAlignment);
	printf("---------------MajorOperatingSystemVersion:%x-------------------\n", OptionHeader->MajorOperatingSystemVersion);
	printf("---------------MinorOperatingSystemVersion:%x-------------------\n", OptionHeader->MinorOperatingSystemVersion);
	printf("---------------MajorImageVersion:%x-------------------\n", OptionHeader->MajorImageVersion);
	printf("---------------MinorImageVersion:%x-------------------\n", OptionHeader->MinorImageVersion);
	printf("---------------MajorSubsystemVersion:%x-------------------\n", OptionHeader->MajorSubsystemVersion);
	printf("---------------MinorSubsystemVersion:%x-------------------\n", OptionHeader->MinorSubsystemVersion);
	printf("---------------Win32VersionValue:%x-------------------\n", OptionHeader->Win32VersionValue);
	printf("---------------SizeOfImage:%x-------------------\n", OptionHeader->SizeOfImage);
	printf("---------------SizeOfHeaders:%x-------------------\n", OptionHeader->SizeOfHeaders);
	printf("---------------CheckSum:%x-------------------\n", OptionHeader->CheckSum);
	printf("---------------Subsystem:%x-------------------\n", OptionHeader->Subsystem);
	printf("---------------DllCharacteristics:%x-------------------\n", OptionHeader->DllCharacteristics);
	printf("---------------SizeOfStackReserve:%x-------------------\n", OptionHeader->SizeOfStackReserve);
	printf("---------------SizeOfStackCommit:%x-------------------\n", OptionHeader->SizeOfStackCommit);
	printf("---------------SizeOfHeapReserve:%x-------------------\n", OptionHeader->SizeOfHeapReserve);
	printf("---------------SizeOfHeapCommit:%x-------------------\n", OptionHeader->SizeOfHeapCommit);
	printf("---------------LoaderFlags:%x-------------------\n", OptionHeader->LoaderFlags);
	printf("---------------NumberOfRvaAndSizes:%x-------------------\n", OptionHeader->NumberOfRvaAndSizes);
	printf("---------------OptionHeader_end-------------------\n", OptionHeader);
	printf("\n");
};

void OutputSectionHeader(PIMAGE_SECTION_HEADER SectionHeader, int NumberOfSections)
{
	for (int j = 0; j < NumberOfSections; j++)
	{
		char name[9] = { 0 };
		for (int i = 0; i < 8; i++)
		{
			name[i] = SectionHeader->Name[i];
		};

		char r = 0;
		char w = 0;
		char e = 0;
		char shared = 0;
		printf("---------------SectionHeader_start-------------------\n");
		printf("---------------Name:%s-------------------\n", name);
		printf("---------------VirtualSize:%x-------------------\n", SectionHeader->Misc);
		printf("---------------VirtualAddress:%x-------------------\n", SectionHeader->VirtualAddress);
		printf("---------------SizeOfRawData:%x-------------------\n", SectionHeader->SizeOfRawData);
		printf("---------------PointerToRawData:%x-------------------\n", SectionHeader->PointerToRawData);
		printf("---------------PointerToRelocations:%x-------------------\n", SectionHeader->PointerToRelocations);
		printf("---------------PointerToLinenumbers:%x-------------------\n", SectionHeader->PointerToLinenumbers);
		printf("---------------NumberOfRelocations:%x-------------------\n", SectionHeader->NumberOfRelocations);
		printf("---------------NumberOfLinenumbers:%x-------------------\n", SectionHeader->NumberOfLinenumbers);

		if (SectionHeader->Characteristics & 0xF0000000)
		{
			if ((SectionHeader->Characteristics >> 28) & 1)shared = 1;
			if ((SectionHeader->Characteristics >> 29) & 1)e = 1;
			if ((SectionHeader->Characteristics >> 30) & 1)w = 1;
			if ((SectionHeader->Characteristics >> 31) & 1)r = 1;
			printf("---------------Characteristics:%x r:%d,w:%d,e:%d,shared:%d,-------------------\n",
				SectionHeader->Characteristics, r, w, e, shared);
		}
		else
		{
			printf("---------------Characteristics:%x-------------------\n", SectionHeader->Characteristics);
		};

		printf("---------------SectionHeader_end-------------------\n");
		printf("\n");

		SectionHeader++;
	};

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
	OutputDosHeader(FileDosHeader);
	IMAGE_FILE_HEADER FileHeader = NtHeaders->FileHeader;
	OutputFileHeader(&FileHeader);
	IMAGE_OPTIONAL_HEADER OptionHeader = NtHeaders->OptionalHeader;
	OutputOptionHeader(&OptionHeader);

	PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)(Address + FileDosHeader->e_lfanew + 4 + 20 + FileHeader.SizeOfOptionalHeader);
	OutputSectionHeader(SectionHeader, FileHeader.NumberOfSections);

	//关闭文件句柄
	CloseHandle(hFile);


	return 0;
}

//文件拉伸：把文件从文件中的格式展开成内存中的格式
void FileToMemoryFunciton(char* FileBuffer, char* ImageBuffer)
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



	PVOID OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;
	PIMAGE_SECTION_HEADER SectionHeaderPtr = (PIMAGE_SECTION_HEADER)(FileBuffer + FileDosHeaderPtr->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER + FileHeaderPtr->SizeOfOptionalHeader);


	PPeAllHeaders HeadersPtr = (PPeAllHeaders)malloc(sizeof(PeAllHeaders));
	HeadersPtr->FileDosHeaderPtr = FileDosHeaderPtr;
	HeadersPtr->NtHeadersPtr = NtHeadersPtr;
	HeadersPtr->FileHeaderPtr = FileHeaderPtr;
	HeadersPtr->u.OptionHeaderPtr = OptionHeaderPtr;
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

//把FileBuffer写回硬盘
BOOL FileBufferToFile(char* FilePath, char* FileBuffer)
{
	HANDLE hFile = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

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

//文件拉伸：把文件从文件中的格式展开成内存中的格式
void FileBufferToImageBuffer(char* FileBuffer, char* ImageBuffer)
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
};

//文件收缩：把文件从内存中的格式收缩成文件中的格式
void ImageBufferToFileBuffer(char* FileBuffer, char* ImageBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(ImageBuffer);
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

//把Foa转换成Rva。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int FoaToRva(unsigned int Foa, char* FileBuffer)
{
	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;


	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		//DWORD MAX = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData > PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize ? PtrToPeHeader->SectionHeaderPtr->SizeOfRawData : PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize;
		DWORD PointerToRaw = PtrToPeHeader->SectionHeaderPtr->PointerToRawData;
		if ((Foa >= PointerToRaw) && (Foa < PointerToRaw + PtrToPeHeader->SectionHeaderPtr->SizeOfRawData))
		{
			Offset = Foa - PtrToPeHeader->SectionHeaderPtr->PointerToRawData;
			return PtrToPeHeader->SectionHeaderPtr->VirtualAddress + Offset;
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
	PtrToPeHeader->SectionHeaderPtr[IndexOfSection].Characteristics |= IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE;

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

//合并所有节为一个节
BOOL MergeAllSection(char* Filepath)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return flag;

	//文件拉伸
	char* ImageBuffer = malloc(PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	if (!ImageBuffer)return 0;
	memset(ImageBuffer, 0, PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	FileBufferToImageBuffer(FileBuffer, ImageBuffer);


	//修改新的节表
	PIMAGE_SECTION_HEADER FirstSection = PtrToPeHeader->SectionHeaderPtr;

	if (!FirstSection)return flag;

	DWORD SizeofNewSection = PtrToPeHeader->OptionHeaderPtr->SizeOfImage - FirstSection->VirtualAddress;

	//计算新节的长度，两种方法都可以，而且有时候程序运行不了不是自己的问题，是编译器的问题。
	//一开始用上面的计算方式运行不了，用下面的计算方式可以运行。后来用上面的计算方法也可以。真是毛病

	//获取最后一个节，用于初始化新的节
	//PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;
	//if (LastSection->Misc.VirtualSize > LastSection->SizeOfRawData) 
	//{
	//	SizeofNewSection = LastSection->VirtualAddress + LastSection->Misc.VirtualSize - FirstSection->VirtualAddress;
	//}
	//else 
	//{
	//	SizeofNewSection = LastSection->VirtualAddress + LastSection->SizeOfRawData - FirstSection->VirtualAddress;
	//};

	FirstSection->SizeOfRawData = SizeofNewSection;
	FirstSection->Misc.VirtualSize = SizeofNewSection;

	//将第一个节的属性改为包含所有节的属性。属性必须要修改，否则不能运行
	for (int i = 1; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		FirstSection->Characteristics |= (PtrToPeHeader->SectionHeaderPtr + i)->Characteristics;
	};

	//修改节数量
	PtrToPeHeader->FileHeaderPtr->NumberOfSections = 1;

	//重新进行头部复制
	memcpy(ImageBuffer, FileBuffer, PtrToPeHeader->OptionHeaderPtr->SizeOfHeaders);
	printf("文件头转移成功！\n");

	//合并后的文件写入NewFileBuffer
	char* NewFileBuffer = malloc(PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, PtrToPeHeader->OptionHeaderPtr->SizeOfImage);
	ImageBufferToFileBuffer(NewFileBuffer, ImageBuffer);

	//写入文件
	flag = FileBufferToFile(NULL, NewFileBuffer);

	//释放内存
	free(FileBuffer);
	free(NewFileBuffer);
	free(ImageBuffer);
	free(PtrToPeHeader);
	return flag;
};

//输出全部目录项
void OutPutAllDirtory(char* FileBuffer)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return flag;

	//注意"IMAGE_DIRECTORY_ENTRY_EXPORT"形式字符串，本身就是一个指针，只占据一个成员的位置。
	//char arr[],才是局部变量。若想在此处使用二维数组，注意要用后者。更进一步想要进行骚操作数组指针，也是用后者
	char* name[16] = {
	"IMAGE_DIRECTORY_ENTRY_EXPORT","IMAGE_DIRECTORY_ENTRY_IMPORT","IMAGE_DIRECTORY_ENTRY_RESOURCE","IMAGE_DIRECTORY_ENTRY_EXCEPTION","IMAGE_DIRECTORY_ENTRY_SECURITY",
	"IMAGE_DIRECTORY_ENTRY_BASERELOC","IMAGE_DIRECTORY_ENTRY_DEBUG","IMAGE_DIRECTORY_ENTRY_ARCHITECTURE","IMAGE_DIRECTORY_ENTRY_GLOBALPTR","IMAGE_DIRECTORY_ENTRY_TLS",
	"IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG","IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT","IMAGE_DIRECTORY_ENTRY_IAT","IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT","IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR" };

	//"IMAGE_DIRECTORY_ENTRY_RELOCATION" *((int*)name + i)
	for (int i = 0; i < PtrToPeHeader->OptionHeaderPtr->NumberOfRvaAndSizes; i++)
	{
		printf("-----------------------------------%s-----------------------------------------\n", name[i]);
		printf("%x\n", PtrToPeHeader->OptionHeaderPtr->DataDirectory[i].VirtualAddress);
		printf("%x\n", PtrToPeHeader->OptionHeaderPtr->DataDirectory[i].Size);
		printf("-----------------------------------%s-----------------------------------------\n", name[i]);
	};

	//释放内存
	free(PtrToPeHeader);

};

//扩展最后一个节
BOOL ExpandLastSection(char* FileBuffer, int ExpandLength)
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
	PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage += ExpandLength;

	//在FileBuffer中新增加0x1000的长度
	char* NewFileBuffer = malloc(PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage);
	memcpy(NewFileBuffer, FileBuffer, PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage - ExpandLength);


	//这里容易错，注意与新增节有所不同
	int numberofsection = PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//在新节中加入数据
	//flag = AddShellcodeToTargetSection(sizeof(shellcode) / sizeof(shellcode[0]), numberofsection, (char *)shellcode, NewFileBuffer);
	//if (!flag)printf("shellcode写入失败!\n");

	//新NewFileBuffer写入文件
	flag = FileBufferToFile(NULL, NewFileBuffer);
	if (!flag)printf("文件生成失败!\n");

	//释放内存
	free(FileBuffer);
	free(NewFileBuffer);
	free(PtrToPeHeader);
	return flag;
};

//如果头部空间不够节表的大小，提升头。返回头提升后，空出的字节数
DWORD HeaderMove(PPeAllHeaders* PtrToPeHeader)
{
	PPeAllHeaders PeHeader = *PtrToPeHeader;
	DWORD ByteofSurplus = 0;
	char*firstchar = (char *)PeHeader->NtHeadersPtr;
	char *lastchar = (char *)(PeHeader->SectionHeaderPtr + PeHeader->FileHeaderPtr->NumberOfSections);
	ByteofSurplus = PeHeader->FileDosHeaderPtr->e_lfanew - 0x40;

	PeHeader->FileDosHeaderPtr->e_lfanew = 0x40;

	char* temp = (char*)malloc(lastchar - firstchar);

	memset(temp, 0, lastchar - firstchar);

	memcpy(temp, firstchar, lastchar - firstchar);

	memset(firstchar, 0, lastchar - firstchar);

	memcpy((char*)PeHeader->FileDosHeaderPtr + 0x40, temp, lastchar - firstchar);


	//获取新的头结构体
	char* FileBuffer = (char*)PeHeader->FileDosHeaderPtr;
	BOOL flag = 0;
	free(PeHeader);
	PeHeader = NULL;

	*PtrToPeHeader = GetPeAllHeaders(FileBuffer);

	if (!PeHeader)printf("头提升后，重新获取节失败！\n");

	free(temp);


	return ByteofSurplus;
};

//在最后一个节后新增一个节，返回增加节后的buffer
char* AddNewSectionToFileEnd(char* FileBuffer, int NewSectionLength)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);


	if (!PtrToPeHeader)return flag;
	if (sizeof(char*) == 8)
	{
		//计算所有头占有的
		unsigned __int64 SizeofAllheader = 0;
		//计算头剩下的字节
		unsigned __int64 ByteofSurplus = 0;

		//如果头的剩余字节小于两个节表的长度，或者最后一个节后有数据，如notepad，就扩张最后一个节
		if (ByteofSurplus <= 80 || (*(unsigned __int64*)SizeofAllheader != 0))
		{
			printf("header空间不足，头提升！\n");
			ByteofSurplus += HeaderMove(&PtrToPeHeader);

			//如果头提升后空间还是不足，进行节的扩展
			if (ByteofSurplus <= 80)
			{
				printf("header空间不足，进行节扩展！\n");
				flag = ExpandLastSection(FileBuffer, NewSectionLength);
				return flag;
			};
		};
	}
	else
	{
		//计算所有头占有的
		DWORD SizeofAllheader = 0;
		//计算头剩下的字节
		DWORD ByteofSurplus = 0;


		//计算所有头占有的
		SizeofAllheader = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections;

		//计算头剩下的字节
		ByteofSurplus = (DWORD)FileBuffer + PtrToPeHeader->u.OptionHeaderPtr->SizeOfHeaders - SizeofAllheader;

		//如果头的剩余字节小于两个节表的长度，或者最后一个节后有数据，如notepad，就扩张最后一个节
		if (ByteofSurplus <= 80 || (*(int*)SizeofAllheader != 0))
		{
			printf("header空间不足，头提升！\n");
			ByteofSurplus += HeaderMove(&PtrToPeHeader);

			//如果头提升后空间还是不足，进行节的扩展
			if (ByteofSurplus <= 80)
			{
				printf("header空间不足，进行节扩展！\n");
				flag = ExpandLastSection(FileBuffer, NewSectionLength);
				return flag;
			};
		};
	};

	//获取最后一个节，用于初始化新的节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//计算文件长度
	DWORD FileLength = LastSection->PointerToRawData + LastSection->SizeOfRawData;
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

	memset(NewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	memcpy(NewSection->Name, "NewSection", 8);
	NewSection->Misc.VirtualSize = NewSectionLength;
	//这里需要内存对齐的长度
	NewSection->VirtualAddress = Align(LastSection->VirtualAddress + LastSection->SizeOfRawData, PtrToPeHeader->u.OptionHeaderPtr->SectionAlignment);

	NewSection->SizeOfRawData = NewSectionLength;
	NewSection->PointerToRawData = LastSection->PointerToRawData + LastSection->SizeOfRawData;
	NewSection->PointerToRelocations = 0;
	NewSection->PointerToLinenumbers = 0;
	NewSection->NumberOfRelocations = 0;
	NewSection->NumberOfLinenumbers = 0;
	NewSection->Characteristics = 0x60000020;

	//新的节写入节表
	memcpy((PIMAGE_SECTION_HEADER)(PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections), NewSection, sizeof(IMAGE_SECTION_HEADER));

	//修改pe头中节数量
	PtrToPeHeader->FileHeaderPtr->NumberOfSections += 1;

	//修改sizeofimage大小
	PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage += NewSectionLength;

	//在FileBuffer中新增加0x1000的长度。这里需要文件长度，不是sizeofimage，之前写错了，现已修改
	char* NewFileBuffer = malloc(FileLength + NewSectionLength);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, FileLength + NewSectionLength);
	memcpy(NewFileBuffer, FileBuffer, FileLength);



	//释放旧FileBuffer内存
	if (FileBuffer)free(FileBuffer);
	//free(NewFileBuffer);
	if (PtrToPeHeader)free(PtrToPeHeader);
	if (NewSection)free(NewSection);

	return NewFileBuffer;
};

//添加一个节后到最后一个节,并形成exe文件
BOOL AddNewSectionToExe(char* Filepath, char* NewFilepath, int NewSectionLength)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);
	//flag = ExpandLastSection(FileBuffer, NewSectionLength);
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, NewSectionLength);
	if (NewFileBuffer)printf("新增节完成！\n");

	flag = FileBufferToFile(NewFilepath, NewFileBuffer);
	if (NewFileBuffer)printf("新增节，文件写入成功！\n");

	return flag;
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


//移动导出表，返回移动后的buffer
char* MoveExportDirtory(char* FileBuffer)
{
	BOOL flag = 0;

	//新增加节
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, 0x1000);
	if (!NewFileBuffer)printf("导出表移位-> 新增节失败！\n");

	//获取pe文件头
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(NewFileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	}
	else
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	};


	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, NewFileBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(NewFileBuffer + ExportDirtoryFoa);

	//获取最后一个节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//最后一个节，从头开始分别放函数表，名称表，序号表，和名字
	char*NewAddressOfFunciton = NewFileBuffer + LastSection->PointerToRawData;
	char*NewAddressOfName = NewAddressOfFunciton + ExportDirtory->NumberOfFunctions * 4;
	char*NewAddressOfNameordnial = NewAddressOfName + ExportDirtory->NumberOfNames * 4;
	char*NewName = NewAddressOfNameordnial + ExportDirtory->NumberOfNames * 2;


	DWORD AddressOfNamesFoa = 0;
	DWORD AddressOfFunctionsFoa = 0;
	DWORD AddressOfNameOrdinalsFoa = 0;

	PDWORD AddressOfNamesFileBuffer = NULL;
	PDWORD AddressOfFunctionsFileBuffer = NULL;
	PWORD AddressOfNameOrdinalsFileBuffer = NULL;

	PDWORD AddressOfFuncitonName = NULL;
	DWORD FunctionVirtualAddress = NULL;
	//获取名称表在FileBuffer中的地址
	AddressOfNamesFoa = RvaToFoa(ExportDirtory->AddressOfNames, NewFileBuffer);
	AddressOfNamesFileBuffer = NewFileBuffer + AddressOfNamesFoa;

	//获取地址表在FileBuffer中的地址
	AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, NewFileBuffer);
	AddressOfFunctionsFileBuffer = NewFileBuffer + AddressOfFunctionsFoa;

	//获取序号表在FileBuffer中的地址
	AddressOfNameOrdinalsFoa = RvaToFoa(ExportDirtory->AddressOfNameOrdinals, NewFileBuffer);
	AddressOfNameOrdinalsFileBuffer = NewFileBuffer + AddressOfNameOrdinalsFoa;



	//复制3张表
	memcpy(NewAddressOfFunciton, AddressOfFunctionsFileBuffer, ExportDirtory->NumberOfFunctions * 4);
	memcpy(NewAddressOfName, AddressOfNamesFileBuffer, ExportDirtory->NumberOfNames * 4);
	memcpy(NewAddressOfNameordnial, AddressOfNameOrdinalsFileBuffer, ExportDirtory->NumberOfNames * 2);

	int FuncitonNameLength = 0;
	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称在FileBuffer中的地址
		AddressOfFuncitonName = NewFileBuffer + RvaToFoa(AddressOfNamesFileBuffer[i], NewFileBuffer);
		//获取函数名长度
		FuncitonNameLength = strlen(AddressOfFuncitonName) + 1;
		//函数名复制
		memcpy(NewName, AddressOfFuncitonName, FuncitonNameLength);

		//函数表修复
		*AddressOfFunctionsFileBuffer = FoaToRva((DWORD)AddressOfFuncitonName - (DWORD)NewFileBuffer, NewFileBuffer);

		//名称表修复
		*AddressOfNamesFileBuffer = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);

		AddressOfFunctionsFileBuffer++;
		AddressOfNamesFileBuffer++;
		NewName += FuncitonNameLength;
	};



	//修复IMAGE_EXPORT_DIRECTORY结构
	ExportDirtory->AddressOfFunctions = FoaToRva(NewAddressOfFunciton - NewFileBuffer, NewFileBuffer);
	ExportDirtory->AddressOfNames = FoaToRva(NewAddressOfName - NewFileBuffer, NewFileBuffer);
	ExportDirtory->AddressOfNameOrdinals = FoaToRva(NewAddressOfNameordnial - NewFileBuffer, NewFileBuffer);


	//修复DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);
	}
	else
	{
		PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);
	};
	//复制IMAGE_EXPORT_DIRECTORY结构
	memcpy(NewName, ExportDirtory, sizeof(IMAGE_EXPORT_DIRECTORY));


	free(PtrToPeHeader);
	return NewFileBuffer;
};

//移动重定位表，返回移动后的buffer.r如果需要修改imagebase，就传入进去
char* MoveRelocationDirtory(char* FileBuffer, int NewImagebase)
{
	BOOL flag = 0;
	PWORD RelocationEntryItem = 0;


	//新增加节
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, 0x1000);
	if (!NewFileBuffer)printf("导出表移位-> 新增节失败！\n");

	//获取pe文件头
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(NewFileBuffer);
	if (!PtrToPeHeader)return 0;

	//修复重定位项,若有必要
	DWORD ImageBase = 0;
	if (NewImagebase)
	{
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr->ImageBase;
		PtrToPeHeader->u.OptionHeaderPtr->ImageBase = NewImagebase;
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


	//重定位表入口 RvaToFoa
	DWORD RelocationEntryFoa = RvaToFoa(RelocationEntry.VirtualAddress, NewFileBuffer);

	PIMAGE_BASE_RELOCATION RelocationDirtory = (PIMAGE_BASE_RELOCATION)(NewFileBuffer + RelocationEntryFoa);

	//获取最后一个节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//移动导出表
	char* Newaddress = Newaddress = NewFileBuffer + LastSection->PointerToRawData;
	int* virtadd = 0;
	while (1)
	{
		RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
		//修复重定位表,若有必要
		if (NewImagebase)
		{
			for (int i = 0; i < (RelocationDirtory->SizeOfBlock - 8) / 2; i++)
			{
				//判断是否是需要重定位的项 
				if (((*RelocationEntryItem) & 0x3000) == 0x3000)
				{
					//计算rva。注意数据类型要进行强转，否则计算失败
					virtadd = (int*)(NewFileBuffer + RvaToFoa(RelocationDirtory->VirtualAddress + (DWORD)((*RelocationEntryItem) & 0xFFF), NewFileBuffer));

					*virtadd = *virtadd - ImageBase + PtrToPeHeader->u.OptionHeaderPtr32->ImageBase;

				};

				//重定位项更新
				RelocationEntryItem++;
			};
		};

		//移动重定位表
		memcpy(Newaddress, RelocationDirtory, RelocationDirtory->SizeOfBlock);

		//更新重定位表  
		RelocationDirtory = (PIMAGE_BASE_RELOCATION)((DWORD)RelocationDirtory + RelocationDirtory->SizeOfBlock);
		Newaddress += RelocationDirtory->SizeOfBlock;
		if ((RelocationDirtory->VirtualAddress == 0) && (RelocationDirtory->SizeOfBlock == 0))
		{
			break;
		};
	};
	free(PtrToPeHeader);
	return NewFileBuffer;
};

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
	
	if (!RelocationEntry.Size)
	{
		return 1;
	};
	//重定位表入口 RvaToFoa
	DWORD RelocationEntryFoa = RvaToFoa(RelocationEntry.VirtualAddress, FileBuffer);

	PIMAGE_BASE_RELOCATION RelocationDirtory = (PIMAGE_BASE_RELOCATION)(FileBuffer + RelocationEntryFoa);



	//移动导出表
	int* virtadd = 0;
	RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
	//修复重定位表,若有必要
	while (1)
	{
		for (int i = 0; i < (RelocationDirtory->SizeOfBlock - 8) / 2; i++)
		{
			//判断是否是需要重定位的项 
			if (((*RelocationEntryItem) & 0x3000) == 0x3000)
			{
				//计算rva。注意数据类型要进行强转，否则计算失败
				virtadd = (int*)(FileBuffer + RvaToFoa(RelocationDirtory->VirtualAddress + (DWORD)((*RelocationEntryItem) & 0xFFF), FileBuffer));

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

char Encode(char code)
{
	return code ^ 0xA;
};

char Decode(char code)
{
	return code ^ 0xA;
};

void CreateShellExe(char* FilePathShell, char* FilePathSrc)
{
	char* FileBufferShell = NULL;
	char* FileBufferSrc = NULL;
	char* encodebuffer = NULL;
	printf("%d\n", sizeof(char*));
	///HMODULE mod=LoadLibrary(L"Dll1.dll");

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePathShell, &FileBufferShell);
	flag = FileToFileBuffer(FilePathSrc, &FileBufferSrc);
	encodebuffer = FileBufferSrc;
	//if (flag)printf("文件读取完成 %x\n", FileBuffer);
	//OutputReloacationDirtory(FileBuffer);
	HANDLE hFile = CreateFile(FilePathSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	int filesize = GetFileSize(hFile, &filesize);
	int FileSize = filesize;
	CloseHandle(hFile);
	char*NewFileBuffer = AddNewSectionToFileEnd(FileBufferShell, filesize + 0x1000);
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(NewFileBuffer);

	//加密
	while (filesize > 0)
	{
		*encodebuffer = Encode(*encodebuffer);
		encodebuffer++;
		filesize--;
	};

	//获取最后一个节，用于初始化新的节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	memcpy(NewFileBuffer + LastSection->PointerToRawData, FileBufferSrc, FileSize);

	//新NewFileBuffer写入文件
	flag = FileBufferToFile(L"C:\\1shell.exe", NewFileBuffer);


};


void RuntimeShell(char* FilePath)
{

	char* SourceFile = NULL;
	char* SourceImageBuffer = NULL;
	char* FileBuffer = NULL;
	BOOL IsSuccess = 0;

	//读取文件到内存，并获取pe文件头
	IsSuccess = FileToFileBuffer(FilePath, &FileBuffer);
	if (!IsSuccess)return;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;



	//获取最后一个节，备用
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//分配内存，并初始化
	SourceFile = (char*)malloc(LastSection->SizeOfRawData + 0x1000);
	if (!SourceFile)return;
	memset(SourceFile, 0, LastSection->SizeOfRawData + 0x1000);

	//把文件最后一个节中的数据读出来
	memcpy(SourceFile, FileBuffer + LastSection->PointerToRawData, LastSection->SizeOfRawData);


	//解密文件最后一个节
	int len = LastSection->SizeOfRawData;
	char* startadd = SourceFile;
	while (len > 0)
	{
		*startadd = Decode(*startadd);
		startadd++;
		len--;
	};
	//IsSuccess = FileBufferToFile(TEXT("C:\SHELLEND.exe"), SourceFile);
	//分配拉伸内存，并初始化
	PPeAllHeaders PtrToPeHeaderSource = GetPeAllHeaders(SourceFile);
	if (!PtrToPeHeaderSource)return 0;
	PIMAGE_OPTIONAL_HEADER OptionHeader = NULL;
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		OptionHeader = PtrToPeHeaderSource->u.OptionHeaderPtr32;
	}
	else
	{
		OptionHeader = PtrToPeHeaderSource->u.OptionHeaderPtr64;
	};
	SourceImageBuffer = (char*)malloc(OptionHeader->SizeOfImage + 0x1000);
	if (!SourceImageBuffer)return;
	memset(SourceImageBuffer, 0, OptionHeader->SizeOfImage + 0x1000);

	//文件拉伸
	FileBufferToImageBuffer(SourceFile, SourceImageBuffer);
	//IsSuccess = FileBufferToFile(TEXT("C:\SHELLEND1.exe"), SourceImageBuffer);

	//挂起方式创建进程，并获取主线程context
	STARTUPINFO ie_si = { 0 };
	PROCESS_INFORMATION ie_pi;
	ie_si.cb = sizeof(ie_si);
	IsSuccess = CreateProcess(FilePath,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&ie_si,&ie_pi);
	if (!IsSuccess)return;

	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_FULL;
	IsSuccess = GetThreadContext(ie_pi.hThread, &context);
	if (!IsSuccess) 
	{
		TerminateProcess(ie_pi.hProcess, 0);
		return;
	}; 

	//获取挂起进程主模块的ImageBase	
	char szBuffer[256] = { 0 };
	DWORD64 imageAddress = 0;
	char* baseAddress = NULL;

	baseAddress = (CHAR *)context.Ebx + 8;
	IsSuccess = ReadProcessMemory(ie_pi.hProcess, baseAddress, &imageAddress, sizeof(PVOID), NULL);
	if (!IsSuccess)return;

	// 获取 NtUnmapViewOfSection 地址
	typedef NTSTATUS(NTAPI* pfnNtUnmapViewOfSection)(HANDLE, PVOID);
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	pfnNtUnmapViewOfSection NtUnmapViewOfSection = (pfnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");

	// 卸载目标进程的主模块
	//NTSTATUS status = NtUnmapViewOfSection(ie_pi.hProcess, (PVOID)imageAddress);
	//if (status != 0)
	//{
	//	printf("卸载主模块失败！\n");
	//	return 0;
	//}


	//在目标进程重新分配主模块空间
	char* Address = NULL;
	char* imagebase = VirtualAllocEx(ie_pi.hProcess, (LPVOID)OptionHeader->ImageBase, OptionHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!imagebase)
	{
		imagebase = VirtualAllocEx(ie_pi.hProcess, NULL, OptionHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!imagebase)
		{
			return 0;
		}
		//修复重定位
		Address =UpdataRelocationDirtory(SourceImageBuffer, imagebase);
		if ((int)Address == 0)
		{
			return 0;
		};

		PPeAllHeaders PtrToPeHeaderImageBuffer = GetPeAllHeaders(SourceImageBuffer);
		if (!PtrToPeHeaderImageBuffer)return 0;
		PtrToPeHeaderImageBuffer->NtHeadersPtr->OptionalHeader.ImageBase = imagebase;
	}


	int writelen = 0;
	IsSuccess = WriteProcessMemory(ie_pi.hProcess, imagebase, SourceImageBuffer, OptionHeader->SizeOfImage, &writelen);
	if (!IsSuccess)return;
	
	context.Eax = OptionHeader->AddressOfEntryPoint + OptionHeader->ImageBase;

	IsSuccess = WriteProcessMemory(ie_pi.hProcess, baseAddress, &imagebase, sizeof(PVOID), &writelen);
	

	IsSuccess = SetThreadContext(ie_pi.hThread,&context);

	ResumeThread(ie_pi.hThread);
	if (!IsSuccess)return;

	if (SourceFile)free(SourceFile);
	if (SourceImageBuffer)free(SourceImageBuffer);
	if (PtrToPeHeader)free(PtrToPeHeader);
	if (PtrToPeHeaderSource)free(PtrToPeHeaderSource);
};




//获取源数据
DWORD GetSrcSource(IN LPVOID fileBuffer, OUT LPVOID* OutFileBuffer) {
	//初始化数据
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);//第一个节位置
	PIMAGE_SECTION_HEADER pLastHeader = pSectionHeader + pNtHeader->FileHeader.NumberOfSections - 1;//最后一个节位置

	*OutFileBuffer = (LPVOID)((DWORD)fileBuffer + pLastHeader->PointerToRawData);

	return pLastHeader->SizeOfRawData;
}
/*
解密函数
参数
文件大小
源文件
写出文件
*/
VOID DecodeSource(DWORD size, PBYTE source, OUT LPVOID buffer) {

	for (DWORD i = 0; i < size; i++) {
		source[i] ^= 8888;
	}

}
//void RuntimeShell1(TCHAR* FilePath) 
//{
//	//获取当前程序运行路径
//	char FilePath[255] = { 0 };
//	GetModuleFileName(NULL, FilePath, 255);
//
//	LPVOID fileBuffer;
//	DWORD size = 0;
//	size = ReadFileBuffer(FilePath, &fileBuffer);
//
//	LPVOID srcImageBuffer;
//	FileToImage(srcSource, &srcImageBuffer);
//
//	//创建进程 记得恢复进程
//	STARTUPINFO si = { 0 };
//	PROCESS_INFORMATION pi;
//	si.cb = sizeof(si);
//
//	//以挂起的方式创建进程                                   
//	CreateProcess(
//		FilePath,                    // name of executable module                              
//		NULL,                // command line string                             
//		NULL,                   // SD           
//		NULL,                      // SD                       
//		FALSE,                   // handle inheritance option                            
//		CREATE_SUSPENDED,       // creation flags                            
//		NULL,                    // new environment block                               
//		NULL,                // current directory name                              
//		&si,                  // startup information                               
//		&pi                   // process information                              
//	);
//
//	CONTEXT contx;
//	contx.ContextFlags = CONTEXT_FULL;
//
//	GetThreadContext(pi.hThread, &contx);
//
//	//获取入口点                                
//	DWORD dwEntryPoint = contx.Eax;
//
//	//获取ImageBase                                  
//	char* baseAddress = (CHAR *)contx.Ebx + 8;
//	DWORD imageBase = 0;
//	SIZE_T byteSize = 0;
//
//	//因为属于别人程序所以使用这个读
//	ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &byteSize);
//
//	typedef long NTSTATUS;
//	typedef NTSTATUS(__stdcall *pfnZwUnmapViewOfSection)(
//		IN HANDLE ProcessHandle,
//		IN LPVOID BaseAddress
//		);
//
//	pfnZwUnmapViewOfSection ZwUnmapViewOfSection;
//	ZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(
//		GetModuleHandleA("ntdll.dll"), "ZwUnmapViewOfSection");
//	if (!ZwUnmapViewOfSection)
//	{
//		TerminateThread(pi.hThread, 2);
//		WaitForSingleObject(pi.hThread, INFINITE);
//		return 0;
//	}
//	//卸载文件外壳镜像
//	DWORD a = 0;
//	a = ZwUnmapViewOfSection(pi.hProcess, (PVOID)imageBase);
//
//	//获取src imagebase  sizeofimage
//	DWORD srcImageBase = 0;
//	DWORD srcSizeOfImage = 0;
//	DWORD srcOEP = 0;
//	GetImageBase(srcImageBuffer, &srcImageBase, &srcSizeOfImage, &srcOEP);
//
//	LPVOID status = NULL;
//	//指定区域分配地址
//	status = VirtualAllocEx(pi.hProcess,
//		(LPVOID)srcImageBase, srcSizeOfImage,
//		MEM_RESERVE | MEM_COMMIT,
//		PAGE_EXECUTE_READWRITE);
//
//	if (status == NULL) {
//		//判断有没有重定位
//		if (isRelocation(srcImageBuffer)) {
//			TerminateThread(pi.hThread, 2);
//			WaitForSingleObject(pi.hThread, INFINITE);
//			return 0;
//		}
//		status = VirtualAllocEx(pi.hProcess,
//			NULL, srcSizeOfImage,
//			MEM_RESERVE | MEM_COMMIT,
//			PAGE_EXECUTE_READWRITE);
//		if (status == 0) {
//			TerminateThread(pi.hThread, 2);
//			WaitForSingleObject(pi.hThread, INFINITE);
//			return 0;
//		}
//		//修复重定位
//		ModificationBaseRel(srcImageBuffer, (DWORD)status);
//		GetImageBase(srcImageBuffer, &srcImageBase, &srcSizeOfImage, &srcOEP);
//	}
//
//	//将src复制到申请成功的地址中
//	SIZE_T srcResult = 0;
//	WriteProcessMemory(pi.hProcess, status, srcImageBuffer, srcSizeOfImage, &srcResult);
//	//修正context结构
//	SIZE_T oepResult = 0;
//	WriteProcessMemory(pi.hProcess, baseAddress, &srcImageBase, sizeof(DWORD), &oepResult);
//	contx.Eax = srcOEP + srcImageBase;
//
//	SetThreadContext(pi.hThread, &contx);
//	//记得恢复线程
//	ResumeThread(pi.hThread);
//};
