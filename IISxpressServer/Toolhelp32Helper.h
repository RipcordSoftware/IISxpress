#pragma once

#include <Tlhelp32.h>

bool LoadToolhelp32Procs(void);

typedef HANDLE __stdcall FNCREATETOOLHELP32SNAPSHOT(DWORD, DWORD);
typedef BOOL __stdcall FNPROCESS32FIRST(HANDLE, LPPROCESSENTRY32);
typedef BOOL __stdcall FNPROCESS32NEXT(HANDLE, LPPROCESSENTRY32);

extern FNCREATETOOLHELP32SNAPSHOT* g_pfnCreateToolhelp32Snapshot;
extern FNPROCESS32FIRST* g_pfnProcess32First;
extern FNPROCESS32NEXT* g_pfnProcess32Next;