// IISxpress.cpp - Implementation file for ISAPI
//    IISxpress Filter and IISxpress Extension

#include "stdafx.h"
#include "IISxpress.h"
#include "IISxpressApp.h"

#include "ResponseStreamMagicNumbers.h"

#include "ResponseCache.h"

#include "MyHTTPResponseData.h"

#include <algorithm>

#define IISXPRESSFILTER_DEFAULTSTREAMSIZE		(1024 * 1024)

#pragma warning (disable : 4311 4312)

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************s

// The one and only CIISxpressFilter object
//CIISxpressFilter	theFilter;

// CIISxpressFilter implementation

IISxpressFilter::IISxpressFilter() : 
	m_bCacheInitialized(false), m_bPerfCountersInitialized(false), m_pInstancePerfCounters(NULL), 
	m_pGlobalPerfCounters(NULL), m_UserAgentCache(2000)
{
	m_OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	::GetVersionEx((OSVERSIONINFO*) &m_OSVerInfo);

	m_Config.Init(Ripcord::IISxpress::IISxpressRegKeys::IISXPRESSFILTER_REGKEY, true, true);
	m_dwLastConfigCookie = 0;
	m_dwOldLoggingLevel = Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_NONE;

	// get the cache enabled state
	m_nCacheEnabled = m_Config.GetCacheEnabled();	

	// get the cache state cookie (we use this to catch flush requests)
	m_dwCacheStateCookie = m_Config.GetCacheStateCookie();
}

IISxpressFilter::~IISxpressFilter()
{
	if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
	{
		m_Log.AppendFromResource(theApp.m_hInstance, IDS_FILTERSTOPPED, true);
	}

	m_ResponseCache.Uninitialize();
	
	CIISxpressISAPIPerfObject* pInstancePerfCounters = (CIISxpressISAPIPerfObject*) ::InterlockedExchangePointer((void**) &m_pInstancePerfCounters, NULL);
	CIISxpressISAPIPerfObject::Release(pInstancePerfCounters);	

	CIISxpressISAPIPerfObject* pGlobalPerfCounters = (CIISxpressISAPIPerfObject*) ::InterlockedExchangePointer((void**) &m_pGlobalPerfCounters, NULL);
	CIISxpressISAPIPerfObject::Release(pGlobalPerfCounters);	
}

void IISxpressFilter::InitializeCache()
{
	if (m_bCacheInitialized == false)
	{
		CComCritSecLock<CComAutoCriticalSection> cs(m_csCacheInitialized);

		if (m_bCacheInitialized == false)
		{
			// init the cache
			HRESULT hr = m_ResponseCache.Initialize(NULL);
			hr = m_ResponseCache.SetMaxAllowedEntries(m_Config.GetCacheMaxEntries());
			hr = m_ResponseCache.SetMaxAllowedSize(m_Config.GetCacheMaxSizeKB() * 1024);

			// get the cache item deallocator
			ResponseCache::GetResponseCacheItemDeallocator(&m_ResponseCacheItemDeallocator);

			m_bCacheInitialized = true;
		}		
	}
}

