// 4-2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "2.h"

struct X
{
	int a;
	int b;
};
struct Y
{
	int a;
	int d;
};
struct Z :X, Y
{
	int e;
	int f;
};


int main()
{
	//test();
	//test1();

	Z z;

	z.X::a = 1;
	z.b = 2;
	z.Y::a = 3;
	z.d = 4;
	z.e = 5;
	z.f = 6;

	Y* Yptr = &z;
	X* Xptr = &z;
	printf("%d\n", Yptr->a);
	printf("%d\n", Xptr->a);


	return 0;
}
