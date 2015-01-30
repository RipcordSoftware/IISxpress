#pragma once

class CIISInfo
{

public:

	CIISInfo() {}

	CIISInfo(const CIISInfo& Other)
	{
		m_sServerName = Other.m_sServerName;
		m_sServerPort = Other.m_sServerPort;
		m_sInstanceId = Other.m_sInstanceId;				
		m_sURLMapPath = Other.m_sURLMapPath;
	}

	CIISInfo(IISInfo* pIISInfo)
	{
		if (pIISInfo != NULL)
		{
			m_sServerName = pIISInfo->pszServerName;
			m_sServerPort = pIISInfo->pszServerPort;
			m_sInstanceId = pIISInfo->pszInstanceId;			
			m_sURLMapPath = pIISInfo->pszURLMapPath;
		}
	}

	CStringA	m_sServerName;
	CStringA	m_sServerPort;
	CStringA	m_sInstanceId;	
	CStringA	m_sURLMapPath;
};

class CRequestInfo
{

public:

	CRequestInfo() {}

	CRequestInfo(const CRequestInfo& Other)
	{		
		m_sUserAgent = Other.m_sUserAgent;
		m_sHostname = Other.m_sHostname;
		m_sURI = Other.m_sURI;
		m_sQueryString = Other.m_sQueryString;
		m_sAcceptEncoding = Other.m_sAcceptEncoding;
		m_sRemoteAddress = Other.m_sRemoteAddress;
	}

	CRequestInfo(RequestInfo* pRequestInfo)
	{
		if (pRequestInfo != NULL)
		{			
			m_sUserAgent = pRequestInfo->pszUserAgent;
			m_sHostname = pRequestInfo->pszHostname;
			m_sURI = pRequestInfo->pszURI;
			m_sQueryString = pRequestInfo->pszQueryString;
			m_sAcceptEncoding = pRequestInfo->pszAcceptEncoding;
			m_sRemoteAddress = pRequestInfo->pszRemoteAddress;
		}
	}
	
	CStringA	m_sUserAgent;
	CStringA	m_sHostname;
	CStringA	m_sURI;
	CStringA	m_sQueryString;
	CStringA	m_sAcceptEncoding;
	CStringA	m_sRemoteAddress;
};

class CResponseInfo
{

public:

	CResponseInfo() : m_dwResponseCode(0), m_dwContentLength(0) {}

	CResponseInfo(const CResponseInfo& Other)
	{
		m_dwResponseCode = Other.m_dwResponseCode;
		m_sContentType = Other.m_sContentType;
		m_dwContentLength = Other.m_dwContentLength;
	}

	CResponseInfo(ResponseInfo* pResponseInfo)
	{
		if (pResponseInfo != NULL)
		{
			m_dwResponseCode = pResponseInfo->dwResponseCode;
			m_sContentType = pResponseInfo->pszContentType;
			m_dwContentLength = pResponseInfo->dwContentLength;			
		}
	}

	DWORD		m_dwResponseCode;
	CStringA	m_sContentType;
	DWORD		m_dwContentLength;	
};


class CIISxpressResponseInfo
{
public:

	CIISxpressResponseInfo() 
		: m_bCacheHit(FALSE), m_bCompressed(FALSE), m_dwCompressedSize(0), m_nStartCompression(0), m_nEndCompression(0) {}

	CIISxpressResponseInfo(BOOL	bCacheHit, BOOL bCompressed, DWORD dwCompressedSize) 
		: m_bCacheHit(bCacheHit), m_bCompressed(bCompressed), m_dwCompressedSize(dwCompressedSize), 
		m_nStartCompression(0), m_nEndCompression(0) {}

	BOOL		m_bCacheHit;
	BOOL		m_bCompressed;
	DWORD		m_dwCompressedSize;
	__int64		m_nStartCompression;
	__int64		m_nEndCompression;
};

class CIISResponseData
{

private:

	CIISResponseData() {}	

	CIISResponseData(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo) :
							m_IISInfo(pIISInfo), m_RequestInfo(pRequestInfo), m_ResponseInfo(pResponseInfo)							
	{
	}

	CIISResponseData(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, 
		BOOL bCacheHit, BOOL bCompressed, DWORD dwCompressedSize) :
							m_IISInfo(pIISInfo), m_RequestInfo(pRequestInfo), m_ResponseInfo(pResponseInfo),
							m_IISxpressResponseInfo(bCacheHit, bCompressed, dwCompressedSize)
	{
	}

public:

    static CIISResponseData* Create(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo);

	static CIISResponseData* Create(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, 
		BOOL bCacheHit, BOOL bCompressed, DWORD dwCompressedSize);

	static void Free(CIISResponseData* pData);

	CIISInfo				m_IISInfo;
	CRequestInfo			m_RequestInfo;
	CResponseInfo			m_ResponseInfo;
	CIISxpressResponseInfo	m_IISxpressResponseInfo;

	static volatile LONG	m_instances;

};