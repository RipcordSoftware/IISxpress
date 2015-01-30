#include "StdAfx.h"
#include "IISxpressModuleFactory.h"

HRESULT CIISxpressModuleFactory::GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator*)
{
	if (ppModule == NULL)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }	

	AutoMemberCriticalSection cs(&m_csIISxpressHttpModule);

	if (m_pIISxpressHttpModule == NULL)
	{	
		m_pIISxpressHttpModule = new CIISxpressHttpModule(m_pHttpServer);
		if (m_pIISxpressHttpModule == NULL)
		{
			return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
		}

		// lock the module in memory
		m_pIISxpressHttpModule->AddRef();
	}

	m_pIISxpressHttpModule->AddRef();

	*ppModule = m_pIISxpressHttpModule;
	return S_OK;
}

void CIISxpressModuleFactory::Terminate()
{
	AutoMemberCriticalSection cs(&m_csIISxpressHttpModule);	

	if (m_pIISxpressHttpModule != NULL)
		m_pIISxpressHttpModule->Release();	
}