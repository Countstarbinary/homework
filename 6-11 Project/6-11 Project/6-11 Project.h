#pragma once

#include "resource.h"
#pragma once
#pragma once
#include <windows.h>

extern BOOL IsSelf;
extern HANDLE EventNotify; 
extern HANDLE EventNotifyInject;
extern DWORD OldImageBase;
extern DWORD NewImageBase;
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


typedef struct _Message 
{
	//开启监控，还是远程调用。1，2，3.开关监控，远程调用
	int type;
	int MonitorNumer; //监控序号，每次增加1.
	int NumberOfChar; //覆盖多少字节
	char* OldAddress;
	char* NewAddress;

	TCHAR FuncName[128];
	//参数
	TCHAR FuncPara[6][128];
}Message,*PMessage;

extern Message MSG_NEW[256];

BOOL LoadProcess(TCHAR* FilePath);

BOOL LoadMemory(DWORD64 pid);

char* UpdataRelocationDirtory(char* FileBuffer, int NewImagebase);

LPVOID CreateMappingAndMapView(DWORD* Handle);

char* WriterThread(PMessage message, LPVOID pBuf);

wchar_t * Asci_to_wchar(const char * pastring, wchar_t * pwstring);

int strlen_new(char * pastring);

void* GetFuncAddrByName(char* ImageBuffer, char * FunctionName);

HMODULE GetKernel32Base();

BOOL wcscmp_NEW(TCHAR* DES, TCHAR* SRC);
BOOL strcmp_NEW(char* DES, char* SRC);



