#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "4-27.h"

#define FilePath L"C:\\fg.exe"
//#define FilePath1 L"C:\\Mytest.dll"
//#define FilePath L"C:\\Mypad.exe"
//Mypad.exe
int main()
{

	char* FileBuffer = NULL;
	printf("%d\n", sizeof(char*));
	///HMODULE mod=LoadLibrary(L"Dll1.dll");

	//开始工作
	BOOL flag = 0;
	flag = FileToFileBuffer(FilePath, &FileBuffer);
	//if (flag)printf("文件读取完成 %x\n", FileBuffer);
	//OutputReloacationDirtory(FileBuffer);

	//AddNewSectionToExe(FilePath, FilePath1,0x2000);
	//char* Newbuffer = MoveExportDirtory(FileBuffer, FilePath1);
	//char* Newbuffer=MoveRelocationDirtory(FileBuffer,0x12000000);
	//FileBufferToFile(FilePath1, Newbuffer);
	//OutputImportDirtory(FileBuffer);
	//OutputIATDirtory(FileBuffer);
	//OutputBoundImportDirtory(FileBuffer);
	OutputResourceDirtory(FileBuffer);
	if (FileBuffer)free(FileBuffer);
	system("pause");
	return 0;
}
