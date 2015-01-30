#include "StdAfx.h"

#include "Toolhelp32Helper.h"

FNCREATETOOLHELP32SNAPSHOT* g_pfnCreateToolhelp32Snapshot = NULL;
FNPROCESS32FIRST* g_pfnProcess32First = NULL;
FNPROCESS32NEXT* g_pfnProcess32Next = NULL;

bool LoadToolhelp32Procs(void)
{
	typedef HANDLE __stdcall FNCREATETOOLHELP32SNAPSHOT(DWORD, DWORD);
	typedef BOOL __stdcall FNPROCESS32FIRST(HANDLE, LPPROCESSENTRY32);
	typedef BOOL __stdcall FNPROCESS32NEXT(HANDLE, LPPROCESSENTRY32);

	// if we can't load kernel32.dll then assume the process isn't already running
	HMODULE hKernel32 = ::LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL)
		return false;

	g_pfnCreateToolhelp32Snapshot = 
		(FNCREATETOOLHELP32SNAPSHOT*) ::GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");

	g_pfnProcess32First = (FNPROCESS32FIRST*) ::GetProcAddress(hKernel32, 
#ifndef _UNICODE
		"Process32First");
#else
		"Process32FirstW");
#endif

	g_pfnProcess32Next = (FNPROCESS32NEXT*) ::GetProcAddress(hKernel32, 
#ifndef _UNICODE
		"Process32Next");
#else
		"Process32NextW");
#endif

	// we can't proceed unless these have loaded up, so return false
	if (g_pfnCreateToolhelp32Snapshot == NULL || g_pfnProcess32First == NULL || g_pfnProcess32Next == NULL)
	{
		::FreeLibrary(hKernel32);

		g_pfnCreateToolhelp32Snapshot = NULL;
		g_pfnProcess32First = NULL;
		g_pfnProcess32Next = NULL;

		return false;
	}
	else
	{
		return true;
	}
}