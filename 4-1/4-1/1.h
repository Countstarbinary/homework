#pragma once

struct TEST
{
	int x;
	int y;

	void init(int x,int y) 
	{
		this->x = x;
		this->y = y;
	};

	int add(int z)
	{
		return x + y +z;
	};
	int sub()
	{
		return x - y;
	};
	int muti()
	{
		return x * y;
	};
	int div()
	{
		return x / y;
	};
};