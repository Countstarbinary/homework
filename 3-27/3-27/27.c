
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "27.h"



//每个函数里面都有一套获取PE头的代码，过于繁琐，索性直接实现一个函数用于获取这些头。注意释放内存
PPeAllHeaders GetPeAllHeaders(char* FileBuffer)
{
	PPeAllHeaders PtrToPeHeader = NULL;
	PIMAGE_DOS_HEADER FileDosHeaderPtr = (PIMAGE_DOS_HEADER)FileBuffer;

	if (FileDosHeaderPtr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_NT_HEADERS  NtHeadersPtr = (PIMAGE_NT_HEADERS)(FileBuffer + FileDosHeaderPtr->e_lfanew);

	if (NtHeadersPtr->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("该文件不是PE文件，请确认！\n");
		return 0;
	};

	PIMAGE_FILE_HEADER FileHeaderPtr = &NtHeadersPtr->FileHeader;



	PVOID OptionHeaderPtr = &NtHeadersPtr->OptionalHeader;
	PIMAGE_SECTION_HEADER SectionHeaderPtr = (PIMAGE_SECTION_HEADER)(FileBuffer + FileDosHeaderPtr->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER + FileHeaderPtr->SizeOfOptionalHeader);


	PPeAllHeaders HeadersPtr = (PPeAllHeaders)malloc(sizeof(PeAllHeaders));
	HeadersPtr->FileDosHeaderPtr = FileDosHeaderPtr;
	HeadersPtr->NtHeadersPtr = NtHeadersPtr;
	HeadersPtr->FileHeaderPtr = FileHeaderPtr;
	HeadersPtr->u.OptionHeaderPtr = OptionHeaderPtr;
	HeadersPtr->SectionHeaderPtr = SectionHeaderPtr;
	return HeadersPtr;
};

//把硬盘文件读取到内存FileBuffer。注意内存释放
BOOL FileToFileBuffer(char* FilePath, char* FileBuffer)
{
	//读取文件
	printf("开始读取文件\n");
	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open file! Error: %lu\n", GetLastError());
		return 0;
	}

	//获取文件大小，其内部还是NtQueryInformationFile函数实现的
	unsigned int FileSize = GetFileSize(hFile, NULL);

	if (FileSize == INVALID_FILE_SIZE)
	{
		printf("获取文件大小失败! Error: %lu\n", GetLastError());
		return 0;
	}

	//分配内存空间，用于存放文件中的数据.注意此处x64可能溢出。已修复
	char* Address = (char*)malloc(FileSize);
	memset(Address, 0, FileSize);

	if (sizeof(char*) == 8)
	{
		*(unsigned __int64*)FileBuffer = (unsigned __int64)Address;
	}
	else
	{
		*(int*)FileBuffer = (int)Address;
	};



	DWORD lpNumberOfBytesRead;
	//读取文件中的数据到我们分配的内存空间中
	BOOL flag = ReadFile(hFile, Address, FileSize, &lpNumberOfBytesRead, NULL);

	//关闭句柄
	CloseHandle(hFile);

	return flag;
};

//把FileBuffer写回硬盘
BOOL FileBufferToFile(char* FilePath, char* FileBuffer)
{
	HANDLE hFile = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) printf("创建句柄失败！\n");

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	DWORD FileSize = PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].PointerToRawData + PtrToPeHeader->SectionHeaderPtr[PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1].SizeOfRawData;
	DWORD lpNumberOfBytesRead;
	BOOL flag = WriteFile(hFile, FileBuffer, FileSize, &lpNumberOfBytesRead, NULL);
	if (!flag) printf("文件生成失败！\n");
	CloseHandle(hFile);
	free(PtrToPeHeader);
	return flag;
};


//把Rva转换成Foa。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int RvaToFoa(unsigned int Rva, char* FileBuffer)
{
	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;


	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		DWORD MAX = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData > PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize ? PtrToPeHeader->SectionHeaderPtr->SizeOfRawData : PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize;
		DWORD VirtualAddress = PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
		if ((Rva >= VirtualAddress) && (Rva < VirtualAddress + MAX))
		{
			Offset = Rva - PtrToPeHeader->SectionHeaderPtr->VirtualAddress;
			return PtrToPeHeader->SectionHeaderPtr->PointerToRawData + Offset;
		};

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 0;
};

