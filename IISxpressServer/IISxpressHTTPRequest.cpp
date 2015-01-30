// IISxpressHTTPRequest.cpp : Implementation of CIISxpressHTTPRequest

#include "stdafx.h"
#include "IISxpressHTTPRequest.h"

#include "IISxpressHTTPRequestHResults.h"

#include "HiResTimer.h"

#include "..\IISxpress\ResponseStreamMagicNumbers.h"

#include "SetThreadAffinity.h"

#ifdef HTTPREQUEST_METHODMETRICS
#include "SimpleBlockTimer.h"
#endif

#include <httpfilt.h>
#include <atlpath.h>

HiResTimer g_Timer;

//***********************************************************************************************

struct HousekeepResponseData
{
	LPSTR		pszURIMapPath;
	LPSTR		pszContentType;
	int			nCompressedStatus;
	DWORD		dwCompressedSize;
	DWORD		dwOriginalSize;

	static HousekeepResponseData* Alloc(
					LPCSTR _pszURIMapPath, 
					int nURIMapPathSize,
					LPCSTR _pszContentType,
					int nContentTypeSize,
					int _nCompressedStatus,
					DWORD _dwCompressedSize,
					DWORD _dwOriginalSize)
	{		
		DWORD dwDataSize = 
			sizeof(HousekeepResponseData) +
			nURIMapPathSize + sizeof(_pszURIMapPath[0]) +
			nContentTypeSize + sizeof(_pszContentType[0]);

		HousekeepResponseData* pData = (HousekeepResponseData*) ::SysAllocStringByteLen(NULL, dwDataSize);

		if (pData != NULL)
		{
			pData->pszURIMapPath = (LPSTR) (pData + 1);
			strcpy_s(pData->pszURIMapPath, nURIMapPathSize + 1, _pszURIMapPath);

			pData->pszContentType = pData->pszURIMapPath + nURIMapPathSize + sizeof(_pszURIMapPath[0]);
			strcpy_s(pData->pszContentType, nContentTypeSize + 1, _pszContentType);

			pData->nCompressedStatus = _nCompressedStatus;
			pData->dwCompressedSize = _dwCompressedSize;
			pData->dwOriginalSize = _dwOriginalSize;
		}

		return pData;
	}	

	void Free(void)
	{
		::SysFreeString((BSTR) this);
	}

};

//***********************************************************************************************

// CIISxpressHTTPRequest

HRESULT CIISxpressHTTPRequest::FinalConstruct()
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);

		// populate the stack with the available request data structures
		for (int i = 0; i < sizeof(m_RequestContexts) / sizeof(m_RequestContexts[0]); i++)
		{
			m_RequestContextStack.push(&m_RequestContexts[i]);
		}

		m_nRequestStackLastUsedTickCount = g_Timer.GetTimerCount();
		m_nRequestStackLastResetTickCount = g_Timer.GetTimerCount();
	}

	// get info on the system (we are mainly interested in the number of CPU cores)
	SYSTEM_INFO SysInfo;
	::GetSystemInfo(&SysInfo);
	
	// allow for core switching
	m_dwNumberOfCores = SysInfo.dwNumberOfProcessors;	
	m_dwActiveCore = 0;

	// we need something to allow us to iterate thru the available CPU cores
	m_dwCompressorOffset = 0;
	
	// allocate as many compressors as there are CPU cores
	m_Compressors = AutoArray<CompressorData>(SysInfo.dwNumberOfProcessors);	

	for (int i = 0; i < m_Compressors.Size(); i++)	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_Compressors[i].m_csCompressor);

		m_Compressors[i].m_CompressedBuffer.Allocate(DEFAULT_COMPRESSION_BUFFER);
		m_Compressors[i].m_nCompressorLastUsedTickCount = g_Timer.GetTimerCount();
	}

	// initialize the notification helper	
	m_NotifyHelper.Initialize(this);

	m_hTermThreads = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hTermThreads != (HANDLE) ERROR_INVALID_HANDLE)
	{
		unsigned nThreadId = 0;
		::_beginthreadex(NULL, 0, HousekeepingProc, this, 0, &nThreadId);
	}

	// startup the perf counter on a different thread because it seems to require a message
	// pump (it will block if it doesn't have one)
	// the idea is that when FinalConstruct returns the service message pump will handle
	// the messages the PDH calls seem to require, the thread will then exit straight away
	unsigned nPerfCounterStartupThreadId = 0;
	::_beginthreadex(NULL, 0, StartupPerfCounterProc, this, 0, &nPerfCounterStartupThreadId);

	// startup the response housekeeping thread
	m_nHousekeepResponseDetailsThreadId = 0;
	::_beginthreadex(NULL, 0, HousekeepResponseDetailsProc, this, 0, &m_nHousekeepResponseDetailsThreadId);		

	// create a cookie to pass back to clients
	m_dwIsAliveCookie = ::GetTickCount();	

	return S_OK;
}

void CIISxpressHTTPRequest::FinalRelease() 
{
	if (m_hTermThreads != (HANDLE) ERROR_INVALID_HANDLE)
	{
		::SetEvent(m_hTermThreads);
	}

	for (int i = 0; i < m_Compressors.Size(); i++)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_Compressors[i].m_csCompressor);

		m_Compressors[i].m_CompressedBuffer.Free();
	}	

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csPerfCounter);

		// we don't need the CPU performance counter any more
		if (!!m_hPerfCounter)
			m_hPerfCounter.Close();

		// close the performance counter query
		if (!!m_hPerfQuery)
			m_hPerfQuery.Close();
	}	
}

