// 3-10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define FilePath L"C:\\Windows\\System32\\notepad.exe"

int main()
{
	//读取文件
	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Failed to open file! Error: %lu\n", GetLastError());
		return 1;
	}

	//获取文件大小，其内部还是NtQueryInformationFile函数实现的
	DWORD fileSize = GetFileSize(hFile, NULL);

	if (fileSize == INVALID_FILE_SIZE) 
	{
		printf("Failed to get file size! Error: %lu\n", GetLastError());
	}
	else 
	{
		printf("File size: %lu bytes\n", fileSize);
	}

	//分配内存空间，用于存放文件中的数据
	char* Address=malloc(fileSize);
	memset(Address,0, fileSize);
	int bytesRead = 0;

	//读取文件中的数据到我们分配的内存空间中
	ReadFile(hFile, Address, fileSize, &bytesRead, NULL);
	
	
	printf("%s", Address);
	
	HANDLE hFile1 = CreateFile(L"C:\\Mynotepad.exe", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile1, Address, fileSize, &bytesRead, NULL);
	//关闭文件句柄
	CloseHandle(hFile);

	return 0;
}


