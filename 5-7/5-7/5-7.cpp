// 5-7.cpp : 定义应用程序的入口点。
//
#include <stdio.h>
#include "framework.h"
#include "5-7.h"
#include "Resource.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN),NULL, (DLGPROC)WndProc);

    return 0;
}

HWND hWndUP;
HWND hWndDown;

DWORD WINAPI ThreadProc(
	LPVOID lpParameter   // thread data				
) 
{
	TCHAR szBufferup[10] = { 0 };
	TCHAR szBufferdown[10] = { 0 };
	GetWindowText(hWndUP, szBufferup,10);
	GetWindowText(hWndDown, szBufferdown,10);

	//字符转数字TCHAR版
	DWORD dwTimerup=0;
	_stscanf_s(szBufferup, TEXT("%d"), &dwTimerup);

	DWORD dwTimerdown = 0;
	_stscanf_s(szBufferdown, TEXT("%d"), &dwTimerdown);
	while (dwTimerup<1000 && dwTimerdown>0)
	{
		dwTimerup++;
		dwTimerdown--;
		//数组转字符
		swprintf(szBufferup, _countof(szBufferup), TEXT("%d\r\n"), dwTimerup);
		swprintf(szBufferdown, _countof(szBufferdown), TEXT("%d\r\n"), dwTimerdown);
		SetWindowText(hWndUP, (LPCWSTR)szBufferup);
		SetWindowText(hWndDown, (LPCWSTR)szBufferdown);
		Sleep(1000);
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

				HANDLE handle=CreateThread(NULL,0, ThreadProc,NULL,0,NULL);
				CloseHandle(handle);
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

