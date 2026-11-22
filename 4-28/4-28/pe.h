#pragma once
#include <windows.h>

#define RT_CURSOR		1		//光标
#define RT_BITMAP		2		//位图
#define RT_ICON			3		//图标
#define RT_MENU			4		//菜单
#define RT_DIALOG		5		//对话框
#define RT_STRING		6		//字符串
#define RT_FONTDIR		7		//字体目录
#define RT_FONT			8		//字体
#define RT_ACCELERATOR	9		//加速键
#define RT_RCDATA		10		//未格式化资源
#define RT_MESSAGETABLE	11		//消息表
#define RT_GROUP_CURSOR	12		//光标组 
#define RT_GROUP_ICON	14		//图标组
#define RT_VERSION		16		//版本




typedef struct _PeAllHeaders
{
	PIMAGE_DOS_HEADER FileDosHeaderPtr;
	PIMAGE_NT_HEADERS  NtHeadersPtr;
	PIMAGE_FILE_HEADER FileHeaderPtr;
	union
	{
		PIMAGE_OPTIONAL_HEADER  OptionHeaderPtr;
		//只要不涉及堆和栈之后的可选头内容，可选头x86和x64没有区别，用上面的OptionHeaderPtr即可
		PIMAGE_OPTIONAL_HEADER32  OptionHeaderPtr32;
		PIMAGE_OPTIONAL_HEADER64 OptionHeaderPtr64;
	}u;
	PIMAGE_OPTIONAL_HEADER OptionHeaderPtr;
	PIMAGE_SECTION_HEADER SectionHeaderPtr;
}PeAllHeaders, *PPeAllHeaders;

PPeAllHeaders GetPeAllHeaders(char* FileBuffer);

BOOL FileToFileBuffer(char* FilePath, char* FileBuffer);

BOOL FileBufferToFile(char* FilePath, char* FileBuffer);

BOOL AddNewSectionToExe(char* Filepath, char* NewFilepath, int NewSectionLength);

int RvaToFoa(unsigned int Rva, char* FileBuffer);

void OutputExportDirtory(char* FileBuffer);

void OutputReloacationDirtory(char* FileBuffer, HWND hwnd);

char* MoveExportDirtory(char* FileBuffer);

char* MoveRelocationDirtory(char* FileBuffer, int NewImagebase);

void OutputImportDirtory(char* FileBuffer, HWND hwnd);

void OutputIATDirtory(char* FileBuffer, HWND hwnd);

void OutputBoundImportDirtory(char* FileBuffer, HWND hwnd);

void OutputResourceDirtory(char* FileBuffer, HWND hwnd);
