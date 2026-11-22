// 5-13.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<windows.h>


HANDLE g_hSet, g_hClear;
int g_Max =100;
int g_Number = 0;

//生产者线程函数  								
DWORD WINAPI ThreadProduct(LPVOID pM)
{
	for (int i = 0; i < g_Max; i++)
	{
		WaitForSingleObject(g_hSet, INFINITE);
		g_Number = 1;
		DWORD id = GetCurrentThreadId();
		printf("生产者%d将数据%d放入缓冲区\n", id, g_Number);
		//SetEvent(g_hClear);
	}
	return 0;
}

//消费者线程函数								
DWORD WINAPI Thread3(LPVOID pM)
{
	//for (int i = 0; i < g_Max; i++)
	//{
	//	WaitForSingleObject(g_hSet, INFINITE);
	//	g_Number = 0;
	//	DWORD id = GetCurrentThreadId();
	//	printf("----消费者%d将数据%d放入缓冲区\n", id, g_Number);
	//	//SetEvent(g_hSet);
	//}
	return 0;
}
//消费者线程函数								
DWORD WINAPI ThreadConsumer(LPVOID pM)
{
	for (int i = 0; i < g_Max; i++)
	{
		WaitForSingleObject(g_hClear, INFINITE);
		g_Number = 0;
		DWORD id = GetCurrentThreadId();
		printf("----消费者%d将数据%d放入缓冲区\n", id, g_Number);
		SetEvent(g_hSet);
	}
	return 0;
}

int main()
{

	HANDLE hThread[3];

	g_hSet = CreateEvent(NULL, FALSE, TRUE, NULL);
	g_hClear = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread[0] = ::CreateThread(NULL, 0, ThreadProduct, NULL, 0, NULL);
	hThread[1] = ::CreateThread(NULL, 0, ThreadConsumer, NULL, 0, NULL);
	hThread[2] = ::CreateThread(NULL, 0, Thread3, NULL, 0, NULL);
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	//销毁 							
	CloseHandle(g_hSet);
	CloseHandle(g_hClear);

	return 0;
}
