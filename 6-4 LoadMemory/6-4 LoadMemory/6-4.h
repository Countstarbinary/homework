#pragma once
#pragma once
#include <windows.h>

extern BOOL IsSelf;

typedef struct _PeAllHeaders
{
	PIMAGE_DOS_HEADER FileDosHeaderPtr;
	PIMAGE_NT_HEADERS  NtHeadersPtr;
	PIMAGE_FILE_HEADER FileHeaderPtr;
	union
	{
		//x86,x64在栈之前的部分都是一样的，尽管imagebase长度有所不同，但用BaseOfData抵消了，是一样的
		PIMAGE_OPTIONAL_HEADER OptionHeaderPtr;

		PIMAGE_OPTIONAL_HEADER32  OptionHeaderPtr32;
		PIMAGE_OPTIONAL_HEADER64 OptionHeaderPtr64;
	}u;
	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr;
	PIMAGE_SECTION_HEADER SectionHeaderPtr;
}PeAllHeaders, *PPeAllHeaders;

BOOL LoadProcess(TCHAR* FilePath);

BOOL LoadMemory(DWORD64 pid);

char* UpdataRelocationDirtory(char* FileBuffer, int NewImagebase);



wchar_t * Asci_to_wchar(const char * pastring, wchar_t * pwstring);

int strlen_new(const char * pastring);