// SHELL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
//#define FilePath L"C:\\SHELLCODE.exe"
#define FilePath L"C:\\Users\\Administrator\\source\\repos\\滴水3期作业\\5-21\\Debug\\SHELL.exe"

int main()
{
	RuntimeShell(FilePath);

	return 0;
}


