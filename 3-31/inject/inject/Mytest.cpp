#include "pch.h"
#include "Mytest.h"
#include <Windows.h>

void Init() 
{
	MessageBox(NULL,L"Init",L"Init",MB_OK);

};

void Destory()
{
	MessageBox(NULL, L"Destory", L"Destory", MB_OK);

	HMODULE hModule;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)L"inject.dll", &hModule);
	
	if (hModule)FreeLibrary(hModule);
};


void ExportFunction()
{
	MessageBox(NULL, L"ExportFunction", L"ExportFunction", MB_OK);
};
