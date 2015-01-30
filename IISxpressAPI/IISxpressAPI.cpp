#include "stdafx.h"

#include "IISxpressAPI.h"

class CIISxpressAPIModule :
	public CAtlDllModuleT<CIISxpressAPIModule>
{
};

CIISxpressAPIModule _AtlModule;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}


#ifdef _MANAGED
#pragma managed(pop)
#endif