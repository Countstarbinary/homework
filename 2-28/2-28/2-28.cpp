#include <ntddk.h>

#pragma once
#include <ntifs.h>

#include <stdio.h>   //驱动可以用这个包
#include <stdlib.h>

//硬编码搜函数，格式ULONG shellcode[] = {0x83485340,0x8B4820EC,0xF192E8DA,0x8548FFC8,0x890274DB,0x48C03303,0x5B20C483,0xCCCCCCC3 };
ULONG64 findAddressByCode(PUCHAR beginAddr, PUCHAR endAddr, ULONG findCode[], ULONG CodeOfLong)
{
	BOOLEAN flags = FALSE;

	while (1)
	{
		if (findCode[0] == *(PULONG)beginAddr)
		{
			for (UCHAR i = 1; i < CodeOfLong; i++)
			{
				beginAddr += 4;
				if (findCode[i] != *(PULONG)beginAddr)
				{
					break;
				};
				if (i == (CodeOfLong - 1))
				{
					flags = TRUE;
					break;
				};
			};
		};
		if (flags)
		{
			return (ULONG64)(beginAddr - (CodeOfLong - 1) * 4);
		};
		if (beginAddr < endAddr)
		{
			beginAddr++;
		}
		else
		{
			return 0;
		};
	};
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	//DbgBreakPoint();
	DbgPrint("good!\n");
	UNREFERENCED_PARAMETER(pDriver);
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriver, IN PUNICODE_STRING pReg)
{

	pDriver->DriverUnload = DriverUnload;
	UNREFERENCED_PARAMETER(pReg);
	return STATUS_SUCCESS;
}