//把Foa转换成Rva。所谓Rva是指变量地址距离该exe文件头在内存中地址的距离。
int FoaToRva(unsigned int Foa, char* FileBuffer)
{
	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;


	for (int i = 0; i < PtrToPeHeader->FileHeaderPtr->NumberOfSections; i++)
	{
		//DWORD MAX = PtrToPeHeader->SectionHeaderPtr->SizeOfRawData > PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize ? PtrToPeHeader->SectionHeaderPtr->SizeOfRawData : PtrToPeHeader->SectionHeaderPtr->Misc.VirtualSize;
		DWORD PointerToRaw = PtrToPeHeader->SectionHeaderPtr->PointerToRawData;
		if ((Foa >= PointerToRaw) && (Foa < PointerToRaw + PtrToPeHeader->SectionHeaderPtr->SizeOfRawData))
		{
			Offset = Foa - PtrToPeHeader->SectionHeaderPtr->PointerToRawData;
			return PtrToPeHeader->SectionHeaderPtr->VirtualAddress + Offset;
		};

		PtrToPeHeader->SectionHeaderPtr++;
	};
	free(PtrToPeHeader);
	return 0;
};



//返回对齐后的长度，第一个参数是没对齐的长度，如0x123，第二个参数是对齐长度，如0x200。
int Align(int length, int align)
{
	int x = length / align;
	int y = length % align;
	if (!y)
	{
		return x * align;
	};
	return (x + 1) * align;
};

//扩展最后一个节
BOOL ExpandLastSection(char* FileBuffer, int ExpandLength)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return flag;


	//获取最后一个节，并进行修改
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	LastSection->SizeOfRawData += ExpandLength;
	//这条必须要加上，不能只加SizeOfRawData，SizeOfRawData反而是次要的。在这里花了2个小时，老是在莫名奇妙的地方浪费大量时间
	LastSection->Misc.VirtualSize += ExpandLength;

	//修改sizeofimage大小
	PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage += ExpandLength;

	//在FileBuffer中新增加0x1000的长度
	char* NewFileBuffer = malloc(PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage);
	memcpy(NewFileBuffer, FileBuffer, PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage - ExpandLength);


	//这里容易错，注意与新增节有所不同
	int numberofsection = PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//在新节中加入数据
	//flag = AddShellcodeToTargetSection(sizeof(shellcode) / sizeof(shellcode[0]), numberofsection, (char *)shellcode, NewFileBuffer);
	//if (!flag)printf("shellcode写入失败!\n");

	//新NewFileBuffer写入文件
	flag = FileBufferToFile(NULL, NewFileBuffer);
	if (!flag)printf("文件生成失败!\n");

	//释放内存
	free(FileBuffer);
	free(NewFileBuffer);
	free(PtrToPeHeader);
	return flag;
};

//如果头部空间不够节表的大小，提升头。返回头提升后，空出的字节数
DWORD HeaderMove(PPeAllHeaders* PtrToPeHeader)
{
	PPeAllHeaders PeHeader = *PtrToPeHeader;
	DWORD ByteofSurplus = 0;
	char*firstchar = (char *)PeHeader->NtHeadersPtr;
	char *lastchar = (char *)(PeHeader->SectionHeaderPtr + PeHeader->FileHeaderPtr->NumberOfSections);
	ByteofSurplus = PeHeader->FileDosHeaderPtr->e_lfanew - 0x40;

	PeHeader->FileDosHeaderPtr->e_lfanew = 0x40;

	char* temp = (char*)malloc(lastchar - firstchar);

	memset(temp, 0, lastchar - firstchar);

	memcpy(temp, firstchar, lastchar - firstchar);

	memset(firstchar, 0, lastchar - firstchar);

	memcpy((char*)PeHeader->FileDosHeaderPtr + 0x40, temp, lastchar - firstchar);


	//获取新的头结构体
	char* FileBuffer = (char*)PeHeader->FileDosHeaderPtr;
	BOOL flag = 0;
	free(PeHeader);
	PeHeader = NULL;

	*PtrToPeHeader = GetPeAllHeaders(FileBuffer);

	if (!PeHeader)printf("头提升后，重新获取节失败！\n");

	free(temp);


	return ByteofSurplus;
};

