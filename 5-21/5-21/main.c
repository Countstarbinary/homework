#include "5-21.h"
#include <windows.h>

#define FilePathShell L"C:\\Users\\Administrator\\source\\repos\\滴水3期作业\\5-21\\Debug\\SHELL.exe"
//#define FilePathShell L"C:\\SHELL.exe"
#define FilePath L"C:\\1.exe"
int main()
{
	CreateShellExe(FilePathShell,FilePath);

	system("pause");
	return 0;
}


