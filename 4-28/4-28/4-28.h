#pragma once
#include<windows.h>
#include "resource.h"

typedef struct _ProcessInfo
{
	DWORD pid;
	DWORD Parentpid;
	WCHAR name[0x128];
	DWORD NumberOfThreads;
	
}ProcessInfo, *PProcessInfo;

typedef struct _MoudleInfo
{
	WCHAR name[0x128];
	DWORD64 Imagebase;
	DWORD64 Imagesize;
	DWORD64 MoudleId;
}MoudleInfo, *PMoudleInfo;

HINSTANCE HInstance;



void EnumProcessesNew(DWORD64 ProcessInfo);

void EnumMoudles(DWORD64 ProcessInfo, DWORD pid);
