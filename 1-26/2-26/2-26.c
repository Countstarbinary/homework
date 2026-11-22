// 2-26.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>

//作业1
int __declspec(naked) plus(int x, int y, int z)
{
	_asm 
	{
		push ebp
		mov ebp,esp
		sub esp,0x40
		push edx
		push esi
		push edi
		lea edi,dword ptr ss:[ebp-0x40]
		mov ecx,0x10
		mov eax,0xcccccccc
		rep stosd

		mov dword ptr ss:[ebp-4],2
		mov dword ptr ss : [ebp - 8], 3
		mov dword ptr ss : [ebp - 0xc], 4

		mov eax,dword ptr ss:[ebp+8]
		add eax, dword ptr ss : [ebp + 0xc]
		add eax, dword ptr ss : [ebp + 0x10]
		add eax, dword ptr ss : [ebp - 4]
		add eax, dword ptr ss : [ebp - 8]
		add eax, dword ptr ss : [ebp - 0xc]

		pop edi
		pop esi
		pop edx
		mov esp,ebp
		pop ebp
		ret
	};

};

//作业3
int _stdcall fn3(int x,int y,int z) 
{
	return x + y + z;
};

int _cdecl fn4(int x, int y)
{
	return x + y;
};
int _fastcall fn2(int x, int y, int z, int n, int m)
{
	int a = x;
	int b = y;
	int c, d;
	c = fn3(a,b,z);
	d = fn4(a,b);

	return fn4(c,d);
};
int main()
{
	//int a=plus(1, 2, 3);
	//int a = fn2(1, 3, 4, 6,7);
	float i = 12.5f;
	int a = '中';
	printf("%f",i);
	getchar();
	return 0;
}

