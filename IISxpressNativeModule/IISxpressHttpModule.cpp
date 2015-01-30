#include "StdAfx.h"
#include "IISxpressHttpModule.h"

#include "ResponseCache.h"

#include "..\IISxpressServer\IISxpressHTTPRequestHResults.h"

#define __FUNCTIONT__ _T(##__FUNCTION__##)

CIISxpressHttpModule::CIISxpressHttpModule(IHttpServer* pHttpServer) : 
		m_nRefCount(0), 
		m_hEventHandlePool(64),
		m_nCacheEnabled(IISXPRESS_CACHE_ENABLED_DEFAULT),
		m_dwCacheStateCookie(IISXPRESS_CACHE_STATECOOKIE_DEFAULT),
		m_dwLastConfigCookie(0),
		m_dwOldLoggingLevel(IISXPRESS_LOGGINGLEVEL_NONE),
		m_pHttpServer(pHttpServer),
		m_UserAgentCache(2000)
{
	// get the number of bytes per sector (fall back to 512 if this doesn't work)
	if (::GetDiskFreeSpace(NULL, NULL, &m_dwBytesPerSector, NULL, NULL) == FALSE)
	{
		m_dwBytesPerSector = 512;
	}

	m_Config.Init(IISXPRESSFILTER_REGKEY, true, true);	

	// get the cache enabled state
	m_nCacheEnabled = m_Config.GetCacheEnabled();

	// get the cache state cookie (we use this to catch flush requests)
	m_dwCacheStateCookie = m_Config.GetCacheStateCookie();

	// init the cache
	HRESULT hr = m_ResponseCache.Initialize(NULL);
	hr = m_ResponseCache.SetMaxAllowedEntries(m_Config.GetCacheMaxEntries());
	hr = m_ResponseCache.SetMaxAllowedSize(m_Config.GetCacheMaxSizeKB() * 1024);

	// get the cache item deallocator
	ResponseCache::GetResponseCacheItemDeallocator(&m_ResponseCacheItemDeallocator);	
}

CIISxpressHttpModule::~CIISxpressHttpModule(void)
{
	if (m_Config.GetLoggingLevel() >= IISXPRESS_LOGGINGLEVEL_BASIC)
	{
		m_Log.AppendFromResource(g_hModule, IDS_FILTERSTOPPED, true);
	}	
}

REQUEST_NOTIFICATION_STATUS CIISxpressHttpModule::OnBeginRequest(IN IHttpContext* pHttpContext, IN IHttpEventProvider* pProvider)
{
    UNREFERENCED_PARAMETER(pProvider);    		

	const TCHAR* const pszMethodName = __FUNCTIONT__;

	// startup the performance counters
	if (m_Config.GetPerfCountersEnabled() && m_pInstancePerfCounters.get() == NULL)
	{
		AutoMemberCriticalSection lock(&m_csPerfCounters);

		if (m_pInstancePerfCounters.get() == NULL)
		{
			CAtlString sAppName;
			GetAppPoolName(pHttpContext, sAppName);

			m_pInstancePerfCounters = auto_ptr<IISxpressNativePerf>(new IISxpressNativePerf(sAppName));			
		}

		// TODO: maybe one day get the global counters to work
		//if (m_pGlobalPerfCounters.get() == NULL)
		//{
		//	m_pGlobalPerfCounters = auto_ptr<IISxpressNativePerf>(new IISxpressNativePerf());
		//}
	}

	IHttpResponse* pHttpResponse = pHttpContext->GetResponse();
	if (pHttpResponse != NULL)
	{
		// Disable caching as HANDLER_HTTPSYS_UNFRIENDLY.
		// TODO: is this really a good idea?
		pHttpResponse->DisableKernelCache();
	}

	// get the registry settings (will only update every 5000ms as default)
	GetRegistrySettings(pHttpContext);

	// only proceed if filter is enabled
	if (m_Config.GetEnabled() == false)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("module is disabled\n"));

		PerfCountersAddRejectedResponse(IISxpressNativePerf::FilterDisabled);

		// don't tell us about the send
		pHttpContext->DisableNotifications(RQ_SEND_RESPONSE, 0);
	}

    // Return processing to the pipeline.
    return RQ_NOTIFICATION_CONTINUE;
}