//在最后一个节后新增一个节，返回增加节后的buffer
char* AddNewSectionToFileEnd(char* FileBuffer, int NewSectionLength)
{
	BOOL flag = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);


	if (!PtrToPeHeader)return flag;
	if (sizeof(char*) == 8)
	{
		//计算所有头占有的
		unsigned __int64 SizeofAllheader = 0;
		//计算头剩下的字节
		unsigned __int64 ByteofSurplus = 0;

		//如果头的剩余字节小于两个节表的长度，或者最后一个节后有数据，如notepad，就扩张最后一个节
		if (ByteofSurplus <= 80 || (*(unsigned __int64*)SizeofAllheader != 0))
		{
			printf("header空间不足，头提升！\n");
			ByteofSurplus += HeaderMove(&PtrToPeHeader);

			//如果头提升后空间还是不足，进行节的扩展
			if (ByteofSurplus <= 80)
			{
				printf("header空间不足，进行节扩展！\n");
				flag = ExpandLastSection(FileBuffer, NewSectionLength);
				return flag;
			};
		};
	}
	else
	{
		//计算所有头占有的
		DWORD SizeofAllheader = 0;
		//计算头剩下的字节
		DWORD ByteofSurplus = 0;


		//计算所有头占有的
		SizeofAllheader = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections;

		//计算头剩下的字节
		ByteofSurplus = (DWORD)FileBuffer + PtrToPeHeader->u.OptionHeaderPtr->SizeOfHeaders - SizeofAllheader;
		
		//如果头的剩余字节小于两个节表的长度，或者最后一个节后有数据，如notepad，就扩张最后一个节
		if (ByteofSurplus <= 80 || (*(int*)SizeofAllheader != 0))
		{
			printf("header空间不足，头提升！\n");
			ByteofSurplus += HeaderMove(&PtrToPeHeader);

			//如果头提升后空间还是不足，进行节的扩展
			if (ByteofSurplus <= 80)
			{
				printf("header空间不足，进行节扩展！\n");
				flag = ExpandLastSection(FileBuffer, NewSectionLength);
				return flag;
			};
		};
	};

	//获取最后一个节，用于初始化新的节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//计算文件长度
	DWORD FileLength = LastSection->PointerToRawData + LastSection->SizeOfRawData;
	//计算最后一个节的实际大小
	DWORD SizeofLastSection = 0;
	if (LastSection->SizeOfRawData > LastSection->Misc.VirtualSize)
	{
		SizeofLastSection = LastSection->SizeOfRawData;
	}
	else
	{
		SizeofLastSection = LastSection->Misc.VirtualSize;
	};


	//初始化新的节
	PIMAGE_SECTION_HEADER NewSection = malloc(sizeof(IMAGE_SECTION_HEADER));


	if (!NewSection)return flag;

	memset(NewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	memcpy(NewSection->Name, "NewSection", 8);
	NewSection->Misc.VirtualSize = NewSectionLength;
	//这里需要内存对齐的长度
	NewSection->VirtualAddress = Align(LastSection->VirtualAddress + LastSection->SizeOfRawData, PtrToPeHeader->u.OptionHeaderPtr->SectionAlignment);

	NewSection->SizeOfRawData = NewSectionLength;
	NewSection->PointerToRawData = LastSection->PointerToRawData + LastSection->SizeOfRawData;
	NewSection->PointerToRelocations = 0;
	NewSection->PointerToLinenumbers = 0;
	NewSection->NumberOfRelocations = 0;
	NewSection->NumberOfLinenumbers = 0;
	NewSection->Characteristics = 0x60000020;

	//新的节写入节表
	memcpy((PIMAGE_SECTION_HEADER)(PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections), NewSection, sizeof(IMAGE_SECTION_HEADER));

	//修改pe头中节数量
	PtrToPeHeader->FileHeaderPtr->NumberOfSections += 1;

	//修改sizeofimage大小
	PtrToPeHeader->u.OptionHeaderPtr->SizeOfImage += NewSectionLength;

	//在FileBuffer中新增加0x1000的长度。这里需要文件长度，不是sizeofimage，之前写错了，现已修改
	char* NewFileBuffer = malloc(FileLength + NewSectionLength);
	if (!NewFileBuffer)return 0;
	memset(NewFileBuffer, 0, FileLength + NewSectionLength);
	memcpy(NewFileBuffer, FileBuffer, FileLength);

	//这里容易错，注意
	//DWORD numberofsection = PtrToPeHeader->FileHeaderPtr->NumberOfSections - 2;

	//在新节中加入数据
	//flag = AddShellcodeToNewSection(sizeof(shellcode) / sizeof(shellcode[0]), numberofsection, (char *)shellcode, NewFileBuffer);
	//if (!flag)return flag;

	//新NewFileBuffer写入文件
	//flag = FileBufferToFile(NewFilepath, NewFileBuffer);

	//释放旧FileBuffer内存
	free(FileBuffer);
	//free(NewFileBuffer);
	free(PtrToPeHeader);
	free(NewSection);

	return NewFileBuffer;
};

