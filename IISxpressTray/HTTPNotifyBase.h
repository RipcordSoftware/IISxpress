#ifndef _HTTPNOTIFYBASE_H
#define _HTTPNOTIFYBASE_H

class CHTTPNotifyBase
{

public:

	virtual HRESULT OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext) = 0;

	virtual HRESULT OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize) = 0;

	virtual HRESULT OnResponseEnd(DWORD dwFilterContext) = 0;

	virtual HRESULT OnResponseAbort(DWORD dwFilterContext) = 0;

	virtual HRESULT OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize) = 0;
};

#endif