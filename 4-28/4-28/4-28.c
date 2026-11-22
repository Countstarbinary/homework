#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "4-28.h"
#include <psapi.h>
#include <stdio.h>

#pragma comment(lib, "psapi.lib")




void EnumProcessesNew(DWORD64 ProcessInfo)
{
	PProcessInfo ProcessInfoNew = (PProcessInfo)ProcessInfo;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{

		return;
	}
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(processEntry);

	int count = 0;
	if (Process32First(hSnapshot, &processEntry))
	{
		while (Process32Next(hSnapshot, &processEntry))
		{
			ProcessInfoNew->pid = processEntry.th32ProcessID;
			//ProcessInfoNew->name = me.szModule;
			
			wcscpy_s(ProcessInfoNew->name, 0X128, processEntry.szExeFile);
			ProcessInfoNew->NumberOfThreads = processEntry.cntThreads;
			ProcessInfoNew->Parentpid = processEntry.th32ParentProcessID;
			count++;
			ProcessInfoNew++;
		}
		CloseHandle(hSnapshot);
	}


};

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	// 1. 打开当前进程的访问令牌
	if (!OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		wprintf(L"OpenProcessToken 失败：%u\n", GetLastError());
		return FALSE;
	}

	// 2. 查 SeDebugPrivilege 对应的 LUID
	if (!LookupPrivilegeValue(
		NULL,
		SE_DEBUG_NAME,
		&luid))
	{
		wprintf(L"LookupPrivilegeValue 失败：%u\n", GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	// 3. 准备要调整的权限结构
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// 4. 调整令牌权限，开启 SeDebugPrivilege
	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(tp),
		NULL,
		NULL))
	{
		wprintf(L"AdjustTokenPrivileges 失败：%u\n", GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}
	// 5. 检查是否真的被分配
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		wprintf(L"令牌不包含 SeDebugPrivilege。\n");
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
};

void EnumMoudles(DWORD64 Moudleinfo, DWORD pid)
{
	EnableDebugPrivilege();
	PMoudleInfo MoudleInfoNew = (PMoudleInfo)Moudleinfo;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	DWORD err = GetLastError();
	wprintf(L"Snapshot 失败，错误码 %u\n", err);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return;
	}
	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(moduleEntry);
	TCHAR lpstrModuleFileName[260];

	int count = 0;
	if (Module32First(hSnapshot, &moduleEntry))
	{
		wcscpy_s(MoudleInfoNew->name, 0X128, moduleEntry.szModule);
		MoudleInfoNew->Imagebase = moduleEntry.modBaseAddr;
		MoudleInfoNew->Imagesize = moduleEntry.modBaseSize;
		MoudleInfoNew->MoudleId = moduleEntry.th32ModuleID;
		MoudleInfoNew++;

		while (Module32Next(hSnapshot, &moduleEntry))
		{
			count++;
			GetModuleFileName(moduleEntry.hModule, lpstrModuleFileName, sizeof(lpstrModuleFileName));
			
			wcscpy_s(MoudleInfoNew->name, 0X128, moduleEntry.szModule);
			MoudleInfoNew->Imagebase = moduleEntry.modBaseAddr;
			MoudleInfoNew->Imagesize = moduleEntry.modBaseSize;
			MoudleInfoNew->MoudleId = moduleEntry.th32ModuleID;
			MoudleInfoNew++;
		}
	}
};



