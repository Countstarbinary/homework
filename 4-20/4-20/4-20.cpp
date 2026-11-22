

#define _CRT_SECURE_NO_WARNINGS  //这些宽字符函数确实不太安全
#include <windows.h>
#include <stdio.h>
#include <locale.h>


int main()
{
	//setlocale(LC_ALL, "");
	char x= '中';
	CHAR X[]= "中";

	wchar_t newclass = '中';

	wchar_t newclass1 = L'中';

	TCHAR newclass2 = TEXT('中');

	wchar_t x1[] = L"中国";
	wchar_t x2[] = L"中";

	wprintf(L"%s\n\n", x1);

	//wcscpy(x1, x1);
	//wprintf(L"%s\n\n", x1);

	wcscat(x1, x2);
	wprintf(L"%s\n\n", x1);

	printf("%d\n", wcscmp(x1, x1));

	wprintf(L"%s\n\n", wcsstr(x1, x2));
	printf("%d\n", wcslen(x1));

	return 0;
}