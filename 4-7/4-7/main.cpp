
#include <iostream>

//struct Base
//{
//public:
//	virtual void Function_1()
//	{
//		printf("Base:Function_1...\n");
//	}
//	virtual void Function_2()
//	{
//		printf("Base:Function_2...\n");
//	}
//	virtual void Function_3()
//	{
//		printf("Base:Function_3...\n");
//	}
//};
//struct Sub :Base
//{
//public:
//	virtual void Function_4()
//	{
//		printf("Sub:Function_4...\n");
//	}
//	virtual void Function_5()
//	{
//		printf("Sub:Function_5...\n");
//	}
//	virtual void Function_6()
//	{
//		printf("Sub:Function_6...\n");
//	}
//};


struct Base
{
public:
	virtual void Function_1()
	{
		printf("Base:Function_1...\n");
	}
	virtual void Function_2()
	{
		printf("Base:Function_2...\n");
	}
	void Function_3()
	{
		printf("Base:Function_3...\n");
	}
};
struct Sub :Base
{
public:
	virtual void Function_1()
	{
		printf("Sub:Function_1...\n");
	}
	virtual void Function_2()
	{
		printf("Sub:Function_2...\n");
	}
	virtual void Function_6()
	{
		printf("Sub:Function_6...\n");
	}
	void Function_3()
	{
		printf("Sub:Function_7...\n");
	}
};

void OutputVirtualTable() 
{
	Sub sub;
	int* subptr = (int*)&sub;
	int* VirTalbe = (int*) *subptr;
	while (1) 
	{
		if (!*VirTalbe)
		{
			return;
		};
		printf("Ðéº¯ÊýµØÖ·%x\n", *VirTalbe);
		VirTalbe++;
	};
};


int main()
{
	//Base base;
	//Base* baseptr = &base;
	//baseptr->Function_1();

	//Sub sub;
	//Sub* subptr = &sub;
	//subptr->Function_1();
	OutputVirtualTable();

	return 0;
}

