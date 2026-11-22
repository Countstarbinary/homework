#include <iostream>
#include "9.h"




int arr1[] = {5,2,6,8,2,3};
char arr2[] = { 5,2,6,8,2,3 };
//struct test
//{
//	int x;
//	int y;
//	test() 
//	{
//		x = 1;
//		y = 2;
//	};
//};
//test t1, t2, t3, t4, t5, t6;
//
//test arr[] = { t1, t2, t3, t4, t5, t6 };
int main()
{
	//printf("%x %x %x %x %x %x\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
	Sort(arr1,6);
	Sort(arr2, 6);
	printf("%x %x %x %x %x %x\n", arr1[0], arr1[1], arr1[2], arr1[3], arr1[4], arr1[5]);
	return 0;
}
