
#include <iostream>
#include "6-9.h"

int main()
{
	OldAddress = (DWORD)MessageBoxW;
	NewAddress = (DWORD)MyMessagebox;
	SetIATHook((char*)MessageBoxW, (char*)MyMessagebox);
	MessageBoxW(NULL,L"IATHOOK",L"IATHOOK",NULL);
	ResetIATHook((char*)OldAddress, (char*)NewAddress);
	MessageBoxW(NULL, L"ResetIATHOOK", L"ResetIATHOOK", NULL);
	return 0;
}
