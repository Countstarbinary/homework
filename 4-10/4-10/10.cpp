#include <Windows.h>

#include"10.h"



Number operator+(Number&  q, Number& p)
{
	int oflag = 0;
	__asm
	{
		pushfd
		pop oflag;
		and oflag, 0x800;
	}
	q.lowValue += p.lowValue;
	//判断低位溢出,有符号溢出
	__asm 
	{
		pushfd
		pop oflag;
		and oflag, 0x800;
	}
	if (oflag) 
	{
		q.highValue = q.highValue + p.highValue + 1;
	}
	else 
	{
		q.highValue +=  p.highValue;
	};
	return q;
};

Number operator-(Number&  q, Number& p)
{
	int oflag = 0;
	q.highValue -= p.highValue;
	//判断高位借位
	__asm
	{
		pushfd
		pop oflag;
		and oflag, 0x800;
	}
	if (oflag)
	{
		q.lowValue = q.lowValue - p.lowValue -1;
	}
	else
	{
		q.lowValue -= p.lowValue;
	};
	return q;
};

//只能进行64位乘法，还可能溢出
//不溢出改进思路1、64位变成128位.2、换个算法：乘数与被乘数的每一位相乘，再相加
Number operator*(Number&  q, T& p)
{
	int bitNum = 0;
	INT64* arr = (INT64*)malloc(USN_PAGE_SIZE);
	memset(arr,0, USN_PAGE_SIZE);
	INT64* ret = arr;
	//拆成4部分相加，这里一定不溢出
	for (int i=0;i<32;i++) 
	{
		if (p.lowValue&(1 << i)) 
		{
			*(arr+i) = (q.lowValue )<< i;                         

			*(arr+0x80 + i) = (INT64)(q.highValue) << i;   
		};
		if (p.highValue&(1 << i))
		{
			*(arr+0x100+ i) = (INT64)(q.lowValue) << i;    

			*(arr +0x180+ i) = (INT64)(q.highValue) << i;  
		};
	};
	arr = ret;
	//这里溢出
	int flag = 0;
	for (int i = 1; i < 0x200; i++)
	{
		flag = i / 0x400;
		switch (flag)
		{
		case 0:
			*(arr) += *(arr + i);
			break;
		case 1:
			*(arr) += (*(arr + i)<<32);
			break;
		case 2:
			*(arr) += (*(arr + i) << 32);
			break;
		case 3:
			*(arr) += (*(arr + i) << 64);
			break;
		};		
	};
	q.lowValue=*(int*)arr;
	q.highValue = *((int*)arr+1);
	free(arr);
	return q;
};

Number operator/(Number&  q, Number& p)
{
	int bitNum = 0;
	INT64* arr = (INT64*)malloc(USN_PAGE_SIZE);
	memset(arr, 0, USN_PAGE_SIZE);
	INT64* ret = arr;

	//拆成4部分相加，这里一定不溢出
	for (int i = 0; i < 32; i++)
	{
		if (p.lowValue&(1 << i))
		{
			*(arr + i) = (q.lowValue) >> i;

			*(arr + 0x80 + i) = (INT64)(q.highValue) >> i;
		};
		if (p.highValue&(1 << i))
		{
			*(arr + 0x100 + i) = (INT64)(q.lowValue) >> i;

			*(arr + 0x180 + i) = (INT64)(q.highValue) >> i;
		};
	};
	arr = ret;
	int flag = 0;
	for (int i = 1; i < 0x200; i++)
	{
		flag = i / 0x400;
		switch (flag)
		{
		case 0:
			*(arr) += *(arr + i);
			break;
		case 1:
			*(arr) += (*(arr + i) << 32);
			break;
		case 2:
			*(arr) += (*(arr + i) << 32);
			break;
		case 3:
			*(arr) += (*(arr + i) << 64);
			break;
		};
	};
	q.lowValue = *(int*)arr;
	q.highValue = *((int*)arr + 1);
	free(arr);
	return q;
};

bool operator>(Number&  q, Number& p)
{
	if (q.highValue > p.highValue)
	{
		return 1;
	};
	if (q.lowValue > p.lowValue) 
	{
		return 1;
	};
	return 0;
};

bool operator>=(Number&  q, Number& p)
{
	if (q.highValue >= p.highValue)
	{
		return 1;
	};
	if (q.lowValue >= p.lowValue)
	{
		return 1;
	};
	return 0;
};

bool operator<(Number&  q, Number& p)
{
	if (q.highValue < p.highValue)
	{
		return 1;
	};
	if (q.lowValue < p.lowValue)
	{
		return 1;
	};
	return 0;
};

bool operator<=(Number&  q, Number& p)
{
	if (q.highValue <= p.highValue)
	{
		return 1;
	};
	if (q.lowValue <= p.lowValue)
	{
		return 1;
	};
	return 0;
};

bool operator==(Number&  q, Number& p)
{
	if ((q.highValue == p.highValue) && (q.lowValue == p.lowValue))
	{
		return 1;
	};
	return 0;
};
