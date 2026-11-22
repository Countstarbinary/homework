// 2-3.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
//这里注意分配内存和索引有点区别
char age[5][10] = { 0 };


int arr1[8] = { 3, 5, 7, 9, 12, 25, 34, 55 };

int arr2[6] = { 4, 7, 9, 11, 13, 16 };

//归并排序
void * Mergesort(int arr1[],int arr2[],int lengtharr1,int lengtharr2)
{
	int k = 0;
	int n = 0;
	int m = 0;
	char * Newaddress=malloc((lengtharr1+lengtharr2)*4);
	memset(Newaddress,0, (lengtharr1 + lengtharr2) * 4);

	while (lengtharr1 + lengtharr2-m)
	{
		if ((lengtharr1 - k) && (lengtharr2 - n))
		{
			if (arr1[k] > arr2[n])
			{
				((int *)Newaddress)[m] = arr2[n];
				n++;
				m++;
			}
			else
			{
				((int *)Newaddress)[m] = arr1[k];
				k++;
				m++;
			};
		}
		else if (!(lengtharr1 - k ))
		{
			((int *)Newaddress)[m] = arr2[n];
			n++;
			m++;
		}
		else if (!(lengtharr2 - n))
		{
			((int *)Newaddress)[m] = arr1[k];
			k++;
			m++;
		};

	};
	return (int *)Newaddress;
};

int main()
{
	//age[1 * 10 + 6];
	//int i = 5;
	//while (i) 
	//{
	//	int k = 10;
	//	while (k) 
	//	{
	//		printf("%d ", age[5-i][10-k]);
	//		k--;
	//	};
	//	printf("%\n");
	//	i--;
	//};


	//int k = 10;
	//while (k)
	//{
	//	if (age[1][10 - k] > 20)
	//	{
	//		age[1][10 - k] = 21;
	//	};
	//	k--;
	//};

	//int i = 5;
	//while (i) 
	//{
	//	int k = 10;
	//	int temp = 0;
	//	while (k) 
	//	{
	//		temp += age[5 - i][10 - k];
	//		k--;
	//	};
	//	printf("%d \n", temp);
	//	i--;
	//};

	Mergesort(arr1, arr2, 8, 6);

	return 0;
}