//添加一个节后到最后一个节,并形成exe文件
BOOL AddNewSectionToExe(char* Filepath, char* NewFilepath, int NewSectionLength)
{
	BOOL flag = 0;
	char* FileBuffer = NULL;

	//开始工作
	flag = FileToFileBuffer(Filepath, &FileBuffer);
	if (flag)printf("文件读取完成 %x\n", FileBuffer);
	//flag = ExpandLastSection(FileBuffer, NewSectionLength);
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, NewSectionLength);
	if (NewFileBuffer)printf("新增节完成！\n");

	flag = FileBufferToFile(NewFilepath, NewFileBuffer);
	if (NewFileBuffer)printf("新增节，文件写入成功！\n");

	return flag;
};


//输出FileBuffer的导出表信息
void OutputExportDirtory(char* FileBuffer)
{
	DWORD AddressOfNamesFoa = 0;
	DWORD AddressOfFunctionsFoa = 0;
	DWORD AddressOfNameOrdinalsFoa = 0;

	PDWORD AddressOfNamesFileBuffer = NULL;
	PDWORD AddressOfFunctionsFileBuffer = NULL;
	PWORD AddressOfNameOrdinalsFileBuffer = NULL;

	PDWORD AddressOfFuncitonName = NULL;
	DWORD FunctionVirtualAddress = NULL;

	int Offset = 0;
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	}
	else
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	};

	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, FileBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(FileBuffer + ExportDirtoryFoa);

	printf("************************************************************\n");
	printf("*****NumberOfFunciton:%d\n", ExportDirtory->NumberOfFunctions);
	printf("*****NumberOfNames:%d\n", ExportDirtory->NumberOfNames);
	printf("*****AddressOfFunctions:%d\n", ExportDirtory->AddressOfFunctions);
	printf("*****AddressOfNames:%d\n", ExportDirtory->AddressOfNames);
	printf("*****AddressOfNameOrdinals:%d\n", ExportDirtory->AddressOfNameOrdinals);
	printf("************************************************************\n");


	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称表在FileBuffer中的地址
		AddressOfNamesFoa = RvaToFoa(ExportDirtory->AddressOfNames, FileBuffer);
		AddressOfNamesFileBuffer = FileBuffer + AddressOfNamesFoa;

		//获取地址表在FileBuffer中的地址
		AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, FileBuffer);
		AddressOfFunctionsFileBuffer = FileBuffer + AddressOfFunctionsFoa;

		//获取序号表在FileBuffer中的地址
		AddressOfNameOrdinalsFoa = RvaToFoa(ExportDirtory->AddressOfNameOrdinals, FileBuffer);
		AddressOfNameOrdinalsFileBuffer = FileBuffer + AddressOfNameOrdinalsFoa;


		//获取名称在FileBuffer中的地址
		AddressOfFuncitonName = FileBuffer + RvaToFoa(AddressOfNamesFileBuffer[i], FileBuffer);

		//获取函数地址
		FunctionVirtualAddress = AddressOfFunctionsFileBuffer[AddressOfNameOrdinalsFileBuffer[i]];

		Offset = RvaToFoa(FunctionVirtualAddress, FileBuffer);

		BYTE jmp = *(PBYTE)(FileBuffer + Offset); //增量链接

		if (jmp == (BYTE)0xe9)
		{
			//有增量链接
			DWORD Off = *(PDWORD)(FileBuffer + Offset + 1);
			printf("*****函数:%s，地址：%x\n", AddressOfFuncitonName, FileBuffer + Offset + Off + 5);
		}
		else
		{
			//无增量链接
			printf("*****函数:%s，地址：%x\n", AddressOfFuncitonName, FileBuffer + RvaToFoa(FunctionVirtualAddress, FileBuffer));
		};

		//???????
		AddressOfNamesFileBuffer++;
	};


	free(PtrToPeHeader);
};