STDMETHODIMP CIISxpressHTTPRequest::Init(
	IStorage* pStorage, 
	ICompressionRuleManager* pCompressionRuleManager,
	IProductLicenseManager* pProductLicenseManager,
	IComIISxpressRegSettings* pRegSettings,
	IIISxpressSettings* pSettings)
{
	if (pStorage == NULL || pCompressionRuleManager == NULL || pProductLicenseManager == NULL)
		return E_POINTER;

	m_pCompressionRuleManager = pCompressionRuleManager;
	m_pNeverCompressRules = m_pCompressionRuleManager;
	m_pStaticFileRules = m_pCompressionRuleManager;
	m_pCompressionHistory = m_pCompressionRuleManager;	
	m_pCompressionHistory2 = m_pCompressionRuleManager;	

	m_pProductLicenseManager = pProductLicenseManager;

	m_pRegSettings = pRegSettings;

	m_pSettings = pSettings;

	// *************************************************************************	
	CompressionMode compressionMode = CompressionModeDefault;
	pSettings->get_CompressionMode(&compressionMode);

	CompressorMode compressorMode = MapCompressionMode(compressionMode);
	if (InitCompressorStreams(compressorMode) != true)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CIISxpressHTTPRequest::GetFilterVersion(IStream* pFilterVersion)
{	
	MyOutputDebugString(m_pRegSettings, 
		_T("CIISxpressHTTPRequest::GetFilterVersion()\n"));

	if (pFilterVersion == NULL)
		return E_POINTER;

	STATSTG Stat;
	pFilterVersion->Stat(&Stat, STATFLAG_NONAME);

	if (Stat.cbSize.LowPart != sizeof(HTTP_FILTER_VERSION))
		return E_INVALIDARG;

	LARGE_INTEGER Offset = { 0, 0 };
	pFilterVersion->Seek(Offset, STREAM_SEEK_SET, NULL);

	HTTP_FILTER_VERSION Ver;
	ULONG nRead = 0;
	pFilterVersion->Read(&Ver, sizeof(Ver), &nRead);

	return S_OK;
}

// returns:
//	S_OK		- succeeded
//  see IISxpressHTTPRequestHResults.h for details of error codes
STDMETHODIMP CIISxpressHTTPRequest::OnSendResponse(
												IISInfo* pIISInfo,
												RequestInfo* pRequestInfo,
												ResponseInfo* pResponseInfo,
												DWORD* pdwFilterContext)
{
#ifdef HTTPREQUEST_METHODMETRICS
	CAutoBlockTimer timer(_T("* CIISxpressHTTPRequest::OnSendResponse()"));
#endif		

	// get the affinity mode from the registry
	DWORD dwProcessorAffinityMode = IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO;
	m_pSettings->get_ProcessorAffinity(&dwProcessorAffinityMode);

	// get the processor affinity mask and construct an object to manage it
	DWORD dwProcessorAffinityMask = GetProcessorAffinityMask(dwProcessorAffinityMode);
	CSetThreadAffinity affinity(dwProcessorAffinityMask);

	// get the debug state
	BOOL bDebugEnabled = FALSE;
	m_pRegSettings->get_DebugEnabled(&bDebugEnabled);

	// get the logging level
	DWORD dwLoggingLevel = IISXPRESS_LOGGINGLEVEL_NONE;
	m_pRegSettings->get_LoggingLevel(&dwLoggingLevel);	

	if (bDebugEnabled == TRUE || dwLoggingLevel == IISXPRESS_LOGGINGLEVEL_FULL)
	{
		CString sMsg;
		sMsg.Format(
			_T("CIISxpressHTTPRequest::OnSendResponse()\n")
			_T(" ServerName:          '%hs'\n")
			_T(" ServerPort:          '%hs'\n")
			_T(" InstanceId:          '%hs'\n")			
			_T(" URIMapPath:          '%hs'\n")
			_T(" UserAgent:           '%hs'\n")
			_T(" Hostname:            '%hs'\n")
			_T(" URI:                 '%hs'\n")
			_T(" Query string:        '%hs'\n")
			_T(" Accept-Encoding:     '%hs'\n")
			_T(" Remote address       '%hs'\n")
			_T(" Content-Type:        '%hs'\n")
			_T(" Content-Length:      %u\n")
			_T(" Response code:       %u\n"),
			pIISInfo->pszServerName != NULL ? pIISInfo->pszServerName : "",
			pIISInfo->pszServerPort != NULL ? pIISInfo->pszServerPort : "",
			pIISInfo->pszInstanceId != NULL ? pIISInfo->pszInstanceId : "",
			pIISInfo->pszURLMapPath != NULL ? pIISInfo->pszURLMapPath : "",
			pRequestInfo->pszUserAgent != NULL ? pRequestInfo->pszUserAgent : "",
			pRequestInfo->pszHostname != NULL ? pRequestInfo->pszHostname : "",
			pRequestInfo->pszURI != NULL ? pRequestInfo->pszURI : "",
			pRequestInfo->pszQueryString != NULL ? pRequestInfo->pszQueryString : "",
			pRequestInfo->pszAcceptEncoding != NULL ? pRequestInfo->pszAcceptEncoding : "",
			pRequestInfo->pszRemoteAddress != NULL ? pRequestInfo->pszRemoteAddress : "",
			pResponseInfo->pszContentType != NULL ? pResponseInfo->pszContentType : "",
			pResponseInfo->dwContentLength,
			pResponseInfo->dwResponseCode);

		MyOutputDebugString(bDebugEnabled, sMsg);		
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_FULL, sMsg);
	}

	if (pIISInfo == NULL || 
		pIISInfo->pszServerName == NULL ||
		pIISInfo->pszServerPort == NULL ||
		pIISInfo->pszInstanceId == NULL ||
		pIISInfo->pszURLMapPath == NULL ||
		pRequestInfo == NULL ||
		pRequestInfo->pszUserAgent == NULL || 
		pRequestInfo->pszURI == NULL || 
		pRequestInfo->pszAcceptEncoding == NULL || 
		pRequestInfo->pszRemoteAddress == NULL || 
		pResponseInfo == NULL ||
		pResponseInfo->pszContentType == NULL ||
		pdwFilterContext == NULL)
	{
		return E_POINTER;
	}

	// only progress if compression enabled 
	BOOL bEnabled = FALSE;
	m_pRegSettings->get_Enabled(&bEnabled);
	if (bEnabled != TRUE)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning SERVERDISABLED because server is not enabled\n");

		MyOutputDebugString(bDebugEnabled, pszMsg);			
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return IISXPRESSHTTPREQUEST_SERVERDISABLED;
	}	

	// only process 200 responses
	if (pResponseInfo->dwResponseCode != 200)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning NOT200 because response code != 200 (%u)\n");

		MyOutputDebugString(bDebugEnabled, pszMsg, pResponseInfo->dwResponseCode);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg, pResponseInfo->dwResponseCode);

		return IISXPRESSHTTPREQUEST_NOT200;
	}

	// we need a lower case version of the accept encoding string
	DWORD dwTempAcceptEncodingSize = (DWORD) strlen(pRequestInfo->pszAcceptEncoding) + 1;
	char* pszTempAcceptEncoding = (char*) alloca(dwTempAcceptEncodingSize);
	strcpy_s(pszTempAcceptEncoding, dwTempAcceptEncodingSize, pRequestInfo->pszAcceptEncoding);
	_strlwr_s(pszTempAcceptEncoding, dwTempAcceptEncodingSize);

	// make sure the client can accept deflate, gzip or bzip2, if not just return now
	bool deflate = strstr(pszTempAcceptEncoding, "deflate") != NULL;
	bool gzip = strstr(pszTempAcceptEncoding, "gzip") != NULL;
	bool bzip2 = strstr(pszTempAcceptEncoding, "bzip2") != NULL;
	if (!deflate && !gzip && !bzip2)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning INCOMPATIBLECLIENT because client won't accept compressed responses\n");

		MyOutputDebugString(bDebugEnabled, pszMsg);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return IISXPRESSHTTPREQUEST_INCOMPATIBLECLIENT;
	}

	// if the client only supports bzip2 then we need to make sure it is enabled for this response
	if (!deflate && !gzip && bzip2)
	{
		BZIP2Mode bzip2Mode = BZIP2ModeDisabled;
		m_pRegSettings->get_BZIP2Mode(&bzip2Mode);

		if (bzip2Mode == BZIP2ModeDisabled ||
			(bzip2Mode == BZIP2ModeStaticOnly && (pResponseInfo->pszLastModified == NULL || pResponseInfo->pszLastModified[0] == '\0')))
		{
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning INCOMPATIBLECLIENT because client only supports bzip2 which is disabled by the server\n");

			MyOutputDebugString(bDebugEnabled, pszMsg);
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

			return IISXPRESSHTTPREQUEST_INCOMPATIBLECLIENT;
		}
	}

	// make sure we have access to the compression rules
	if (m_pNeverCompressRules == NULL || m_pStaticFileRules == NULL)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning E_FAIL because unable to reference never and static rules\n");

		MyOutputDebugString(bDebugEnabled, pszMsg);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return E_FAIL;
	}	

	BOOL bCheckUserRules = TRUE;
	m_pRegSettings->get_CheckUserRules(&bCheckUserRules);

	BOOL bCheckNeverRules = TRUE;
	m_pRegSettings->get_CheckNeverRules(&bCheckNeverRules);

	if (bCheckUserRules == TRUE)
	{
		// get rule information on the file
		int nUserCompress = -1;
		int nSystemCompress = -1;
		HRESULT hr = m_pStaticFileRules->GetFileRule(pIISInfo->pszURLMapPath, &nUserCompress, &nSystemCompress);
		if (FAILED(hr) == TRUE)
		{
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning STATICRULEERROR because unable to determine static file rule\n");

			MyOutputDebugString(bDebugEnabled, pszMsg);
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

			return IISXPRESSHTTPREQUEST_STATICRULEERROR;
		}	

		// determine what to do with the request based on user and system prefs
		if (hr == S_OK)
		{
			// we got a match, determine what to do next (user decision wins)
			if (nUserCompress == 0)
			{			
				// the user has specified not to compress the file
				TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning USERRULEOVERLOAD because user has specified to not compress\n");
				
				MyOutputDebugString(bDebugEnabled, pszMsg);
				MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

				return IISXPRESSHTTPREQUEST_USERRULEOVERLOAD;		
			}		
			else if (nUserCompress == 1)
			{
				// the user has specified to compress the file, so don't allow the never rules to be applied 
				bCheckNeverRules = FALSE;
			}
			else if (nSystemCompress == 1)
			{
				// the file has not changed and the system has already determinated it should compress the file
			}			
			else if (nSystemCompress == 0 && (pRequestInfo->pszQueryString == NULL || pRequestInfo->pszQueryString[0] == '\0'))
			{
				// attempt to discover the details of the response last time it was encountered
				CTransientResponseHistory::ResponseHistory History;
				bool bGotHistory = m_TransientHistory.GetEntry(pIISInfo->pszURLMapPath, History);

				// if we got the history and the content type and length still match then we REALLY
				// don't want to compress it
				if (bGotHistory == true && 
					History.dwContentLength == pResponseInfo->dwContentLength &&
					History.sContentType == pResponseInfo->pszContentType)
				{
					// the file has not changed and the system has determined that it should not be compressed
					TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning STATICRULEMATCH because system has determined file should not be compressed\n");
					
					MyOutputDebugString(bDebugEnabled, pszMsg);
					MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

					return IISXPRESSHTTPREQUEST_STATICRULEMATCH;
				}			
			}			
		}
		else if (hr == S_FALSE)
		{
			// we didn't get a match, this means we should attempt compression
		}
		else
		{
			// something else happened, so assume it's bad and return IISXPRESSHTTPREQUEST_STATICRULEERROR
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning STATICRULEERROR because an unknown error code was encountered (0x%08x)\n");
			
			MyOutputDebugString(bDebugEnabled, pszMsg, hr);
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg, hr);
			
			return IISXPRESSHTTPREQUEST_STATICRULEERROR;
		}	
	}
	
	if (bCheckNeverRules == TRUE)
	{
		// match the extension, content type and URI against the NEVER rules
		const char* pszExtension = strrchr(pRequestInfo->pszURI, '.');
		if (m_pNeverCompressRules->MatchRule(
			pszExtension, 
			pResponseInfo->pszContentType, 
			pIISInfo->pszInstanceId, pRequestInfo->pszURI, 
			pRequestInfo->pszUserAgent,
			pRequestInfo->pszRemoteAddress) != S_FALSE)
		{
			// we got a match, don't progress
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning MATCHEDNEVERRULE because ")
				_T("extension, content type, folder or IP matched never rule\n");
			
			MyOutputDebugString(bDebugEnabled, pszMsg);
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

			return IISXPRESSHTTPREQUEST_MATCHEDNEVERRULE;
		}	
	}
		
	// *********************************************************************************************
	// START: Performance counter stuff
	
	BOOL bCheckCPULoad = TRUE;
	m_pRegSettings->get_CheckCPULoad(&bCheckCPULoad);
	if (bCheckCPULoad == TRUE)
	{
		// everything is OK here...
		HRESULT hr = S_OK;

		// make the perf counter stuff thread safe
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csPerfCounter);

			// only if PDH.DLL is linked in and we have a perf counter running
			if (!!m_hPerfCounter)
			{
				// wait for up to 10 iterations for CPU to free up
				int nRetry = 0;
				while (nRetry < 10)		
				{				
					::PdhCollectQueryData(m_hPerfQuery);

					PDH_FMT_COUNTERVALUE PdhFormattedValue;
					PdhFormattedValue.longValue = 0;

					::PdhGetFormattedCounterValue(m_hPerfCounter, PDH_FMT_LONG, NULL, &PdhFormattedValue);

					// if the CPU usage is less than 75% then we can proceed
					// (if the call above failed then longValue should be 0, this will mean the compression
					//  will proceed)
					if (PdhFormattedValue.longValue < 75)
					{				
						hr = S_OK;				

						break;
					}				

					nRetry++;

					// give up some CPU time
					::Sleep(0);
				}		
			}	
		}	

		// either the CPU is too busy or something went wrong; return the state to the client
		if (hr != S_OK)
		{		
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning CPUBUSY because IISxpress has determined CPU too busy\n");

			MyOutputDebugString(bDebugEnabled, pszMsg);
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

			return IISXPRESSHTTPREQUEST_CPUBUSY;
		}
	}

	// END: Performance counter stuff
	// *********************************************************************************************	

	BOOL bCheckPhysicalMemory = TRUE;
	m_pRegSettings->get_CheckPhysicalMemory(&bCheckPhysicalMemory);
	if (bCheckPhysicalMemory == TRUE)
	{
		__int64 nPhysicalMemoryAvailable = 0;
		__int64 nPhysicalMemory = 0;
		
		MEMORYSTATUSEX MemoryStatus;
		MemoryStatus.dwLength = sizeof(MemoryStatus);
		if (::GlobalMemoryStatusEx(&MemoryStatus) == TRUE)
		{
			nPhysicalMemoryAvailable = (__int64) MemoryStatus.ullAvailPhys;
			nPhysicalMemory = (__int64) MemoryStatus.ullTotalPhys;
		}			

		// only handle the call if:
		//  at least 5% of physical memory is available, and
		//  we don't need more than 20% of the physical memory available to complete the call (NB. the content length
		//  may be zero but that doesn't really matter here)
		if ((nPhysicalMemory / nPhysicalMemoryAvailable) >= 20 ||
			pResponseInfo->dwContentLength > (nPhysicalMemoryAvailable / 5))
		{			
			TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning MEMORYLOW because there is not enough memory available\n");

			MyOutputDebugString(bDebugEnabled, pszMsg);			
			MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

			return IISXPRESSHTTPREQUEST_MEMORYLOW;
		}				
	}

	DWORD dwContext = (DWORD) ::InterlockedIncrement((LPLONG) &m_dwContext);

	IISxpressRequestContext* pRequestContext = LeaseContextObject(dwContext);
	if (pRequestContext == NULL)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendResponse() returning LEASECONTEXTFAILED because unable to get context object\n");

		MyOutputDebugString(bDebugEnabled, pszMsg);			
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return IISXPRESSHTTPREQUEST_LEASECONTEXTFAILED;
	}	

	// store the stuff we will need to process the request
	pRequestContext->sUserAgent = pRequestInfo->pszUserAgent;
	pRequestContext->sURI = pRequestInfo->pszURI;
	pRequestContext->sAcceptEncoding = pRequestInfo->pszAcceptEncoding;	
	pRequestContext->sContentType = pResponseInfo->pszContentType;
	pRequestContext->sURLMapPath = pIISInfo->pszURLMapPath;
	pRequestContext->sLastModified = pResponseInfo->pszLastModified;
	pRequestContext->dwContentLength = pResponseInfo->dwContentLength;
	pRequestContext->dwResponseCode = pResponseInfo->dwResponseCode;	
	pRequestContext->dwProcessorAffinityMask = dwProcessorAffinityMask;

	if (pRequestContext->dwContentLength != 0)
	{
		// reset the buffer making sure we have enough memory available to handle all the response
		pRequestContext->ResponseData.Init(false, pRequestContext->dwContentLength);
	}
	else
	{
		pRequestContext->ResponseData.Init();
	}

	*pdwFilterContext = dwContext;	

	// inform clients something is up
	m_NotifyHelper.Fire_OnResponseStart(pIISInfo, pRequestInfo, pResponseInfo, dwContext);	

	return S_OK;
}

