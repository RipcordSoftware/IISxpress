#include "stdafx.h"

#include "IISxpressISAPIPerf.h"

#include <atlstr.h>

static CIISxpressISAPIPerf perfMon;

CIISxpressISAPIPerfObject* CIISxpressISAPIPerfObject::Create(LPCSTR pszInstanceName)
{
	CIISxpressISAPIPerfObject* pInstance = NULL;

	HRESULT hr = perfMon.Initialize();
	hr = perfMon.RegisterAllStrings();

	hr = perfMon.CreateInstanceByName(CIISxpressISAPIPerfObject::kCategoryId, CAtlStringW(pszInstanceName), (CPerfObject**) &pInstance);
	if (hr == S_OK && pInstance != NULL)
	{
		::InterlockedIncrement(&pInstance->instanceCount);
	}	

	return pInstance;
}

void CIISxpressISAPIPerfObject::Release(CIISxpressISAPIPerfObject* pInstance)
{	
	if (pInstance != NULL && ::InterlockedDecrement(&pInstance->instanceCount) == 0)
	{
		HRESULT hr = perfMon.ReleaseInstance(pInstance);		
	}
}