// 6-3.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

BOOL Inject(DWORD pid) 
{
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);

	if ((DWORD)pHandle == -1)
	{
		return 0;
	};
	TCHAR szBuffer[] = L"inject.dll";
	char* Address=(char*)VirtualAllocEx(pHandle,NULL,0x1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (!Address) 
	{
		return 0;
	};
	
	BOOL IsSuccess=WriteProcessMemory(pHandle, Address, szBuffer,sizeof(szBuffer),NULL);

	if (!IsSuccess) 
	{
		return 0;
	};

	HANDLE tHandle=CreateRemoteThread(pHandle,NULL,NULL, (LPTHREAD_START_ROUTINE)LoadLibrary, Address,NULL,NULL);
	if ((DWORD)tHandle == -1)
	{
		return 0;
	};
	WaitForSingleObject(tHandle,INFINITE);
	DWORD ExitCode;
	GetExitCodeThread(tHandle, &ExitCode);

	IsSuccess=VirtualFreeEx(pHandle, Address, 0x1000, MEM_RELEASE);
	

	CloseHandle(pHandle);
	CloseHandle(tHandle);
	return ExitCode;
};

//DLL卸载函数执行了，但是dll没有卸载成功，真是奇葩
/*
去掉DLL中的分支：
	//case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
*/
BOOL Deject(DWORD pid, DWORD HModule)
{
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if ((DWORD)pHandle == -1)
	{
		return 0;
	};


	HANDLE tHandle = CreateRemoteThread(pHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeLibrary, (LPVOID)HModule, NULL, NULL);
	if ((DWORD)tHandle == -1)
	{
		return 0;
	};
	WaitForSingleObject(tHandle, INFINITE);
	DWORD ExitCode;
	GetExitCodeThread(tHandle, &ExitCode);


	CloseHandle(pHandle);
	CloseHandle(tHandle);

};
int main()
{
	
	DWORD hmodlue=Inject(25768);
	Deject(25768, hmodlue);
	//HMODULE hmodule= LoadLibrary(L"inject.dll");
	return 0;
}
