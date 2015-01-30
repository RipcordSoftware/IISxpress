// IISxpressPerf.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "IISxpressPerf.h"
#include "dlldatax.h"
#include "IISxpressISAPIPerf.h"


class CIISxpressPerfModule : public CAtlDllModuleT< CIISxpressPerfModule >
{
public :
	DECLARE_LIBID(LIBID_IISxpressPerfLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IISXPRESSPERF, "{625AF4F1-8037-4CBE-9DF4-93AF90F06225}")
};

CIISxpressPerfModule _AtlModule;


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




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// return true if the performance counters can be registered on this platform
bool CanRegisterPerfCounters()
{
	OSVERSIONINFO versionInfo;
	memset(&versionInfo, 0, sizeof(versionInfo));
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

	if (::GetVersionEx(&versionInfo) == 0)
	{
		return false;
	}

	// don't attempt to register on Vista/2008
	return versionInfo.dwMajorVersion < 6 ? true : false;	
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	if (!CanRegisterPerfCounters())
	{
		return S_FALSE;
	}

	HRESULT hres = MYATL::RegisterPerfMon();
	if (hres != S_OK)
		return hres;
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	if (!CanRegisterPerfCounters())
	{		
		return S_FALSE;
	}

	HRESULT hres  = MYATL::UnregisterPerfMon();
	if (hres != S_OK)
		return hres;
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}

