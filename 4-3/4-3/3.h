#pragma once
class DateInfo
{
	int year;
	int month;
	int day;
public:
	DateInfo(int year, int month, int day)
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
	void SetDay(int day);
	int GetDay();
	void SetMonth(int month);
	int GetMonth();
	void SetYear(int year);
	int GetYear();
	~DateInfo() {};
};

class TimeInfo:DateInfo
{
	int hour;
	int min;
	int sec;
public:
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
	void SetHour(int hour);
	int GetHour();
	void SetMin(int min);
	int GetMin();
	void SetSec(int sec);
	int GetSec();
	~TimeInfo() {};
};

class MyString
{
	char*arr = NULL;
public:
	MyString(int len)
	{
		arr = (char*)malloc(len);
	};
	MyString()
	{
		arr = (char*)malloc(1024);
	};

	void SetString(char* str);
	void PrintString();
	void AppendString(char* str);
	int Size();

	~MyString()
	{
		if (arr)free(arr);
	};
};

void test();

void test1();