void IISxpressFilter::OnGetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
	MyOutputDebugString(L"IISxpressFilter::GetFilterVersion()\n");

	// Call default implementation for initialization
	//CHttpFilter::GetFilterVersion(pVer);

	// Clear the flags set by base class
	pVer->dwFlags &= ~SF_NOTIFY_ORDER_MASK;

	// Set the flags we are interested in
	pVer->dwFlags |= 		
		SF_NOTIFY_URL_MAP | 
		SF_NOTIFY_SEND_RESPONSE |
		SF_NOTIFY_SEND_RAW_DATA | 		
		SF_NOTIFY_END_OF_REQUEST | 		
		SF_NOTIFY_END_OF_NET_SESSION;

	// set load priority from the registry
	pVer->dwFlags |= m_Config.GetISAPILoadPriority();

	// Load description string
	char sz[SF_MAX_FILTER_DESC_LEN+1];
	::LoadStringA(AfxGetResourceHandle(), IDS_FILTER, sz, SF_MAX_FILTER_DESC_LEN);
	strcpy_s(pVer->lpszFilterDesc, _countof(sz), sz);

	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;
	GetHTTPRequestObject(&pIISxpressHTTPRequest);

	MyOutputDebugString(L"IISxpressFilter::GetFilterVersion(): GetHTTPRequestObject() returns 0x%08x\n", pIISxpressHTTPRequest.p);		

	if (pIISxpressHTTPRequest != NULL)
	{
		__int64 nStartTimer = g_Timer.GetTimerCount();		

		HANDLE hFilterVersion = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(HTTP_FILTER_VERSION));

		CComPtr<IStream> pFilterStream;
		HRESULT hr = ::CreateStreamOnHGlobal(hFilterVersion, TRUE, &pFilterStream);
		hFilterVersion = NULL;

		ULONG nWritten = 0;
		hr = pFilterStream->Write(pVer, sizeof(HTTP_FILTER_VERSION), &nWritten);		

		hr = pIISxpressHTTPRequest->GetFilterVersion(pFilterStream);

		if (m_Config.GetDebugEnabled() == true)
		{
			__int64 nEndTimer = g_Timer.GetTimerCount();

			MyOutputDebugString(
				L"IISxpressFilter::GetFilterVersion(): remote GetFilterVersion() returns 0x%08x, call took %I64dms\n", 
				hr, nEndTimer - nStartTimer);
		}
	}	
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnUrlMap(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_URL_MAP pMapInfo)
{
	MyOutputDebugString(L"IISxpressFilter::OnUrlMap()\n");

	// initialize the response cache
	InitializeCache();

	// initialize the performance counters
	InitializePerfCounters(fc);

	// handle any config changes
	CheckConfigSettings(fc);

	// this is the first place we can query the web server for its version,
	// it's not the best place but it's the only one available at the moment
	static volatile bool bLogServerVersion = true;
	if (bLogServerVersion == true && m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
	{	
		bLogServerVersion = false;

		m_Log.AppendFromResource(theApp.m_hInstance, IDS_FILTERSTARTED, true);
		m_Log.DumpSysInfo(_T("IISxpressFilter:"));

		char szIISVersion[128] = "\0";
		DWORD dwIISVersionLength = sizeof(szIISVersion);
		if (fc.GetServerVariable("SERVER_SOFTWARE", szIISVersion, &dwIISVersionLength) == TRUE)
		{
			m_Log.Append(true, _T("IISxpressFilter: ") _T(__FUNCTION__) _T(" - IIS version='%s'\n"), szIISVersion);
		}		
	}	

	// only proceed if filter is enabled
	if (m_Config.GetEnabled() == false)
	{
		MyOutputDebugString(L"OnUrlMap() filter is disabled\n");		

		PerfCountersAddRejectedResponse(FilterDisabled);

		// ask IIS to not inform us of the send notifications (5.1 and above)						
		DisableIISNotifications(fc, 
			SF_NOTIFY_URL_MAP |
			SF_NOTIFY_SEND_RESPONSE | 
			SF_NOTIFY_SEND_RAW_DATA | 
			SF_NOTIFY_END_OF_REQUEST |
			SF_NOTIFY_END_OF_NET_SESSION);		

		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// do some basic checks on physical path
	if (pMapInfo->pszPhysicalPath == NULL || pMapInfo->pszPhysicalPath[0] == '\0' || pMapInfo->cbPathBuff == 0)
	{
		const WCHAR* pszError = L"OnUrlMap() IIS failed to provide physical path\n";
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);		

		PerfCountersAddRejectedResponse(Internal);
				
		if (GetContainer(fc) == NULL)
		{
			// ask IIS to not inform us of the send notifications (5.1 and above)						
			DisableIISNotifications(fc, 
				SF_NOTIFY_URL_MAP |
				SF_NOTIFY_SEND_RESPONSE | 
				SF_NOTIFY_SEND_RAW_DATA | 
				SF_NOTIFY_END_OF_REQUEST |
				SF_NOTIFY_END_OF_NET_SESSION);
		}

		return SF_STATUS_REQ_NEXT_NOTIFICATION;	
	}	
	
	if (GetContainer(fc) == NULL)
	{		
		FilterContextContainer* pContainer = InitContainer(fc, pMapInfo->pszPhysicalPath, pMapInfo->cbPathBuff);
		if (pContainer == NULL)
		{
			const WCHAR* pszError = L"OnUrlMap() failed to allocate container memory\n";
			MyOutputDebugString(pszError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, pszError);			

			PerfCountersAddRejectedResponse(MemoryAllocationFailed);

			// we can't proceed
			DisableIISNotifications(fc, SF_NOTIFY_URL_MAP | SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}		

		// allow the next handler access to the container data		
		SetContainer(fc, pContainer);
	}
	else 
	{
		// get the old container
		FilterContextContainer* pContainer = GetContainer(fc);

		// if the incoming container already has a context then we need to clean it up
		IISxpressFilterContext* pContext = pContainer->GetContext();
		if (pContext != NULL)
		{
			MyOutputDebugString(L"OnUrlMap() incoming pContext is not NULL\n");			

			pContainer->FreeContext();
			pContext = NULL;
		}

		// check to see if we need to grow the buffer
		if (pMapInfo->cbPathBuff > pContainer->dwPhysicalPathSize)
		{
			// get the new container
			FilterContextContainer* pNewContainer = InitContainer(fc, pMapInfo->pszPhysicalPath, pMapInfo->cbPathBuff);		
			if (pNewContainer != NULL)
			{			
				// preserve the context data
				pNewContainer->SetContext(pContainer->GetContext());

				// allow the next handler access to the container data				
				SetContainer(fc, pNewContainer);
			}
		}
		else
		{
			// we don't need to grow the buffer, just copy the physical path string
			memcpy(pContainer->pszPhysicalPath, pMapInfo->pszPhysicalPath, pMapInfo->cbPathBuff);
		}
	}

	// since we are only interested in the first call per response ask IIS to not tell 
	// us about any other URL mappings
	DisableIISNotifications(fc, SF_NOTIFY_URL_MAP);

	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

FilterContextContainer* IISxpressFilter::InitContainer(Ripcord::IIS::ISAPIFilterContent& fc, char* pszPhysicalPath, DWORD dwPhysicalPathSize)
{
	if (pszPhysicalPath == NULL || dwPhysicalPathSize == 0)
		return NULL;

	// calculate the size of the memory block we will need
	DWORD dwContainerBlockSize = sizeof(FilterContextContainer) + dwPhysicalPathSize;

	// create a container from the IIS heap
	void* pVoid = fc.AllocMem(dwContainerBlockSize);
	if (pVoid == NULL)
		return NULL;

	FilterContextContainer* pContainer = new(pVoid) FilterContextContainer();
	pContainer->pszPhysicalPath = (char*) (pContainer + 1);
	memcpy(pContainer->pszPhysicalPath, pszPhysicalPath, dwPhysicalPathSize);
	pContainer->dwPhysicalPathSize = dwPhysicalPathSize;

	return pContainer;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnSendResponse(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_SEND_RESPONSE pHttpFilterSendResponse)
{	
	MyOutputDebugString(L"IISxpressFilter::OnSendResponse()\n");

	// get the container
	FilterContextContainer* pContainer = GetContainer(fc);

	// we must have a valid container
	if (pContainer == NULL)
	{		
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() incoming container is NULL\n";
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);

		PerfCountersAddRejectedResponse(Internal);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// make sure the URL map path has been put into the container for us by OnURLMap
	if (pContainer->pszPhysicalPath == NULL)
	{
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() incoming URL path is NULL\n";
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);				

		PerfCountersAddRejectedResponse(Internal);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	// we must be able to determine some information about the response
	if (pHttpFilterSendResponse == NULL || pHttpFilterSendResponse->GetHeader == NULL)
	{			
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() incoming PHTTP_FILTER_SEND_RESPONSE is NULL\n";
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);

		PerfCountersAddRejectedResponse(Internal);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// ***********************************************************************************************************************************

	// get the response code
	DWORD dwResponseCode = pHttpFilterSendResponse->HttpStatus;	

	// only deal with 200 responses
	if (dwResponseCode != 200)
	{		
		const WCHAR* pszFormat = L"IISxpressFilter::OnSendResponse() response code is not 200 (%u)\n";
		CString sError; sError.Format(pszFormat, dwResponseCode);
		MyOutputDebugString(sError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, sError);

		PerfCountersAddRejectedResponse(ResponseCode);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// ***********************************************************************************************************************************
	
	char szRemoteAddr[256] = "\0";
	DWORD dwRemoteAddrSize = sizeof(szRemoteAddr);
	fc.GetServerVariable("REMOTE_ADDR", szRemoteAddr, &dwRemoteAddrSize);

	// determine if the request came from localhost
	if (m_Config.GetCompressLocalhost() == false)
	{		
		// handle 127.0.0.1 as a special case - it's always here and we want to avoid the
		// critical section involved in the ip address set lookup
		if (strcmp(szRemoteAddr, "127.0.0.1") == 0)
		{
			const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() localhost is disabled\n";			
			MyOutputDebugString(pszError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, pszError);

			PerfCountersAddRejectedResponse(LocalhostDisabled);

			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}	
		
		// lookup the IP address
		bool found = m_IPLookup.MatchIPAddress(szRemoteAddr);

		if (found == true)
		{
			const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() localhost is disabled\n";			
			MyOutputDebugString(pszError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, pszError);			

			PerfCountersAddRejectedResponse(LocalhostDisabled);

			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
	}

	// ***********************************************************************************************************************************

	char szContentType[256] = "";
	DWORD dwContentTypeLength = sizeof(szContentType);
	
	// get the context type from the header	
	if (pHttpFilterSendResponse->GetHeader(fc, "Content-Type:", szContentType, &dwContentTypeLength) == FALSE)
	{
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() unable to get content type from header\n";			
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);

		PerfCountersAddRejectedResponse(MissingContentType);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	// ***********************************************************************************************************************************

	// get the context length from the header (it may not be present)
	DWORD dwContentLength = 0;
	char szContentLength[64] = "";	
	DWORD dwContentLengthLength = sizeof(szContentLength);
	if (pHttpFilterSendResponse->GetHeader(fc, "Content-Length:", szContentLength, &dwContentLengthLength) == TRUE)
	{
		dwContentLength = (DWORD) _atoi64(szContentLength);
	}

	// if the content length is zero then we have nothing to compress
	if (dwContentLength == 0)
	{		
		char szConnection[64] = "";	
		DWORD dwConnectionLength = _countof(szConnection);
		if (pHttpFilterSendResponse->GetHeader(fc, "Connection:", szConnection, &dwConnectionLength) == FALSE || strcmp(szConnection, "close") != 0)
		{
			const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() content length is zero or not available\n";			
			MyOutputDebugString(pszError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, pszError);		

			PerfCountersAddRejectedResponse(InvalidContentLength);

			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
	}

	// ***********************************************************************************************************************************

	char szAcceptEncoding[128] = "\0";
	DWORD dwAcceptEncodingLength = sizeof(szAcceptEncoding);
	fc.GetServerVariable("HTTP_ACCEPT_ENCODING", szAcceptEncoding, &dwAcceptEncodingLength);

	if (strstr(szAcceptEncoding, "deflate") == NULL && strstr(szAcceptEncoding, "gzip") == NULL && strstr(szAcceptEncoding, "bzip2") == NULL)
	{
		const WCHAR* pszFormat = L"IISxpressFilter::OnSendResponse() the client does not accept compressed responses ('%s')\n";
		CString sError; sError.Format(pszFormat, szAcceptEncoding);
		MyOutputDebugString(sError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, sError);		

		PerfCountersAddRejectedResponse(IncompatibleClient);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// ***********************************************************************************************************************************

	char szContentEncoding[128] = "\0";
	DWORD dwContentEncodingLength = sizeof(szContentEncoding);
	BOOL bAlreadyEncoded = pHttpFilterSendResponse->GetHeader(fc, "Content-Encoding:", szContentEncoding, &dwContentEncodingLength);

	char szTransferEncoding[128] = "\0";
	DWORD dwTransferEncodingLength = sizeof(szTransferEncoding);
	bAlreadyEncoded |= pHttpFilterSendResponse->GetHeader(fc, "Transfer-Encoding:", szTransferEncoding, &dwTransferEncodingLength);
	
	if (bAlreadyEncoded == TRUE)
	{
		const WCHAR* pszFormat = L"IISxpressFilter::OnSendResponse() the response is already encoded ('%s' or '%s')\n";
		CString sError; sError.Format(pszFormat, szContentEncoding, szTransferEncoding);
		MyOutputDebugString(sError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, sError);		

		PerfCountersAddRejectedResponse(AlreadyEncoded);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	// ***********************************************************************************************************************************

	// make sure the request method	is supported
	char szMethod[32] = "\0";
	DWORD dwMethodSize = sizeof(szMethod);
	fc.GetServerVariable("HTTP_METHOD", szMethod, &dwMethodSize);
	if (szMethod[0] != 'G' || szMethod[1] != 'E' || szMethod[2] != 'T')
	{
		if (!m_Config.HandlePOSTResponses() || szMethod[0] != 'P' || szMethod[1] != 'O' || szMethod[2] != 'S' || szMethod[3] != 'T')
		{
			const WCHAR* pszFormat = L"IISxpressFilter::OnSendResponse() the request method is not supported ('%s')\n";
			CString sError; sError.Format(pszFormat, szMethod);
			MyOutputDebugString(sError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, sError);

			PerfCountersAddRejectedResponse(RequestMethod);

			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
	}

	// ***********************************************************************************************************************************

	char szUserAgent[256] = "\0";
	DWORD dwUserAgentSize = sizeof(szUserAgent);
	fc.GetServerVariable("HTTP_USER_AGENT", szUserAgent, &dwUserAgentSize);	

	bool excludedUserAgent = false;
	if (m_Config.GetUserAgentExclusionEnabled())
	{
		DWORD dwUserAgentCacheCookie = m_Config.GetLoadCookie();
		
		if (!m_UserAgentCache.GetUserAgentState(dwUserAgentCacheCookie, szUserAgent, excludedUserAgent))
		{
			Ripcord::IISxpress::HttpUserAgent::UserAgentProducts<std::string> agent;
			if (agent.ParseUserAgentString(szUserAgent) == S_OK)
			{
				const Ripcord::IISxpress::HttpUserAgent::RuleUserAgents<std::string>& ruleAgents = m_Config.GetExcludedUserAgents();
				if (ruleAgents.Compare(agent))
				{
					excludedUserAgent = true;
				}

				m_UserAgentCache.AddUserAgentState(dwUserAgentCacheCookie, szUserAgent, excludedUserAgent);
			}
		}
	}
	
	if (excludedUserAgent)
	{
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() the user agent has been excluded\n";			
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
		{
			m_Log.Append(true, pszError);			
		}

		PerfCountersAddRejectedResponse(NeverRuleMatch);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	// ***********************************************************************************************************************************

	// get a connection to the server from the GIT (if it is connected)
	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;
	if (m_pGlobalIISxpressHTTPRequest.GetCookie() != 0)	
	{
		m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest);
	}

	if (pIISxpressHTTPRequest == NULL)
	{
		// revoke the pointer in the GIT
		m_pGlobalIISxpressHTTPRequest.Revoke();

		// try to connect to the server
		if (GetHTTPRequestObject(&pIISxpressHTTPRequest) == false || pIISxpressHTTPRequest == NULL)	
		{
			const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() unable to connect to server\n";			
			MyOutputDebugString(pszError);

			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
				m_Log.Append(true, pszError);			

			PerfCountersAddRejectedResponse(NoCompressionServer);

			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}		

		// store the resulting pointer in the GIT
		m_pGlobalIISxpressHTTPRequest = pIISxpressHTTPRequest;
	}		
	
	IISInfo IIS;
	memset(&IIS, 0, sizeof(IIS));
	
	char szServerName[256] = "\0";
	DWORD dwServerNameSize = sizeof(szServerName);
	fc.GetServerVariable("SERVER_NAME", szServerName, &dwServerNameSize);
	
	char szServerPort[16] = "\0";
	DWORD dwServerPortSize = sizeof(dwServerPortSize);
	fc.GetServerVariable("SERVER_PORT", szServerPort, &dwServerPortSize);

	char szInstanceId[32] = "\0";
	DWORD dwInstanceIdSize = sizeof(szInstanceId);
	fc.GetServerVariable("INSTANCE_ID", szInstanceId, &dwInstanceIdSize);	

	IIS.pszServerName = szServerName;
	IIS.pszServerPort = szServerPort;
	IIS.pszInstanceId = szInstanceId;
	IIS.pszURLMapPath = pContainer->pszPhysicalPath;	

	char szURI[2048] = "\0";
	DWORD dwURISize = sizeof(szURI);
	fc.GetServerVariable("URL", szURI, &dwURISize);

	char szHostname[256] = "\0";
	DWORD dwHostnameSize = sizeof(szHostname);
	fc.GetServerVariable("HTTP_HOST", szHostname, &dwHostnameSize);

	char szQueryString[4096] = "\0";
	DWORD dwQueryStringSize = sizeof(szQueryString);
	fc.GetServerVariable("QUERY_STRING", szQueryString, &dwQueryStringSize);	

	RequestInfo Request;
	memset(&Request, 0, sizeof(Request));	
	Request.pszUserAgent = szUserAgent;
	Request.pszHostname = szHostname;
	Request.pszURI = szURI;
	Request.pszQueryString = szQueryString;
	Request.pszAcceptEncoding = szAcceptEncoding;
	Request.pszRemoteAddress = szRemoteAddr;			

	char szLastModified[128] = "\0";
	DWORD dwLastModifiedLength = sizeof(szLastModified);
	pHttpFilterSendResponse->GetHeader(fc, "Last-Modified:", szLastModified, &dwLastModifiedLength);

	ResponseInfo Response;			
	memset(&Response, 0, sizeof(Response));
	Response.pszContentType = szContentType;
	Response.dwContentLength = dwContentLength;
	Response.dwResponseCode = dwResponseCode;
	Response.pszLastModified = szLastModified;

	if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)	
	{				
		m_Log.Append(true, L"IISxpressFilter(0x%08x): IIS: server name='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szServerName);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): IIS: server port='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szServerPort);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): IIS: instance id='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szInstanceId);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): IIS: URL map path='%s'\n", (PHTTP_FILTER_CONTEXT)fc, pContainer->pszPhysicalPath);
	}			

	if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)	
	{				
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: method='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szMethod);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: hostname='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szHostname);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: user agent='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szUserAgent);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: URI='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szURI);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: accept-encoding='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szAcceptEncoding);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: client address='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szRemoteAddr);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): REQUEST: query string='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szQueryString);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): RESPONSE: content-type='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szContentType);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): RESPONSE: content-length=%u\n", (PHTTP_FILTER_CONTEXT)fc, dwContentLength);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): RESPONSE: response code=%u\n", (PHTTP_FILTER_CONTEXT)fc, dwResponseCode);
		m_Log.Append(true, L"IISxpressFilter(0x%08x): RESPONSE: last-modified='%s'\n", (PHTTP_FILTER_CONTEXT)fc, szLastModified);
	}						

	IISxpressFilterContextHolder Context(IISxpressFilterContext::AllocateContext());
	if (!Context)
	{
		const WCHAR* pszError = L"IISxpressFilter::OnSendResponse() unable to allocate context memory\n";			
		MyOutputDebugString(pszError);

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_ENH)
			m_Log.Append(true, pszError);

		PerfCountersAddRejectedResponse(MemoryAllocationFailed);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	if (m_nCacheEnabled != 0 && szQueryString[0] == '\0' && dwContentLength != 0 && szLastModified[0] != '\0')
	{
		ResponseCache::CreateResponseCacheKey(
			szInstanceId, dwInstanceIdSize - 1,
			szURI, dwURISize - 1,
			szContentType, dwContentTypeLength - 1,
			dwContentLength, 
			szLastModified, dwLastModifiedLength - 1,
			szAcceptEncoding, dwAcceptEncodingLength - 1,
			Context->sCacheKey);

		HCACHEITEM hCacheItem = NULL;
		HRESULT hr = m_ResponseCache.LookupEntry(Context->sCacheKey, &hCacheItem);
		if (hr == S_OK)
		{				
			Context->state = IISxpressFilterContext::Cached;				
			Context->hCacheItem = hCacheItem;
		}
	}

	// update the cache perf counters based on the cache cookie
	PerfCountersUpdateCacheStatus(false);

	// ***********************************************************************
	// *** NOTE: past this point we must allow OnEndOfRequest to be called
	// ***********************************************************************

	const DWORD dwTimerLoggingLevel = Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_FULL;
	bool useTimer = (m_Config.GetDebugEnabled() || m_Config.GetLoggingLevel() >= dwTimerLoggingLevel);

	// start timer
	__int64 nStartTimer = useTimer ? g_Timer.GetMicroSecTimerCount() : 0;

	DWORD dwFilterContext = 0;
	HRESULT hr = E_FAIL;
	if (Context->state != IISxpressFilterContext::Cached)
	{
		hr = pIISxpressHTTPRequest->OnSendResponse(&IIS, &Request, &Response, &dwFilterContext);				
	}
	else
	{
		// get the item from the cache
		ResponseCacheItem* pCacheItem = NULL;
		DWORD dwCacheItemSize = 0;
		hr = m_ResponseCache.GetData(Context->hCacheItem, (void**) &pCacheItem, &dwCacheItemSize);

		if (hr == S_OK)
		{
			pIISxpressHTTPRequest->NotifyCacheHit(&IIS, &Request, &Response, pCacheItem->dwContentLength);
		}
		else
		{
			m_ResponseCache.ReleaseEntry(Context->hCacheItem);
			Context->hCacheItem = NULL;
		}		
	}

	__int64 nEndTimer = useTimer ? g_Timer.GetMicroSecTimerCount() : 0;
	__int64 nInterval = nEndTimer - nStartTimer;
	
	if (FAILED(hr) == TRUE)
	{
		// get the facility code of the failure
		DWORD dwFacility = HRESULT_FACILITY(hr);

		// if it isn't NULL or ITF then assume the server is now invalid
		if (dwFacility != FACILITY_NULL && dwFacility != FACILITY_ITF)
		{
			// the GIT is invalid, dump it
			m_pGlobalIISxpressHTTPRequest.Revoke();

			// the server connection is invalid, dump it
			pIISxpressHTTPRequest = NULL;		
		}
	}

	// output duration if debug enabled
	if (m_Config.GetDebugEnabled() == true)
	{				
		MyOutputDebugString(L"IISxpressFilter(0x%08x): pIISxpressHTTPRequest->OnSendResponse() returns 0x%08x, call took %I64d us\n", 
			(PHTTP_FILTER_CONTEXT)fc, hr, nInterval);	
	}

	// output duration info to logging
	if (m_Config.GetLoggingLevel() >= dwTimerLoggingLevel)
	{
		m_Log.Append(true, L"IISxpressFilter(0x%08x): OnSendResponse() returns 0x%08x, call took %I64d us\n", 
			(PHTTP_FILTER_CONTEXT)fc, hr, nInterval);
	}	

	if (hr == S_OK)
	{						
		// store the context cookie since we will need it later
		Context->dwContext = dwFilterContext;		

		// we want to know when the 'compression' started
		Context->nCompressionStart = nStartTimer;		

		// we need to store the content length so that the block handler knows how big the response is
		// (if we don't know what the content length is - it may be 0 - then just assume the response is very big)
		Context->dwRemainingDataBytes = dwContentLength != 0 ? dwContentLength : -1;
		Context->dwContentLength = dwContentLength != 0 ? dwContentLength : -1;

		// we keep the context with the container
		pContainer->SetContext(Context.Detach());	

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)	
		{
			m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - will handle response (0x%08x)\n"), (PHTTP_FILTER_CONTEXT)fc, hr);				
		}				
	}
	else
	{
		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)	
		{
			m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - won't handle response (0x%08x)\n"), (PHTTP_FILTER_CONTEXT)fc, hr);				
		}

		PerfCountersAddRejectedResponse(hr);

		// ask IIS to not tell us any more about this response (we have already freed the memory
		// so we don't need to allow OnEndOfRequest to be called)
		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
	}

	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnSendRawData(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData)
{	
	DWORD dwStatus = SF_STATUS_REQ_NEXT_NOTIFICATION;

	// get the container
	FilterContextContainer* pContainer = GetContainer(fc);		

	// if there is no container then ask to know nothing else about this response
	if (pContainer == NULL)
	{
		PerfCountersAddRejectedResponse(Default);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return dwStatus;
	}

	// get the context data
	IISxpressFilterContext* pContext = pContainer->GetContext();

	// if there is no filter context then we are not to handle it
	if (pContext == NULL || pContext->state == IISxpressFilterContext::Ignore || pContext->dwContext == IISXPRESSFILTER_INVALID_CONTEXT)
	{
		PerfCountersAddRejectedResponse(Default);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return dwStatus;
	}	

	if (pContext->dwRawBlockCount == 0)
	{
		// the response may come from the cache
		if (pContext->state == IISxpressFilterContext::Cached)
		{
			return HandleCacheHit(fc, pRawData);
		}
		else
		{
			dwStatus = OnSendRawHeaderData(fc, *pContext, pRawData);
		}
	}
	else
	{
		dwStatus = OnSendRawBodyData(fc, *pContext, pRawData);
	}

	// keep a count of the number of raw blocks received
	pContext->dwRawBlockCount++;

	return dwStatus;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::HandleCacheHit(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData)
{
	DWORD dwStatus = SF_STATUS_REQ_NEXT_NOTIFICATION;

	// get the container
	FilterContextContainer* pContainer = GetContainer(fc);		

	// if there is no container then ask to know nothing else about this response
	if (pContainer == NULL)
	{
		PerfCountersAddRejectedResponse(Default);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);
		return dwStatus;
	}

	// get the context data
	IISxpressFilterContext* pContext = pContainer->GetContext();

	if (pContext->state != IISxpressFilterContext::Cached)
	{
		return dwStatus;
	}

	// get the item from the cache
	ResponseCacheItem* pCacheItem = NULL;
	DWORD dwCacheItemSize = 0;
	HRESULT hr = m_ResponseCache.GetData(pContext->hCacheItem, (void**) &pCacheItem, &dwCacheItemSize);	

	CMyHTTPResponseData header;
	header.Init(false, pRawData->cbInData, pRawData->cbInData);
	header.AppendData((BYTE*) pRawData->pvInData, pRawData->cbInData, true);

	header.SetParam("Content-Length", pCacheItem->sContentLength);
	header.SetParam("Content-Encoding", pCacheItem->sContentEncoding);

	std::string sHeader;
	header.GetHTTPHeader(sHeader);

	// write the header to the client
	DWORD dwHeaderLength = (DWORD) sHeader.size();
	fc.WriteClient((LPVOID) sHeader.c_str(), &dwHeaderLength);

	// write the compressed body data to the client
	DWORD dwContentLength = pCacheItem->dwContentLength;
	fc.WriteClient(pCacheItem->pbyData, &dwContentLength);

	PerfCountersAddCachedResponse(pContext->dwContentLength, pCacheItem->dwContentLength);

	m_ResponseCache.ReleaseEntry(pContext->hCacheItem);
	pContext->hCacheItem = NULL;

	pRawData->cbInData = 0;
	dwStatus = SF_STATUS_REQ_FINISHED_KEEP_CONN;	

	DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST);

	return dwStatus;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnSendRawHeaderData(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData)
{	
	// determine whether to handle the response or not
	if (Context.state == IISxpressFilterContext::Ignore || Context.dwContext == IISXPRESSFILTER_INVALID_CONTEXT)
	{		
		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	

	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;		
	if (m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest) != S_OK)
	{
		// we couldn't get a server, don't progress the response
		Context.state = IISxpressFilterContext::Ignore;

		PerfCountersAddRejectedResponse(NoCompressionServer);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// allocate the stream object
	if (Context.AllocateStream() != S_OK)
	{
		// we couldn't allocate the stream, don't progress
		Context.state = IISxpressFilterContext::Ignore;

		PerfCountersAddRejectedResponse(MemoryAllocationFailed);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// calculate how many body bytes are in this block (following the header)
	int nBodySize = 0;
	const char* buffer = (const char*) pRawData->pvInData;
	const char* bufferEnd = (const char*) pRawData->pvInData + pRawData->cbInData;
	const char headerEndToken[] = "\r\n\r\n";
	const char* headerEnd = std::search(buffer, bufferEnd, &headerEndToken[0], &headerEndToken[3]);
	if (headerEnd != bufferEnd)
	{
		// take account of the end of header token
		headerEnd += 4;

		// calculate how long the header is
		int headerSize = int(headerEnd - buffer);		

		// calculate how much body data is in this block
		nBodySize = pRawData->cbInData - headerSize;
	}

	// attach the response data
	Context.pResponseStream->AttachBuffer(pRawData->pvInData, pRawData->cbInBuffer, pRawData->cbInData);

	HRESULT hr = DispatchStream(pIISxpressHTTPRequest, fc, Context, pRawData);

	if (hr == S_OK)
	{
		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
		{
			m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes\n"), 
				(PHTTP_FILTER_CONTEXT)fc, pRawData->cbInData);
		}

		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}
	else if (hr == E_PENDING)
	{
		// keep track of the number of body bytes written to the stream
		Context.dwRemainingDataBytes -= nBodySize;

		// the compression engine wants more data, ask IIS not to return any data yet
		pRawData->cbInData = 0;

		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}
	else
	{
		// the server doesn't want to handle the response, so don't progress any further
		Context.state = IISxpressFilterContext::Ignore;

		PerfCountersAddRejectedResponse(hr);

		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);		

		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}	
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnSendRawBodyData(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData)
{	
	// determine whether to handle the response or not
	if (Context.state == IISxpressFilterContext::Ignore || Context.dwContext == IISXPRESSFILTER_INVALID_CONTEXT)
	{		
		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	// handle the special case of the body data being contained in the first block
	if (Context.dwRawBlockCount == 1 && Context.dwContentLength == pRawData->cbInData)
	{
		return HandleSingleBodyBlock(fc, Context, pRawData);
	}	
		
	// if we are on the first block then we need to allocate memory to hold the raw response data
	if (Context.dwRawBlockCount == 1)
	{		
		// use the default buffer size
		DWORD dwBufferSize = IISXPRESSFILTER_DEFAULTSTREAMSIZE;

		// modify the buffer size if the request is smaller
		if (Context.dwContentLength != 0 && dwBufferSize > Context.dwContentLength)
			dwBufferSize = Context.dwContentLength;		

		// allocate the memory we need for the stream buffer
		ULARGE_INTEGER nNewSize;
		nNewSize.HighPart = RESPONSESTREAM_SETSIZE_RESETANDALLOC;
		nNewSize.LowPart = dwBufferSize;
		HRESULT hr = Context.pResponseStream->SetSize(nNewSize);
		if (FAILED(hr) == TRUE)
		{
			// we couldn't allocate memory
			Context.state = IISxpressFilterContext::Ignore;

			PerfCountersAddRejectedResponse(MemoryAllocationFailed);
			
			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
	}

	// write the data into the stream
	ULONG nWritten = 0;
	HRESULT hr = Context.pResponseStream->Write(pRawData->pvInData, pRawData->cbInData, &nWritten);

	if (hr != S_OK)
	{
		PerfCountersAddRejectedResponse(Internal);

		// if we are on the first block we can at least send back the original data
		if (Context.dwRawBlockCount == 1)
		{			
			Context.state = IISxpressFilterContext::Ignore;
			
			DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
		else
		{	
			// we can't do much but generate a fatal
			SendFatalError(fc, pRawData);
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}
	}

	// keep track of the number of bytes written to the stream
	Context.dwRemainingDataBytes -= nWritten;

	// if we have overflowed the buffer or have no more data left then we need to send the data
	if (nWritten != pRawData->cbInData || Context.dwRemainingDataBytes == 0)
	{
		// get hold of the server		
		CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;			
		if (m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest) != S_OK)
		{
			PerfCountersAddRejectedResponse(NoCompressionServer);

			// if we are on the first block we can at least send back the original data
			if (Context.dwRawBlockCount == 1)
			{			
				Context.state = IISxpressFilterContext::Ignore;
				
				DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
				return SF_STATUS_REQ_NEXT_NOTIFICATION;
			}
			else
			{
				// we can't do much but generate a fatal
				SendFatalError(fc, pRawData);
				return SF_STATUS_REQ_NEXT_NOTIFICATION;
			}
		}

		hr = DispatchStream(pIISxpressHTTPRequest, fc, Context, pRawData);		

		if (hr == S_OK)
		{			
			if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
			{
				m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes\n"), 
					(PHTTP_FILTER_CONTEXT)fc, pRawData->cbInData);
			}

			// the response is complete, all is done
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
		}

		if (hr == E_PENDING)
		{
			// put any extra data into the stream for the next call
			if (nWritten != pRawData->cbInData)
			{
				// we need to reset to the beginning of the stream				
				Context.pResponseStream->ResetOffset();

				// write any data still outstanding
				hr = Context.pResponseStream->Write(pRawData->pvInData, pRawData->cbInData - nWritten, &nWritten);

				// keep track of the number of bytes written to the stream
				Context.dwRemainingDataBytes -= nWritten;

				// handle the case where the response is complete
				if (Context.dwRemainingDataBytes == 0)
				{
					// the response is complete
					hr = DispatchStream(pIISxpressHTTPRequest, fc, Context, pRawData);

					if (hr == S_OK && m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
					{
						m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes\n"), 
							(PHTTP_FILTER_CONTEXT)fc, pRawData->cbInData);
					}

					return SF_STATUS_REQ_NEXT_NOTIFICATION;
				}
			}
		}
		else
		{
			PerfCountersAddRejectedResponse(Internal);

			// something real weird happened...
			// if we are on the first block we can at least send back the original data
			if (Context.dwRawBlockCount == 1)
			{			
				Context.state = IISxpressFilterContext::Ignore;
				
				DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
				return SF_STATUS_REQ_NEXT_NOTIFICATION;
			}
			else
			{
				// we can't do much but generate a fatal
				SendFatalError(fc, pRawData);
				return SF_STATUS_REQ_NEXT_NOTIFICATION;
			}
		}
	}
	
	// we are not sending any data back to the client yet
	pRawData->cbInData = 0;

	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

HRESULT IISxpressFilter::DispatchStream(IIISxpressHTTPRequest* pIISxpressHTTPRequest, 
										 Ripcord::IIS::ISAPIFilterContent& fc, 
										 IISxpressFilterContext& Context, 
										 PHTTP_FILTER_RAW_DATA pRawData)
{
	if (pIISxpressHTTPRequest == NULL || pRawData == NULL)
		return E_FAIL;	

	// get the amount of data in the stream			
	DWORD dwStreamOffset = Context.pResponseStream->GetOffset();

	// we need to reset to the beginning of the stream			
	Context.pResponseStream->ResetOffset();

	__int64 nStartTimer = g_Timer.GetMicroSecTimerCount();			

	char szContentEncoding[32] = "";

	// pass the data to the server
	HRESULT hr = pIISxpressHTTPRequest->OnSendRawData(Context.dwContext, Context.pResponseStream, dwStreamOffset, TRUE, (signed char*) szContentEncoding, _countof(szContentEncoding));	

	__int64 nEndTimer = g_Timer.GetMicroSecTimerCount();
	__int64 nInterval = nEndTimer - nStartTimer;

	// output duration if debug enabled
	if (m_Config.GetDebugEnabled() == true)
	{				
		MyOutputDebugString(
			L"IISxpressFilter(0x%08x): pIISxpressHTTPRequest->OnSendRawData() returns 0x%08x, call took %I64d us\n", 
			(PHTTP_FILTER_CONTEXT)fc, hr, nInterval);	
	}

	// output duration info to logging
	if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_FULL)
	{
		m_Log.Append(true, L"IISxpressFilter(0x%08x): OnSendRawData() returns 0x%08x, call took %I64d us\n", 
			(PHTTP_FILTER_CONTEXT)fc, hr, nInterval);	
	}

	if (hr == S_OK)
	{
		Context.pResponseStream->GetBuffer(
			&pRawData->pvInData, 
			&pRawData->cbInBuffer,
			&pRawData->cbInData);

		// mark as having been sent to the client
		Context.state = IISxpressFilterContext::SentToClient;

		DWORD dwBodySize = GetResponseBodySize((const BYTE*) pRawData->pvInData, pRawData->cbInData);
		if (dwBodySize > 0 && dwBodySize < pRawData->cbInData)
		{
			const BYTE* pbyBody = (const BYTE*) pRawData->pvInData + pRawData->cbInData - dwBodySize;
			AddResponseDataToCache(Context.sCacheKey, szContentEncoding, pbyBody, dwBodySize);
				
			PerfCountersAddCompressedResponse(Context.dwContentLength, dwBodySize);
		}
	}

	// we need to reset to the beginning of the stream			
	Context.pResponseStream->ResetOffset();

	return hr;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::HandleSingleBodyBlock(Ripcord::IIS::ISAPIFilterContent& fc, IISxpressFilterContext& Context, PHTTP_FILTER_RAW_DATA pRawData)
{	
	// this is the first body block so since the block is the same size as
	// the body then we don't need to allocate, just attach to the stream
	Context.pResponseStream->AttachBuffer(pRawData->pvInData, pRawData->cbInBuffer, 0);

	// get hold of the server		
	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;		
	if (m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest) != S_OK)
	{
		Context.state = IISxpressFilterContext::Ignore;

		// we don't want to know about any more body data
		DisableIISNotifications(fc, SF_NOTIFY_SEND_RAW_DATA);
		return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}

	char szContentEncoding[32] = "";

	// pass the data to the server
	HRESULT hr = pIISxpressHTTPRequest->OnSendRawData(Context.dwContext, Context.pResponseStream, pRawData->cbInData, TRUE, (signed char*) szContentEncoding, _countof(szContentEncoding));

	// get the updated stream variables
	if (hr == S_OK)
	{
		Context.pResponseStream->GetBuffer(
				&pRawData->pvInData, 
				&pRawData->cbInBuffer,
				&pRawData->cbInData);

		DWORD dwBodySize = GetResponseBodySize((const BYTE*) pRawData->pvInData, pRawData->cbInData);
		if (dwBodySize > 0 && dwBodySize < pRawData->cbInData)
		{
			const BYTE* pbyBody = (const BYTE*) pRawData->pvInData + pRawData->cbInData - dwBodySize;
			AddResponseDataToCache(Context.sCacheKey, szContentEncoding, pbyBody, dwBodySize);				
					
			PerfCountersAddCompressedResponse(Context.dwContentLength, dwBodySize);
		}

		if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
		{
			m_Log.Append(true, _T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes\n"), 
				(PHTTP_FILTER_CONTEXT)fc, pRawData->cbInData);
		}
	}

	// mark as handled
	Context.state = IISxpressFilterContext::SentToClient;

	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnEndOfRequest(Ripcord::IIS::ISAPIFilterContent& fc)
{
	// get the container
	FilterContextContainer* pContainer = GetContainer(fc);
	if (pContainer != NULL)
	{
		// if the response is not marked for handling then just return now
		IISxpressFilterContext* pContext = pContainer->GetContext();
		if (pContext != NULL && pContext->dwContext != IISXPRESSFILTER_INVALID_CONTEXT)
		{
			MyOutputDebugString(L"IISxpressFilter::OnEndOfRequest()\n");

			CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;			
			m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest);

			if (pIISxpressHTTPRequest != NULL)
			{	
				// if we haven't already sent the response to the client then we need to get the data now
				CComPtr<IStream> pStream;
				DWORD dwStreamOffset = 0;
				if (pContext->state == IISxpressFilterContext::Default &&
					pContext->pResponseStream != NULL)
				{								
					pStream = pContext->pResponseStream;

					// get the amount of data in the stream			
					dwStreamOffset = pContext->pResponseStream->GetOffset();			

					// we need to reset to the beginning of the stream			
					pContext->pResponseStream->ResetOffset();
				}			

				char szContentEncoding[32] = "";

				HRESULT hr = pIISxpressHTTPRequest->OnEndOfRequest(pContext->dwContext, pStream, dwStreamOffset, TRUE, (signed char*) szContentEncoding, _countof(szContentEncoding));

				// the context is now invalid (this will also prevent filter re-entry on the WriteClient)
				pContext->dwContext = IISXPRESSFILTER_INVALID_CONTEXT;

				if (SUCCEEDED(hr) == TRUE)
				{				
					if (pStream != NULL && hr == S_OK)
					{
						void* pData = NULL;
						DWORD dwSize = 0;
						DWORD dwOffset = 0;
						pContext->pResponseStream->GetBuffer(&pData, &dwSize, &dwOffset);

						// since the write will invoke the filter again we need to switch notifications off					
						DisableIISNotifications(fc, SF_NOTIFY_SEND_RESPONSE | SF_NOTIFY_SEND_RAW_DATA);

						// send the data to the client
						DWORD dwWriteBytes = dwOffset;
						fc.WriteClient(pData, &dwWriteBytes);										

						if (m_Config.GetDebugEnabled() == true)
						{					
							// we want to know when the 'compression' ended
							__int64 nCompressionEnd = g_Timer.GetMicroSecTimerCount();

							// calculate the interval in ms
							__int64 nCompressionInterval = nCompressionEnd - pContext->nCompressionStart;

							MyOutputDebugString(
								_T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes in %I64d us\n"), 
								(PHTTP_FILTER_CONTEXT)fc, dwOffset, nCompressionInterval);
						}

						if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)	
						{
							// we want to know when the 'compression' ended
							__int64 nCompressionEnd = g_Timer.GetMicroSecTimerCount();

							// calculate the interval in ms
							__int64 nCompressionInterval = nCompressionEnd - pContext->nCompressionStart;

							m_Log.Append(true, 
								_T("IISxpressFilter(0x%08x): ") _T(__FUNCTION__) _T(" - compressed to %u bytes in %I64d us\n"), 
								(PHTTP_FILTER_CONTEXT)fc, dwOffset, nCompressionInterval);
						}

						DWORD dwBodySize = GetResponseBodySize((const BYTE*) pData, dwOffset);
						if (dwBodySize > 0 && dwBodySize < dwOffset)
						{
							const BYTE* pbyBody = (const BYTE*) pData + dwOffset - dwBodySize;
							AddResponseDataToCache(pContext->sCacheKey, szContentEncoding, pbyBody, dwBodySize);
												
							PerfCountersAddCompressedResponse(pContext->dwContentLength, dwBodySize);
						}
					}
				}			
			}
		}

		// ask the container to clean the context up
		pContext = NULL;
		pContainer->FreeContext();
	}
	
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::OnEndOfNetSession(Ripcord::IIS::ISAPIFilterContent& fc)
{
	// get the container
	FilterContextContainer* pContainer = GetContainer(fc);
	if (pContainer != NULL)
	{
		IISxpressFilterContext* pContext = pContainer->GetContext();
		if (pContext != NULL && pContext->dwContext != IISXPRESSFILTER_INVALID_CONTEXT)
		{				
			CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;			
			m_pGlobalIISxpressHTTPRequest.CopyTo(&pIISxpressHTTPRequest);

			if (pIISxpressHTTPRequest != NULL)
			{
				HRESULT hr = pIISxpressHTTPRequest->OnEndOfNetSession(pContext->dwContext);
				if (SUCCEEDED(hr) == TRUE)
				{
					pContext->dwContext = IISXPRESSFILTER_INVALID_CONTEXT;
				}
			}		

			// ask the container to clean the context up
			pContext = NULL;
			pContainer->FreeContext();
		}
	}

	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::MyOutputDebugString(const WCHAR* pszMsg, ...)
{
	if (m_Config.GetDebugEnabled() == true)
	{
		va_list args;
		va_start(args, pszMsg);

		WCHAR szBuffer[8192];
		vswprintf_s(szBuffer, _countof(szBuffer), pszMsg, args);

		::OutputDebugStringW(szBuffer);

		va_end(args);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

bool IISxpressFilter::GetHTTPRequestObject(IIISxpressHTTPRequest** ppIISxpressHTTPRequest)
{
	if (ppIISxpressHTTPRequest == NULL)
		return false;

	CComPtr<IUnknown> pUnk;
	HRESULT hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, &pUnk);
	if (hr != S_OK)
	{
		MyOutputDebugString(L"IISxpressFilter::GetHTTPRequestObject(): GetActiveObject() returns hr=0x%08x\n", hr);		
		return false;
	}
	
	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;
	hr = pUnk->QueryInterface(IID_IIISxpressHTTPRequest, (void**) &pIISxpressHTTPRequest);		
	if (hr != S_OK || pIISxpressHTTPRequest == NULL)
	{
		MyOutputDebugString(L"IISxpressFilter::GetHTTPRequestObject(): QueryInterface() returns hr=0x%08x\n", hr);
		return false;
	}
	
	*ppIISxpressHTTPRequest = pIISxpressHTTPRequest.Detach();
	return true;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::OpenLog(Ripcord::IIS::ISAPIFilterContent& fc)
{
	if (m_Config.GetLoggingLevel() >= Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_BASIC)
	{
		WCHAR szLogPath[512];
		::GetModuleFileName(theApp.m_hInstance, szLogPath, sizeof(szLogPath));

		CPathT<CString> LogPath(szLogPath);
		LogPath.RemoveFileSpec();
		LogPath.Append(_T("\\Logs"));

		if (LogPath.IsDirectory() == FALSE)
		{
			::CreateDirectory(LogPath, NULL);	
		}

		CString sLogFilePrefix("IISxpressFilter-");

		CStringA sAppPoolName;
		if (GetAppPoolName(fc, sAppPoolName) == true)
		{
			sLogFilePrefix += sAppPoolName;
			sLogFilePrefix += "-";
		}		

		m_Log.Init(LogPath, sLogFilePrefix, _T("log"));		
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::SendFatalError(Ripcord::IIS::ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pRawData)
{
	// IIS should not send any data from this response
	pRawData->cbInData = 0;

	// load up the error string
	CStringA sError;
	sError.LoadString(IDS_SERVER500);	

	// get the length of the string
	DWORD dwErrorLength = sError.GetLength();

	// inform the client there is a problem
	fc.WriteClient((void*) (const char*) sError, &dwErrorLength);

	// terminate the connection
	return SF_STATUS_REQ_FINISHED;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::DisableIISNotifications(Ripcord::IIS::ISAPIFilterContent& fc, DWORD dwNotifications)
{
	fc.ServerSupportFunction(SF_REQ_DISABLE_NOTIFICATIONS, NULL, dwNotifications, 0);
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::CheckConfigSettings(Ripcord::IIS::ISAPIFilterContent& fc)
{	
	// get the current load cookie
	DWORD dwLoadCookie = m_Config.GetLoadCookie();

	// load the registry settings if the cookie has changed
	if (m_dwLastConfigCookie != dwLoadCookie)
	{		
		MyOutputDebugString(L"IISxpressFilter::GetRegistrySettings() refreshing registry settings\n");		

		// get the new logging level
		DWORD dwNewLoggingLevel = m_Config.GetLoggingLevel();

		// if the logging level has changed then we need to handle it
		if (m_dwOldLoggingLevel != dwNewLoggingLevel)
		{
			// if the old logging level was set to none then we have to open the log file
			// since the user must have asked for logging
			if (m_dwOldLoggingLevel == Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_NONE)
			{
				OpenLog(fc);
				m_Log.AppendFromResource(theApp.m_hInstance, IDS_LOGGINGSTARTED, true);				
			}
			// if the user has set the logging level to none then they want logging switched off,
			// so close the log file
			else if (dwNewLoggingLevel == Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_NONE)
			{
				m_Log.AppendFromResource(theApp.m_hInstance, IDS_LOGGINGSTOPPED, true);
				m_Log.Close();
			}

			// set the old logging level
			::InterlockedExchange((LONG*) &m_dwOldLoggingLevel, dwNewLoggingLevel);
		}						

		LONG nOldCacheEnabled = ::InterlockedExchange(&m_nCacheEnabled, m_Config.GetCacheEnabled());

		// if the cache has been newly switched off then flush it
		if (nOldCacheEnabled != 0 && m_nCacheEnabled == 0)		
		{			
			m_ResponseCache.SetMaxAllowedEntries(0);
			m_ResponseCache.Flush();
		}

		// get the cache state cookie, if it's changed then the user wants to flush the cache
		DWORD dwCacheStateCookie = (DWORD) ::InterlockedExchange((volatile LONG*) &m_dwCacheStateCookie, m_Config.GetCacheStateCookie());
		if (dwCacheStateCookie != m_dwCacheStateCookie)
		{			
			m_ResponseCache.SetMaxAllowedEntries(0);
			m_ResponseCache.Flush();			
		}

		HRESULT hr = m_ResponseCache.SetMaxAllowedEntries(m_Config.GetCacheMaxEntries());
		hr = m_ResponseCache.SetMaxAllowedSize(m_Config.GetCacheMaxSizeKB() * 1024);

		::InterlockedExchange((LONG*) &m_dwLastConfigCookie, dwLoadCookie);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

FilterContextContainer* IISxpressFilter::GetContainer(Ripcord::IIS::ISAPIFilterContent& fc)
{
	return (FilterContextContainer*) fc.GetContext()->pFilterContext;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::SetContainer(Ripcord::IIS::ISAPIFilterContent& fc, FilterContextContainer* pContainer)
{
	fc.GetContext()->pFilterContext = (void*) pContainer;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

DWORD IISxpressFilter::GetResponseBodySize(const BYTE* pbyData, DWORD dwResponseSize)
{
	DWORD dwResponseBodySize = 0;

	if (pbyData != NULL && dwResponseSize > 0)
	{
		const BYTE headerEnd[4] = { '\r', '\n', '\r', '\n' };
		const BYTE* pbyBody = std::search(pbyData, pbyData + dwResponseSize, &headerEnd[0], &headerEnd[3]);
		if (pbyBody != NULL)
		{
			pbyBody += _countof(headerEnd);
			DWORD dwHeaderSize = DWORD(pbyBody - pbyData);
			dwResponseBodySize = dwResponseSize - dwHeaderSize;
		}
	}

	return dwResponseBodySize;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::AddResponseDataToCache(LPCSTR pszCacheKey, LPCSTR pszContentEncoding, const BYTE* pbyData, DWORD dwResponseSize)
{
	// add the item to the cache if we have a key (it means the data is cachable)
	if (pbyData == NULL || dwResponseSize == 0 || m_nCacheEnabled == 0 || pszCacheKey == NULL || pszCacheKey[0] == '\0' || 
		pszContentEncoding == NULL || pszContentEncoding[0] == '\0')
	{
		return;
	}

	std::auto_ptr<ResponseCacheItem> pCacheItem(new ResponseCacheItem);
	if (pCacheItem.get() != NULL)
	{				
		pCacheItem->sContentEncoding = pszContentEncoding;
		pCacheItem->sContentLength.Format("%u", dwResponseSize);
		pCacheItem->dwContentLength = dwResponseSize;
		pCacheItem->pbyData = new BYTE[dwResponseSize];
		if (pCacheItem->pbyData != NULL)
		{
			ResponseCacheItem* pItem = pCacheItem.release();

			// copy the compressed data and add it to the cache
			memcpy(pItem->pbyData, pbyData, dwResponseSize);			
			m_ResponseCache.Add(pszCacheKey, pItem, pItem->dwContentLength, NULL, NULL, NULL, m_ResponseCacheItemDeallocator);			
			
			PerfCountersUpdateCacheStatus(true);
		}
	}	
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::InitializePerfCounters(Ripcord::IIS::ISAPIFilterContent& fc)
{
	if (m_Config.GetPerfCountersEnabled() == true && m_bPerfCountersInitialized == false)
	{
		CComCritSecLock<CComAutoCriticalSection> cs(m_csPerfCountersInitialized);

		if (m_bPerfCountersInitialized == false)
		{
			CStringA sAppPoolName;
			if (GetAppPoolName(fc, sAppPoolName) == true)
			{
				m_pInstancePerfCounters = CIISxpressISAPIPerfObject::Create(sAppPoolName);
			}

			m_pGlobalPerfCounters = CIISxpressISAPIPerfObject::Create("_Total");

			m_bPerfCountersInitialized = true;
		}		
	}
}

void IISxpressFilter::PerfCountersAddRejectedResponse(HRESULT hr)
{
	PerfCountersAddRejectedResponse(PerfCountersMapServerHResult(hr));
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::PerfCountersAddRejectedResponse(RejectedResponseReasons reason)
{
	PerfCountersAddRejectedResponse(reason, m_pInstancePerfCounters);
	PerfCountersAddRejectedResponse(reason, m_pGlobalPerfCounters);	
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::PerfCountersAddRejectedResponse(RejectedResponseReasons reason, CIISxpressISAPIPerfObject* pObject)
{
	if (pObject != NULL)
	{
		::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses);

		switch (reason)
		{
		case Internal:						::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_Internal); break;
		case FilterDisabled:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_FilterDisabled); break;
		case MemoryAllocationFailed:		::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_MemoryAllocationFailed); break;
		case ResponseCode:					::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_ResponseCode); break;
		case LocalhostDisabled:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_LocalhostDisabled); break;
		case MissingContentType:			::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_MissingContentType); break;
		case InvalidContentLength:			::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_InvalidContentLength); break;
		case IncompatibleClient:			::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_IncompatibleClient); break;
		case AlreadyEncoded:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_AlreadyEncoded); break;
		case RequestMethod:					::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_RequestMethod); break;
		case NoCompressionServer:			::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_NoCompressionServer); break;
		case CompressionServerDisabled:		::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_CompressionServerDisabled); break;
		case NeverRuleMatch:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_NeverRuleMatch); break;
		case StaticRuleMatch:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_StaticRuleMatch); break;
		case CPUBusy:						::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_CPUBusy); break;
		case MemoryLow:						::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_MemoryLow); break;
		case UserRuleExcluded:				::InterlockedIncrement((volatile LONG*) &pObject->totalRejectedResponses_UserRuleExcluded); break;
		}
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

RejectedResponseReasons IISxpressFilter::PerfCountersMapServerHResult(HRESULT hr)
{
	switch (hr)
	{
	case IISXPRESSHTTPREQUEST_SERVERDISABLED:		return CompressionServerDisabled;
	case IISXPRESSHTTPREQUEST_NOT200:				return ResponseCode;
	case IISXPRESSHTTPREQUEST_INCOMPATIBLECLIENT:	return IncompatibleClient;
	case IISXPRESSHTTPREQUEST_MATCHEDNEVERRULE:		return NeverRuleMatch;
	case IISXPRESSHTTPREQUEST_STATICRULEMATCH:		return StaticRuleMatch;
	case IISXPRESSHTTPREQUEST_CPUBUSY:				return CPUBusy;
	case IISXPRESSHTTPREQUEST_LEASECONTEXTFAILED:	return Internal;
	case IISXPRESSHTTPREQUEST_MEMORYLOW:			return MemoryLow;
	case IISXPRESSHTTPREQUEST_ACTIVECONTEXTFAILED:	return Internal;
	case IISXPRESSHTTPREQUEST_BADINCOMINGSTREAM:	return Internal;
	case IISXPRESSHTTPREQUEST_USERRULEOVERLOAD:		return UserRuleExcluded;
	default:										return Default;
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::PerfCountersAddCachedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize)
{
	if (m_pInstancePerfCounters != NULL)
	{
		::InterlockedIncrement((LONG*) &m_pInstancePerfCounters->totalCompressedResponses);
		::InterlockedIncrement((LONG*) &m_pInstancePerfCounters->totalCacheHits);
		g_interlock.InterlockedAdd64(&m_pInstancePerfCounters->totalRawResponseSize, dwOriginalSize);
		g_interlock.InterlockedAdd64(&m_pInstancePerfCounters->totalCompressedResponseSize, dwCompressedSize);
	}

	if (m_pGlobalPerfCounters != NULL)
	{
		::InterlockedIncrement((LONG*) &m_pGlobalPerfCounters->totalCompressedResponses);
		::InterlockedIncrement((LONG*) &m_pGlobalPerfCounters->totalCacheHits);
		g_interlock.InterlockedAdd64(&m_pGlobalPerfCounters->totalRawResponseSize, dwOriginalSize);
		g_interlock.InterlockedAdd64(&m_pGlobalPerfCounters->totalCompressedResponseSize, dwCompressedSize);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::PerfCountersAddCompressedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize)
{
	if (m_pInstancePerfCounters != NULL)
	{
		::InterlockedIncrement((LONG*) &m_pInstancePerfCounters->totalCompressedResponses);
		g_interlock.InterlockedAdd64(&m_pInstancePerfCounters->totalRawResponseSize, dwOriginalSize);
		g_interlock.InterlockedAdd64(&m_pInstancePerfCounters->totalCompressedResponseSize, dwCompressedSize);
	}

	if (m_pGlobalPerfCounters != NULL)
	{
		::InterlockedIncrement((LONG*) &m_pGlobalPerfCounters->totalCompressedResponses);
		g_interlock.InterlockedAdd64(&m_pGlobalPerfCounters->totalRawResponseSize, dwOriginalSize);
		g_interlock.InterlockedAdd64(&m_pGlobalPerfCounters->totalCompressedResponseSize, dwCompressedSize);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void IISxpressFilter::PerfCountersUpdateCacheStatus(bool forceUpdate)
{
	static DWORD dwLastCookie = 0;

	CIISxpressISAPIPerfObject* pPerfCounters = m_pInstancePerfCounters;
	if (pPerfCounters == NULL)
	{
		pPerfCounters = m_pGlobalPerfCounters;
	}

	if (pPerfCounters != NULL && (forceUpdate == true || dwLastCookie != CResponseCacheItemFree::m_dwCacheCookie))
	{
		m_ResponseCache.GetCurrentEntryCount(&pPerfCounters->totalCacheItems);
		m_ResponseCache.GetCurrentAllocSize(&pPerfCounters->totalCacheSize);
	}

	dwLastCookie = CResponseCacheItemFree::m_dwCacheCookie;
}

bool IISxpressFilter::GetAppPoolName(Ripcord::IIS::ISAPIFilterContent& fc, CStringA& sAppPoolName)
{
	bool status = false;

	if (m_OSVerInfo.dwMajorVersion >= 5 && m_OSVerInfo.dwMinorVersion >= 2 && m_OSVerInfo.wProductType == VER_NT_SERVER)
	{
		char szAppPoolName[256] = { '\0' };
		char szAppPath[256] = { '\0' };
		DWORD dwAppPathSize = _countof(szAppPath);
		if (fc.GetServerVariable("APPL_MD_PATH", szAppPath, &dwAppPathSize) == TRUE)
		{
			// assume the instance name will be the application path
			strcpy_s(szAppPoolName, szAppPath);

			// attempt to get the name of the application pool
			Ripcord::Utils::IISMetaBase metaBase;
			if (metaBase.Init() == true)
			{
				CComPtr<IMSAdminBase> pAdminBase;
				metaBase.GetAdminBase(&pAdminBase);

				Ripcord::Utils::IISMetaBaseData appPoolData;
				if (appPoolData.ReadData(pAdminBase, CAtlStringW(szAppPath), MD_APP_APPPOOL_ID) == true)
				{
					std::wstring sAppPoolName;
					if (appPoolData.GetAsString(sAppPoolName) == true)
					{
						strcpy_s(szAppPoolName, CAtlStringA(sAppPoolName.c_str()));
					}
				}
				else if (appPoolData.ReadData(pAdminBase, CAtlStringW(szAppPath), MD_APP_FRIENDLY_NAME) == true)
				{
					std::wstring sAppPoolName;
					if (appPoolData.GetAsString(sAppPoolName) == true)
					{
						strcpy_s(szAppPoolName, CAtlStringA(sAppPoolName.c_str()));
					}
				}
			}

			sAppPoolName = szAppPoolName;
			status = true;
		}
	}

	return status;
}