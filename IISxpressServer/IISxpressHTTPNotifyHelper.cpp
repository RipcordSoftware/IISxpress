#include "StdAfx.h"
#include "IISxpressHTTPNotifyHelper.h"

CIISxpressHTTPNotifyHelper::CIISxpressHTTPNotifyHelper()	
{	
}

CIISxpressHTTPNotifyHelper::~CIISxpressHTTPNotifyHelper()
{
}

bool CIISxpressHTTPNotifyHelper::Initialize(IConnectionPointContainer* pConnectionPointContainer)
{
	pConnectionPointContainer->FindConnectionPoint(IID_IIISxpressHTTPNotify, &m_pConnectionPoint);

	NotifyClientsChanged();

	return m_pConnectionPoint != NULL;
}

HRESULT CIISxpressHTTPNotifyHelper::GetNotificationObjects(AsyncClientCollection& asyncClients, SyncClientCollection& syncClients)
{
	asyncClients.RemoveAll();
	syncClients.RemoveAll();

	CComPtr<IConnectionPoint> pConnectionPoint = m_pConnectionPoint;

	if (pConnectionPoint == NULL)
	{
		return E_FAIL;
	}	

	CComPtr<IEnumConnections> pConnections;
	if (pConnectionPoint->EnumConnections(&pConnections) == S_OK && pConnections != NULL)
	{
		CONNECTDATA connectionData;
		ULONG nTemp = 0;
		while (pConnections->Next(1, &connectionData, &nTemp) == S_OK)
		{				
			CComPtr<IUnknown> pUnk;
			pUnk.Attach(connectionData.pUnk);

			CComQIPtr<ICallFactory> pCallFactory = pUnk;		
			if (pCallFactory != NULL)
			{
				asyncClients.Add(pCallFactory);					
			}
			else
			{
				CComQIPtr<IIISxpressHTTPNotify> pNotify = pUnk;
				if (pNotify != NULL)
				{
					syncClients.Add(pNotify);
				}
			}
		}
	}

	return S_OK;	
}

HRESULT CIISxpressHTTPNotifyHelper::Fire_OnResponseStart(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext)
{
	AutoMemberCriticalSection cs(&m_cs);

	for (int i = 0; i < (int) m_AsyncClients.GetCount(); i++)
	{
		CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
		pAsyncNotify.Attach(GetAsyncInterface(m_AsyncClients[i]));

		if (pAsyncNotify != NULL)
		{
			pAsyncNotify->Begin_OnResponseStart(pIISInfo, pRequestInfo, pResponseInfo, dwFilterContext);
		}
	}

	for (int i = 0; i < (int) m_SyncClients.GetCount(); i++)
	{
		m_SyncClients[i]->OnResponseStart(pIISInfo, pRequestInfo, pResponseInfo, dwFilterContext);
	}	

	return S_OK;
}

HRESULT CIISxpressHTTPNotifyHelper::Fire_OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
{
	AutoMemberCriticalSection cs(&m_cs);
	
	for (int i = 0; i < (int) m_AsyncClients.GetCount(); i++)
	{
		CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
		pAsyncNotify.Attach(GetAsyncInterface(m_AsyncClients[i]));

		if (pAsyncNotify != NULL)
		{
			pAsyncNotify->Begin_OnResponseComplete(dwFilterContext, bCompressed, dwRawSize, dwCompressedSize);
		}
	}

	for (int i = 0; i < (int) m_SyncClients.GetCount(); i++)
	{
		m_SyncClients[i]->OnResponseComplete(dwFilterContext, bCompressed, dwRawSize, dwCompressedSize);
	}	

	return S_OK;
}

HRESULT CIISxpressHTTPNotifyHelper::Fire_OnResponseEnd(DWORD dwFilterContext)
{
	AutoMemberCriticalSection cs(&m_cs);

	for (int i = 0; i < (int) m_AsyncClients.GetCount(); i++)
	{
		CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
		pAsyncNotify.Attach(GetAsyncInterface(m_AsyncClients[i]));

		if (pAsyncNotify != NULL)
		{
			pAsyncNotify->Begin_OnResponseEnd(dwFilterContext);
		}
	}

	for (int i = 0; i < (int) m_SyncClients.GetCount(); i++)
	{
		m_SyncClients[i]->OnResponseEnd(dwFilterContext);
	}	

	return S_OK;
}

HRESULT CIISxpressHTTPNotifyHelper::Fire_OnResponseAbort(DWORD dwFilterContext)
{
	AutoMemberCriticalSection cs(&m_cs);

	for (int i = 0; i < (int) m_AsyncClients.GetCount(); i++)
	{
		CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
		pAsyncNotify.Attach(GetAsyncInterface(m_AsyncClients[i]));

		if (pAsyncNotify != NULL)
		{
			pAsyncNotify->Begin_OnResponseAbort(dwFilterContext);
		}
	}

	for (int i = 0; i < (int) m_SyncClients.GetCount(); i++)
	{
		m_SyncClients[i]->OnResponseAbort(dwFilterContext);
	}	

	return S_OK;
}

HRESULT CIISxpressHTTPNotifyHelper::Fire_OnCacheHit(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize)
{
	AutoMemberCriticalSection cs(&m_cs);

	for (int i = 0; i < (int) m_AsyncClients.GetCount(); i++)
	{		
		CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
		pAsyncNotify.Attach(GetAsyncInterface(m_AsyncClients[i]));

		if (pAsyncNotify != NULL)
		{
			pAsyncNotify->Begin_OnCacheHit(pIISInfo, pRequestInfo, pResponseInfo, dwCompressedSize);
		}
	}

	for (int i = 0; i < (int) m_SyncClients.GetCount(); i++)
	{
		m_SyncClients[i]->OnCacheHit(pIISInfo, pRequestInfo, pResponseInfo, dwCompressedSize);
	}

	return S_OK;
}

void CIISxpressHTTPNotifyHelper::NotifyClientsChanged()
{
	AutoMemberCriticalSection cs(&m_cs);
	GetNotificationObjects(m_AsyncClients, m_SyncClients);
}

AsyncIIISxpressHTTPNotify* CIISxpressHTTPNotifyHelper::GetAsyncInterface(ICallFactory* pCallFactory)
{
	CComPtr<AsyncIIISxpressHTTPNotify> pAsyncNotify;
	if (pCallFactory != NULL && pCallFactory->CreateCall(IID_AsyncIIISxpressHTTPNotify, NULL, IID_AsyncIIISxpressHTTPNotify, (IUnknown**) &pAsyncNotify) == S_OK)
	{
		return pAsyncNotify.Detach();
	}
	else
	{
		return NULL;
	}
}