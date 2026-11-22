#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "31.h"

#define FilePath L"C:\\notepad.exe"
#define FilePath1 L"C:\\2PCHunter64.exe"
//#define FilePath L"C:\\CRACKME.EXE"
//notepad.exe
//#pragma comment(lib,"inject.dll")

typedef void( *ExportFunctionProc)();




int main()
{

	char* FileBuffer = NULL;
	//printf("%d\n", sizeof(char*));
	HMODULE mod=LoadLibrary(L"inject.dll");
	//ExportFunctionProc ExportFunction= (ExportFunctionProc)GetProcAddress(mod, "ExportFunction");
	//ExportFunction();
	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);
	//if (flag)printf("文件读取完成 %x\n", FileBuffer);
	OutputReloacationDirtory(FileBuffer);
	//OutputImportDirtory(FileBuffer);
 	//char* Newbuffer = MoveImportDirtory(FileBuffer);
	//FileBufferToFile(FilePath1, Newbuffer);

	//Per p1 = {};
	//Per p2 = {};

	//printf("%x %x\n", &p1, &p2);
	system("pause");
	return 0;
}