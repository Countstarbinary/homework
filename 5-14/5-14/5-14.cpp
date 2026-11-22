// 5-7.cpp : 定义应用程序的入口点。
//
#include <stdio.h>
#include "framework.h"
#include "5-14.h"
#include "Resource.h"


/*
	这种写法线程严格同步，没有线程并发，所以没用到临界区进行资源互斥。
	如果按照视频中的生产者和消费者是需要用到互斥的
*/
#define MAX_LOADSTRING 100


HANDLE Event;
HANDLE EventEND[2];

HANDLE Eventasy[2];

HWND hWndResource;
HWND hWndBuffer1;
HWND hWndBuffer2;
HWND hWnda;
HWND hWndb;
HWND hWndc;
HWND hWndd;

BOOL EXIT=0;

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	EventEND[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	EventEND[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

	Eventasy[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	Eventasy[1] = CreateEvent(NULL, FALSE, FALSE, NULL);


	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)WndProc);

	return 0;
}



DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{
	SetEvent(Event);

	return 0;
};

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	TCHAR szBufferup[0x128] = { 0 };
	TCHAR szBuffer1[0x128] = { 0 };
	TCHAR szCHAR[2] = { 0 };
	//字符转数字TCHAR版
	DWORD dwTimerResource = 0;
	DWORD len = 0;
	DWORD len1 = 0;
	while (1)
	{
		//数组转字符
		WaitForSingleObject(Event, INFINITE);
		GetWindowText(hWndResource, szBufferup, 0x128);
		GetWindowText(hWndBuffer1, szBuffer1, 0x128);

		//VS6++默认编码是asci，所以视频是用的strlen函数。而不是这个函数兼容宽字符
		len = wcslen(szBufferup);
		len1 = wcslen(szBuffer1);
		if (!len&&!len1)
		{
			SetEvent(Event);
			break;
		};
		if (len1)
		{
			Sleep(1000);
			SetEvent(Event);
			continue;
		};
		if (len)
		{
			szCHAR[0] = szBufferup[len - 1];

			SetWindowText(hWndBuffer1, szCHAR);

			szBufferup[len - 1] = 0;
			SetWindowText(hWndResource, szBufferup);
		};

		SetEvent(Event);
		SetEvent(Eventasy[0]);
		Sleep(1000);
	};
	SetEvent(EventEND[0]);
	return 0;
};

DWORD WINAPI ThreadProc3(LPVOID lpParameter)
{
	TCHAR szBufferup[0x128] = { 0 };
	TCHAR szBuffer2[0x128] = { 0 };
	TCHAR szCHAR[2] = {0};
	//字符转数字TCHAR版
	DWORD dwTimerResource = 0;
	DWORD len = 0;
	DWORD len1 = 0;
	while (1)
	{
		//数组转字符
		WaitForSingleObject(Event, INFINITE);
		GetWindowText(hWndResource, szBufferup, 0x128);
		GetWindowText(hWndBuffer2, szBuffer2, 0x128);

		len = wcslen(szBufferup);
		len1 = wcslen(szBuffer2);
		if (!len && !len1)
		{
			SetEvent(Event);
			break;
		};

		if (len1) 
		{
			Sleep(1000);
			SetEvent(Event);
			
			
			continue;
		};
		if (len)
		{
			szCHAR[0] = szBufferup[len - 1];

			SetWindowText(hWndBuffer2, szCHAR);

			szBufferup[len - 1] = 0;
			SetWindowText(hWndResource, szBufferup);
		};

		SetEvent(Event);
		SetEvent(Eventasy[1]);

		
		Sleep(1000);
	};
	SetEvent(EventEND[1]);
	return 0;
};


DWORD WINAPI ThreadProc4(LPVOID lpParameter)
{
	HWND hwnd;
	TCHAR szBuffer[0x128] = { 0 };
	TCHAR szBuffer1[0x128] = { 0 };
	TCHAR szBuffer2[0x128] = { 0 };
	DWORD len1 = 0;
	DWORD len2 = 0;
	hwnd = (HWND)lpParameter;
	DWORD dzIndex;
	while (1) 
	{
		if (EXIT) 
		{
			SetEvent(Eventasy[1]);
			break;
		};

		//返回值的妙用
		dzIndex = WaitForMultipleObjects(2, Eventasy, FALSE, INFINITE);

		
		if (dzIndex==0)
		{

			GetWindowText(hWndBuffer1, szBuffer1, 0x128);
			GetWindowText(hwnd, szBuffer, 0x128);
			wcscat_s(szBuffer, TEXT("-"));
			wcscat_s(szBuffer, szBuffer1);
			SetWindowText(hwnd, szBuffer);


			SetWindowText(hWndBuffer1,NULL);
			
			
			Sleep(1000);
			continue;
		};
		

		
		if (dzIndex == 1)
		{
			
			GetWindowText(hwnd, szBuffer, 0x128);
			GetWindowText(hWndBuffer2, szBuffer2, 0x128);
			wcscat_s(szBuffer, TEXT("-"));
			wcscat_s(szBuffer, szBuffer2);

			SetWindowText(hwnd, szBuffer);
			SetWindowText(hWndBuffer2, NULL);
			
			Sleep(1000);
			continue;
		};

	};

	return 0;
};

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	HANDLE handle[7];
	handle[0] = CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
	handle[1] = CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
	handle[2] = CreateThread(NULL, 0, ThreadProc3, NULL, 0, NULL);
	
		
		
		
	handle[3] = CreateThread(NULL, 0, ThreadProc4, hWnda, 0, NULL);
	handle[4] = CreateThread(NULL, 0, ThreadProc4, hWndb, 0, NULL);
	handle[5] = CreateThread(NULL, 0, ThreadProc4, hWndc, 0, NULL);
	handle[6] = CreateThread(NULL, 0, ThreadProc4, hWndd, 0, NULL);


	WaitForMultipleObjects(2,EventEND, TRUE, INFINITE);
	EXIT = 1;
	SetEvent(Eventasy[1]);//通知dzIndex = WaitForMultipleObjects(2, Eventasy, FALSE, INFINITE);避免卡死
	WaitForMultipleObjects(7, handle, TRUE, INFINITE);
	CloseHandle(handle[0]);
	CloseHandle(handle[1]);
	CloseHandle(handle[2]);
	CloseHandle(handle[3]);
	CloseHandle(handle[4]);
	CloseHandle(handle[5]);
	CloseHandle(handle[6]);
	CloseHandle(Event);
	CloseHandle(EventEND[0]);
	CloseHandle(EventEND[1]);
	CloseHandle(Eventasy[0]);
	CloseHandle(Eventasy[1]);

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
		hWndResource = GetDlgItem(hWnd, IDC_EDIT_RESOURCE);
		hWndBuffer1 = GetDlgItem(hWnd, IDC_EDIT_BUFFER1);
		hWndBuffer2 = GetDlgItem(hWnd, IDC_EDIT_BUFFER2);
		hWnda = GetDlgItem(hWnd, IDC_EDIT_A);
		hWndb = GetDlgItem(hWnd, IDC_EDIT_B);
		hWndc = GetDlgItem(hWnd, IDC_EDIT_C);
		hWndd = GetDlgItem(hWnd, IDC_EDIT_D);



		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON:

			HANDLE handle1 = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
			CloseHandle(handle1);
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