//输出FileBuffer的重定位表信息
void OutputReloacationDirtory(char* FileBuffer)
{
	int Rva = 0;
	PWORD RelocationEntryItem = 0;

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取重定位表入口
	IMAGE_DATA_DIRECTORY RelocationEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	}
	else
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	};

	//重定位表入口 RvaToFoa
	DWORD RelocationEntryFoa = RvaToFoa(RelocationEntry.VirtualAddress, FileBuffer);


	PIMAGE_BASE_RELOCATION RelocationDirtory = (PIMAGE_BASE_RELOCATION)(FileBuffer + RelocationEntryFoa);

	//获取导出表
	while (1)
	{
		//获取导出项
		RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
		printf("************************************************************\n");

		//打印重定位项
		for (int i = 0; i < (RelocationDirtory->SizeOfBlock - 8) / 2; i++)
		{
			//判断是否是需要重定位的项 
			if (((*RelocationEntryItem) & 0x3000) == 0x3000)
			{
				//计算rva。注意数据类型要进行强转，否则计算失败
				Rva = RelocationDirtory->VirtualAddress + (DWORD)((*RelocationEntryItem) & 0xFFF);
				printf("*****地址:%x\n", Rva);


			};

			//重定位项更新
			RelocationEntryItem++;
		};

		//更新重定位表
		RelocationDirtory = (PIMAGE_BASE_RELOCATION)((DWORD)RelocationDirtory + RelocationDirtory->SizeOfBlock);
		if ((RelocationDirtory->VirtualAddress == 0) && (RelocationDirtory->SizeOfBlock == 0))
		{
			return;
		};
	};

	free(PtrToPeHeader);
};

//移动导出表，返回移动后的buffer
char* MoveExportDirtory(char* FileBuffer)
{
	BOOL flag = 0;

	//新增加节
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, 0x1000);
	if (!NewFileBuffer)printf("导出表移位-> 新增节失败！\n");

	//获取pe文件头
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(NewFileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导出表入口
	IMAGE_DATA_DIRECTORY ExportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	}
	else
	{
		ExportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	};


	//导出表入口 RvaToFoa
	DWORD ExportDirtoryFoa = RvaToFoa(ExportEntry.VirtualAddress, NewFileBuffer);

	//获取导出表
	PIMAGE_EXPORT_DIRECTORY ExportDirtory = (PIMAGE_EXPORT_DIRECTORY)(NewFileBuffer + ExportDirtoryFoa);

	//获取最后一个节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//最后一个节，从头开始分别放函数表，名称表，序号表，和名字
	char*NewAddressOfFunciton = NewFileBuffer + LastSection->PointerToRawData;
	char*NewAddressOfName = NewAddressOfFunciton + ExportDirtory->NumberOfFunctions * 4;
	char*NewAddressOfNameordnial = NewAddressOfName + ExportDirtory->NumberOfNames * 4;
	char*NewName = NewAddressOfNameordnial + ExportDirtory->NumberOfNames * 2;


	DWORD AddressOfNamesFoa = 0;
	DWORD AddressOfFunctionsFoa = 0;
	DWORD AddressOfNameOrdinalsFoa = 0;

	PDWORD AddressOfNamesFileBuffer = NULL;
	PDWORD AddressOfFunctionsFileBuffer = NULL;
	PWORD AddressOfNameOrdinalsFileBuffer = NULL;

	PDWORD AddressOfFuncitonName = NULL;
	DWORD FunctionVirtualAddress = NULL;
	//获取名称表在FileBuffer中的地址
	AddressOfNamesFoa = RvaToFoa(ExportDirtory->AddressOfNames, NewFileBuffer);
	AddressOfNamesFileBuffer = NewFileBuffer + AddressOfNamesFoa;

	//获取地址表在FileBuffer中的地址
	AddressOfFunctionsFoa = RvaToFoa(ExportDirtory->AddressOfFunctions, NewFileBuffer);
	AddressOfFunctionsFileBuffer = NewFileBuffer + AddressOfFunctionsFoa;

	//获取序号表在FileBuffer中的地址
	AddressOfNameOrdinalsFoa = RvaToFoa(ExportDirtory->AddressOfNameOrdinals, NewFileBuffer);
	AddressOfNameOrdinalsFileBuffer = NewFileBuffer + AddressOfNameOrdinalsFoa;



	//复制3张表
	memcpy(NewAddressOfFunciton, AddressOfFunctionsFileBuffer, ExportDirtory->NumberOfFunctions * 4);
	memcpy(NewAddressOfName, AddressOfNamesFileBuffer, ExportDirtory->NumberOfNames * 4);
	memcpy(NewAddressOfNameordnial, AddressOfNameOrdinalsFileBuffer, ExportDirtory->NumberOfNames * 2);

	int FuncitonNameLength = 0;
	for (int i = 0; i < ExportDirtory->NumberOfNames; i++)
	{
		//获取名称在FileBuffer中的地址
		AddressOfFuncitonName = NewFileBuffer + RvaToFoa(AddressOfNamesFileBuffer[i], NewFileBuffer);
		//获取函数名长度
		FuncitonNameLength = strlen(AddressOfFuncitonName) + 1;
		//函数名复制
		memcpy(NewName, AddressOfFuncitonName, FuncitonNameLength);

		//函数表修复
		*AddressOfFunctionsFileBuffer = FoaToRva((DWORD)AddressOfFuncitonName - (DWORD)NewFileBuffer, NewFileBuffer);

		//名称表修复
		*AddressOfNamesFileBuffer = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);

		AddressOfFunctionsFileBuffer++;
		AddressOfNamesFileBuffer++;
		NewName += FuncitonNameLength;
	};



	//修复IMAGE_EXPORT_DIRECTORY结构
	ExportDirtory->AddressOfFunctions = FoaToRva(NewAddressOfFunciton - NewFileBuffer, NewFileBuffer);
	ExportDirtory->AddressOfNames = FoaToRva(NewAddressOfName - NewFileBuffer, NewFileBuffer);
	ExportDirtory->AddressOfNameOrdinals = FoaToRva(NewAddressOfNameordnial - NewFileBuffer, NewFileBuffer);


	//修复DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);
	}
	else 
	{
		PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = FoaToRva(NewName - NewFileBuffer, NewFileBuffer);
	};
	//复制IMAGE_EXPORT_DIRECTORY结构
	memcpy(NewName, ExportDirtory, sizeof(IMAGE_EXPORT_DIRECTORY));


	free(PtrToPeHeader);
	return NewFileBuffer;
};