// returns:
//	S_OK		- succeeded
//	S_FALSE		- call will not be handled
//	E_PENDING	- more data required
//	E_ABORT		- an error has occurred, the stream will contain previous data on return
STDMETHODIMP CIISxpressHTTPRequest::OnSendRawData(
										DWORD dwFilterContext, 
										IStream* pRawDataStream, 
										DWORD dwStreamDataLength, 
										BOOL bHasHeader, signed char* pszResponseContentEncoding, 
										DWORD dwResponseContentEncodingBufferSize)
{
#ifdef HTTPREQUEST_METHODMETRICS
	CAutoBlockTimer timer(_T("* CIISxpressHTTPRequest::OnSendRawData()"));
#endif	

	// get the debug state
	BOOL bDebugEnabled = FALSE;
	m_pRegSettings->get_DebugEnabled(&bDebugEnabled);

	// get the logging level
	DWORD dwLoggingLevel = IISXPRESS_LOGGINGLEVEL_NONE;
	m_pRegSettings->get_LoggingLevel(&dwLoggingLevel);

	// log entry to the method
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendRawData(%u)\n");
		MyOutputDebugString(bDebugEnabled, pszMsg, dwFilterContext);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg, dwFilterContext);
	}

	// get hold of the request data
	IISxpressRequestContext* pRequestContext = GetActiveContextObject(dwFilterContext);

	// set the thread affinity
	CSetThreadAffinity affinity(pRequestContext->dwProcessorAffinityMask);	

	// if we don't have a valid context data pointer then it's an error
	if (pRequestContext == NULL)
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendRawData() unable to resolve filter context, returning ACTIVECONTEXTFAILED\n");
		MyOutputDebugString(bDebugEnabled, pszMsg);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return IISXPRESSHTTPREQUEST_ACTIVECONTEXTFAILED;
	}

	CMyHTTPResponseData* pResponseData = &pRequestContext->ResponseData;	

	// determine whether we have already got the header or not
	bool bGotHeader = pResponseData->GotHeader();	

	// if the incoming stream pointer is NULL then something bad has happened,
	// abort the compression process
	if (pRawDataStream == NULL)
	{
		ReturnContextObject(dwFilterContext);

		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendRawData() the incoming stream was bad, returning BADINCOMINGSTREAM\n");
		MyOutputDebugString(bDebugEnabled, pszMsg);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg);

		return IISXPRESSHTTPREQUEST_BADINCOMINGSTREAM;
	}	

	// append the stream data
	HRESULT hr = pRequestContext->ResponseData.AppendData(pRawDataStream, dwStreamDataLength, bHasHeader == TRUE ? true : false);
	if (FAILED(hr) == TRUE)
	{
		if (bGotHeader == true)
		{
			BuildStream(pResponseData, pRawDataStream);
			hr = E_ABORT;
		}

		ReturnContextObject(dwFilterContext);

		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnSendRawData() unable to handle response data, returning 0x%08x\n");
		MyOutputDebugString(bDebugEnabled, pszMsg, hr);
		MyOutputLoggingString(dwLoggingLevel, IISXPRESS_LOGGINGLEVEL_ENH, pszMsg, hr);

		return hr;
	}

	// we now know the full size of the response
	char* pchEndOfScan = NULL;
	DWORD dwExpectedBodySize = pRequestContext->dwContentLength;

	// if we are on the first pass make sure there is enough physical memory
	if (bGotHeader == false)
	{
		// dump the header to the debug window or log
		if ((bDebugEnabled == TRUE || dwLoggingLevel == IISXPRESS_LOGGINGLEVEL_FULL) && pResponseData->GotHeader() == true)
		{
			string sHeader;
			pResponseData->GetHTTPHeader(sHeader);

			if (bDebugEnabled == TRUE)
			{
				CDebugOutput::OutputDebugString(true, _T("*********************\nRaw header follows:\n"));
				CDebugOutput::OutputDebugString(true, sHeader.c_str());
				CDebugOutput::OutputDebugString(true, _T("*********************\n"));
			}

			if (dwLoggingLevel == IISXPRESS_LOGGINGLEVEL_FULL)
			{
				MyOutputLoggingString(IISXPRESS_LOGGINGLEVEL_FULL, IISXPRESS_LOGGINGLEVEL_FULL, _T("*********************\nRaw header follows:\n"));
				MyOutputLoggingString(IISXPRESS_LOGGINGLEVEL_FULL, IISXPRESS_LOGGINGLEVEL_FULL, CString(sHeader.c_str()));
				MyOutputLoggingString(IISXPRESS_LOGGINGLEVEL_FULL, IISXPRESS_LOGGINGLEVEL_FULL, _T("*********************\n"));
			}
		}				
	}

	// get the size of the data we are current holding
	DWORD dwReportedBodySize = pResponseData->GetBodySize();	

	// if we have all the data we expect then we need to return it to the client
	if (dwExpectedBodySize != 0 && 
		dwReportedBodySize >= dwExpectedBodySize)
	{
		return CompressResponse(dwFilterContext, pRequestContext, pRawDataStream, (char*) pszResponseContentEncoding, dwResponseContentEncodingBufferSize);
	}
	else
	{		
		// the data stream isn't finished yet
		return E_PENDING;
	}
}

