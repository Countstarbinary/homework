// 3-9.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

int main()
{
	unsigned char x = 0;
	x |= 0xa4;

	unsigned char y=x & 0x4;

	unsigned char z = (x & 0xe0)>>5;


	return 0;
}


