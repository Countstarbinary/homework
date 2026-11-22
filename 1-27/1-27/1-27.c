// 1-27.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
char arr[] = { "china中国verygood天朝nice" };
void funciton(int LongofString,char * string)
{
	//重新复制一份，不能把原来的字符串改了。
	//这个变量应该放到堆中，未知长度把返回值改了，我说怎么这函数怎么一直出问题
	char tempstring[] = {0};
	//定义字符的数量
	char index = 0;
	//定义临时变量存放字符
	char temp = 0;

	for (int i = 0; i <= LongofString*2; i++)
	{
		if (index == LongofString)
		{
			tempstring[i + 0] = 0x00;
			tempstring[i + 1] = 0x00;

			break;
		};
		temp = *(string + i);
		if (temp & 0x80)
		{
			tempstring[i] = *(string + i);
			tempstring[i + 1] = *(string + i + 1);
			index++;

			i++;
		}
		else
		{
			tempstring[i] = *(string + i);

			index++;
		};
		
		
	};

	printf("%s\n", tempstring);
	//这个函数是什么毛病？
};
int shellcode[10] = { 0x48C48B48,0x48085889,0x48106889,0x4C207089,0x57184089,0x55415441,0x57415641,0xA0EC8148,0x45000001,0x8B4CF18B };


int main()
{
	//char * a = arr;
	//char *b = a + 1;

	//short * c = (short *)arr;
	//short * d = (short *)arr + 1;

	//int * e = (int *)arr;
	//int * f = (int *)arr + 1;
	//int g=(int)e*(int)f;
	funciton(13, arr);
	//printf("%x\n", e);
	//printf("%x\n",f);
	//printf("%x\n", g);
	//printf("%d\n", sizeof(shellcode)/sizeof(int));
	//printf("%d\n", sizeof(int *));
	//printf("%d\n", sizeof(long *));
	getchar();
	return 0;
}