// returns:
//	S_OK		- succeeded, data being returned
//	S_FALSE		- succeeded, no data is being returned
STDMETHODIMP CIISxpressHTTPRequest::OnEndOfRequest(
									DWORD dwFilterContext, 
									IStream* pRawDataStream, 
									DWORD dwStreamDataLength, 
									BOOL bHasHeader,
									signed char* pszResponseContentEncoding, 
									DWORD dwResponseContentEncodingBufferSize)
{
#ifdef HTTPREQUEST_METHODMETRICS
	CAutoBlockTimer timer(_T("* CIISxpressHTTPRequest::OnEndOfRequest()"));
#endif		
	
	// log the entry to the method
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::OnEndOfRequest(%u)\n");
		MyOutputDebugString(m_pRegSettings, pszMsg, dwFilterContext);	
		MyOutputLoggingString(m_pRegSettings, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg, dwFilterContext);
	}

	HRESULT hr = S_FALSE;	
	
	if (pRawDataStream != NULL)
	{
		IISxpressRequestContext* pRequestContext = GetActiveContextObject(dwFilterContext);
		if (pRequestContext != NULL)
		{
			// set the thread affinity
			CSetThreadAffinity affinity(pRequestContext->dwProcessorAffinityMask);			

			// append any extra data sent by the caller (if there is any)
			if (dwStreamDataLength > 0)
			{
				hr = pRequestContext->ResponseData.AppendData(pRawDataStream, dwStreamDataLength, bHasHeader == TRUE ? true : false);
			}
			else
			{
				hr = S_OK;
			}

			if (SUCCEEDED(hr) == TRUE && pRequestContext->ResponseData.GotHeader() == true)
			{			
				hr = CompressResponse(dwFilterContext, pRequestContext, pRawDataStream, (char*) pszResponseContentEncoding, dwResponseContentEncodingBufferSize);
			}
		}
	}

	// return the context object because the request is now finished
	ReturnContextObject(dwFilterContext);

	// inform attached clients	
	m_NotifyHelper.Fire_OnResponseEnd(dwFilterContext);

	return hr;
}

STDMETHODIMP CIISxpressHTTPRequest::AbortRequest(DWORD dwFilterContext)
{
	ATLTRACE("CIISxpressHTTPRequest::AbortRequest(%u)\n", dwFilterContext);

	// return the context object because the request is now finished
	ReturnContextObject(dwFilterContext);

	// inform attached clients
	m_NotifyHelper.Fire_OnResponseAbort(dwFilterContext);

	return S_OK;
}

STDMETHODIMP CIISxpressHTTPRequest::OnReadRawData(IStream* pRawDataStream)
{
	ATLTRACE("CIISxpressHTTPRequest::OnReadRawData()\n");

	return E_NOTIMPL;
}

STDMETHODIMP CIISxpressHTTPRequest::OnEndOfNetSession(DWORD dwFilterContext)
{
	ATLTRACE("CIISxpressHTTPRequest::OnEndOfNetSession(%u)\n", dwFilterContext);

	// return the context object - this is required for the case where IIS is told to
	// terminate processing before we reach the end of the response (or maybe the client dies...)
	ReturnContextObject(dwFilterContext);

	// inform attached clients
	m_NotifyHelper.Fire_OnResponseEnd(dwFilterContext);

	return S_OK;
}

bool CIISxpressHTTPRequest::BuildStream(const string& sHeader, const BYTE* pbyBody, DWORD dwBodySize, IStream* pStream)
{
#ifdef HTTPREQUEST_METHODMETRICS
	CAutoBlockTimer timer(_T("* CIISxpressHTTPRequest::BuildStream()"));
#endif

	if (pbyBody == NULL || dwBodySize == 0)
		return false;

	DWORD dwHeaderSize = (DWORD) sHeader.size();

	// calculate the total response size
	DWORD dwTotalResponseSize = dwHeaderSize + dwBodySize;		

	// resize the stream and seek to the beginning
	ULARGE_INTEGER nSize;
	nSize.HighPart = RESPONSESTREAM_SETSIZE_RESETANDALLOC;
	nSize.LowPart = dwTotalResponseSize;
	HRESULT hr = pStream->SetSize(nSize);

	// write the header into the stream
	ULONG nWritten = 0;

	// if we have a header then we need to write it into the stream
	if (dwHeaderSize > 0)
	{
		hr = pStream->Write((void*) sHeader.c_str(), (ULONG) sHeader.size(), &nWritten);	
		if (FAILED(hr) == true)
		{
			return false;
		}
	}

	// write the body into the stream
	if (pbyBody != NULL && dwBodySize > 0)
	{
		hr = pStream->Write((void*) pbyBody, dwBodySize, &nWritten);
		if (FAILED(hr) == TRUE)
		{
			return false;
		}
	}	

	return true;
}

