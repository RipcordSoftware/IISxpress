#include "StdAfx.h"
#include "IISxpressConsoleNotify.h"

CIISxpressConsoleNotify::CIISxpressConsoleNotify(void)
{
}

CIISxpressConsoleNotify::~CIISxpressConsoleNotify(void)
{
}

STDMETHODIMP CIISxpressConsoleNotify::OnResponseStart(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext)
{
	m_csActiveResponses.Lock();
	m_ActiveResponses[dwFilterContext] = CIISResponseData::Create(pIISInfo, pRequestInfo, pResponseInfo);
	m_csActiveResponses.Unlock();

	return S_OK;
}

STDMETHODIMP CIISxpressConsoleNotify::OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
{
	m_csActiveResponses.Lock();

	std::map<DWORD, CIISResponseData*>::iterator iter = m_ActiveResponses.find(dwFilterContext);
	if (iter != m_ActiveResponses.end() && iter->second != NULL)
	{
		OutputResponseInfo((LPCSTR) iter->second->m_RequestInfo.m_sURI, dwRawSize, dwCompressedSize);

		delete iter->second;
		iter->second = NULL;		
	}	

	// when we have more than 100 active responses in the map try to clean them up
	if (m_ActiveResponses.size() > 100)
	{
		std::map<DWORD, CIISResponseData*> temp;

		std::map<DWORD, CIISResponseData*>::const_iterator iter = m_ActiveResponses.begin();
		for (; iter != m_ActiveResponses.end(); iter++)
		{
			if (iter->second != NULL)
				temp[iter->first] = iter->second;
		}

		m_ActiveResponses.swap(temp);
	}

	m_csActiveResponses.Unlock();	

	return S_OK;
}

STDMETHODIMP CIISxpressConsoleNotify::OnCacheHit(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize)
{
	if (pRequestInfo != NULL && pRequestInfo->pszURI && pResponseInfo != NULL)
	{
		OutputResponseInfo(pRequestInfo->pszURI, pResponseInfo->dwContentLength, dwCompressedSize);
	}

	return S_OK;
}

void CIISxpressConsoleNotify::OutputResponseInfo(LPCSTR pszURI, DWORD dwRawSize, DWORD dwCompressedSize)
{
	SYSTEMTIME stTime;
	::GetLocalTime(&stTime);
	
	CString sTime;		
	CInternationalHelper::Format24hTime(stTime, sTime);		

	CString sOriginalSize;
	sOriginalSize.Format(_T("%u"), dwRawSize);
	CInternationalHelper::FormatIntegerNumber(sOriginalSize);		

	CString sCompressedSize;
	sCompressedSize.Format(_T("%u"), dwCompressedSize);
	CInternationalHelper::FormatIntegerNumber(sCompressedSize);

	printf("%S %10S %10S %s\n", 
		(LPCWSTR) sTime, 
		(LPCWSTR) sOriginalSize, 
		(LPCWSTR) sCompressedSize, 
		(LPCSTR) pszURI);			
}

STDMETHODIMP CIISxpressConsoleNotify::OnResponseEnd(DWORD dwFilterContext)
{
	return S_OK;
}

STDMETHODIMP CIISxpressConsoleNotify::OnResponseAbort(DWORD dwFilterContext)
{
	return S_OK;
}