// 5-7.cpp : 定义应用程序的入口点。
//
#include <stdio.h>
#include "framework.h"
#include "5-11.h"
#include "Resource.h"

#define MAX_LOADSTRING 100

CRITICAL_SECTION cs1;
CRITICAL_SECTION cs2;

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	InitializeCriticalSection(&cs1);
	InitializeCriticalSection(&cs2);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)WndProc);

	return 0;
}

HWND hWndUP;
HWND hWndDown;

DWORD WINAPI ThreadProc1(
	LPVOID lpParameter   // thread data				
)
{
	TCHAR szBufferup[10] = { 0 };

	GetWindowText(hWndUP, szBufferup, 10);


	//字符转数字TCHAR版
	DWORD dwTimerup = 0;
	_stscanf_s(szBufferup, TEXT("%d"), &dwTimerup);


	while (dwTimerup < 1000)
	{
		dwTimerup++;
		//数组转字符
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimerup);
		EnterCriticalSection(&cs1);
		EnterCriticalSection(&cs2);
		SetWindowText(hWndUP, (LPCWSTR)szBufferup);
		LeaveCriticalSection(&cs2);
		LeaveCriticalSection(&cs1);
		Sleep(10);
	};
	return 0;
};

DWORD WINAPI ThreadProc2(
	LPVOID lpParameter   // thread data				
)
{
	
	TCHAR szBufferdown[10] = { 0 };
	GetWindowText(hWndDown, szBufferdown, 10);

	DWORD dwTimerdown = 0;
	_stscanf_s(szBufferdown, TEXT("%d"), &dwTimerdown);
	while (dwTimerdown>0)
	{
		dwTimerdown--;
		//数组转字符
		swprintf(szBufferdown, _countof(szBufferdown), TEXT("%d\r\n"), dwTimerdown);
		EnterCriticalSection(&cs2);
		EnterCriticalSection(&cs1);
		SetWindowText(hWndDown, (LPCWSTR)szBufferdown);
		LeaveCriticalSection(&cs1);
		LeaveCriticalSection(&cs2);
		Sleep(10);
	};
	return 0;
};

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	case WM_INITDIALOG:

		hWndUP = GetDlgItem(hWnd, IDC_EDIT_UP);
		hWndDown = GetDlgItem(hWnd, IDC_EDIT_DOWN);

		SetWindowText(hWndUP, (LPCWSTR)TEXT("0"));
		SetWindowText(hWndDown, (LPCWSTR)TEXT("1000"));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_START:

			HANDLE handle1 = CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
			HANDLE handle2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
			
			return (INT_PTR)TRUE;
		}
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

