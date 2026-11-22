// 5-18.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

VOID TestCreateProcessByAPPName()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szApplicationName[] = TEXT("c://program files//internet explorer//iexplore.exe");

	BOOL res = CreateProcess(
		szApplicationName,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);

}

VOID TestCreateProcessByCmdline()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT("c://program files//internet explorer//iexplore.exe http://www.ifeng.com");

	BOOL res = CreateProcess(
		NULL,
		szCmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
}
VOID TestCreateProcess()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT(" http://www.ifeng.com");

	BOOL res = CreateProcess(
		TEXT("c://program files//internet explorer//iexplore.exe"),
		szCmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
}
VOID TestCreate()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT(" http://www.ifeng.com");

	BOOL res = CreateProcess(
		NULL,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
}
//主函数的参数，可以没有，创建程序的时候就默认无参数。也可以像下面这样加上，也是可以收到参数的。
int main(int argc,char*argv[])
{
	////字符串指针数组，就是一组指针
	//printf("%s\n",argv[0]);
	//printf("%s\n", argv[1]);
	//printf("%s\n", argv[2]);
	//printf("%s\n", argv[3]);


	//TestCreateProcessByAPPName();
	//TestCreateProcessByCmdline();
	//TestCreateProcess();
	TestCreate();
	system("pause");
	return 0;
}


