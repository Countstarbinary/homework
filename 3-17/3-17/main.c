// 3-17.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "17.h"

#define FilePath L"C:\\fg.exe"
#define FilePath1 L"C:\\Myfg.exe"
int main()
{
	AddCodeToExe(FilePath,1);
	getchar();
	return 0;
}

