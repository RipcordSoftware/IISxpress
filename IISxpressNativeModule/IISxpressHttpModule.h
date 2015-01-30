#pragma once

#include <atlcache.h>
#include <memory>

#include "HandlePool.h"
#include "AppLog.h"

#include "IISxpressNativePerf.h"

#include "UserAgent.h"

class CIISxpressHttpModule : public CHttpModule
{

public:

	CIISxpressHttpModule(IHttpServer* pHttpServer);
	virtual ~CIISxpressHttpModule(void);

	virtual REQUEST_NOTIFICATION_STATUS OnBeginRequest(IN IHttpContext* pHttpContext, IN IHttpEventProvider* pProvider);
	virtual REQUEST_NOTIFICATION_STATUS OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider);

	virtual void Dispose();

	ULONG AddRef();
	ULONG Release();

private:

	bool GetHTTPRequestObject(IHttpContext* pHttpContext, IIISxpressHTTPRequest** ppIISxpressHTTPRequest);

	BYTE* AllocateMemoryBlockForOverlappedIO(IHttpContext* pHttpContext, DWORD dwSize, DWORD& dwAllocatedBlockSize);	

	void GetRegistrySettings(IHttpContext* pHttpContext);

	bool IsUserLocalhost(const HTTP_REQUEST* pRawRequest);

	bool MakeResponseSingleEntityBlock(IHttpContext* pHttpContext, IHttpResponse* pHttpResponse, const HTTP_RESPONSE* pRawResponse);

	void OpenLog(IHttpContext* pHttpContext);
	void AppendLogMessage(DWORD dwMinLogLevel, LPCTSTR pszMethodName, IHttpContext* pHttpContext, LPCTSTR pszMessageFormat, ...);

	bool GetAppPoolName(IHttpContext* pHttpContext, CAtlString& name);

	const char* EnsureNotNull(const char* ptr) { return ptr ? ptr : ""; }
	const WCHAR* EnsureNotNull(const WCHAR* ptr) { return ptr ? ptr : L""; }

	void PerfCountersAddRejectedResponse(IISxpressNativePerf::RejectedResponseReasons reason);		
	void PerfCountersAddRejectedResponse(HRESULT hr);	
	void PerfCountersAddCachedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize);
	void PerfCountersAddCompressedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize);
	IISxpressNativePerf::RejectedResponseReasons PerfCountersMapServerHResult(HRESULT hr);
	void PerfCountersUpdateCacheStatus(bool forceUpdate);

	CIISxpressRegSettingsEx				m_Config;
	CAppLog								m_Log;

	DWORD								m_dwLastConfigCookie;
	DWORD								m_dwOldLoggingLevel;
	
	DWORD								m_dwBytesPerSector;		// store the sector size

	LONG								m_nRefCount;			// ref count for self

	CComGITPtr<IIISxpressHTTPRequest>	m_pGlobalIISxpressHTTPRequest;

	EventHandlePool						m_hEventHandlePool;
	
	volatile LONG						m_nCacheEnabled;
	CBlobCache< CWorkerThread<> > 		m_ResponseCache;
	CComPtr<IMemoryCacheClient>			m_ResponseCacheItemDeallocator;
	volatile DWORD						m_dwCacheStateCookie;

	CComAutoCriticalSection				m_csPerfCounters;
	std::auto_ptr<IISxpressNativePerf>	m_pInstancePerfCounters;
	std::auto_ptr<IISxpressNativePerf>	m_pGlobalPerfCounters;

	HttpUserAgent::UserAgentCache<std::string>		m_UserAgentCache;

	IHttpServer*						m_pHttpServer;
};