bool CIISxpressHTTPRequest::BuildStream(CMyHTTPResponseData* pResponseData, IStream* pStream)
{
	if (pResponseData == NULL || pStream == NULL)
		return false;	

	string sHeader;
	if (pResponseData->GetHTTPHeader(sHeader) == false)
	{
		return false;
	}

	const BYTE* pbyBody = pResponseData->GetBody();
	DWORD dwBodySize = pResponseData->GetBodySize();

	return BuildStream(sHeader, pbyBody, dwBodySize, pStream);	
}

HRESULT CIISxpressHTTPRequest::CompressResponse(DWORD dwFilterContext, IISxpressRequestContext* pRequestContext, IStream* pRawDataStream, char* pszResponseContentEncoding, DWORD dwResponseContentEncodingBufferSize)
{
#ifdef HTTPREQUEST_METHODMETRICS
	CAutoBlockTimer timer(_T("* CIISxpressHTTPRequest::CompressResponse()"));
#endif

	if (pRequestContext == NULL || pRawDataStream == NULL)
	{
		return E_POINTER;
	}

	CMyHTTPResponseData* pResponseData = &pRequestContext->ResponseData;	
	if (pResponseData == NULL)
	{
		return E_POINTER;
	}

	// we need a lower case version of the accept encoding string
	DWORD dwAcceptEncodingSize = (DWORD) pRequestContext->sAcceptEncoding.GetLength() + 1;
	char* pszAcceptEncoding = (char*) alloca(dwAcceptEncodingSize);
	strcpy_s(pszAcceptEncoding, dwAcceptEncodingSize, pRequestContext->sAcceptEncoding);
	_strlwr_s(pszAcceptEncoding, dwAcceptEncodingSize);		
	
	bool bBZIP2 = (strstr(pszAcceptEncoding, "bzip2") != NULL ? true : false);			

	// if the client supports BZIP2 then we need to check the configuration settings
	if (bBZIP2)
	{
		BZIP2Mode modeBZIP2 = BZIP2ModeStaticOnly;
		m_pRegSettings->get_BZIP2Mode(&modeBZIP2);		

		switch (modeBZIP2)
		{
		case BZIP2ModeAll: break;
		case BZIP2ModeDisabled: bBZIP2 = FALSE; break;
		case BZIP2ModeStaticOnly:
			{
				// we use last-modified to determine whether the response is static or not
				if (pRequestContext->sLastModified.GetLength() <= 0)
				{
					bBZIP2 = false;
				}
			}
		}
	}

	bool bDeflate = (strstr(pszAcceptEncoding, "deflate") != NULL ? true : false);		
	bool bGZIP = (strstr(pszAcceptEncoding, "gzip") != NULL ? true : false);

	BOOL bDebugEnabled = FALSE;
	m_pRegSettings->get_DebugEnabled(&bDebugEnabled);

	DWORD dwAHA363Enabled = 0;	
	if (m_pRegSettings->get_AHAEnabled(&dwAHA363Enabled) == S_OK && dwAHA363Enabled != 0)
	{		
		// TODO: make this more elegant
		static volatile DWORD dwLastAHA363Enabled = 1;

		// do the AHA temperature check
		DWORD dwAHATempCheck = m_dwAHATempCheck++;
		if (m_pAHA363Status != NULL && (dwAHATempCheck % 200) == 0)
		{
			DWORD dwAHADetectOverheat = 1;
			m_pRegSettings->get_AHADetectOverheat(&dwAHADetectOverheat);

			DWORD dwFPGATemp = 0;
			DWORD dwBoardTemp = 0;
			if (dwAHADetectOverheat != 0 && m_pAHA363Status->GetTemps(dwFPGATemp, dwBoardTemp) && dwFPGATemp >= AHA::CAHA363::MAX_AHA_FPGA_TEMP)
			{
				// we are over temp
				dwAHA363Enabled = 0;
			}

			// store the enabled state for the next call
			dwLastAHA363Enabled = dwAHA363Enabled;
		}
		else
		{
			// we didn't calculate the temp, use the last enabled value
			dwAHA363Enabled = dwLastAHA363Enabled;
		}
	}

	const BYTE* pbyBodyData = pResponseData->GetBody();
	DWORD dwBodySize = pResponseData->GetBodySize();		

	// if debug output is enabled then we need to start the system timer so we can tell
	// how long it takes to compress the data
	__int64 nStartTickCount = 0;
	if (bDebugEnabled == TRUE)
	{			
		nStartTickCount = g_Timer.GetMicroSecTimerCount();
	}	

	// work out which compressor to use
	DWORD dwCompressor = ::InterlockedIncrement((LONG*) &m_dwCompressorOffset);
	dwCompressor %= m_Compressors.Size();

	// get hold of the compressor
	CompressorData& Compressor = m_Compressors[dwCompressor];	
	
	CompressorStatus status = ::Error;
	DWORD dwCompressedSize = (DWORD) -1;
	{
		AutoMemberCriticalSection cs(&Compressor.m_csCompressor);					

		// select the correct compression stream
		const char* pszContentEncoding = NULL;
		ICompressor* pAlgorithm = NULL;		
		
		if (dwAHA363Enabled != 0 && Compressor.m_pAHA363DeflateCompressor != NULL && bDeflate)
		{
			pAlgorithm = Compressor.m_pAHA363DeflateCompressor;
			pszContentEncoding = "deflate";
		}
		else if (dwAHA363Enabled != 0 && Compressor.m_pAHA363GZIPCompressor != NULL && bGZIP)
		{
			pAlgorithm = Compressor.m_pAHA363GZIPCompressor;
			pszContentEncoding = "gzip";
		}
		else if (bBZIP2)
		{		
			// we only allow BZIP2 if the client supports it and if it is enabled
			pAlgorithm = &Compressor.m_BZip2Compressor;
			pszContentEncoding = "bzip2";
		}		
		else if (bDeflate)
		{
			pAlgorithm = &Compressor.m_DeflateCompressor;
			pszContentEncoding = "deflate";
		}
		else if (bGZIP)
		{
			pAlgorithm = &Compressor.m_GZipCompressor;
			pszContentEncoding = "gzip";
		}		

		if (pAlgorithm != NULL && pszContentEncoding != NULL)
		{
			#ifdef HTTPREQUEST_METHODMETRICS
				CSimpleBlockTimer DeflateTimer;
				DeflateTimer.Start();
			#endif

			status = pAlgorithm->Compress(pbyBodyData, dwBodySize, Compressor.m_CompressedBuffer, Compressor.m_CompressedBuffer.Size(), dwCompressedSize);

			#ifdef HTTPREQUEST_METHODMETRICS
				DeflateTimer.End();
				DeflateTimer.OutputIntervalToDebugWindow(CAtlString(pszContentEncoding));
			#endif		

			// we expect Finished when it's finished, if we get OutBufferTooSmall the buffer wasn't big enough
			// only attempt again if the block size is smaller than the raw data
			while (status == OutBufferTooSmall && Compressor.m_CompressedBuffer.Size() < dwBodySize)
			{
				// guess the new buffer size
				DWORD dwNewCompressedDataBlockSize = Compressor.m_CompressedBuffer.Size() + (dwBodySize / 4);
				
				// if we are over the body size then just guess the theoretical maximum
				if (dwNewCompressedDataBlockSize >= dwBodySize)
				{
					dwNewCompressedDataBlockSize = dwBodySize + (dwBodySize / 9);
				}
				
				if (pAlgorithm->CanContinueOnOutBufferTooSmall())
				{					
					CompressorBuffer<DEFAULT_COMPRESSION_BUFFER> newBuffer;					
					if (newBuffer.Allocate(dwNewCompressedDataBlockSize))
					{
						CompressorBuffer<DEFAULT_COMPRESSION_BUFFER>& oldBuffer = Compressor.m_CompressedBuffer;

						// copy the data we have compressed so far
						memcpy(newBuffer, oldBuffer, oldBuffer.Size());

						BYTE* pbyNextOut = newBuffer + oldBuffer.Size();
						DWORD dwAvailOut = newBuffer.Size() - oldBuffer.Size();
						status = pAlgorithm->Compress(NULL, 0, pbyNextOut, dwAvailOut, dwCompressedSize);

						Compressor.m_CompressedBuffer.Attach(newBuffer);
					}
					else
					{
						status = ::Error;
					}
				}
				else
				{
					if (Compressor.m_CompressedBuffer.Allocate(dwNewCompressedDataBlockSize))
					{
						status = pAlgorithm->Compress(pbyBodyData, dwBodySize, Compressor.m_CompressedBuffer, Compressor.m_CompressedBuffer.Size(), dwCompressedSize);
					}
					else
					{
						status = ::Error;
					}
				}
			}
		}		

		if (bDebugEnabled == TRUE)
		{
			__int64 nEndTickCount = g_Timer.GetMicroSecTimerCount();

			CDebugOutput::OutputDebugString(true, 
					_T("CIISxpressHTTPRequest::OnSendRawData(%u), compressed %u bytes to %u bytes in %I64u us\n"), 
					dwFilterContext, dwBodySize, dwCompressedSize, nEndTickCount - nStartTickCount);
		}

		// if the response was compressed then return it with the correct header info
		if (status == Finished && dwBodySize > dwCompressedSize)
		{			
			string sHeader;
			if (pResponseData->GotHeader() == true)
			{
				char szContentLength[32];
				wsprintfA(szContentLength, "%d", dwCompressedSize);
				pResponseData->SetParam("Content-Length", szContentLength);

				pResponseData->SetParam("Content-Encoding", pszContentEncoding);

				// get the header			
				pResponseData->GetHTTPHeader(sHeader);
			}

			// tell the caller which content encoding we used
			if (pszResponseContentEncoding != NULL)
			{
				strcpy_s(pszResponseContentEncoding, dwResponseContentEncodingBufferSize, pszContentEncoding);
			}

			// build the response stream
			BuildStream(sHeader, Compressor.m_CompressedBuffer, dwCompressedSize, pRawDataStream);
		}		
		else
		{
			// get the header
			string sHeader;
			pResponseData->GetHTTPHeader(sHeader);

			// build the response stream for the original data
			BuildStream(sHeader, pbyBodyData, dwBodySize, pRawDataStream);
		}

		if (pAlgorithm != NULL)
		{
			pAlgorithm->Reset();		
		}

		if (Compressor.m_CompressedBuffer.Size() > COMPRESSION_BUFFER_MEMLIMIT)
		{
			Compressor.m_CompressedBuffer.Allocate(DEFAULT_COMPRESSION_BUFFER);
		}

		// mark the compressor buffer as having been used
		Compressor.m_nCompressorLastUsedTickCount = g_Timer.GetTimerCount();	
	}

	// TODO: consider replacing the ZLIB specific compression status
	int nCompressedStatus = (status == Finished ? Z_OK : Z_ERRNO);

	// allocate a block for the response housekeeping thread to handle
	HousekeepResponseData* pHousekeepResponseData = HousekeepResponseData::Alloc(
		pRequestContext->sURLMapPath, 
		pRequestContext->sURLMapPath.GetLength(),
		pRequestContext->sContentType,
		pRequestContext->sContentType.GetLength(),
		nCompressedStatus, dwCompressedSize, dwBodySize);	

	// tell the housekeeping thread about the response
	if (pHousekeepResponseData != NULL)
	{			
		::PostThreadMessage(m_nHousekeepResponseDetailsThreadId, WM_HOUSEKEEPRESPONSE, (WPARAM) pHousekeepResponseData, NULL);
	}
	
	// *************************************************************************************************
	// AFTER THIS POINT THE FILTER CONTEXT IS NOT VALID
	// *************************************************************************************************
	ReturnContextObject(dwFilterContext);
	pRequestContext = NULL;	

	// inform attached clients (the filter context is not valid but it doesn't matter since
	// the client only uses it as a cookie)
	m_NotifyHelper.Fire_OnResponseComplete(dwFilterContext, 
		(nCompressedStatus == Z_OK && dwBodySize > dwCompressedSize) ? TRUE : FALSE, dwBodySize, dwCompressedSize);

	return S_OK;
}

