
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "pe.h"

#define FilePath L"C:\\Windows\\System32\\notepad.exe"
int main()
{
	GetPeHeaders(FilePath);

	return 0;
}


