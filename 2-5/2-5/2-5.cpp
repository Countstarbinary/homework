// 2-5.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <string.h>

struct locate 
{
	int x;
	int y;
	int z;
};
struct Monster
{
	int id;
	char name[20];
	char sex;
	int age;
	int hp;
	locate loc;
};

typedef Monster MyMonster[10];

MyMonster M;
void function() 
{
	
	M[0].id = 0;
	M[0].age = 100000;
	M[0].sex = 0;
	M[0].hp = 9999;
	strcpy_s(M[0].name ,"老大");
	M[0].loc.x = 1;
	M[0].loc.y = 2;
	M[0].loc.z = 3;
};

void function2(int id) 
{
	printf("id: %d, age: %d, sex: %d, hp: %d, x: %d, y: %d, z:%d, name:%s\n", M[id].id, M[id].age, M[id].sex, M[id].hp, M[id].loc.x, M[id].loc.y, M[id].loc.z, M[0].name);
	
};

struct S1
{
	char c;
	double i;
};
struct S4
{
	int c1;
	char c2[10];
};
struct S3
{
	char c1;
	S1 s;
	char c2;
	char c3;
};
struct S5		
{
	char c1;
	S1 s;
	char c2;
	double c3;
};
struct S6
{
	char c1;
	char c2;
	char c3;
	short c4;
	int c5;
};
struct S7
{
	double c0;
	char c1;
	char c2;
	char c3;
	short c4;
	int c5;
};
struct S8
{
	char c1;
	char c2;
	char c3;
	short c4;
	int c5;
	double c0;
};
struct S8
{
	char c1;
	double c0;
	char m;
	char c2;
	char c3;
	short c4;
	int c5;
	
};
S1 s1;
S4 s4;
S3 s3;
S5 s5;
S6 s6;
int main()
{
	//function();
	//function2(0);
	printf("%d %d %d %d %d\n",sizeof(s1), sizeof(s4), sizeof(s3), sizeof(s5), sizeof(s6));
	return 0;
}


