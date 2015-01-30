// IISxpressNativeModule.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;

    return TRUE;
}

// *****************************************************************************************

class CIISxpressNativeAtlModule :
	public CAtlDllModuleT<CIISxpressNativeAtlModule>
{
};

CIISxpressNativeAtlModule _AtlModule;

// *****************************************************************************************

#ifdef _MANAGED
#pragma managed(pop)
#endif

//  The RegisterModule entrypoint implementation.
//  This method is called by the server when the module DLL is 
//  loaded in order to create the module factory,
//  and register for server events.
HRESULT
__stdcall
RegisterModule(
    DWORD                           dwServerVersion,
    IHttpModuleRegistrationInfo *   pModuleInfo,
    IHttpServer *                   pHttpServer
)
{
	if (pModuleInfo == NULL || pHttpServer == NULL)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);        
    }

	// create the module factory
	CIISxpressModuleFactory* pFactory = new CIISxpressModuleFactory(pHttpServer);
    if ( pFactory == NULL )
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);        
    }

	// register for server events
    HRESULT hr = pModuleInfo->SetRequestNotifications(pFactory, RQ_BEGIN_REQUEST | RQ_SEND_RESPONSE, 0);
	if (FAILED(hr) == TRUE)
	{
		delete pFactory;
	}

	return hr;
}