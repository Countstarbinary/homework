// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <windows.h>

// 声明一个名为 "SHARED" 的节，用于存放需要跨进程共享的全局数据
#pragma data_seg("SHARED")

// 共享缓冲区和信号量
volatile LONG g_Flag = 0;                // 通信标志：0 = 空闲，1 = 写入就绪
char       g_Buffer[256] = { 0 };         // 数据缓冲区

#pragma data_seg()

// 告诉链接器将 SHARED 节设置为共享、可读写
#pragma comment(linker, "/SECTION:SHARED,RWS")

extern "C" __declspec(dllexport)
void WriteMessage(const char* msg)
{
	// 将消息拷贝到共享缓冲区
	strncpy_s(g_Buffer, sizeof(g_Buffer), msg, _TRUNCATE);
	// 标记数据就绪
	InterlockedExchange(&g_Flag, 1);
}

extern "C" __declspec(dllexport)
const char* ReadMessage()
{
	// 自旋等待数据就绪
	while (InterlockedCompareExchange(&g_Flag, 0, 0) == 0) {
		Sleep(1);
	}
	// 读取完成后清除标志
	InterlockedExchange(&g_Flag, 0);
	return g_Buffer;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

