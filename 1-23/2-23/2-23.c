// 2-23.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
char contoresult[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

void function(int base,int number )
{

	char numeral[16] = { 0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f' };
	char index = 0;

	char m, n;

	m = number % base;
	n=number / base;

	while (1)
	{
		contoresult[index] = numeral[m];
		if (n < base)
		{
			contoresult[index+1] = numeral[n];
			break;
		};
		index++;
		m = n % base;
		n = n / base;
	};

};
int main()
{
	function(16, 1000);
	printf("%c", *(int*)contoresult);
	getchar();
	return 0;
}
