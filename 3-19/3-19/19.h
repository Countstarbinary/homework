#pragma once

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

void FileBufferToImageBuffer(char* FileBuffer, char* ImageBuffer);

void ImageBufferToFileBuffer(char* FileBuffer, char* ImageBuffer);

int RvaToFoa(int Rva, char* ImageBuffer);

int AddCodeToExe(char* Filepath, int IndexOfSection);

BOOL ExpandLastSection(char* FileBuffer, int ExpandLength);

BOOL AddNewSectionToFileEnd(char* FileBuffer, int NewSectionLength);

BOOL AddNewSectionToExe(char* Filepath, int NewSectionLength);