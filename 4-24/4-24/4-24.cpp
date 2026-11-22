// 4-24.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "4-24.h"

#define MAX_LOADSTRING 100

HINSTANCE HInstance;
BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message			
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HInstance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MY424_DIALOG), NULL, (DLGPROC)WndProc);

	return 0;
}




BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HICON hIOC=LoadIcon(HInstance, MAKEINTRESOURCE(IDI_SMALL));
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (DWORD)hIOC);

		
		HANDLE Image=LoadImage(HInstance, MAKEINTRESOURCE(IDB_PNG1), IMAGE_ICON, 1920, 1080, LR_VGACOLOR);
		SendMessage(hWnd, STM_SETIMAGE, ICON_SMALL, (DWORD)Image);
	};
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON3:
					MessageBox(NULL, NULL, NULL, MB_OK);
					return TRUE;
			};
			break;
		}
		
	}
	return FALSE;
}