REQUEST_NOTIFICATION_STATUS CIISxpressHttpModule::OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider)
{
	const TCHAR* const pszMethodName = __FUNCTIONT__;	

	// only proceed if filter is enabled
	if (m_Config.GetEnabled() == false)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("module is disabled\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::FilterDisabled);
		return RQ_NOTIFICATION_CONTINUE;
	}

	IHttpRequest* pHttpRequest = pHttpContext->GetRequest();
	if (pHttpRequest == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetRequest() returned NULL\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}	

	IHttpResponse* pHttpResponse = pHttpContext->GetResponse();
	if (pHttpResponse == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetResponse() returned NULL\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}	

	const HTTP_REQUEST* pRawRequest = pHttpRequest->GetRawHttpRequest();
	if (pRawRequest == NULL)
	{		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetRawHttpRequest() returned NULL\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}		

	const HTTP_RESPONSE* pRawResponse = pHttpResponse->GetRawHttpResponse();
	if (pRawResponse == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetRawHttpResponse() returned NULL\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}		

	// we only handle GET or POST (if POST handling is enabled)
	if (pRawRequest->Verb != HttpVerbGET && !(pRawRequest->Verb == HttpVerbPOST && m_Config.HandlePOSTResponses()))
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("request was not GET or POST\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::RequestMethod);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// only handle status code 200
	if (pRawResponse->StatusCode != 200)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("response code is not 200 (%u)\n"),  pRawResponse->StatusCode);
		PerfCountersAddRejectedResponse(IISxpressNativePerf::ResponseCode);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// determine if the request came from localhost
	if (m_Config.GetCompressLocalhost() == false && IsUserLocalhost(pRawRequest) == true)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext,_T("localhost is disabled\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::LocalhostDisabled);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// ***********************************************************************************************************************************

	const char* pszUserAgent = EnsureNotNull(pHttpRequest->GetHeader(HttpHeaderUserAgent));	

	bool excludedUserAgent = false;
	if (m_Config.GetUserAgentExclusionEnabled())
	{
		DWORD dwUserAgentCacheCookie = m_Config.GetLoadCookie();
		
		if (!m_UserAgentCache.GetUserAgentState(dwUserAgentCacheCookie, pszUserAgent, excludedUserAgent))
		{
			HttpUserAgent::UserAgentProducts<std::string> agent;
			if (agent.ParseUserAgentString(pszUserAgent) == S_OK)
			{
				const HttpUserAgent::RuleUserAgents<std::string>& ruleAgents = m_Config.GetExcludedUserAgents();
				if (ruleAgents.Compare(agent))
				{
					excludedUserAgent = true;
				}

				m_UserAgentCache.AddUserAgentState(dwUserAgentCacheCookie, pszUserAgent, excludedUserAgent);
			}
		}
	}

	if (excludedUserAgent)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("the user agent '%s' has been excluded\n"),  pszUserAgent);		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::NeverRuleMatch);
		return RQ_NOTIFICATION_CONTINUE;
	}	

	// ***********************************************************************************************************************************

	// we must have only one chunk and data from memory or file
	if (pRawResponse->EntityChunkCount == 0)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext,_T("can't convert multi-entity buffer, incoming buffer is zero length\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::InvalidContentLength);
		return RQ_NOTIFICATION_CONTINUE;
	}
	else if (pRawResponse->EntityChunkCount != 1)
	{		
		// turn the multi-chunk response into a single chunk - NB. the response
		// must all be in memory
		if (MakeResponseSingleEntityBlock(pHttpContext, pHttpResponse, pRawResponse) == false)
		{
			AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext,_T("can't convert multi-entity buffer\n"));
			PerfCountersAddRejectedResponse(IISxpressNativePerf::MemoryAllocationFailed);
			return RQ_NOTIFICATION_CONTINUE;
		}				
	}	
	else if (pRawResponse->pEntityChunks->DataChunkType != HttpDataChunkFromMemory &&
			pRawResponse->pEntityChunks->DataChunkType != HttpDataChunkFromFileHandle)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext,_T("response isn't HttpDataChunkFromMemory or HttpDataChunkFromFileHandle\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}

	PHTTP_DATA_CHUNK pEntityChunk = pRawResponse->pEntityChunks;

	if (pEntityChunk == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("the response does not contain any data\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;	
	}

	// TODO: review max size here
	// we don't handle non-zero starting offsets or very large files (>250MB)
	if (pEntityChunk->DataChunkType == HttpDataChunkFromFileHandle &&
		(pEntityChunk->FromFileHandle.ByteRange.StartingOffset.QuadPart > 0 ||
		pEntityChunk->FromFileHandle.ByteRange.Length.QuadPart > (250 * 1024 * 1024)))
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("response has offset > 0 or is too big\n"));
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}

	PCSTR pszContentEncoding = pHttpResponse->GetHeader(HttpHeaderContentEncoding);
	if (pszContentEncoding != NULL && pszContentEncoding[0] != '\0')
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("response already has content encoding\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::AlreadyEncoded);
		return RQ_NOTIFICATION_CONTINUE;
	}

	PCSTR pszTransferEncoding = pHttpResponse->GetHeader(HttpHeaderTransferEncoding);
	if (pszTransferEncoding != NULL && pszTransferEncoding[0] != '\0')
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("response already has transfer encoding\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::AlreadyEncoded);
		return RQ_NOTIFICATION_CONTINUE;
	}

	USHORT nContentTypeLength = 0;
	PCSTR pszContentType = pHttpResponse->GetHeader(HttpHeaderContentType, &nContentTypeLength);			
	if (pszContentType == NULL || nContentTypeLength == 0)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetHeader(HttpHeaderContentType) returned NULL\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::MissingContentType);
		return RQ_NOTIFICATION_CONTINUE;
	}

	USHORT nLastModifiedLength = 0;
	PCSTR pszLastModified = pHttpResponse->GetHeader(HttpHeaderLastModified, &nLastModifiedLength);			

	// ***********************************************************************
	
	USHORT nAcceptEncodingLength = 0;
	PCSTR pszAcceptEncoding = pHttpRequest->GetHeader(HttpHeaderAcceptEncoding, &nAcceptEncodingLength);	
	if (pszAcceptEncoding == NULL || nAcceptEncodingLength == 0 ||
		(strstr(pszAcceptEncoding, "deflate") == NULL && strstr(pszAcceptEncoding, "gzip") == NULL && strstr(pszAcceptEncoding, "bzip2") == NULL))
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("the client does not accept compressed responses\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::IncompatibleClient);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// ***********************************************************************

	const WCHAR* pszScriptTranslated = pHttpContext->GetScriptTranslated();
	if (pszScriptTranslated == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetScriptTranslated() returned NULL\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}
		
	// get the url
	CAtlStringA sScriptTranslated(pszScriptTranslated);

	// get the site id
	CAtlStringA sInstanceId;
	sInstanceId.Format("%u", pHttpRequest->GetSiteId());
	
	// get the server name
	PCSTR pszServerName = NULL;		
	DWORD dwServerNameLength = 0;
	pHttpContext->GetServerVariable("SERVER_NAME", &pszServerName, &dwServerNameLength);
	if (pszServerName == NULL || dwServerNameLength == 0)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetServerVariable(\"SERVER_NAME\") returned NULL\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// get the server port 		
	PCSTR pszServerPort = NULL;		
	DWORD dwServerPortLength = 0;
	pHttpContext->GetServerVariable("SERVER_PORT", &pszServerPort, &dwServerPortLength);
	if (pszServerPort == NULL || dwServerPortLength == 0)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetServerVariable(\"SERVER_PORT\") returned NULL\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}

	DWORD dwContentLength = 0;
	if (pEntityChunk->DataChunkType == HttpDataChunkFromMemory)
	{
		dwContentLength = pRawResponse->pEntityChunks->FromMemory.BufferLength;
	}
	else if (pEntityChunk->DataChunkType == HttpDataChunkFromFileHandle)
	{
		dwContentLength = pEntityChunk->FromFileHandle.ByteRange.Length.LowPart;
	}
	else
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("the DataChunkType (%u) is not supported\n"), pEntityChunk->DataChunkType);		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}
	
	IISInfo iisinfo;
	iisinfo.pszInstanceId = sInstanceId;		
	iisinfo.pszServerName = pszServerName;
	iisinfo.pszServerPort = pszServerPort;
	iisinfo.pszURLMapPath = sScriptTranslated;		

	if (m_Config.GetDebugEnabled() || m_Config.GetLoggingLevel() >= IISXPRESS_LOGGINGLEVEL_ENH)	
	{				
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("IIS: server name='%hs'\n"), pszServerName);		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("IIS: server port='%hs'\n"), pszServerPort);
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("IIS: instance id='%hs'\n"), sInstanceId);
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("IIS: URL map path='%hs'\n"), sScriptTranslated);
	}			

	// ***********************************************************************

	const WCHAR* pszURI = pHttpContext->GetScriptName();
	if (pszURI == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetScriptName() returned NULL\n"));		
		PerfCountersAddRejectedResponse(IISxpressNativePerf::Internal);
		return RQ_NOTIFICATION_CONTINUE;
	}

	// get the uri
	CAtlStringA sURI(pszURI);

	// get the query string
	CAtlStringA sQueryString;
	if (pRawRequest->CookedUrl.pQueryString != NULL)
	{
		sQueryString = pRawRequest->CookedUrl.pQueryString + 1;
	}

	// get the remote address
	// TODO: consider using a cache lookup here 
	CAtlStringA sRemoteAddress;
	if (pRawRequest != NULL && pRawRequest->Address.pRemoteAddress != NULL)			
	{
		if (pRawRequest->Address.pRemoteAddress->sa_family == AF_INET)
		{
			struct sockaddr_in* paddrin = (struct sockaddr_in*) pRawRequest->Address.pRemoteAddress;

			// TODO: make this a bit more efficient
			sRemoteAddress.Format("%u.%u.%u.%u", 
				(unsigned) paddrin->sin_addr.S_un.S_un_b.s_b1,
				(unsigned) paddrin->sin_addr.S_un.S_un_b.s_b2,
				(unsigned) paddrin->sin_addr.S_un.S_un_b.s_b3,
				(unsigned) paddrin->sin_addr.S_un.S_un_b.s_b4);
		}
		else if (pRawRequest->Address.pRemoteAddress->sa_family == AF_INET6)
		{
			SOCKADDR_IN6* paddrin = (SOCKADDR_IN6*) pRawRequest->Address.pRemoteAddress;

			// TODO: make this a bit more efficient
			sRemoteAddress.Format("%0x:%0x:%0x:%0x:%0x:%0x:%0x:%0x",				
				::htons(paddrin->sin6_addr.u.Word[0]), ::htons(paddrin->sin6_addr.u.Word[1]),
				::htons(paddrin->sin6_addr.u.Word[2]), ::htons(paddrin->sin6_addr.u.Word[3]),
				::htons(paddrin->sin6_addr.u.Word[4]), ::htons(paddrin->sin6_addr.u.Word[5]),
				::htons(paddrin->sin6_addr.u.Word[6]), ::htons(paddrin->sin6_addr.u.Word[7]));
		}
	}	
		
	RequestInfo requestinfo;				
	requestinfo.pszAcceptEncoding = pszAcceptEncoding;
	requestinfo.pszHostname = EnsureNotNull(pHttpRequest->GetHeader(HttpHeaderHost));
	requestinfo.pszQueryString = sQueryString;
	requestinfo.pszRemoteAddress = sRemoteAddress;
	requestinfo.pszURI = sURI;
	requestinfo.pszUserAgent = pszUserAgent;

	if (m_Config.GetDebugEnabled() || m_Config.GetLoggingLevel() >= IISXPRESS_LOGGINGLEVEL_BASIC)	
	{				
		char* pszMethod = "Unknown";		
		switch (pRawRequest->Verb)
		{
		case HttpVerbGET: pszMethod = "GET"; break;
		case HttpVerbPOST: pszMethod = "POST"; break;
		}

		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: method='%hs'\n"), pszMethod);		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: hostname='%hs'\n"), requestinfo.pszHostname);		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: user agent='%hs'\n"), requestinfo.pszUserAgent);		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: URI='%hs'\n"), requestinfo.pszURI);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: accept-encoding='%hs'\n"), requestinfo.pszAcceptEncoding);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: client address='%hs'\n"), requestinfo.pszRemoteAddress);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: query string='%hs'\n"), requestinfo.pszQueryString);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: content-type='%hs'\n"), pszContentType);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: content-length=%u\n"), dwContentLength);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: response code=%u\n"), pRawResponse->StatusCode);	
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("REQUEST: last-modified='%hs'\n"), pszLastModified != NULL ? pszLastModified : "null");	
	}			

	// ***********************************************************************	
	
	ResponseInfo responseinfo;		
	responseinfo.dwContentLength = dwContentLength;
	responseinfo.dwResponseCode = pRawResponse->StatusCode;	
	responseinfo.pszContentType = pszContentType;
	responseinfo.pszLastModified = pszLastModified;

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
		if (GetHTTPRequestObject(pHttpContext, &pIISxpressHTTPRequest) == false || pIISxpressHTTPRequest == NULL)	
		{
			AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("unable to connect to server\n"));					
			PerfCountersAddRejectedResponse(IISxpressNativePerf::NoCompressionServer);
			return RQ_NOTIFICATION_CONTINUE;
		}		

		// store the resulting pointer in the GIT
		m_pGlobalIISxpressHTTPRequest = pIISxpressHTTPRequest;
	}			

	const DWORD dwTimerLoggingLevel = IISXPRESS_LOGGINGLEVEL_FULL;
	bool useTimer = (m_Config.GetDebugEnabled() || m_Config.GetLoggingLevel() >= dwTimerLoggingLevel);

	// start timer
	__int64 nStartTimer = useTimer ? g_Timer.GetMicroSecTimerCount() : 0;

	CAtlStringA sCacheKey;
	if (m_nCacheEnabled != 0)
	{	
		// update the cache perf counters based on the cache cookie
		PerfCountersUpdateCacheStatus(false);	

		if (pszLastModified != NULL && sQueryString.GetLength() == 0)
		{
			ATLASSERT(pszAcceptEncoding != NULL);

			// create the unique key for the response
			ResponseCache::CreateResponseCacheKey(
				sInstanceId, sInstanceId.GetLength(),
				sURI, sURI.GetLength(),
				responseinfo.pszContentType, nContentTypeLength,
				dwContentLength, 
				pszLastModified, nLastModifiedLength,
				pszAcceptEncoding, nAcceptEncodingLength,
				sCacheKey);

			// try to get a compressed response from the cache
			HCACHEITEM hCacheItem = NULL;
			HRESULT hr = m_ResponseCache.LookupEntry(sCacheKey, &hCacheItem);
			if (hr == S_OK)
			{
				// get the item from the cache
				ResponseCacheItem* pCacheItem;
				DWORD dwCacheItemSize = 0;
				hr = m_ResponseCache.GetData(hCacheItem, (void**) &pCacheItem, &dwCacheItemSize);	

				void* pBuffer = pHttpContext->AllocateRequestMemory(pCacheItem->dwContentLength);
				if (pBuffer != NULL)
				{
					pHttpResponse->SetHeader(HttpHeaderContentLength, pCacheItem->sContentLength, 
						(USHORT) pCacheItem->sContentLength.GetLength(), TRUE);

					pHttpResponse->SetHeader(HttpHeaderContentEncoding, pCacheItem->sContentEncoding, 
						(USHORT) pCacheItem->sContentEncoding.GetLength(), TRUE);

					pEntityChunk->DataChunkType = HttpDataChunkFromMemory;
					pEntityChunk->FromMemory.pBuffer = pBuffer;
					pEntityChunk->FromMemory.BufferLength = pCacheItem->dwContentLength;
					memcpy(pEntityChunk->FromMemory.pBuffer, pCacheItem->pbyData, pCacheItem->dwContentLength);

					hr = m_ResponseCache.ReleaseEntry(hCacheItem);

					pIISxpressHTTPRequest->NotifyCacheHit(&iisinfo, &requestinfo, &responseinfo, pCacheItem->dwContentLength);

					PerfCountersAddCachedResponse(dwContentLength, pCacheItem->dwContentLength);

					// calculate timer duration
					__int64 nEndTimer = useTimer ? g_Timer.GetMicroSecTimerCount() : 0;
					__int64 nInterval = nEndTimer - nStartTimer;

					AppendLogMessage(dwTimerLoggingLevel, pszMethodName, pHttpContext, _T("response resolved from cache, call took %I64d us\n"), nInterval);

					return RQ_NOTIFICATION_CONTINUE;
				}
				
				// we failed to handle the cached response, so just proceed as normal (we need to free the
				// cache entry tho)
				hr = m_ResponseCache.ReleaseEntry(hCacheItem);
			}		
		}
	}

	DWORD dwFilterContext = 0;
	HRESULT hr = pIISxpressHTTPRequest->OnSendResponse(&iisinfo, &requestinfo, &responseinfo, &dwFilterContext);

	// calculate timer duration
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

	AppendLogMessage(dwTimerLoggingLevel, pszMethodName, pHttpContext, _T("OnSendResponse() returns 0x%08x, call took %I64d us\n"), hr, nInterval);	

	if (FAILED(hr) == TRUE)
	{			
		// TODO: need to handle generic HRs
		PerfCountersAddRejectedResponse(hr);

		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("won't handle response (0x%08x)\n"), hr);					

		return RQ_NOTIFICATION_CONTINUE;
	}

	AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("will handle response (0x%08x)\n"), hr);		

	if (pEntityChunk->DataChunkType == HttpDataChunkFromFileHandle)
	{
		DWORD dwAllocatedBlockSize = 0;
		BYTE* pbyData = AllocateMemoryBlockForOverlappedIO(pHttpContext, dwContentLength, dwAllocatedBlockSize);
		if (pbyData != NULL)
		{		
			// we are about to perform overlapped IO, so setup the OVERLAPPED struct into a default state
			OVERLAPPED overlapped;
			memset(&overlapped, 0, sizeof(overlapped));

			HandleObject FileEvent;
			bool bHandleFromPool = m_hEventHandlePool.GetHandleObject(FileEvent);									

			// if we failed to get a handle from the pool then we need to make one
			if (bHandleFromPool == false)
			{
				HANDLE hReadOK = ::CreateEvent(NULL, TRUE, FALSE, NULL);
				FileEvent = HandleObject(hReadOK);
			}

			// use the event handle so we can track the IO completion
			overlapped.hEvent = FileEvent;

			// ask for a file read (asynchronous)		
			DWORD dwStatus = ::ReadFile(pEntityChunk->FromFileHandle.FileHandle, pbyData, dwAllocatedBlockSize, NULL, &overlapped);

			// wait for the IO to complete (NB. don't really use this since INFINITE can block forever)			
			::WaitForSingleObject(FileEvent, INFINITE);

			// we must return the handle (if it came from the pool)
			if (bHandleFromPool == true)
			{
				m_hEventHandlePool.ReturnHandleObject(FileEvent);
			}

			pEntityChunk->DataChunkType = HttpDataChunkFromMemory;
			pEntityChunk->FromMemory.BufferLength = dwContentLength;
			pEntityChunk->FromMemory.pBuffer = pbyData;
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}
	else if (pEntityChunk->DataChunkType == HttpDataChunkFromMemory)
	{
		// allocate the memory block
		BYTE* pbyData = (BYTE*) pHttpContext->AllocateRequestMemory(dwContentLength);			
		if (pbyData != NULL)
		{
			// copy the response data
			memcpy(pbyData, pEntityChunk->FromMemory.pBuffer, pEntityChunk->FromMemory.BufferLength);

			// change the buffer pointer
			pEntityChunk->FromMemory.pBuffer = pbyData;
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}		

	// allocate the response stream
	CComPtr<IStream> pStream;
	CComObject<CResponseStream>* pResponseStream = NULL;	
	if (hr == S_OK)
	{
		hr = CComObject<CResponseStream>::CreateInstance(&pResponseStream);
		if (hr == S_OK)
		{		
			pStream = pResponseStream;
		}
	}

	// catch any memory issues
	if (FAILED(hr) == TRUE)
	{			
		// abort the compression
		pIISxpressHTTPRequest->AbortRequest(dwFilterContext);

		// TODO: need to handle generic HRs
		PerfCountersAddRejectedResponse(hr);

		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("won't handle response (0x%08x)\n"), hr);					

		return RQ_NOTIFICATION_CONTINUE;
	}

	pResponseStream->AttachBuffer(pEntityChunk->FromMemory.pBuffer, dwContentLength, 0);

	char szContentEncoding[32] = "";
	hr = pIISxpressHTTPRequest->OnSendRawData(dwFilterContext, pStream, dwContentLength, FALSE, (signed char*) szContentEncoding, _countof(szContentEncoding));
	if (hr == S_OK)
	{		
		DWORD dwOriginalSize = dwContentLength;
		DWORD dwCompressedSize = pResponseStream->GetOffset();				

		// set the new content length into the header
		CAtlStringA sContentLength;
		sContentLength.Format("%u", dwCompressedSize);
		pHttpResponse->SetHeader(HttpHeaderContentLength, sContentLength, (USHORT) sContentLength.GetLength(), TRUE);

		// set the new size into the buffer
		pRawResponse->pEntityChunks->FromMemory.BufferLength = dwCompressedSize;
		
		pHttpResponse->SetHeader(HttpHeaderContentEncoding, szContentEncoding, (USHORT) strlen(szContentEncoding), TRUE);

		// add the item to the cache if we have a key (it means the data is cachable)
		if (dwCompressedSize < dwOriginalSize &&
			m_nCacheEnabled != 0 && sCacheKey.GetLength() > 0)
		{
			ResponseCacheItem* pCacheItem = new ResponseCacheItem();
			if (pCacheItem != NULL)
			{
				pCacheItem->sContentEncoding = szContentEncoding;
				pCacheItem->sContentLength = sContentLength;
				pCacheItem->dwContentLength = dwCompressedSize;
				pCacheItem->pbyData = new BYTE[dwCompressedSize];
				if (pCacheItem->pbyData != NULL)
				{
					// copy the compressed data and add it to the cache
					memcpy(pCacheItem->pbyData, pEntityChunk->FromMemory.pBuffer, dwCompressedSize);
					m_ResponseCache.Add(sCacheKey, pCacheItem, pCacheItem->dwContentLength, NULL, NULL, NULL, m_ResponseCacheItemDeallocator);

					PerfCountersUpdateCacheStatus(true);
				}
			}
		}

		PerfCountersAddCompressedResponse(dwContentLength, dwCompressedSize);

		// make sure the context is released
		hr = pIISxpressHTTPRequest->OnEndOfRequest(dwFilterContext, NULL, 0, FALSE, NULL, 0);		
	}
	else
	{
		// abort the compression
		pIISxpressHTTPRequest->AbortRequest(dwFilterContext);

		// TODO: need to handle generic HRs
		PerfCountersAddRejectedResponse(hr);

		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_BASIC, pszMethodName, pHttpContext, _T("won't handle response (0x%08x)\n"), hr);							
	}

	return RQ_NOTIFICATION_CONTINUE;
}

