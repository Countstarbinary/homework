// 6-4 LoadMemory.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "6-4.h"
int main()
{
	//char name[] = "china";
	//int len=strlen_new(name);
	//wchar_t* wname = Asci_to_wchar(name);
	if (IsSelf)
	{

		//MessageBox(NULL, L"测试", L"不是本进程??", NULL);
		LoadMemory(22192);
		return 0;
	};

	MessageBox(NULL, L"测试", L"不是本进程", NULL);
	//printf("是否是本进程%x\n", IsSelf);
	for (int i=0;i<10;i++) 
	{
		MessageBox(NULL, L"测试", L"test", NULL);
	};
	return 0;
}
