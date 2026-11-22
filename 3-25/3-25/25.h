#pragma once
#include <windows.h>

typedef struct _PeAllHeaders
{
	PIMAGE_DOS_HEADER FileDosHeaderPtr;
	PIMAGE_NT_HEADERS  NtHeadersPtr;
	PIMAGE_FILE_HEADER FileHeaderPtr;
	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr;
	PIMAGE_SECTION_HEADER SectionHeaderPtr;
}PeAllHeaders, *PPeAllHeaders;

BOOL FileToFileBuffer(char* FilePath, char* FileBuffer);

BOOL FileBufferToFile(char* FilePath, char* FileBuffer);



int RvaToFoa(unsigned int Rva, char* FileBuffer);

void OutputExportDirtory(char* FileBuffer);

void OutputReloacationDirtory(char* FileBuffer);