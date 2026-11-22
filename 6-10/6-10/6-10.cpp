// 6-10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <assert.h>

typedef struct _HookData
{
	ULONG64 Address;
	UCHAR Arr[0x21];
}HookData, *PHookData;
UCHAR Hooknamber = 0;
HookData Hookdata[0x100] = { 0 };

PUCHAR HookFunction2(ULONG64 OwnerFunctionAddress, ULONG64 Address)
{

	//char shellcode0[14] = { 0 };
	char shellcode1[] = { 0x50,0x50,0x48,0xb8,0x0,0x0,0x0,0xb0,0x0,0x0,0x0,0x0,0x48,0xb8 };//push rax,push rax, mov rax,0, mov rax,objectfunction

	char shellcode2[] = { 0x48,0x83,0xc4,0x10,0x50,0x48,0x83,0xec,0x08,0x58,0xc3 }; //


	//保存原有数据，驱动卸载用。真是傻，不能返回局部变量
	Hookdata[Hooknamber].Address = Address;
	memcpy(Hookdata[Hooknamber].Arr, (char*)Address, 0x21);
	Hooknamber++;

	DWORD OldProtect;
	VirtualProtect((char*)Address,0x1000,PAGE_EXECUTE_READWRITE,&OldProtect);
	memcpy((char*)Address, shellcode1, 0xe); //覆盖成调用的硬编码
	memcpy((char*)Address + 14, &OwnerFunctionAddress, 0x8);
	memcpy((char*)Address + 22, shellcode2, 0xb);
	VirtualProtect((char*)Address, 0x1000, OldProtect, &OldProtect);
	return 0;
};



// Inline Hook
//   target     - 要 Hook 的函数地址
//   detour     - 跳转到的目标函数（你的替身函数）
//   origBytes  - 缓存原始字节，大小至少为 length
//   length     - 要覆盖的字节数（最少 5）
// 返回 TRUE 表示成功
BOOL InlineHook(void* target, void* detour, BYTE* origBytes, SIZE_T length)
{
	DWORD oldProtect;
	BYTE  patch[5];
	SIZE_T i;

	assert(length >= 5);

	// 1. 备份原始字节
	memcpy(origBytes, target, length);

	// 2. 修改页面保护，确保可写
	if (!VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &oldProtect))
		return FALSE;

	// 3. 构造 JMP 指令 (0xE9 + 相对偏移)
	//    rel = detour - target - 5
	*(BYTE*)(patch + 0) = 0xE9;
	*(DWORD*)(patch + 1) = (DWORD)((BYTE*)detour - (BYTE*)target - 5);

	// 4. 写入 JMP 指令
	memcpy(target, patch, sizeof(patch));

	// 5. 用 NOP 填充剩余字节（如果 length>5）
	for (i = 5; i < length; ++i) {
		*((BYTE*)target + i) = 0x90;
	}

	// 6. 刷新指令缓存
	FlushInstructionCache(GetCurrentProcess(), target, length);

	// 7. 恢复原保护
	VirtualProtect(target, length, oldProtect, &oldProtect);
	return TRUE;
}

// Unhook：恢复原始字节
BOOL InlineUnhook(void* target, const BYTE* origBytes, SIZE_T length)
{
	DWORD oldProtect;

	assert(length >= 5);

	// 1. 解除写保护
	if (!VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &oldProtect))
		return FALSE;

	// 2. 恢复原始字节
	memcpy(target, origBytes, length);

	// 3. 刷新指令缓存
	FlushInstructionCache(GetCurrentProcess(), target, length);

	// 4. 恢复保护
	VirtualProtect(target, length, oldProtect, &oldProtect);
	return TRUE;
}

/*---------------- 使用示例 ----------------*/

// 声明函数指针和缓存区
typedef int (WINAPI *PFN_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
static PFN_MessageBoxA gOrigMessageBoxA = NULL;
static BYTE             gOrigBytes[16];

// 自定义替身函数
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	// 在显示之前，修改一下传入参数
	return gOrigMessageBoxA(hWnd, "[Hooked]", lpCaption, uType);
}

void InstallHook()
{
	HMODULE hUser32 = GetModuleHandle(L"user32.dll");
	void*   pFunc = (void*)GetProcAddress(hUser32, "MessageBoxA");
	gOrigMessageBoxA = (PFN_MessageBoxA)pFunc;

	if (InlineHook(pFunc, MyMessageBoxA, gOrigBytes, sizeof(gOrigBytes))) {
		OutputDebugStringA("Hook Installed\n");
	}
	else {
		OutputDebugStringA("Hook Failed\n");
	}
}

void RemoveHook()
{
	HMODULE hUser32 = GetModuleHandleA("user32.dll");
	void*   pFunc = (void*)GetProcAddress(hUser32, "MessageBoxA");

	if (InlineUnhook(pFunc, gOrigBytes, sizeof(gOrigBytes))) {
		OutputDebugStringA("Unhooked\n");
	}
	else {
		OutputDebugStringA("Unhook Failed\n");
	}
}

int Add(int x,int y) 
{
	return x + y;
};

int NewAdd(int x, int y) 
{
	
	int ret= x + y;
	printf("%x\n", ret);
	return ret;
};
int main()
{
	//HookFunction2((ULONG64)NewAdd, (ULONG64)Add);

	//int ret = Add(2,3);

	//printf("%x\n",ret);

	InlineHook(Add, NewAdd, gOrigBytes,16);
	int ret = Add(2, 3);
	InlineUnhook(Add, gOrigBytes, 16);
	ret = Add(2, 3);
	return 0;
}


