// 5-7.cpp : 定义应用程序的入口点。
//
#include <stdio.h>
#include "framework.h"
#include "5-12.h"
#include "Resource.h"

#define MAX_LOADSTRING 100

CRITICAL_SECTION cs1;
HANDLE mutex1;
HANDLE mutex2;
HANDLE mutex3;

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	mutex1 = CreateMutex(NULL, FALSE, NULL);
	mutex2 = CreateMutex(NULL, FALSE, NULL);
	mutex3 = CreateMutex(NULL, FALSE, NULL);
	InitializeCriticalSection(&cs1);
	ReleaseMutex(mutex2);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)WndProc);

	return 0;
}


HWND hWndAll;
HWND hWnd1;
HWND hWnd2;
HWND hWnd3;
DWORD WINAPI ThreadProc1(
	LPVOID lpParameter   // thread data				
)
{
	TCHAR szBufferup[10] = { 0 };

	//字符转数字TCHAR版
	DWORD dwTimerall = 0;
	DWORD dwTimer1 = 0;

	



	while (1)
	{
		//数组转字符
		//EnterCriticalSection(&cs1);
		WaitForSingleObject(mutex1,-1);
		GetWindowText(hWndAll, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimerall);

		GetWindowText(hWnd1, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimer1);

		if (dwTimerall < 50)break;
		dwTimer1 += 50;

		dwTimerall -= 50;
		//ExitThread(1);//EnterCriticalSection的话，其他线程也凉了
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimer1);
		SetWindowText(hWnd1, (LPCWSTR)szBufferup);
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimerall);
		SetWindowText(hWndAll, (LPCWSTR)szBufferup);

		//LeaveCriticalSection(&cs1);
		//ReleaseMutex(mutex1);
		
		Sleep(1000);
	};

	return 0;
};

DWORD WINAPI ThreadProc2(
	LPVOID lpParameter   // thread data				
)
{
	TCHAR szBufferup[10] = { 0 };

	//字符转数字TCHAR版
	DWORD dwTimerall = 0;
	DWORD dwTimer2 = 0;


	while (1)
	{

		//数组转字符
		//EnterCriticalSection(&cs1);
		WaitForSingleObject(mutex2, -1);
		GetWindowText(hWndAll, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimerall);

		GetWindowText(hWnd2, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimer2);
		if (dwTimerall < 50)break;
		dwTimer2 += 50;

		dwTimerall -= 50;

		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimer2);
		SetWindowText(hWnd2, (LPCWSTR)szBufferup);
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimerall);
		SetWindowText(hWndAll, (LPCWSTR)szBufferup);

		//LeaveCriticalSection(&cs1);
		ReleaseMutex(mutex1);
		
		Sleep(1000);
	};

	return 0;
};

DWORD WINAPI ThreadProc3(
	LPVOID lpParameter   // thread data				
)
{
	TCHAR szBufferup[10] = { 0 };

	//字符转数字TCHAR版
	DWORD dwTimerall = 0;
	DWORD dwTimer3 = 0;

	
	while (1)
	{

		//数组转字符
		//EnterCriticalSection(&cs1);
		WaitForSingleObject(mutex3, -1);
		GetWindowText(hWndAll, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimerall);

		GetWindowText(hWnd3, szBufferup, 10);
		_stscanf_s(szBufferup, TEXT("%d"), &dwTimer3);
		if (dwTimerall < 50)break;
		dwTimer3 += 50;

		dwTimerall -= 50;

		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimer3);
		SetWindowText(hWnd3, (LPCWSTR)szBufferup);
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimerall);
		SetWindowText(hWndAll, (LPCWSTR)szBufferup);

		//LeaveCriticalSection(&cs1);
		
		ReleaseMutex(mutex3);
		Sleep(1000);
	};

	return 0;
};

DWORD WINAPI ThreadProc(
	LPVOID lpParameter   // thread data				
)
{
	HANDLE handle[3];
	handle[0] = CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
	handle[1] = CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
	handle[2] = CreateThread(NULL, 0, ThreadProc3, NULL, 0, NULL);

	WaitForMultipleObjects(3, handle, TRUE, INFINITE);
	CloseHandle(mutex1);
	CloseHandle(mutex2);
	CloseHandle(mutex3);

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

		hWndAll = GetDlgItem(hWnd, IDC_EDIT_ALL);
		hWnd1 = GetDlgItem(hWnd, IDC_EDIT_1);
		hWnd2 = GetDlgItem(hWnd, IDC_EDIT_2);
		hWnd3 = GetDlgItem(hWnd, IDC_EDIT_3);


		SetWindowText(hWndAll, (LPCWSTR)TEXT("1000"));
		SetWindowText(hWnd1, (LPCWSTR)TEXT("0"));
		SetWindowText(hWnd2, (LPCWSTR)TEXT("0"));
		SetWindowText(hWnd3, (LPCWSTR)TEXT("0"));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON:

			HANDLE handle1 = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

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

