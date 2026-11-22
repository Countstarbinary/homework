#include <windows.h>

void FindTargetWindow() 
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		GetWindowText(hwnd, szTitle, MAX_PATH);
		//修改窗口标题							
		::SetWindowText(hwnd, TEXT("新的窗口标题"));
		GetWindowText(hwnd, szTitle, MAX_PATH);
	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	}

};

void ControlWindow()
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		typedef void (WINAPI *PSWITCHTOTHISWINDOW) (HWND, BOOL);
		PSWITCHTOTHISWINDOW SwitchToThisWindow;
		HMODULE hUser32 = LoadLibrary(TEXT("user32.dll"));
		SwitchToThisWindow = (PSWITCHTOTHISWINDOW)GetProcAddress(hUser32, "SwitchToThisWindow");
		SwitchToThisWindow(hwnd,TRUE);
		//SwitchToThisWindow(hwnd, FALSE);
		
	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	}
	TCHAR szTitle1[MAX_PATH] = { 0 };
	HWND hwnd1 = ::FindWindow(TEXT("No need to disasm the code!"), TEXT("CrackMe v1.0"));
	
	SwitchToThisWindow(hwnd1, TRUE);
	SwitchToThisWindow(hwnd1, FALSE);

	HWND hwnd2 = ::FindWindow(NULL,NULL);
	SwitchToThisWindow(hwnd2, TRUE);
};

void FindChileWindow() 
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		//查找子窗口						
		//HWND hEdit = FindWindowEx(hwnd, NULL, L"Edit", L"");
		//HWND hEdit = ::FindWindow(TEXT("Edit"), TEXT(""));
		//设置标题						
		//::SetWindowText(hEdit, L"文本框新的标题");
		//修改内容						
		::SendMessage((HWND)0x00130C4E, WM_SETTEXT, 0, (LPARAM)L"新的内容");

		GetWindowText((HWND)0x00130C4E, szTitle, MAX_PATH);
	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	}

};

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	::GetWindowText(hWnd, szTitle, MAX_PATH);
	MessageBox(NULL, szTitle, L"[子窗口]", MB_OK);
	return true;
}

VOID EnumChildWindow()
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		EnumChildWindows(hwnd, EnumChildProc,0);
	}
	else 
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	};
}

BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	::GetWindowText(hWnd, szTitle, MAX_PATH);
	MessageBox(NULL, szTitle, L"[顶层窗口]", MB_OK);
	return true;
}

VOID EnumWindow()
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		EnumWindows(EnumProc, 0);
	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	};
}

void SimulateMouse() 
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		HWND Button = FindWindowEx(hwnd, NULL, L"Button", L"刷新(&R)");
		RECT Rect = { 0 };
		BOOL IsSuccess=GetWindowRect(Button, &Rect);
		Sleep(2000);
		SetCursorPos(Rect.left+10, Rect.top+10);
		//Sleep(2000);
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		//Sleep(2000);
	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	};

};

void Simulatekb()
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	HWND hwnd = ::FindWindow(TEXT("#32770"), TEXT("飞鸽传书  IP Messenger"));
	if (hwnd != NULL)
	{
		HWND hEdit = FindWindowEx(hwnd, NULL, L"Edit", L"");
		RECT Rect = { 0 };
		BOOL IsSuccess = GetWindowRect(hEdit, &Rect);
		//Sleep(2000);
		SetCursorPos(Rect.left + 10, Rect.top + 10);
		//Sleep(2000);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		//Sleep(2000);

		keybd_event(97, 0, 0, 0);
		keybd_event(97, 0, KEYEVENTF_KEYUP, 0);
		Sleep(1000);
		keybd_event(66, 0, 0, 0);
		keybd_event(66, 0, KEYEVENTF_KEYUP, 0);
		Sleep(1000);
		keybd_event(16, 0, 0, 0);
		keybd_event(67, 0, 0, 0);
		keybd_event(67, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(16, 0, KEYEVENTF_KEYUP, 0);

	}
	else
	{
		::MessageBox(NULL, TEXT("窗口没有找到"), TEXT("[ERROR]"), MB_OK);
	};

};

