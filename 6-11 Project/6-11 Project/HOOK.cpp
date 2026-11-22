#pragma once
#include <windows.h>
#include <stdio.h>
#include <assert.h>

int jmpaddress = 0;

typedef HANDLE(WINAPI *tCreateFileW)(
	LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
tCreateFileW pOrigCreateFileW = CreateFileW;

// 辅助日志函数：把 lpFileName 写入 C:\log.txt
void LogCreateFileW(LPCWSTR lpFileName)
{
	// 打开或创建日志文件
	HANDLE hLog = CreateFile(
		L"C:\\log.txt",
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hLog == INVALID_HANDLE_VALUE) return;

	// 把宽字符串转换成 UTF-8
	//int cb = WideCharToMultiByte(CP_UTF8, 0, lpFileName, -1, NULL, 0, NULL, NULL);
	//char *buf = (char*)malloc(cb);
	//WideCharToMultiByte(CP_UTF8, 0, lpFileName, -1, buf, cb, NULL, NULL);

	DWORD written;
	WriteFile(hLog, lpFileName, wcslen(lpFileName) * sizeof(wchar_t), &written, NULL);
	//WriteFile(hLog, buf, cb - 1, &written, NULL);
	WriteFile(hLog, "\r\n", 2, &written, NULL);

	//free(buf);
	CloseHandle(hLog);
}

//去掉checkesp
#pragma runtime_checks("", off) 

void __declspec(naked) MyMessagebox()
{

	__asm
	{
		pushad;
		pushfd;
		sub esp, 0x100;
	};

	//__asm
	//{
	//	mov eax, [esp + 0x100 + 9 * 4 + 4 + 4];
	//	push eax;
	//	CALL LogCreateFileW;

	//	mov eax, [esp + 0x100 + 9 * 4 + 4 + 8];
	//	push eax;
	//	CALL LogCreateFileW;
	//}
	OutputDebugStringA("hook Test\n");


	__asm
	{
		mov eax, [MessageBoxW];
		add eax, 5;
		mov[jmpaddress], eax;
		add esp, 0x100;
		popfd;
		popad;

	};


	__asm
	{
		_emit 0x8b;
		_emit 0xFF;
		_emit 0x55;
		_emit 0x8b;
		_emit 0xEC
	};
	__asm
	{
		jmp jmpaddress;
	};
};

//有问题
void __declspec(naked) MyCreateFile()
{
	int *p1;
	__asm
	{
		pushad;
		pushfd;
		sub esp, 0x100;
	};
	__asm 
	{
		mov eax, [esp + 100 + 9 * 4 + 4 + 4];
		push eax;
		CALL LogCreateFileW;
	}
	OutputDebugStringA("hook Test CreateFileW\n");
	

	__asm
	{
		mov eax, [CreateFileW];
		add eax, 5;
		mov[jmpaddress], eax;
		add esp, 0x100;
		popfd;
		popad;

	};


	__asm
	{
		_emit 0xFF;
		_emit 0x25;
		_emit 0x98;
		_emit 0x13;
		_emit 0x4d;
		_emit 0x76;
	};

};

void __declspec(naked) MyOpenProcess()
{

	__asm
	{
		pushad;
		pushfd;
		sub esp, 0x100;
	};
	OutputDebugStringA("hook Test OpenProcess\n");


	__asm
	{
		mov eax, [OpenProcess];
		add eax, 5;
		mov[jmpaddress], eax;
		add esp, 0x100;
		popfd;
		popad;

	};


	__asm
	{
		_emit 0x8b;
		_emit 0xFF;
		_emit 0x55;
		_emit 0x8b;
		_emit 0xEC
	};
	__asm
	{
		jmp jmpaddress;
	};
};

#pragma runtime_checks("", restore)