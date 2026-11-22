#pragma once
#include <stdio.h>
template<class T>
class Number
{
private:
	int lowValue;
	int highValue;
public:
	Number() 
	{
		lowValue = 0x12345678;
		highValue = 0x12345678;
	};
	Number(int x,int y)
	{
		lowValue = x;
		highValue = y;
	};
	void print() 
	{
		printf("lowValue: %x  highValue: %x\n", lowValue, highValue);
	};
	friend Number operator+(Number& q, Number& p);
	friend Number operator-(Number&  q, Number& p);
	friend Number operator*(Number&  q, T& p);
	friend Number operator/(Number&  q, Number& p);
	friend bool operator>(Number&  q, Number& p);
	friend bool operator>=(Number&  q, Number& p);
	friend bool operator<(Number&  q, Number& p);
	friend bool operator<=(Number&  q, Number& p);
	friend bool operator==(Number&  q, Number& p);
};