bool CIISxpressHttpModule::GetHTTPRequestObject(IHttpContext* pHttpContext, IIISxpressHTTPRequest** ppIISxpressHTTPRequest)
{
	if (ppIISxpressHTTPRequest == NULL)
		return false;

	const TCHAR* const pszMethodName = __FUNCTIONT__;

	CComPtr<IUnknown> pUnk;
	HRESULT hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, &pUnk);
	if (hr != S_OK)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("GetActiveObject() returns hr=0x%08x\n"), hr);		
		return false;
	}
	
	CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;
	hr = pUnk->QueryInterface(IID_IIISxpressHTTPRequest, (void**) &pIISxpressHTTPRequest);		
	if (hr != S_OK || pIISxpressHTTPRequest == NULL)
	{
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_ENH, pszMethodName, pHttpContext, _T("QueryInterface() returns hr=0x%08x\n"), hr);				
		return false;
	}
	
	*ppIISxpressHTTPRequest = pIISxpressHTTPRequest.Detach();
	return true;
}


BYTE* CIISxpressHttpModule::AllocateMemoryBlockForOverlappedIO(IHttpContext* pHttpContext, DWORD dwSize, DWORD& dwAllocatedBlockSize)
{		
		// calculate the size of the file buffer we need for the overlapped xfer (must be a multiple of sector size)
		DWORD dwFileBlockSize = dwSize;
		dwFileBlockSize += m_dwBytesPerSector;
		dwFileBlockSize &= ~(m_dwBytesPerSector - 1);

		// allocate the memory block allowing for enough space for a move to a sector size boundary
		BYTE* pbyData = (BYTE*) pHttpContext->AllocateRequestMemory(dwFileBlockSize + m_dwBytesPerSector);
		if (pbyData == NULL)
			return NULL;

		// calculate how many bytes we are away from the sector boundary 
		// pragma out the warning if required since the rounding isn't a problem here
		DWORD dwDataBlockOffset = reinterpret_cast<DWORD>(pbyData) % m_dwBytesPerSector;

		// move the pointer to a sector boundary
		pbyData += m_dwBytesPerSector;
		pbyData -= dwDataBlockOffset;				

		dwAllocatedBlockSize = dwFileBlockSize;
		return pbyData;
}

