#pragma once
#include <windows.h>
#include "4-28.h"

MoudleInfo processModule[0x128];
ProcessInfo processInfo[0x128];

void GetPeItem(TCHAR* FilePath, HWND hWnd);
void GetDirItem(TCHAR* FilePath, HWND hWnd);
void AddListProcessMoudle(HWND hDlg);
void AddListProcess(HWND hDlg);
void AddListProcessItem(HWND hDlg);
void AddListSection(HWND hDlg);

void AddListSectionItem(TCHAR* FilePath, HWND hDlg);

void AppendTextToEdit(TCHAR* newText, HWND hWnd);
