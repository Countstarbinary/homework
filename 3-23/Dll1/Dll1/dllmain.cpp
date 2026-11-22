// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

// 线程处理函数
DWORD WINAPI ThreadProc(LPVOID lpParam) {
	MessageBoxW(NULL, L"haohell", L"提示", MB_OK);
	return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		MessageBoxW(NULL, L"haohell", L"提示", MB_OK);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"haohell", L"提示", MB_OK);
        break;
    }
    return TRUE;
}

