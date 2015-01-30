#include "stdafx.h"

#include "HTTPNotifyHelper.h"

volatile LONG CIISResponseData::m_instances = 0;

CIISResponseData* CIISResponseData::Create(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo)
{
	++m_instances;
	return new CIISResponseData(pIISInfo, pRequestInfo, pResponseInfo);
}

CIISResponseData* CIISResponseData::Create(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, BOOL bCacheHit, BOOL bCompressed, DWORD dwCompressedSize)
{
	++m_instances;
	return new CIISResponseData(pIISInfo, pRequestInfo, pResponseInfo, bCacheHit, bCompressed, dwCompressedSize);
}

void CIISResponseData::Free(CIISResponseData* pData)
{
	if (pData != NULL)
	{
		--m_instances;
		delete pData;
	}
}