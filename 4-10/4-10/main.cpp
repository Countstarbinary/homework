// 4-10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "10.h"
struct Base
{
	int x;
	int y;
	Base(int x, int y)
	{
		x = x;
		y = y;
	}
};
void PrintByPoint(Base* pb)
{
	printf("%d  %d\n", pb->x, pb->y);

	pb = (Base*)0x123456;

	//为所欲为...							
}
void PrintByRef(Base& refb, Base* pb)
{
	printf("%d  %d\n", refb.x, refb.y);
	Base b1(21, 31);
	//&refb = b1; //引用不能重新赋值							
	refb = b1;  //这个不是重新赋值，这个是把b1的值赋给refb代表的对象							
	printf("%d  %d\n", pb->x, pb->y);
}

//为了避免出现这种情况，可以将refb声明为常量，不可修改：
void PrintByRef(const Base& refb)
{
	//printf("%d  %d\n", refb.x, refb.y);
	//Base b1(21, 31);
	////&refb = b1; //引用不能重新赋值							
	////refb = b1;  //不允许			



}




int main()
{
	//Base base(1, 2);
	//int x = 8;
	////PrintByRef(base);
	//if (x >> 1) 
	//{
	//	int x = 9;
	//};

	Number n(0x7fffffff,0);
	Number m(0X2,0);
	n / m;

	n.print();

	
	return 0;
}

