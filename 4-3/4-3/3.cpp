#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "3.h"

void DateInfo::SetDay(int day)
{
	this->day = day;
};
int DateInfo::GetDay()
{
	return this->day;
};
void DateInfo::SetMonth(int month)
{
	this->month = month;
};
int DateInfo::GetMonth()
{
	return this->month;
};
void DateInfo::SetYear(int year)
{
	this->year = year;
};
int DateInfo::GetYear()
{
	return this->year;
};



void TimeInfo::SetHour(int hour)
{
	this->hour = hour;
};
int TimeInfo::GetHour()
{
	return this->hour;
};
void TimeInfo::SetMin(int min)
{
	this->min = min;
};
int TimeInfo::GetMin()
{
	return this->min;
};
void TimeInfo::SetSec(int sec)
{
	this->sec = sec;
};
int TimeInfo::GetSec()
{
	return this->sec;
};

typedef int (*GetDayProc)();

void test()
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	TimeInfo data;
	//GetDayProc Myday=(GetDayProc)(TimeInfo::GetDay);
	
	//printf("Myday%d\n", Myday());
	int* dataptr = (int*)&data;
	
	year = *dataptr;
	month = *(dataptr+1);
	day = *(dataptr + 1);
	printf("%d/%d/%d-", year, month, day);


	TimeInfo* timeptr = &data;

	hour = timeptr->GetHour();
	min = timeptr->GetMin();
	sec = timeptr->GetSec();
	printf("%d/%d/%d\n", hour, min, sec);
};



void MyString::SetString(char* str)
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

void MyString::PrintString()
{
	if (arr)
	{
		printf("%s\n", arr);
	};
};

void MyString::AppendString(char* str)
{
	int i = 0;
	char* NewAddress = NULL;
	if (arr)
	{
		int len = strlen(arr);
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

int MyString::Size()
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


void test1()
{
	MyString stri;
	stri.SetString((char*)"test");
	stri.PrintString();
	stri.AppendString((char*)"append");
	stri.PrintString();
	printf("字符串总长度%d\n", stri.Size());

};