IISxpressRequestContext* CIISxpressHTTPRequest::LeaseContextObject(DWORD dwContext)
{
	// log entry to the method
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::LeaseContextObject(%u)\n");
		MyOutputDebugString(m_pRegSettings, pszMsg, dwContext);
		MyOutputLoggingString(m_pRegSettings, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg, dwContext);
	}

	// get the time now
	__int64 nNow = g_Timer.GetTimerCount();

	IISxpressRequestContext* pRequestContext = NULL;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);

	// if we have no context objects left then try and recover dead ones (only once per interval)
	if ((m_nRequestStackLastUsedTickCount + REQUEST_STACK_MIN_SCAVENGE_PERIOD) <= nNow && m_RequestContextStack.size() == 0)
	{
		RecoverContextObjects();
	}

	if (m_RequestContextStack.size() >= 1)
	{
		pRequestContext = m_RequestContextStack.top();
		m_RequestContextStack.pop();		

		m_ActiveRequests[dwContext] = pRequestContext;

		// set the last used
		if (pRequestContext != NULL)
		{
			pRequestContext->nLastUsed = nNow;
		}
	}	

	m_nRequestStackLastUsedTickCount = nNow;

	return pRequestContext;
}

void CIISxpressHTTPRequest::ReturnContextObject(DWORD dwContext)
{
	// log entry to the method
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::ReturnContextObject(%u)\n");
		MyOutputDebugString(m_pRegSettings, pszMsg, dwContext);
		MyOutputLoggingString(m_pRegSettings, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg, dwContext);
	}

	CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);

	// put the active request object onto the available request stack, take account of the double
	// release condition - so check that iter->second isn't already NULL
	map<DWORD, IISxpressRequestContext*>::iterator iter = m_ActiveRequests.find(dwContext);
	if (iter != m_ActiveRequests.end() && iter->second != NULL)
	{
		// reset the last used
		iter->second->nLastUsed = 0;

		m_RequestContextStack.push(iter->second);
		iter->second = NULL;
	}

	// clear out the active request map (removes the inactive contexts)
	{
		size_t activeRequests = m_ActiveRequests.size();
		
		if (activeRequests > (MAX_CONTEXTS * 4))
		{
			map<DWORD, IISxpressRequestContext*> cleanedUpActiveRequests;
			map<DWORD, IISxpressRequestContext*>::const_iterator iter = m_ActiveRequests.begin();
			while (iter != m_ActiveRequests.end())
			{
				if (iter->second != NULL)
				{
					cleanedUpActiveRequests[iter->first] = iter->second;
				}

				++iter;
			}

			m_ActiveRequests = cleanedUpActiveRequests;
		}
	}

	m_nRequestStackLastUsedTickCount = g_Timer.GetTimerCount();
}

