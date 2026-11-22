

#include <iostream>
#include "6-4.h"
//#define FilePath L"C:\\fg.exe"
//#define FilePath L"C:\\CRACKME.EXE"
#define FilePath L"C:\\Users\\Administrator\\source\\repos\\滴水3期作业\\1-27\\Debug\\1-27.exe"
//#pragma   comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\""   ) 	
int main()
{
	LoadProcess((TCHAR *)FilePath);
	return 0;
}


