// 1-29.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>

//判断数字是否是素数
int prime(int x)
{
	for (int i = 2; i < x;i++)
	{
		if ((x % i) == 0)
		{
			return 0;
		};
	};
	return 1;
};

//把数组中最大的元素放到数组末尾
void cmp(int length, char arr[])
{
	char temp = arr[length-1];
	int index = 0xFF;
	for (int i = 0; i < length; i++)
	{
		if (temp < arr[i])
		{
			temp = arr[i];
			index = i;
		};
	};
	if (index != 0xFF)
	{
		arr[index] = arr[length - 1];
		arr[length - 1] = temp;
	};

}

//简单的冒泡排序
void BubbleSort(int length, char arr[])
{

	for (int i = 0; i < length; i++)
	{
		cmp(length-i, arr);
	};
	
}

//判断数组是否对称
char Symmetry(int length, char arr[])
{
	for (int i = 0; i < length/2; i++)
	{
		if (arr[i] != arr[length - i-1]) 
		{
			return 0;
		};
	};
	return 1;
}

char arr[] = { 1,4,'a',6,7,10,8,9 };
char arr1[] = { 1,2,3,4,5,4,3,2,1 };
int main()
{
	//cmp(8,arr);
	//BubbleSort(8, arr);
	//char a=prime(89);

	char a = Symmetry(9, arr1);
	printf("%x\n", a);
	getchar();
	return 0;
}


