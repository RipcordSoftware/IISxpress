#pragma once

#include "resource.h"

// IISxpress.h - Header file for your Internet Information Server
//    IISxpress Filter and IISxpress Extension

#include "IISxpressFilterContext.h"

#include "LocalhostLookup.h"

#include <set>
#include <string>

enum RejectedResponseReasons
{
	Default = 0,
	Internal,
	FilterDisabled,
	MemoryAllocationFailed,
	ResponseCode,
	LocalhostDisabled,
	MissingContentType,
	InvalidContentLength,
	IncompatibleClient,
	AlreadyEncoded,
	RequestMethod,
	NoCompressionServer,
	CompressionServerDisabled,
	NeverRuleMatch,
	StaticRuleMatch,
	CPUBusy,
	MemoryLow,
	UserRuleExcluded
};

class IISxpressFilter : public Ripcord::IIS::ISAPIFilter
{

public:

	IISxpressFilter();
	~IISxpressFilter();

// Overrides
public:

	virtual void OnGetFilterVersion(PHTTP_FILTER_VERSION pVer);

	virtual DWORD OnUrlMap(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_URL_MAP pMapInfo);
	virtual DWORD OnSendRawData(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData);	
	virtual DWORD OnEndOfRequest(Ripcord::IIS::ISAPIFilterContent& fc);
	virtual DWORD OnEndOfNetSession(Ripcord::IIS::ISAPIFilterContent& fc);	
	virtual DWORD OnSendResponse(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_SEND_RESPONSE);
	virtual void OnTerminateFilter() {}

private:

	DWORD OnSendRawHeaderData(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData);
	DWORD OnSendRawBodyData(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData);

	DWORD HandleSingleBodyBlock(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData);	

	HRESULT DispatchStream(IIISxpressHTTPRequest* pIISxpressHTTPRequest, Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData);

	DWORD SendFatalError(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData);

	void OpenLog(Ripcord::IIS::ISAPIFilterContent& fc);

	bool GetHTTPRequestObject(IIISxpressHTTPRequest** ppIISxpressHTTPRequest);	

	void DisableIISNotifications(Ripcord::IIS::ISAPIFilterContent& fc, DWORD dwNotifications);

	void CheckConfigSettings(Ripcord::IIS::ISAPIFilterContent& fc);

	void MyOutputDebugString(const WCHAR*, ...);

	FilterContextContainer* InitContainer(Ripcord::IIS::ISAPIFilterContent& fc, char* pszPhysicalPath, DWORD dwPhysicalPathSize);

	FilterContextContainer* GetContainer(Ripcord::IIS::ISAPIFilterContent& fc);
	void SetContainer(Ripcord::IIS::ISAPIFilterContent& fc, FilterContextContainer* pContainer);

	void InitializeCache();
	DWORD HandleCacheHit(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData);
	void AddResponseDataToCache(LPCSTR pszCacheKey, LPCSTR pszContentEncoding, const BYTE* pbyData, DWORD dwResponseSize);

	DWORD GetResponseBodySize(const BYTE* pbyData, DWORD dwResponseSize);
	
	void InitializePerfCounters(Ripcord::IIS::ISAPIFilterContent& fc);
	void PerfCountersAddRejectedResponse(RejectedResponseReasons reason);	
	void PerfCountersAddRejectedResponse(RejectedResponseReasons reason, CIISxpressISAPIPerfObject* pObject);
	void PerfCountersAddRejectedResponse(HRESULT hr);	
	void PerfCountersAddCachedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize);
	void PerfCountersAddCompressedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize);
	RejectedResponseReasons PerfCountersMapServerHResult(HRESULT hr);
	void PerfCountersUpdateCacheStatus(bool forceUpdate);

	bool GetAppPoolName(Ripcord::IIS::ISAPIFilterContent& fc, CStringA& sAppPoolName);

	Ripcord::IISxpress::IISxpressRegSettingsEx		m_Config;
	Ripcord::Utils::AppLog		m_Log;

	DWORD						m_dwLastConfigCookie;
	DWORD						m_dwOldLoggingLevel;

	CLocalhostLookup			m_IPLookup;

	volatile LONG						m_nCacheEnabled;
	volatile bool						m_bCacheInitialized;
	CComAutoCriticalSection				m_csCacheInitialized;
	CBlobCache< CWorkerThread<> > 		m_ResponseCache;
	CComPtr<IMemoryCacheClient>			m_ResponseCacheItemDeallocator;
	volatile DWORD						m_dwCacheStateCookie;

	CComGITPtr<IIISxpressHTTPRequest>	m_pGlobalIISxpressHTTPRequest;

	volatile bool						m_bPerfCountersInitialized;
	CComAutoCriticalSection				m_csPerfCountersInitialized;
	CIISxpressISAPIPerfObject*			m_pInstancePerfCounters;
	CIISxpressISAPIPerfObject*			m_pGlobalPerfCounters;

	Ripcord::IISxpress::HttpUserAgent::UserAgentCache<std::string> m_UserAgentCache;

	OSVERSIONINFOEX						m_OSVerInfo;
};
