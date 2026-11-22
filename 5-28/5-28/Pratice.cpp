#pragma once
#include <windows.h>
#include <stdio.h>

BOOL CALLBACK EnumAllProc(HWND hWnd, LPARAM lParam)
{
	DWORD len = 0;
	TCHAR szTitle[MAX_PATH] = TEXT("Intermediate D3D Window");
	TCHAR szBuffer[MAX_PATH] = { 0 };
	//::GetWindowText(hWnd, szTitle, MAX_PATH);
	len = RealGetWindowClass(hWnd, szBuffer, MAX_PATH);
	if (len)
	{
		//if (wcscmp(szBuffer, szTitle) == 0) 
		//{
		//	MessageBox(NULL, szTitle, L"[顶层窗口]", MB_OK);
		//};
		MessageBox(NULL, szBuffer, L"[顶层窗口]", MB_OK);
	};

	return true;
}

HWND GetTargetWindowHwnd() 
{
	HWND hd = GetDesktopWindow();        //得到桌面窗口
	hd = GetWindow(hd, GW_CHILD);        //得到屏幕上第一个子窗口
	TCHAR s[200] = { 0 };
	int num = 1;
	while (hd != NULL)                    //循环得到所有的子窗口
	{
		memset(s, 0, 200);
		GetWindowText(hd, s, 200);
		printf("%ws\n", s);
		if (wcscmp(s, TEXT("QQ")) == 0)
		{
			break;
		};
		hd = GetNextWindow(hd, GW_HWNDNEXT);
	}
	return hd;
};

void account() 
{
	keybd_event(50, 0, 0, 0); //2
	keybd_event(50, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(57, 0, 0, 0); //9
	keybd_event(57, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(51, 0, 0, 0); //3
	keybd_event(51, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(53, 0, 0, 0);  //5
	keybd_event(53, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(52, 0, 0, 0);  //4
	keybd_event(52, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(57, 0, 0, 0);   //9
	keybd_event(57, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(54, 0, 0, 0);  //6
	keybd_event(54, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(50, 0, 0, 0);  //2
	keybd_event(50, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(50, 0, 0, 0); //2
	keybd_event(50, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	keybd_event(53, 0, 0, 0); //5
	keybd_event(53, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);
	
};

void PassWord()
{
	keybd_event(20, 0, 0, 0);
	keybd_event(20, 0, KEYEVENTF_KEYUP, 0);
	
	keybd_event(51, 0, 0, 0); //3
	keybd_event(51, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100);

	Sleep(100);

};
BOOL AutoLogin()
{
	//创建进程
	TCHAR szBuffer[MAX_PATH] = TEXT("C:\\Program Files\\Tencent\\QQNT\\QQ.exe");
	STARTUPINFO ie_si = { 0 };
	PROCESS_INFORMATION ie_pi;
	ie_si.cb = sizeof(ie_si);
	BOOL IsSuccess = CreateProcess(szBuffer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ie_si, &ie_pi);
	
	//获取窗口句柄，睡一下，不然可能得不到句柄。QQ的句柄用普通的FindWindow得不到，需要用下面这种方式获取
	Sleep(1000);
	HWND hd = GetTargetWindowHwnd();
	if (!hd) 
	{
		return 0;
	};

	//获取窗口坐标
	RECT Rect = { 0 };
	IsSuccess = GetWindowRect(hd, &Rect);

	//设置鼠标坐标，并点击用账号登录按钮
	//SetCursorPos(Rect.left+100, Rect.top+410);
	//Sleep(2000);
	//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	//设置鼠标坐标，清空账号
	//SetCursorPos(Rect.left+ 200, Rect.top+ 180);
	//Sleep(1000);
	//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	//SetCursorPos(Rect.left + 240, Rect.top + 185);
	//Sleep(1000);
	//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	//
	////设置鼠标坐标，并键入账号
	//SetCursorPos(Rect.left + 100, Rect.top + 180);
	//Sleep(1000);
	//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	//account();


	//设置鼠标坐标，并键入密码
	SetCursorPos(Rect.left + 100, Rect.top + 240);
	Sleep(1000);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	PassWord();

	Sleep(1000);

	//设置鼠标坐标，并点击同意服务协议
	SetCursorPos(Rect.left + 40, Rect.top + 290);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(1000);

	//设置鼠标坐标，点击登录按钮
	SetCursorPos(Rect.left + 100, Rect.top + 350);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 1;
};