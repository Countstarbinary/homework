#pragma once
#include <windows.h>

typedef struct _PeAllHeaders
{
	PIMAGE_DOS_HEADER FileDosHeaderPtr;
	PIMAGE_NT_HEADERS  NtHeadersPtr;
	PIMAGE_FILE_HEADER FileHeaderPtr;
	union
	{
		PIMAGE_OPTIONAL_HEADER  OptionHeaderPtr;
		//只要不涉及堆和栈之后的可选头内容，可选头x86和x64没有区别，用上面的OptionHeaderPtr即可
		PIMAGE_OPTIONAL_HEADER32  OptionHeaderPtr32;
		PIMAGE_OPTIONAL_HEADER64 OptionHeaderPtr64;
	}u;
	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr;
	PIMAGE_SECTION_HEADER SectionHeaderPtr;
}PeAllHeaders, *PPeAllHeaders;

BOOL FileToFileBuffer(char* FilePath, char* FileBuffer);

BOOL FileBufferToFile(char* FilePath, char* FileBuffer);

BOOL AddNewSectionToExe(char* Filepath, char* NewFilepath, int NewSectionLength);

int RvaToFoa(unsigned int Rva, char* FileBuffer);

void OutputExportDirtory(char* FileBuffer);

void OutputReloacationDirtory(char* FileBuffer);

char* MoveExportDirtory(char* FileBuffer);

char* MoveRelocationDirtory(char* FileBuffer, int NewImagebase);

void OutputImportDirtory(char* FileBuffer);

void OutputIATDirtory(char* FileBuffer);

void OutputBoundImportDirtory(char* FileBuffer);