void CIISxpressHttpModule::GetRegistrySettings(IHttpContext* pHttpContext)
{	
	const TCHAR* const pszMethodName = __FUNCTIONT__;

	// get the current load cookie
	DWORD dwLoadCookie = m_Config.GetLoadCookie();

	// load the registry settings if the cookie has changed
	if (m_dwLastConfigCookie != dwLoadCookie)
	{		
		AppendLogMessage(IISXPRESS_LOGGINGLEVEL_FULL, pszMethodName, pHttpContext, _T("refreshing registry settings\n"));

		// get the new logging level
		DWORD dwNewLoggingLevel = m_Config.GetLoggingLevel();

		// if the logging level has changed then we need to handle it
		if (m_dwOldLoggingLevel != dwNewLoggingLevel)
		{
			// if the old logging level was set to none then we have to open the log file
			// since the user must have asked for logging
			if (m_dwOldLoggingLevel == IISXPRESS_LOGGINGLEVEL_NONE)
			{
				OpenLog(pHttpContext);					
				m_Log.AppendFromResource(g_hModule, IDS_LOGGINGSTARTED, true);				
				m_Log.DumpSysInfo(_T("IISxpressNativeModule:"));
			}
			// if the user has set the logging level to none then they want logging switched off,
			// so close the log file
			else if (dwNewLoggingLevel == IISXPRESS_LOGGINGLEVEL_NONE)
			{
				m_Log.AppendFromResource(g_hModule, IDS_LOGGINGSTOPPED, true);
				m_Log.Close();
			}

			// set the old logging level
			::InterlockedExchange((LONG*) &m_dwOldLoggingLevel, dwNewLoggingLevel);
		}	

		LONG nOldCacheEnabled = ::InterlockedExchange(&m_nCacheEnabled, m_Config.GetCacheEnabled());

		// if the cache has been newly switched off then flush it as well
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

void CIISxpressHttpModule::Dispose() 
{
	Release();
}

ULONG CIISxpressHttpModule::AddRef()
{
	return (ULONG) ::InterlockedIncrement(&m_nRefCount);
}

ULONG CIISxpressHttpModule::Release()
{
	LONG nRefCount = ::InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0)
		delete this;

	return (ULONG) nRefCount;
}

bool CIISxpressHttpModule::IsUserLocalhost(const HTTP_REQUEST* pRawRequest)
{
	if (pRawRequest == NULL)
		return false;

	if (pRawRequest->Address.pRemoteAddress->sa_family == AF_INET)
	{
		struct sockaddr_in* paddrin = (struct sockaddr_in*) pRawRequest->Address.pRemoteAddress;

		// if the address is 127.0.0.1 then it's localhost
		if (paddrin->sin_addr.S_un.S_un_b.s_b1 == 127 &&
			paddrin->sin_addr.S_un.S_un_b.s_b2 == 0 &&
			paddrin->sin_addr.S_un.S_un_b.s_b3 == 0 &&
			paddrin->sin_addr.S_un.S_un_b.s_b4 == 1)
		{
			return true;
		}
	}
	else if (pRawRequest->Address.pRemoteAddress->sa_family == AF_INET6)
	{
		SOCKADDR_IN6* paddrin = (SOCKADDR_IN6*) pRawRequest->Address.pRemoteAddress;

		// if the address is ::1 then it's localhost
		if (paddrin->sin6_addr.u.Word[0] == 0 &&
			paddrin->sin6_addr.u.Word[1] == 0 &&
			paddrin->sin6_addr.u.Word[2] == 0 &&
			paddrin->sin6_addr.u.Word[3] == 0 &&
			paddrin->sin6_addr.u.Word[4] == 0 &&
			paddrin->sin6_addr.u.Word[5] == 0 &&
			paddrin->sin6_addr.u.Word[6] == 0 &&
			paddrin->sin6_addr.u.Word[7] == 0x0100)
		{
			return true;
		}
	}

	return false;
}

bool CIISxpressHttpModule::MakeResponseSingleEntityBlock(IHttpContext* pHttpContext, IHttpResponse* pHttpResponse, const HTTP_RESPONSE* pRawResponse)
{
	if (pHttpContext == NULL || pHttpResponse == NULL || pRawResponse == NULL)
		return false;

	// we need to calculate the final buffer size
	DWORD dwBufferSize = 0;
	for (USHORT i = 0; i < pRawResponse->EntityChunkCount; i++)
	{
		// if the buffer isn't in memory then we can't deal with it
		if (pRawResponse->pEntityChunks[i].DataChunkType != HttpDataChunkFromMemory)
			return false;

		dwBufferSize += pRawResponse->pEntityChunks[i].FromMemory.BufferLength;
	}

	// allocate the unified buffer
	BYTE* pbyBuffer = (BYTE*) pHttpContext->AllocateRequestMemory(dwBufferSize);
	if (pbyBuffer == NULL)
		return false;

	// copy the data (use an offset to preserve the pointer)
	DWORD dwBufferOffset = 0;
	for (USHORT i = 0; i < pRawResponse->EntityChunkCount; i++)
	{
		memcpy(pbyBuffer + dwBufferOffset, 
			pRawResponse->pEntityChunks[i].FromMemory.pBuffer,
			pRawResponse->pEntityChunks[i].FromMemory.BufferLength);

		dwBufferOffset += pRawResponse->pEntityChunks[i].FromMemory.BufferLength;
	}	

	// take a copy of the original headers
	HTTP_RESPONSE_HEADERS headers = pRawResponse->Headers;

	// clear the headers and the response entities
	pHttpResponse->Clear();	

	// copy the original known headers back in - excluding the content length since
	// IIS will fill this back in correctly for us (it will be wrong if we leave it)
	for (int i = 0; i < HttpHeaderResponseMaximum; i++)
	{
		if (headers.KnownHeaders[i].pRawValue != NULL && 
			headers.KnownHeaders[i].RawValueLength > 0 &&
			i != HttpHeaderContentLength)
		{
			pHttpResponse->SetHeader((HTTP_HEADER_ID) i, headers.KnownHeaders[i].pRawValue, headers.KnownHeaders[i].RawValueLength, TRUE);
		}
	}		

	// copy the original unknown headers back in 
	for (USHORT i = 0; i < headers.UnknownHeaderCount; i++)
	{
		pHttpResponse->SetHeader(headers.pUnknownHeaders[i].pName, 
			headers.pUnknownHeaders[i].pRawValue,
			headers.pUnknownHeaders[i].RawValueLength,
			TRUE);
	}

	// make the new entity
	HTTP_DATA_CHUNK chunk;
	chunk.DataChunkType = HttpDataChunkFromMemory;
	chunk.FromMemory.pBuffer = pbyBuffer;
	chunk.FromMemory.BufferLength = dwBufferSize;

	// put the new entity into the response
	DWORD dwSent = 0;
	return pHttpResponse->WriteEntityChunks(&chunk, 1, FALSE, TRUE, &dwSent) == S_OK;
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::OpenLog(IHttpContext* pHttpContext)
{
	if (pHttpContext != NULL && m_Config.GetLoggingLevel() >= IISXPRESS_LOGGINGLEVEL_BASIC)
	{
		CAtlString installPath;
		m_Config.GetInstallPath(installPath);

		if (installPath.GetLength() > 0)
		{		
			CPathT<CAtlString> LogPath(installPath);			
			LogPath.Append(_T("\\Logs"));

			if (LogPath.IsDirectory() == FALSE)
			{
				::CreateDirectory(LogPath, NULL);	
			}

			CAtlString sLogFilePrefix;
			sLogFilePrefix.LoadString(IDS_LOGFILENAMEPREFIX);

			CAtlString sAppName;
			if (GetAppPoolName(pHttpContext, sAppName) == true)
			{
				sLogFilePrefix += sAppName;
				sLogFilePrefix += "-";
			}		

			m_Log.Init(LogPath, sLogFilePrefix, _T("log"));		
		}
	}
}

bool CIISxpressHttpModule::GetAppPoolName(IHttpContext* pHttpContext, CAtlString& name)
{
	bool status = false;

	if (m_pHttpServer != NULL)
	{
		LPCWSTR pszAppPoolName = m_pHttpServer->GetAppPoolName();
		if (pszAppPoolName != NULL)
		{
			name = CAtlString(pszAppPoolName);
			status = true;
		}
	}

	// if we didn't get the application pool name from the server then at least try and fall back to the application id
	if (!status && pHttpContext != NULL)
	{
		IHttpApplication* pApp = pHttpContext->GetApplication();
		if (pApp != NULL)
		{
			PCWSTR pszAppId = pApp->GetApplicationId();
			if (pszAppId != NULL)
			{
				name = CAtlString(pszAppId);
				name.Replace('/', '_');
				status = true;
			}
		}
	}
	
	return status;
}

void CIISxpressHttpModule::AppendLogMessage(DWORD dwMinLogLevel, LPCTSTR pszMethodName, IHttpContext* pHttpContext, LPCTSTR pszMessageFormat, ...)
{
	if (pszMethodName == NULL || pszMessageFormat == NULL)
	{
		return;
	}

	if (m_Config.GetDebugEnabled() == false && m_Config.GetLoggingLevel() < dwMinLogLevel)
	{
		return;
	}

	// format the message prefix
	CString sMsgPrefix;
	sMsgPrefix.Format(_T("%s(0x%08x): "), pszMethodName, pHttpContext);

	va_list args;
	va_start(args, pszMessageFormat);

	TCHAR szMsgBody[8192];
	_vstprintf_s(szMsgBody, _countof(szMsgBody), pszMessageFormat, args);

	va_end(args);

	// create the full message
	CString sMsg = sMsgPrefix + szMsgBody;

	// output the message to the debug window
	if (m_Config.GetDebugEnabled())
	{
		::OutputDebugString(sMsg);
	}

	// output the message to the file log
	if (m_Config.GetLoggingLevel() >= dwMinLogLevel)
	{
		m_Log.Append(true, sMsg);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::PerfCountersAddRejectedResponse(HRESULT hr)
{
	PerfCountersAddRejectedResponse(PerfCountersMapServerHResult(hr));
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::PerfCountersAddRejectedResponse(IISxpressNativePerf::RejectedResponseReasons reason)
{
	if (m_pInstancePerfCounters.get() != NULL)
	{
		m_pInstancePerfCounters->IncrementUnhandledCount(reason);
	}	

	if (m_pGlobalPerfCounters.get() != NULL)
	{
		m_pGlobalPerfCounters->IncrementUnhandledCount(reason);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

IISxpressNativePerf::RejectedResponseReasons CIISxpressHttpModule::PerfCountersMapServerHResult(HRESULT hr)
{
	switch (hr)
	{
	case IISXPRESSHTTPREQUEST_SERVERDISABLED:		return IISxpressNativePerf::CompressionServerDisabled;
	case IISXPRESSHTTPREQUEST_NOT200:				return IISxpressNativePerf::ResponseCode;
	case IISXPRESSHTTPREQUEST_INCOMPATIBLECLIENT:	return IISxpressNativePerf::IncompatibleClient;
	case IISXPRESSHTTPREQUEST_MATCHEDNEVERRULE:		return IISxpressNativePerf::NeverRuleMatch;
	case IISXPRESSHTTPREQUEST_STATICRULEMATCH:		return IISxpressNativePerf::StaticRuleMatch;
	case IISXPRESSHTTPREQUEST_CPUBUSY:				return IISxpressNativePerf::CPUBusy;
	case IISXPRESSHTTPREQUEST_LEASECONTEXTFAILED:	return IISxpressNativePerf::Internal;
	case IISXPRESSHTTPREQUEST_MEMORYLOW:			return IISxpressNativePerf::MemoryLow;
	case IISXPRESSHTTPREQUEST_ACTIVECONTEXTFAILED:	return IISxpressNativePerf::Internal;
	case IISXPRESSHTTPREQUEST_BADINCOMINGSTREAM:	return IISxpressNativePerf::Internal;
	case IISXPRESSHTTPREQUEST_USERRULEOVERLOAD:		return IISxpressNativePerf::UserRuleExcluded;
	default:										return IISxpressNativePerf::Default;
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::PerfCountersAddCachedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize)
{
	if (m_pInstancePerfCounters.get() != NULL)
	{
		m_pInstancePerfCounters->IncrementCompressedResponses();
		m_pInstancePerfCounters->IncrementCacheHits();
		m_pInstancePerfCounters->IncrementCompressedBytes(dwCompressedSize);
		m_pInstancePerfCounters->IncrementRawBytes(dwOriginalSize);
	}

	if (m_pGlobalPerfCounters.get() != NULL)
	{
		m_pGlobalPerfCounters->IncrementCompressedResponses();
		m_pGlobalPerfCounters->IncrementCacheHits();
		m_pGlobalPerfCounters->IncrementCompressedBytes(dwCompressedSize);
		m_pGlobalPerfCounters->IncrementRawBytes(dwOriginalSize);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::PerfCountersAddCompressedResponse(DWORD dwOriginalSize, DWORD dwCompressedSize)
{
	if (m_pInstancePerfCounters.get() != NULL)
	{
		m_pInstancePerfCounters->IncrementCompressedResponses();
		m_pInstancePerfCounters->IncrementCompressedBytes(dwCompressedSize);
		m_pInstancePerfCounters->IncrementRawBytes(dwOriginalSize);
	}

	if (m_pGlobalPerfCounters.get() != NULL)
	{
		m_pGlobalPerfCounters->IncrementCompressedResponses();
		m_pGlobalPerfCounters->IncrementCompressedBytes(dwCompressedSize);
		m_pGlobalPerfCounters->IncrementRawBytes(dwOriginalSize);
	}
}

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************

void CIISxpressHttpModule::PerfCountersUpdateCacheStatus(bool forceUpdate)
{
	static DWORD dwLastCookie = 0;

	if (m_pInstancePerfCounters.get() != NULL && (forceUpdate == true || dwLastCookie != CResponseCacheItemFree::m_dwCacheCookie))	
	{
		DWORD dwCacheSize = 0;
		m_ResponseCache.GetCurrentAllocSize(&dwCacheSize);

		DWORD dwCacheItems = 0;
		m_ResponseCache.GetCurrentEntryCount(&dwCacheItems);
		
		m_pInstancePerfCounters->SetCacheSizeAndItems(dwCacheSize, dwCacheItems);
	}

	dwLastCookie = CResponseCacheItemFree::m_dwCacheCookie;
}