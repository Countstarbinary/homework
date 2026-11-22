// 6-11 Project.cpp : 定义应用程序的入口点。
//
#include <stdio.h>
#include "resource.h"
#include "framework.h"
#include "6-11 Project.h"
#include "HOOK.h"

#define MAX_LOADSTRING 100


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//HMODULE ImageBuffer = LoadLibrary(L"kernel32.dll");
	//GetFuncAddrByName((char*) ImageBuffer, (char*)"GetProcAddress");
	//HMODULE ImageBuffer= GetKernel32Base();strcmp_NEW
	//BOOL IS=wcscmp_NEW((TCHAR*)"KERNEL72.DLL", (TCHAR*)"KERNEL32.DLL");
	//IS = strcmp_NEW((char*)"KERNEL32.DLL", (char*)"KERNEL32.DLL");
	//HMODULE kernel32 = GetKernel32Base();
	//if (!kernel32) { return 0; };
	//typedef HMODULE(WINAPI* LoadLibraryproc)(_In_ LPCWSTR lpLibFileName);
	//LoadLibraryproc LoadLib = (LoadLibraryproc)GetFuncAddrByName((char*)kernel32, (char*)"LoadLibraryW");

	EventNotify = CreateEvent(NULL, FALSE, TRUE, L"EventNotify");
	BOOL IsSuccess = LoadMemory(15816);
	if (!IsSuccess)
	{
		return 0;
	};
	Sleep(100);
	EventNotifyInject = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventNotifyInject");
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, About);

	return 0;
}


HWND EDIT_MB;
HWND EDIT_CF;
HWND EDIT_OP;
HWND BUTTON_START_MB;
HWND BUTTON_START_CF;
HWND BUTTON_START_OP;
HWND BUTTON_CALL_MB;
HWND BUTTON_CALL_CF;
HWND BUTTON_CALL_OP;
HANDLE hMap;
LPVOID pBuf;

void InitFunc(HWND hwnd)
{
	EDIT_MB = GetDlgItem(hwnd, IDC_EDIT_MB);
	EDIT_CF = GetDlgItem(hwnd, IDC_EDIT_CF);
	EDIT_OP = GetDlgItem(hwnd, IDC_EDIT_OP);

	BUTTON_START_MB = GetDlgItem(hwnd, IDC_BUTTON_START_MB);
	BUTTON_START_CF = GetDlgItem(hwnd, IDC_BUTTON_START_CF);
	BUTTON_START_OP = GetDlgItem(hwnd, IDC_BUTTON_START_OP);

	BUTTON_CALL_MB = GetDlgItem(hwnd, IDC_BUTTON_CALL_MB);
	BUTTON_CALL_CF = GetDlgItem(hwnd, IDC_BUTTON_CALL_CF);
	BUTTON_CALL_OP = GetDlgItem(hwnd, IDC_BUTTON_CALL_OP);
	SendMessage(EDIT_MB, WM_SETTEXT, 0, (LPARAM)L"MessageBox");
	SendMessage(EDIT_CF, WM_SETTEXT, 0, (LPARAM)L"CreateFile");
	SendMessage(EDIT_OP, WM_SETTEXT, 0, (LPARAM)L"OpenProcess");


	pBuf = CreateMappingAndMapView((DWORD*)&hMap);
};

void MonitorFunc(HWND hwnd,int NumberOfChar, TCHAR* FuncName,char* OldFunc, char* NewdFunc)
{
	TCHAR szBuffer[256] = { 0 };
	GetWindowText(hwnd, szBuffer, 256);
	if (wcscmp(szBuffer, L"开启监控") == 0)
	{
		SetWindowText(hwnd, L"关闭监控");



		MSG_NEW[0].OldAddress = (char*)OldFunc;
		MSG_NEW[0].NewAddress = (char*)NewdFunc - OldImageBase + NewImageBase;
		MSG_NEW[0].type = 1;
		MSG_NEW[0].MonitorNumer = 0;
		MSG_NEW[0].NumberOfChar = NumberOfChar;
		memcpy(MSG_NEW[0].FuncName, FuncName,128);
		WriterThread(&MSG_NEW[0], pBuf);
	}
	else if (wcscmp(szBuffer, L"关闭监控") == 0)
	{
		SetWindowText(hwnd, L"开启监控");

		MSG_NEW[0].OldAddress = (char*)OldFunc;
		MSG_NEW[0].NewAddress = (char*)NewdFunc - OldImageBase + NewImageBase;
		MSG_NEW[0].type = 2;
		MSG_NEW[0].MonitorNumer = 0;
		MSG_NEW[0].NumberOfChar = NumberOfChar;
		WriterThread(&MSG_NEW[0], pBuf);
	}
};

void RemoteCall(HWND hwnd, char* OldFunc, char* NewdFunc)
{
	TCHAR szBuffer[256] = { 0 };
	GetWindowText(hwnd, szBuffer, 256);
	if (wcscmp(szBuffer, L"远程调用") == 0)
	{
		MSG_NEW[0].OldAddress = (char*)OldFunc;
		MSG_NEW[0].NewAddress = (char*)NewdFunc - OldImageBase + NewImageBase;
		MSG_NEW[0].type = 3;
		MSG_NEW[0].MonitorNumer = 0;

		TCHAR szBuffer1[128] = L"测试Messagebox";
		memcpy(MSG_NEW[0].FuncPara[1], szBuffer1, 128);
		TCHAR szBuffer2[128] = L"这是标题";
		memcpy(MSG_NEW[0].FuncPara[2], szBuffer2, 128);
		WriterThread(&MSG_NEW[0], pBuf);
	};
};

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		InitFunc(hDlg);
		
		return (INT_PTR)TRUE;
	}

	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_START_MB:
		{
			TCHAR szBuffer[128] = L"MessageBox";
			MonitorFunc(BUTTON_START_MB, 5, szBuffer,(char*)MessageBox, (char*) MyMessagebox);

			return TRUE;
		};
		case IDC_BUTTON_START_CF:
		{
			TCHAR szBuffer[128] = L"CreateFile";
			MonitorFunc(BUTTON_START_CF,6, szBuffer,(char*)CreateFile, (char*)MyMessagebox);
			return TRUE;
		};
		case IDC_BUTTON_START_OP:
		{
			TCHAR szBuffer[128] = L"OpenProcess";
			MonitorFunc(BUTTON_START_OP,5, szBuffer,(char*)OpenProcess, (char*)MyMessagebox);
			return TRUE;
		};
		case IDC_BUTTON_CALL_MB:
		{
			RemoteCall(BUTTON_CALL_MB, (char*)MessageBox, (char*)MyMessagebox);

			return TRUE;
		};
		case IDC_BUTTON_CALL_CF:
		{
			RemoteCall(BUTTON_CALL_CF, (char*)CreateFile, (char*)MyMessagebox);

			return TRUE;
		};
		case IDC_BUTTON_CALL_OP:
		{
			RemoteCall(BUTTON_CALL_OP, (char*)OpenProcess, (char*)MyMessagebox);

			return TRUE;
		};
		};
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	};

	}
	return (INT_PTR)FALSE;
}
