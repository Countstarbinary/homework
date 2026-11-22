
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>
const wchar_t* PIPE_NAME = L"\\.\pipe\MyNamedPipe";

void ServerThread()
{
	// Create a named pipe instance
	HANDLE hPipe = CreateNamedPipe(
		L"\\.\pipe\MyNamedPipe",                   // pipe name
		PIPE_ACCESS_DUPLEX,          // read/write access
		PIPE_TYPE_MESSAGE |          // message-type pipe
		PIPE_READMODE_MESSAGE |      // message-read mode
		PIPE_WAIT,                   // blocking mode
		1,                           // max. instances
		512,                         // output buffer size
		512,                         // input buffer size
		NMPWAIT_USE_DEFAULT_WAIT,                           // default timeout
		nullptr);                    // default security attributes

	if (hPipe == INVALID_HANDLE_VALUE) {
		GetLastError();
		return;
	}

	printf("Server: Waiting for client connection...\n");
	BOOL connected = ConnectNamedPipe(hPipe, nullptr) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (!connected) {
		printf("Server: ConnectNamedPipe failed, error \n");
		CloseHandle(hPipe);
		return;
	}

	printf("Server: Client connected.\n");

	// Receive a message from client
	wchar_t buffer[128] = { 0 };
	DWORD bytesRead = 0;
	if (ReadFile(hPipe, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, nullptr)) {
		printf("Server received: \n");
	}
	else {
		printf("Server: ReadFile failed, error\n");
	}

	// Send a reply
	const wchar_t* reply = L"Hello from server thread!";
	DWORD bytesWritten = 0;
	if (!WriteFile(hPipe, reply, (lstrlenW(reply) + 1) * sizeof(wchar_t), &bytesWritten, nullptr)) {
		printf("Server: WriteFile failed, error \n");
	}

	// Clean up
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	printf("Server: Done.\n");
}

void ClientThread()
{
	// Give server time to start
	Sleep(10000);

	HANDLE hPipe = CreateFile(
		PIPE_NAME,               // pipe name
		GENERIC_READ | GENERIC_WRITE,  // read/write access
		0,                       // no sharing
		nullptr,                 // default security attributes
		OPEN_EXISTING,           // opens existing pipe
		0,                       // default attributes
		nullptr);                // no template file

	if (hPipe == INVALID_HANDLE_VALUE) {
		printf("Client: CreateFile failed, error \n");
		return;
	}

	printf("Client: Connected to server.\n");

	// Send a message to server
	const wchar_t* msg = L"Hello from client thread!";
	DWORD bytesWritten = 0;
	if (!WriteFile(hPipe, msg, (lstrlenW(msg) + 1) * sizeof(wchar_t), &bytesWritten, nullptr)) {
		printf("Client: WriteFile failed, error \n");
	}

	// Receive server reply
	wchar_t buffer[128] = { 0 };
	DWORD bytesRead = 0;
	if (ReadFile(hPipe, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, nullptr)) {
		printf("Client received: \n");
	}
	else {
		printf("Client: ReadFile failed, error \n");
	}

	// Clean up
	CloseHandle(hPipe);
	printf("Client: Done.\n");
}

const wchar_t* MAPPING_NAME = L"Global\MySharedFileMapping";
const size_t BUFFER_SIZE = 1024;

// Writer thread: creates file mapping and writes a message
void WriterThread() {
	// Create file mapping in memory (backed by system paging file)
	HANDLE hMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		nullptr,                 // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUFFER_SIZE,             // maximum object size (low-order DWORD)
		MAPPING_NAME);           // name of mapping object

	if (!hMap) {
		std::cerr << "Writer: CreateFileMapping failed, error " << GetLastError() << std::endl;
		return;
	}

	// Map view of file to this process's address space
	LPVOID pBuf = MapViewOfFile(
		hMap,                    // handle to mapping object
		FILE_MAP_WRITE,          // read/write permission
		0,
		0,
		BUFFER_SIZE);
	if (!pBuf) {
		std::cerr << "Writer: MapViewOfFile failed, error " << GetLastError() << std::endl;
		CloseHandle(hMap);
		return;
	}

	// Write a message
	const char* message = "Hello from writer thread via shared file mapping!";
	strcpy_s(reinterpret_cast<char*>(pBuf), BUFFER_SIZE, message);
	std::cout << "Writer: Message written.\n";

	// Keep alive for reader。这里用事件通知更好
	Sleep(5000);

	// Clean up
	UnmapViewOfFile(pBuf);
	CloseHandle(hMap);
	std::cout << "Writer: Done.\n";
}

// Reader thread: opens existing file mapping and reads the message
void ReaderThread() {
	// Give writer time to create and write
	Sleep(100);

	HANDLE hMap = OpenFileMapping(
		FILE_MAP_READ,           // read access
		FALSE,                   // do not inherit
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