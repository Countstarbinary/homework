// 4-28.cpp : 定义应用程序的入口点。
//
#pragma once
#include "framework.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include "4-28.h"
#include "UI.h"



#define MAX_LOADSTRING 100

TCHAR FilePath[256];

TCHAR DetailFlags = 0;

BOOL CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//主页面消息处理函数
BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME szOpenfilename = { 0 };
	switch (message)
	{
	case WM_INITDIALOG:
		AddListProcess(hWnd);
		AddListProcessItem(hWnd);

		AddListMoudle(hWnd);

		return (INT_PTR)TRUE;

	case WM_NOTIFY:
	{
		NMHDR * Nmhdr = (NMHDR *)lParam;
		switch (wParam)
		{
		case IDC_LIST_PROCESS:
			if (Nmhdr->code == NM_CLICK)
			{

				AddListProcessMoudle(hWnd);
			};
			return TRUE;
		};

	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ABOUT:
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWnd, (DLGPROC)AboutProc);
			return TRUE;
		case IDC_BUTTON_PEVIEW:
			TCHAR szPeFileText[100] = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
			TCHAR szFileName[256] = { 0 };
			szOpenfilename.lStructSize = sizeof(OPENFILENAME);
			szOpenfilename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			szOpenfilename.hwndOwner = hWnd;
			szOpenfilename.lpstrFilter = szPeFileText;
			szOpenfilename.lpstrFile = FilePath;
			szOpenfilename.nMaxFile = MAX_PATH;
			GetOpenFileName(&szOpenfilename);
			//FilePath = szFileName;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_PEINFO), hWnd, (DLGPROC)PEProc);
			return TRUE;
		case IDC_BUTTON_EXIT:
			EndDialog(hWnd, 0);
			return TRUE;
		};


	}
	break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}

	return FALSE;
}

//关于页面消息处理函数
BOOL CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		//return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:  // 处理"确定"按钮
		case IDCANCEL: // 处理ESC/关闭
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	}
	return FALSE; // 未处理的消息交给系统
}

//节表页面消息处理函数
BOOL CALLBACK SectionProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		AddListSection(hWnd);
		AddListSectionItem(FilePath, hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PESECTION:

			return TRUE;
		case IDCANCEL: // 处理ESC/关闭
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	}
	return FALSE; // 未处理的消息交给系统
}

//详细页面消息处理函数
BOOL CALLBACK DetailProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	char* FileBuffer = NULL;

	BOOL flag = 0;
	switch (message)
	{
	case WM_INITDIALOG:

		flag = FileToFileBuffer(FilePath, &FileBuffer);
		switch (DetailFlags)
		{
		case IDC_BUTTON_DIR_EXPORT:
			OutputExportDirtory(FileBuffer, hWnd);
			return TRUE;
		case IDC_BUTTON_DIR_IMPORT:
			OutputImportDirtory(FileBuffer, hWnd);
			return TRUE;
		case IDC_BUTTON_DIR_RESOURCE:
			OutputResourceDirtory(FileBuffer, hWnd);
			return TRUE;
		case IDC_BUTTON_DIR_RELOCA:
			OutputReloacationDirtory(FileBuffer, hWnd);
			return TRUE;
		case IDC_BUTTON_DIR_BOUNDIMPORT:
			OutputBoundImportDirtory(FileBuffer, hWnd);
			return TRUE;
		case IDC_BUTTON_DIR_IAT:
			OutputIATDirtory(FileBuffer, hWnd);
			return TRUE;
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PESECTION:

			return TRUE;
		case IDCANCEL: // 处理ESC/关闭
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	}
	return FALSE; // 未处理的消息交给系统
}

//目录页面消息处理函数
BOOL CALLBACK DirProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		GetDirItem(FilePath, hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_DIR_EXPORT:
			DetailFlags = IDC_BUTTON_DIR_EXPORT;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);

			return TRUE;
		case IDC_BUTTON_DIR_IMPORT:
			DetailFlags = IDC_BUTTON_DIR_IMPORT;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);
			return TRUE;
		case IDC_BUTTON_DIR_RESOURCE:
			DetailFlags = IDC_BUTTON_DIR_RESOURCE;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);
			return TRUE;
		case IDC_BUTTON_DIR_RELOCA:
			DetailFlags = IDC_BUTTON_DIR_RELOCA;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);
			return TRUE;
		case IDC_BUTTON_DIR_BOUNDIMPORT:
			DetailFlags = IDC_BUTTON_DIR_BOUNDIMPORT;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);
			return TRUE;
		case IDC_BUTTON_DIR_IAT:
			DetailFlags = IDC_BUTTON_DIR_IAT;
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hWnd, (DLGPROC)DetailProc);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	}
	return FALSE; // 未处理的消息交给系统
}

//PE页面消息处理函数
BOOL CALLBACK PEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:


		GetPeItem(FilePath, hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PEDIR:
			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_DIR), hWnd, (DLGPROC)DirProc);
			return TRUE;
		case IDC_BUTTON_PESECTION:

			DialogBox(HInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTION), hWnd, (DLGPROC)SectionProc);
			return TRUE;
		case IDC_BUTTON_PECLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}
	return FALSE; // 未处理的消息交给系统
}



int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HInstance = hInstance;

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, (DLGPROC)WndProc);
	return 0;
}







