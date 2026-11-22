#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "20.h"

#define FilePath L"C:\\fg.exe"
#define FilePath1 L"C:\\Myfg.exe"

int main()
{
	//AddCodeToExe(FilePath, 3);
	//AddNewSectionToExe(FilePath, 0x3000);
	//MergeAllSection(FilePath);
	char* FileBuffer = NULL;

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);
	OutPutAllDirtory(FileBuffer);
	system("pause");
	return 0;
}
