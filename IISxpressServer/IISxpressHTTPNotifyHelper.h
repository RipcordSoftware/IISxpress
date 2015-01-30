#pragma once

#include <atlcoll.h>

class CIISxpressHTTPNotifyHelper
{
public:
	CIISxpressHTTPNotifyHelper();
	~CIISxpressHTTPNotifyHelper();

	bool Initialize(IConnectionPointContainer* pConnectionPointContainer);

	STDMETHOD(Fire_OnResponseStart)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext);
	STDMETHOD(Fire_OnResponseComplete)(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize);
	STDMETHOD(Fire_OnResponseEnd)(DWORD dwFilterContext);
	STDMETHOD(Fire_OnResponseAbort)(DWORD dwFilterContext);
	STDMETHOD(Fire_OnCacheHit)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize);

	void NotifyClientsChanged();

private:

	typedef CAtlArray<CComPtr<ICallFactory> > AsyncClientCollection;
	typedef CAtlArray<CComPtr<IIISxpressHTTPNotify> > SyncClientCollection;

	HRESULT GetNotificationObjects(AsyncClientCollection& asyncClients, SyncClientCollection& syncClients);

	AsyncIIISxpressHTTPNotify* GetAsyncInterface(ICallFactory* pCallFactory);

	CComPtr<IConnectionPoint> m_pConnectionPoint;

	CComAutoCriticalSection m_cs;
	AsyncClientCollection m_AsyncClients;
	SyncClientCollection m_SyncClients;
};
