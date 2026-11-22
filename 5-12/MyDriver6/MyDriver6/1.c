#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	DbgBreakPoint();
	DbgPrint("good!\n");
	UNREFERENCED_PARAMETER(pDriver);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriver, IN PUNICODE_STRING pReg)
{
	nt

	pDriver->DriverUnload = DriverUnload;
	UNREFERENCED_PARAMETER(pReg);
	return STATUS_SUCCESS;
}