//移动重定位表，返回移动后的buffer.r如果需要修改imagebase，就传入进去
char* MoveRelocationDirtory(char* FileBuffer, int NewImagebase)
{
	BOOL flag = 0;
	PWORD RelocationEntryItem = 0;


	//新增加节
	char* NewFileBuffer = AddNewSectionToFileEnd(FileBuffer, 0x1000);
	if (!NewFileBuffer)printf("导出表移位-> 新增节失败！\n");

	//获取pe文件头
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(NewFileBuffer);
	if (!PtrToPeHeader)return 0;

	//修复重定位项,若有必要
	DWORD ImageBase = 0;
	if (NewImagebase)
	{
		ImageBase = PtrToPeHeader->u.OptionHeaderPtr->ImageBase;
		PtrToPeHeader->u.OptionHeaderPtr->ImageBase = NewImagebase;
	};

	//获取重定位表入口。x64的头有些不同，堆和栈的保留宽度宏变，如果解析的exe是32位就编译成32位程序，64就编译成64位
	IMAGE_DATA_DIRECTORY RelocationEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	}
	else
	{
		RelocationEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	};


	//重定位表入口 RvaToFoa
	DWORD RelocationEntryFoa = RvaToFoa(RelocationEntry.VirtualAddress, NewFileBuffer);

	PIMAGE_BASE_RELOCATION RelocationDirtory = (PIMAGE_BASE_RELOCATION)(NewFileBuffer + RelocationEntryFoa);

	//获取最后一个节
	PIMAGE_SECTION_HEADER LastSection = PtrToPeHeader->SectionHeaderPtr + PtrToPeHeader->FileHeaderPtr->NumberOfSections - 1;

	//移动导出表
	char* Newaddress = Newaddress = NewFileBuffer + LastSection->PointerToRawData;
	int* virtadd = 0;
	while (1)
	{
		RelocationEntryItem = (PWORD)(RelocationDirtory + 1);
		//修复重定位表,若有必要
		if (NewImagebase)
		{
			for (int i = 0; i < (RelocationDirtory->SizeOfBlock - 8) / 2; i++)
			{
				//判断是否是需要重定位的项 
				if (((*RelocationEntryItem) & 0x3000) == 0x3000)
				{
					//计算rva。注意数据类型要进行强转，否则计算失败
					virtadd = (int*)(NewFileBuffer + RvaToFoa(RelocationDirtory->VirtualAddress + (DWORD)((*RelocationEntryItem) & 0xFFF), NewFileBuffer));

					*virtadd = *virtadd - ImageBase + PtrToPeHeader->u.OptionHeaderPtr32->ImageBase;

				};

				//重定位项更新
				RelocationEntryItem++;
			};
		};

		//移动重定位表
		memcpy(Newaddress, RelocationDirtory, RelocationDirtory->SizeOfBlock);

		//更新重定位表  
		RelocationDirtory = (PIMAGE_BASE_RELOCATION)((DWORD)RelocationDirtory + RelocationDirtory->SizeOfBlock);
		Newaddress += RelocationDirtory->SizeOfBlock;
		if ((RelocationDirtory->VirtualAddress == 0) && (RelocationDirtory->SizeOfBlock == 0))
		{
			break;
		};
	};
	free(PtrToPeHeader);
	return NewFileBuffer;
};

