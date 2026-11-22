// 2-27.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
void Function()
{
	int arr[5] = { 1,2,3,4,5 };

	//..此处添加代码，使用指针，将数组的值倒置	
	int temp;
	for (int k = 0; k < 5/2; k++)
	{
		temp = *(arr + k);
		*(arr + k)=*(arr + 4 - k);
		*(arr + 4 - k) = temp;
	}

	//打印数组值的代码已经写完，不需要修改				
	for (int k = 0; k < 5; k++)
	{
		printf("%d\n", *(arr + k));
	}

}

int main()
{
	Function();
	return 0;

}