void CIISxpressHTTPRequest::RecoverContextObjects(void)
{
	// log entry to the method
	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::RecoverContextObjects()\n");
		MyOutputDebugString(m_pRegSettings, pszMsg);
		MyOutputLoggingString(m_pRegSettings, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg);
	}

	// get the time now
	__int64 nNow = g_Timer.GetTimerCount();	

	CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);

	// create a vector to hold the potential dead contexts
	vector<DWORD> dwaDeadContexts;
	dwaDeadContexts.reserve(m_ActiveRequests.size());

	// loop through the active request map
	map<DWORD, IISxpressRequestContext*>::const_iterator iter = m_ActiveRequests.begin();
	while (iter != m_ActiveRequests.end())
	{		
		if (iter->second != NULL)
		{
			// if the context is active, has no body size and is too old then it's probably dead
			if (iter->second->ResponseData.GetBodySize() == 0 &&
				(iter->second->nLastUsed + REQUEST_STACK_MAXDEADLIFETIME) < nNow)
			{			
				// store it in the dead array
				DWORD dwContext = iter->first;
				dwaDeadContexts.push_back(dwContext);
			}
			else if ((iter->second->nLastUsed + REQUEST_STACK_FORCEKILLLIFETIME) < nNow)
			{
				// we are forcing a kill, store it in the dead array
				DWORD dwContext = iter->first;
				dwaDeadContexts.push_back(dwContext);
			}
		}

		++iter;
	}

	// get rid of the dead contexts
	for (int i = 0; i < (int) dwaDeadContexts.size(); i++)
	{
		DWORD dwContext = dwaDeadContexts[i];
		ReturnContextObject(dwContext);
	}

	{
		TCHAR* pszMsg = _T("CIISxpressHTTPRequest::RecoverContextObjects() - recovered %d objects\n");
		MyOutputDebugString(m_pRegSettings, pszMsg, (int) dwaDeadContexts.size());
		MyOutputLoggingString(m_pRegSettings, IISXPRESS_LOGGINGLEVEL_FULL, pszMsg, (int) dwaDeadContexts.size());
	}
}

IISxpressRequestContext* CIISxpressHTTPRequest::GetActiveContextObject(DWORD dwContext)
{
	IISxpressRequestContext* pRequestContext = NULL;

	__int64 nNow = g_Timer.GetTimerCount();

	CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);

	map<DWORD, IISxpressRequestContext*>::iterator iter = m_ActiveRequests.find(dwContext);
	if (iter != m_ActiveRequests.end())
	{
		pRequestContext = iter->second;

		// keep the last used up to date
		pRequestContext->nLastUsed = nNow;
	}

	m_nRequestStackLastUsedTickCount = nNow;

	return pRequestContext;
}

unsigned __stdcall CIISxpressHTTPRequest::HousekeepingProc(void* pData)
{
	CIISxpressHTTPRequest* pThis = (CIISxpressHTTPRequest*) pData;
	return pThis->HousekeepingProc();
}

DWORD CIISxpressHTTPRequest::HousekeepingProc(void)
{
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CComQIPtr<IComIISxpressRegSettings> pRegSettings = m_pRegSettings;

	MyOutputDebugString(pRegSettings, 
		_T("CIISxpressHTTPRequest::HousekeepingProc() started\n"));

	while (::WaitForSingleObject(m_hTermThreads, HOUSEKEEPING_CYCLE_PERIOD) == WAIT_TIMEOUT)
	{
		MyOutputDebugString(pRegSettings, 
			_T("CIISxpressHTTPRequest::HousekeepingProc() cleaning!\n"));

		__int64 now = g_Timer.GetTimerCount();

		// clean up the compressors (use a double lock)
		for (int i = 0; i < (int) m_Compressors.Size(); i++)
		{
			__int64 period = now - m_Compressors[i].m_nCompressorLastUsedTickCount;

			if (period >= COMPRESSION_BUFFER_RESET_PERIOD &&
				m_Compressors[i].m_CompressedBuffer.Size() > DEFAULT_COMPRESSION_BUFFER)
			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_Compressors[i].m_csCompressor);		

				period = now - m_Compressors[i].m_nCompressorLastUsedTickCount;

				if (m_Compressors[i].m_CompressedBuffer.Size() > DEFAULT_COMPRESSION_BUFFER)
				{
					m_Compressors[i].m_CompressedBuffer.Allocate(DEFAULT_COMPRESSION_BUFFER);

					m_Compressors[i].m_nCompressorLastUsedTickCount = g_Timer.GetTimerCount();

					MyOutputDebugString(pRegSettings, 
						_T("CIISxpressHTTPRequest::HousekeepingProc() reset compression buffer\n"));
				}
			}
		}
		
		// clean up the memory used by the non-active context stack
		{			
			__int64 period = now - m_nRequestStackLastResetTickCount;
			
			if (period >= REQUEST_STACK_RESET_PERIOD)
			{
				// store the last reset time for next time
				m_nRequestStackLastResetTickCount = now;

				CComCritSecLock<CComAutoCriticalSection> lock(m_csRequestStack);				

				stack<IISxpressRequestContext*> TempRequestContextStack = m_RequestContextStack;			 			

				while (TempRequestContextStack.size() >= 1)
				{
					IISxpressRequestContext* pRequestContext = TempRequestContextStack.top();
					TempRequestContextStack.pop();

					if (pRequestContext != NULL)
					{
						int nOldAllocatedSize = pRequestContext->ResponseData.GetAllocatedBufferSize();

						pRequestContext->ResponseData.Init(true);

						int nNewAllocatedSize = pRequestContext->ResponseData.GetAllocatedBufferSize();

						if (nNewAllocatedSize < nOldAllocatedSize)
						{
							MyOutputDebugString(pRegSettings, 
								_T("CIISxpressHTTPRequest::HousekeepingProc() reset request stack, %d bytes freed\n"), 
								nOldAllocatedSize - nNewAllocatedSize);
						}
					}
				}
			}		
		}
	}

	MyOutputDebugString(pRegSettings, 
		_T("CIISxpressHTTPRequest::HousekeepingProc() ended\n"));

	::CoUninitialize();

	return 0;
}

STDMETHODIMP CIISxpressHTTPRequest::IsAlive(DWORD* pdwData)
{
	if (pdwData == NULL)
		return E_POINTER;

	*pdwData = m_dwIsAliveCookie;
	return S_OK;
}

unsigned __stdcall CIISxpressHTTPRequest::StartupPerfCounterProc(void* pVoid)
{
	if (pVoid == NULL)
		return 0;

	CIISxpressHTTPRequest* pThis = (CIISxpressHTTPRequest*) pVoid;
	return pThis->StartupPerfCounterProc();
}

DWORD CIISxpressHTTPRequest::StartupPerfCounterProc(void)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csPerfCounter);	

	// if PDH.DLL is loaded then try to start the performance counter	
	HQUERY hPerfQuery = NULL;
	if (::PdhOpenQuery(NULL, 1, &hPerfQuery) == ERROR_SUCCESS)
	{
		m_hPerfQuery.Attach(hPerfQuery);
		hPerfQuery = NULL;
		
		HCOUNTER hPerfCounter = NULL;
		if (::PdhAddCounter(m_hPerfQuery, _T("\\Processor(_Total)\\% Processor Time"), 0, &hPerfCounter) == ERROR_SUCCESS)
		{
			m_hPerfCounter.Attach(hPerfCounter);
			hPerfCounter = NULL;

			MyOutputDebugString(m_pRegSettings, 
				_T("CIISxpressHTTPRequest::FinalConstruct() - added performance counter!\n"));
		}
	}	

	return 0;
}

CompressorMode CIISxpressHTTPRequest::MapCompressionMode(CompressionMode Mode)
{
	switch (Mode)
	{
	case CompressionModeNone:		return None;
	case CompressionModeSize:		return Size;
	case CompressionModeSpeed:		return Speed;

	default:
	case CompressionModeDefault:	return Default;
	}
}

bool CIISxpressHTTPRequest::InitCompressorStreams(CompressorMode mode)
{
	for (int i = 0; i < (int) m_Compressors.Size(); i++)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_Compressors[i].m_csCompressor);
		m_Compressors[i].InitCompressorStreams(mode);	
	}

	return true;
}

unsigned __stdcall CIISxpressHTTPRequest::HousekeepResponseDetailsProc(void* pData)
{
	return ((CIISxpressHTTPRequest*) pData)->HousekeepResponseDetailsProc();
}

