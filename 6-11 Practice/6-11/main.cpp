// 6-11.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread>
#include "6-11.h"

extern "C" __declspec(dllimport) void WriteMessage(const char* msg);
BOOL TestDllWrite() 
{
	HMODULE hDll = LoadLibrary(L"sharedll.dll");
	typedef void(*WriteMessageProc)(const char*);
	WriteMessageProc WriteMessage = (WriteMessageProc)GetProcAddress(hDll, "WriteMessage");
	if (!hDll) {
		std::cerr << "Producer: 加载 DLL 失败，错误码 " << GetLastError() << std::endl;
		return 1;
	}

	const char* messages[] = { "Hello", "from", "Producer", nullptr };
	for (int i = 0; messages[i]; ++i) {
		WriteMessage(messages[i]);
		std::cout << "Producer: 写入 '" << messages[i] << "'" << std::endl;
		Sleep(1000);
	}
	Sleep(8000);
	FreeLibrary(hDll);
};
int main()
{
	
	
	//std::thread server(ServerThread);
	//std::thread client(ClientThread);
	//std::thread server(WriterThread);
	//std::thread client(ReaderThread);
	TestDllWrite();
}