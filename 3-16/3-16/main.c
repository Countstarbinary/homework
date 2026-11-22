// 3-13.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "16.h"

#define FilePath L"C:\\Windows\\System32\\notepad.exe"
int main()
{
	GetPeHeaders(FilePath);
	return 0;
}