DWORD CIISxpressHTTPRequest::HousekeepResponseDetailsProc(void)
{		
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CComPtr<ICompressionHistory> pCompressionHistory;
	CComPtr<IStaticFileRules> pStaticFileRules;

	// we want low priority
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	// force Windows to create the thread's message queue (this is according to MSDN)
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	
	BOOL bExit = FALSE;
	while (bExit == FALSE)
	{
		// wait until we get a message or the termination event is triggered
		DWORD dwStatus = ::MsgWaitForMultipleObjects(1, &m_hTermThreads, FALSE, INFINITE, QS_ALLINPUT);		
		if (dwStatus == WAIT_OBJECT_0)
			break;

		// eat everything in the message queue		
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			switch (msg.message)
			{

			case WM_HOUSEKEEPRESPONSE:

				{
					if (pCompressionHistory == NULL)
					{
						pCompressionHistory = m_pCompressionHistory;
					}

					if (pStaticFileRules == NULL)
					{
						pStaticFileRules = m_pStaticFileRules;
					}

					HousekeepResponseData* pData = (HousekeepResponseData*) msg.wParam;

					// update history
					if (pData != NULL && pCompressionHistory != NULL)
					{			
						pCompressionHistory->UpdateEntry(pData->pszURIMapPath, pData->dwOriginalSize, 
							(pData->nCompressedStatus == Z_OK && pData->dwOriginalSize > pData->dwCompressedSize) ? TRUE : FALSE, 
							pData->dwCompressedSize);
					}

					// update static file info
					if (pData != NULL && pStaticFileRules != NULL)
					{		
						int nUserCompress = -1;
						int nSystemCompress = -1;										
						if (pStaticFileRules->GetFileRule(pData->pszURIMapPath, &nUserCompress, &nSystemCompress) == S_FALSE)
						{
							pStaticFileRules->AddFileRule(pData->pszURIMapPath, nUserCompress, nSystemCompress);
						}

						// only update if the user has not overridden the behaviour
						if (nUserCompress < 0)
						{
							// since we don't want to update the storage each time determine whether
							// there is actually a need to change the compression flag state 
							bool bChanged = false;

							// if the compression was successful...
							if (pData->nCompressedStatus == Z_OK && pData->dwOriginalSize > pData->dwCompressedSize)
							{						
								// set the new system compression flag and mark as changed
								if (nSystemCompress != 1)
								{
									nSystemCompress = 1;
									bChanged = true;
								}
							}
							else if (nSystemCompress != 0)
							{
								// the compression wasn't successful
								nSystemCompress = 0;
								bChanged = true;
							}

							// if the compression flag state has changed then we need to update the storage							
							if (bChanged == true)
							{
								pStaticFileRules->SetFileRule(pData->pszURIMapPath, nUserCompress, nSystemCompress);
							}
						}
					}

					// keep track of some simple response details
					m_TransientHistory.SetEntry(pData->pszURIMapPath, pData->pszContentType, pData->dwOriginalSize);

					// get rid of the response data
					pData->Free();

					break;
				}

			case WM_QUIT:
				bExit = TRUE;
				break;

			default:
				// we've got to pass everything else on, otherwise DEADLOCK!
				::DispatchMessage(&msg);
				break;
			}

			if (bExit == TRUE)
				break;
		}		
	}

	::CoUninitialize();

	return 0;
}

STDMETHODIMP CIISxpressHTTPRequest::NotifyCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo,
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize)
{
	m_NotifyHelper.Fire_OnCacheHit(pIISInfo, pRequestInfo, pResponseInfo, dwCompressedSize);

	m_pCompressionHistory2->UpdateEntry(pIISInfo->pszURLMapPath, pResponseInfo->dwContentLength, TRUE, dwCompressedSize, TRUE);

	return S_OK;
}

DWORD CIISxpressHTTPRequest::GetProcessorAffinityMask(DWORD dwMode)
{
	// default to off (OS decides)
	DWORD dwProcessorAffinityMask = 0;

	if (dwMode == IISXPRESS_SERVER_PROCESSORAFFINITY_OFF || m_dwNumberOfCores == 1)
	{
		// the affinity mode is off or there is only one core
		dwProcessorAffinityMask = 0;
	}
	else if (dwMode == IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO)
	{
		// figure out which core to activate
		DWORD dwCore = (DWORD) ::InterlockedIncrement((LONG*) &m_dwActiveCore);
		dwCore %= m_dwNumberOfCores;
		dwCore = 1 << dwCore;
		dwProcessorAffinityMask = dwCore;
	}
	else
	{
		if (dwMode == 1 || dwMode == 2 || dwMode == 4 || dwMode == 8 ||
			dwMode == 16 || dwMode == 32 || dwMode == 64 || dwMode == 128)
		{
			dwProcessorAffinityMask = dwMode;
		}
		else
		{
			// loop until we get a core match or we run out of cores
			DWORD i = 0;
			for (; i < m_dwNumberOfCores; i++)
			{
				// figure out which core to activate
				DWORD dwCore = (DWORD) ::InterlockedIncrement((LONG*) &m_dwActiveCore);
				dwCore %= m_dwNumberOfCores;
				dwCore = 1 << dwCore;

				// have we found a core we can use?
				if ((dwMode & dwCore) != 0)
				{
					dwProcessorAffinityMask = dwCore;
					break;
				}
			}				

			// if we didn't find a core to use then default to off
			if (i == m_dwNumberOfCores)
			{
				dwProcessorAffinityMask = 0;
			}
		}
	}

	return dwProcessorAffinityMask;
}

STDMETHODIMP CIISxpressHTTPRequest::get_CompressionMode(CompressionMode* pMode)
{
	if (m_pSettings == NULL)
		return E_FAIL;

	return m_pSettings->get_CompressionMode(pMode);
}

STDMETHODIMP CIISxpressHTTPRequest::put_CompressionMode(CompressionMode Mode)
{
	if (m_pSettings == NULL)
		return E_FAIL;

	CompressionMode LastMode = CompressionModeDefault;
	HRESULT hr = m_pSettings->get_CompressionMode(&LastMode);
	if (hr != S_OK)
	{
		return hr;
	}

	// don't do anything if the caller is asking for the old compression mode
	if (LastMode == Mode)
	{
		return S_OK;
	}
	
	// map the compression mode into ZLIB land
	CompressorMode compressorMode = MapCompressionMode(Mode);	

	if (InitCompressorStreams(compressorMode) == true)
	{
		// set the new mode
		hr = m_pSettings->put_CompressionMode(Mode);
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;	
}

STDMETHODIMP CIISxpressHTTPRequest::get_ProcessorAffinity(DWORD* pdwProcessorAffinity)
{
	if (m_pSettings == NULL)
		return E_FAIL;

	return m_pSettings->get_ProcessorAffinity(pdwProcessorAffinity);
}

STDMETHODIMP CIISxpressHTTPRequest::put_ProcessorAffinity(DWORD dwProcessorAffinity)
{
	if (m_pSettings == NULL)
		return E_FAIL;

	return m_pSettings->put_ProcessorAffinity(dwProcessorAffinity);
}

STDMETHODIMP CIISxpressHTTPRequest::Advise(IUnknown* pUnkSink, DWORD* pdwCookie)
{
	HRESULT hr = IConnectionPointImpl<CIISxpressHTTPRequest, &IID_IIISxpressHTTPNotify>::Advise(pUnkSink, pdwCookie);
	m_NotifyHelper.NotifyClientsChanged();
	return hr;
}

STDMETHODIMP CIISxpressHTTPRequest::Unadvise(DWORD dwCookie)
{
	HRESULT hr = IConnectionPointImpl<CIISxpressHTTPRequest, &IID_IIISxpressHTTPNotify>::Unadvise(dwCookie);
	m_NotifyHelper.NotifyClientsChanged();
	return hr;
}

STDMETHODIMP CIISxpressHTTPRequest::GetBoardTemps(DWORD* pdwFPGA, DWORD* pdwBoard)
{
	if (pdwFPGA == NULL || pdwBoard == NULL)
	{
		return E_POINTER;
	}

	if (m_pAHA363Status == NULL)
	{
		return E_FAIL;
	}

	m_pAHA363Status->GetTemps(*pdwFPGA, *pdwBoard);

	return S_OK;
}

STDMETHODIMP CIISxpressHTTPRequest::GetBoardInfo(AHABoardInfo* pBoardInfo)
{
	if (pBoardInfo == NULL)
	{
		return E_POINTER;
	}

	if (m_pAHA363Status == NULL)
	{
		return E_FAIL;
	}

	AHA::AHABoardInfo info;
	if (m_pAHA363Status->GetBoardInfo(info))
	{
		pBoardInfo->bEnabled = info.enabled;
		wcscpy_s(pBoardInfo->szDescription, info.szDescription);
		pBoardInfo->InstallState = (AHABoardInstallState) info.InstallState;

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}