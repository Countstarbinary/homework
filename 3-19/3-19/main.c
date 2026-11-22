#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "19.h"

#define FilePath L"C:\\fg.exe"
#define FilePath1 L"C:\\Myfg.exe"
int main()
{
	//AddCodeToExe(FilePath, 3);
	AddNewSectionToExe(FilePath,0x3000);
	
	system("pause");
	return 0;
}
