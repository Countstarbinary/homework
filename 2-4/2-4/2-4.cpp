// 2-4.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>

struct coordinate
{
	int x;
	int y;
};
struct Gamer
{
	int hp;
	int level;
	coordinate coo;
};

Gamer game;
void function1() 
{
	game.hp = 100;
	game.level = 40;
	game.coo.x = 60;
	game.coo.y = 55;
};

void function2() 
{
	printf("%d %d %d %d\n", game.hp, game.level, game.coo.x, game.coo.y);
};

char arr[20];


Gamer test1(Gamer game)
{
	return game;
};

void test2(char arr[])
{

};
int main()
{
	/*function1();
	function2();*/

	test1(game);
	test2(arr);
	return 0;
}


