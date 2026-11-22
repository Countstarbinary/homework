// 2-2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
__int64 Function1()
{
	__int64 x = 0x1234567890;
	return x;
}

void Function2()
{
	int x = 1;
	int y = 2;
	int r;
	int arr[10] = { 1,2,3,4,5,6,7,8,9,10 };

	r = arr[1];
	r = arr[2];
	r = arr[3];
	r = arr[x];
	r = arr[x + y];
	r = arr[x * 2 + y];
};

//桶排序，返回排序后的地址
void * BucketSort(char arr[],int length)
{
	//找到数组中组大的数
	char tempmax = arr[length - 1];
	for (int i = 0; i < length-1; i++)
	{
		if (tempmax < arr[i])
		{
			tempmax = arr[i];
		};
	};

	//申请内存：函数内部不能申明未知长度的数组，只能申请内存了。内存释放要在外层调用的函数了
	char * MyArr=malloc(tempmax+1);    //此处应该*4转成int类型，不然char有点小了
	//清空内存
	memset(MyArr,0, tempmax + 1);     
	//遍历数组
	int index = 0;
	while (1)
	{
		if (arr[index])
		{
			MyArr[arr[index]]++;
		};
		if (index >= length)
		{
			break;
		}
		index++;
	};
	//排好序的内存                                  //此处应该*4转成int类型，不然char有点小了
	char * MyArr2 = malloc(length + 1);
	//清空内存
	memset(MyArr2, 0, length + 1);

	index = 0;
	while (1) 
	{
		//这里竟然还要进行数据转换，我是万万没想到的
		if (index>= (int)(tempmax + 1))
		{
			break;
		};
		if (MyArr[index])
		{
			while (MyArr[index])
			{
				*MyArr2 = index;
				MyArr[index]--;
				MyArr2++;
			};
			index++;
		}
		else 
		{
			index++;
		};
		
	}
	//释放内存MyArr
	free(MyArr);
	return MyArr2;
};

char arr3[] = { 2,4,6,5,3,1,2,7};

void test(const int x) 
{
	int y = x;
	int x = y;
	printf("%d\n",x);
};
int main()
{
	char arr[3] = { 1,2,3 };
	char arr2[4] = { 1,2,3,4 };
	//Function();
	BucketSort(arr3, 8);
	return 0;
}


