#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "24.h"

#define FilePath L"C:\\Dll1.dll"
#define FilePath1 L"C:\\Myfg.exe"

int main()
{

	char* FileBuffer = NULL;
	///HMODULE mod=LoadLibrary(L"Dll1.dll");

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);
	OutputExportDirtory(FileBuffer);
	system("pause");
	return 0;
}
