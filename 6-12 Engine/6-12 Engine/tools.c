#include <windows.h>

BOOL SafeReadChar(char* ptr) {
	__try
	{
		*ptr = *ptr;  // 可能抛出访问违例
		return TRUE;
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
		? EXCEPTION_EXECUTE_HANDLER
		: EXCEPTION_CONTINUE_SEARCH)
	{
		return FALSE;
	}
}

WORD GetOSArch()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	return si.wProcessorArchitecture;
}




