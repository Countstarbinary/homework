// 4-1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "1.h"

int Myadd(int x,int y)
{
	return x + y;
};

struct non 
{

};

struct Person
{
	void Fn_1()
	{
		printf("Person:Fn_1()\n");
	}
	void Fn_2()
	{
		printf("Person:Fn_2()%x\n");
	}
};


int main()
{
	//TEST test;
	//test.init(10,2);

	//int ret=test.add(4);
	//printf("%d\n", ret);
	//Myadd(1,2);

	//non n = {};
	//int len = sizeof(non);

	//printf("%d\n", len);

	Person* p = NULL;
	p->Fn_1();
	p->Fn_2();


	return 0;
}

