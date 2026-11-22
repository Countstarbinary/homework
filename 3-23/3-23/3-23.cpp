// 3-23.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <iostream>
//#include "dll1.h"

#pragma comment(lib,"Dll1.dll")

typedef int(__stdcall *lpPlus)(int, int);
int main()
{
	lpPlus Plus;
	HINSTANCE   hModule = LoadLibrary(L"inject.dll");
	Plus =(lpPlus)GetProcAddress(hModule, "Plus");
	int n=Plus(1,2);

	std::cout << "Hello World!\n"<< n;
}

