// 3-4.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

//char arr1[10] = { 1,2,3,4,5,6,7,8,9,0 };
//char arr[999][888] = { 1,2,3,4,5 };
//void Function()
//{
//	char* m[999];
//	char(*ret)[888] = arr;
//	for (int i = 0; i < 999; i++)
//	{
//		m[i] = (char *)ret;
//		ret++;
//	};
//
//	//char(*px)[888];
//
//	char**p6;
//	p6 = (char**)m;
//
//
//	printf("%x %x \n", *(*(p6 + 0) + 1), p6[0][1]);
//
//	printf("%x %x \n", *(*(arr + 0) + 1), arr[0][1]);
//
//
//};

//void Function1() 
//{
//	char* p1;
//	char** p2;
//	char*** p3;
//	char**** p4;
//	char***** p5;
//	char****** p6;
//	char******* p7;
//
//	char x = 0;
//
//	p1 = &x;
//	p2 = &p1;
//	p3 = &p2;
//	p4 = &p3;
//	p5 = &p4;
//	p6 = &p5;
//	p7 = &p6;
//
//	printf("%x %x \n", *(p7+1), p7[1]);
//};

//使用数组指针遍历一个一维数组
//void Function2() 
//{
//	char(*p)[1];
//	p = (char(*)[1])arr1;
//	for (int i = 0; i < 10; i++)
//	{
//		printf("%d \n", **p);
//		p++;
//	};
//};






//下面部分验证，在 C 语言中，数组名 arr 在使用时会被隐式转换为指向数组首元素的指针。

struct stud
{
	int age;
	char clas;
	char* name;
};
stud st[5] = { 0 };
int* Fun[5] = { 0};

void Function(stud st[5])
{
	printf("%x \n",st);
	st++;
	printf("%x \n", st);
};

void Function1(int* Fun[5])
{
	printf("%x \n", Fun);
	Fun++;
	printf("%x \n", Fun);
};

int main()
{
	//Function1();
	//void(*p)();
	//p = Function2;
	//p();

	//Function(st);
	Function1(Fun);
	getchar();
	return 0;
}


