// 2-26.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

char* arr[10];

struct Student
{
	int x;
	int y;
};
void function()
{
	Student**** s;

	s = (Student****)100;
	printf("%d \n", s);
	s++;		//s的值是多少？	
	printf("%d \n", s);
	s = s + 2;		//s的值是多少？	
	printf("%d \n", s);
	s = s - 3;		//s的值是多少？	
	printf("%d \n", s);


	Student**** s1;
	Student**** s2;
	int x;

	s1 = (Student****)200;

	s2 = (Student****)100;

	x = s1 - s2;		//x的值是多少？	
	printf("%d \n", x);


	Student* s3;

	s3 = (Student*)100;

	s3++;		//s的值是多少？	
	printf("%d \n", s3);
	s3 = s3 + 2;		//s的值是多少？	
	printf("%d \n", s3);
	s3 = s3 - 3;		//s的值是多少？	
	printf("%d \n", s3);


	Student* s4;
	Student* s5;
	int y;

	s4 = (Student*)200;

	s5 = (Student*)100;

	y = s4 - s5;		//y的值是多少？	
	printf("%d \n",y);
};

int main()
{
	char a = 5;
	function();
	return 0;
}


