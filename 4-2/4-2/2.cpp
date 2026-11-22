#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DateInfo 
{
	int year;
	int month;
	int day;
	DateInfo(int year,int month,int day)
	{
		this->year = year;
		this->month = month;
		this->day = day;
	};
	DateInfo()
	{
		this->year = 2015;
		this->month = 4;
		this->day = 2;
	};
	int SetDay(int day)
	{
		this->day = day;
	};
	int GetDay()
	{
		return this->day;
	};
	int SetMonth(int month)
	{
		this->month = month;
	};
	int GetMonth()
	{
		return this->month;
	};
	int SetYear(int year)
	{
		this->year = year;
	};
	int GetYear() 
	{
		return this->year;
	};
	~DateInfo(){};
};


struct TimeInfo:DateInfo
{
	int hour;
	int min;
	int sec;
	TimeInfo(int hour, int min, int sec)
	{
		this->hour = hour;
		this->min = min;
		this->sec = sec;
	};
	TimeInfo()
	{
		this->hour = 15;
		this->min = 8;
		this->sec = 35;
	};
	int SetHour(int hour)
	{
		this->hour = hour;
	};
	int GetHour()
	{
		return this->hour;
	};
	int SetMin(int min)
	{
		this->min = min;
	};
	int GetMin()
	{
		return this->min;
	};
	int SetSec(int sec)
	{
		this->sec = sec;
	};
	int GetSec()
	{
		return this->sec;
	};
	~TimeInfo() {};
};

void test() 
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	TimeInfo data;


	DateInfo* dataptr = &data;

	year = dataptr->year;
	month = dataptr->month;
	day = dataptr->day;
	printf("%d/%d/%d\n", year, month, day);


	TimeInfo* timeptr = &data;

	hour = timeptr->hour;
	min = timeptr->min;
	sec = timeptr->sec;
	printf("%d/%d/%d\n", hour, min, sec);
};


struct MyString 
{
	char*arr=NULL;

	MyString(int len) 
	{
		arr=(char*)malloc(len);
	};
	MyString()
	{
		arr = (char*)malloc(1024);
	};

	void SetString(char* str) 
	{
		int i = 0;
		while (1)
		{
			if (!str[i])
			{
				arr[i] = str[i];
				break;
			}
			arr[i] = str[i];	
			i++;
		};
	};

	void PrintString()
	{
		if (arr) 
		{
			printf("%s\n", arr);
		};
	};

	void AppendString(char* str)
	{
		int i = 0;
		char* NewAddress = NULL;
		if (arr)
		{
			int len=strlen(arr);
			NewAddress = arr + len;

			while (1)
			{
				if (!str[i])
				{
					NewAddress[i] = str[i];
					break;
				}
				NewAddress[i] = str[i];
				i++;
			};
		};
	};

	
	int Size() 
	{
		int i = 0;
		int len = 0;
		if (arr)
		{

			while (1)
			{
				if (!arr[i])
				{
					len = i;
					break;
				}
				i++;
			};
		}
		return len;
	};

	~MyString()
	{
		if (arr)free(arr);
	};
};

void test1()
{
	MyString stri;
	stri.SetString((char*)"test");
	stri.PrintString();
	stri.AppendString((char*)"append");
	stri.PrintString();
	printf("字符串总长度%d\n", stri.Size());

};
