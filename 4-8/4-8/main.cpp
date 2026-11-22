// 4-8.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
class Base
{
public:
	int x;
public:
	Base()
	{
		x = 100;
	}
	void Function_1()
	{
		printf("Base:Function_1...\n");
	}
	virtual void Function_2()
	{
		printf("Base:Function_2...virtual\n");
	}
};
class Sub :public Base
{
public:
	int x;
public:
	Sub()
	{
		x = 200;
	}
	void Function_1()
	{
		printf("Sub:Function_1...\n");
	}
	virtual void Function_2()
	{
		printf("Sub:Function_2...virtual\n");
	}
};
void TestBound(Base* pb)
{
	int n = pb->x;
	printf("%x\n", n);

	pb->Function_1();		//函数调用	

	pb->Function_2();
}


int main()
{

	return 0;
}