//输出导入表信息
void OutputImportDirtory(char* FileBuffer)
{
	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导入表入口
	IMAGE_DATA_DIRECTORY ImportEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		ImportEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	};

	//导入表入口 RvaToFoa
	DWORD ImportEntryFoa = RvaToFoa(ImportEntry.VirtualAddress, FileBuffer);
	ImportEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
	int* a = FileBuffer+RvaToFoa(ImportEntry.VirtualAddress, FileBuffer);
	printf("iat：%x\n", a);
	PIMAGE_IMPORT_DESCRIPTOR ImportDirtory = (PIMAGE_IMPORT_DESCRIPTOR)(FileBuffer + ImportEntryFoa);

	DWORD OFT = NULL;
	int* OFTitem;
	char* name;
	while (1) 
	{
		printf("*******************************************************\n");
		if (ImportDirtory->Characteristics==0&& ImportDirtory->FirstThunk==0
			&&ImportDirtory->ForwarderChain==0&& ImportDirtory->Name==0&& 
			ImportDirtory->OriginalFirstThunk==0&& ImportDirtory->TimeDateStamp==0)
		{
			break;
		};

		//OFT = ImportDirtory->OriginalFirstThunk;
		OFT = ImportDirtory->FirstThunk;
		
		OFTitem = FileBuffer + RvaToFoa(OFT, FileBuffer);
		
		name = FileBuffer + RvaToFoa(ImportDirtory->Name, FileBuffer);
		printf("动态链接库：%s\n", name);
		while (1) 
		{
			
			if (!(*OFTitem)) break;
	
			if ((*OFTitem & 0x80000000) != 0x80000000)
			{
				name = FileBuffer + RvaToFoa(*OFTitem, FileBuffer) + 2;
				printf("函数名导出 %x %x %s\n",OFT, *OFTitem, name);
			}
			else
			{
				printf("序号导出 %x %x\n", OFT, *OFTitem&0x7FFFFFFF);
			};
			OFTitem ++;
		};

		ImportDirtory++;
	};


};

//输出IAT表
void OutputIATDirtory(char* FileBuffer)
{

	PPeAllHeaders PtrToPeHeader = GetPeAllHeaders(FileBuffer);
	if (!PtrToPeHeader)return 0;

	//获取导入表入口
	IMAGE_DATA_DIRECTORY IATEntry = { 0 };
	if (PtrToPeHeader->FileHeaderPtr->SizeOfOptionalHeader == 0xe0)
	{
		IATEntry = PtrToPeHeader->u.OptionHeaderPtr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
	}
	else
	{
		IATEntry = PtrToPeHeader->u.OptionHeaderPtr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
	};

	//导入表入口 RvaToFoa
	DWORD IATEntryFoa = RvaToFoa(IATEntry.VirtualAddress, FileBuffer);


	PIMAGE_THUNK_DATA ImportDirtory = (PIMAGE_THUNK_DATA)(FileBuffer + IATEntryFoa);

	int* IATitem= ImportDirtory;
	char* name;
	
	while (1)
	{	
		if (!(*IATitem)) break;

		/*if ((*IATitem & 0x80000000) != 0x80000000)
		{
			name = FileBuffer + RvaToFoa(*IATitem, FileBuffer) + 2;
			printf("函数名导出 %x %s\n", *IATitem, name);
		}
		else
		{
			printf("序号导出%x\n",*IATitem & 0x7FFFFFFF);
		};*/
		printf("函数名导出 %x\n", *IATitem);
		IATitem++;
	};

};