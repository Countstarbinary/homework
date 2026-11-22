// 3-10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "13.h"



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
	printf("---------------BaseOfData:%x-------------------\n", OptionHeader->BaseOfData);
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

void OutputSectionHeader(PIMAGE_SECTION_HEADER SectionHeader,int NumberOfSections)
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
		
		SectionHeader ++;
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


