// ProcessB.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>

const wchar_t* MAPPING_NAME = L"Global\MySharedFileMapping";
const size_t BUFFER_SIZE = 1024;
void ReaderThread() {
	// Give writer time to create and write
	Sleep(100);

	//HANDLE hMap = OpenFileMapping(
	//	FILE_MAP_READ,           // read access
	//	FALSE,                   // do not inherit
	//	MAPPING_NAME);           // name of mapping object
	//跨进程用创建，否则打开不了
	HANDLE hMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		nullptr,                 // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUFFER_SIZE,             // maximum object size (low-order DWORD)
		MAPPING_NAME);           // name of mapping object


	if (!hMap) {
		std::cerr << "Reader: OpenFileMapping failed, error " << GetLastError() << std::endl;
		return;
	}

	LPVOID pBuf = MapViewOfFile(
		hMap,                    // handle to mapping object
		FILE_MAP_READ,           // read permission
		0,
		0,
		BUFFER_SIZE);
	if (!pBuf) {
		std::cerr << "Reader: MapViewOfFile failed, error " << GetLastError() << std::endl;
		CloseHandle(hMap);
		return;
	}

	// Read and print message
	std::cout << "Reader: Received message: '";
	std::cout << reinterpret_cast<char*>(pBuf);
	std::cout << "'\n";

	// Clean up
	UnmapViewOfFile(pBuf);
	CloseHandle(hMap);
	std::cout << "Reader: Done.\n";
}

extern "C" __declspec(dllimport) const char* ReadMessage();

BOOL TestDllRead()
{
	HMODULE hDll = LoadLibrary(L"sharedll.dll");
	typedef const char*(*ReadMessageProc)();
	ReadMessageProc ReadMessage =(ReadMessageProc) GetProcAddress(hDll,"ReadMessage");
	if (!hDll) {
		std::cerr << "Consumer: 加载 DLL 失败，错误码 " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "开始读取 '"<< std::endl;
	for (int i = 0; i < 3; ++i) {
		const char* msg = ReadMessage();
		std::cout << "Consumer: 读取 '" << msg << "'" << std::endl;
	}

	FreeLibrary(hDll);
};
int main()
{
	//ReaderThread();
	TestDllRead();
    std::cout << "Hello World!\n";
	getchar();
}

