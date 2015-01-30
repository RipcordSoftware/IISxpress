// EnumStaticFileInfo.cpp : Implementation of CEnumStaticFileInfo

#include "stdafx.h"
#include "EnumStaticFileInfo.h"


// CEnumStaticFileInfo

HRESULT CEnumStaticFileInfo::FinalConstruct()
{
	return S_OK;
}
	
void CEnumStaticFileInfo::FinalRelease() 
{	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csStaticFileInfo);

	int nSize = (int) m_StaticFileInfo.size();
	for (int i = 0; i < nSize; i++)
	{
		DestroyStaticFileInfo(m_StaticFileInfo[i]);
	}

	m_StaticFileInfo.clear();	
}

STDMETHODIMP CEnumStaticFileInfo::Next(ULONG celt, StaticFileInfo* rgelt, ULONG* pceltFetched)
{
	HRESULT hr = S_FALSE;

	try
	{
		THROW_HR_ON_NULL(rgelt, hr, E_POINTER);
		THROW_HR_ON_NULL(pceltFetched, hr, E_POINTER);

		if (celt != 1)
		{
			hr = E_INVALIDARG;
			throw hr;
		}

		CComCritSecLock<CComAutoCriticalSection> lock(m_csStaticFileInfo);

		if (m_nIndex < 0)
			m_nIndex = 0;

		int nSize = (int) m_StaticFileInfo.size();

		if (nSize > 0 && m_nIndex < nSize)
		{			
			CopyStaticFileInfo(m_StaticFileInfo[m_nIndex], *rgelt);
			m_nIndex++;

			hr = S_OK;
		}

		*pceltFetched = 1;
	}
	catch (...)
	{
	}	

	return hr;
}

STDMETHODIMP CEnumStaticFileInfo::Skip(ULONG celt)
{
	HRESULT hr = S_FALSE;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csStaticFileInfo);

	if (m_nIndex < 0)
		m_nIndex = 0;

	if (m_nIndex >= 0 && (m_nIndex + celt) < (int) m_StaticFileInfo.size())
	{
		m_nIndex += celt;
		hr = S_OK;
	}	

	return hr;
}

STDMETHODIMP CEnumStaticFileInfo::Reset(void)
{
	HRESULT hr = S_FALSE;
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csStaticFileInfo);

	if (m_StaticFileInfo.size() > 0)
	{
		m_nIndex = 0;
		hr = S_OK;
	}	

	return hr;
}

STDMETHODIMP CEnumStaticFileInfo::Clone(IEnumStaticFileInfo** ppenum)
{
	return E_NOTIMPL;
}

HRESULT CEnumStaticFileInfo::AddElement(BSTR bsPath, BSTR bsFilename, int nUserCompression, int nSystemCompression)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(bsPath, hr, E_POINTER);
		THROW_HR_ON_NULL(bsFilename, hr, E_POINTER);		

		// if the enumeration has started then don't allow any more elements to be added
		if (m_nIndex > 0)
		{
			hr = E_FAIL;
			throw hr;
		}

		StaticFileInfo Info;
		Info.bsPath = ::SysAllocString(bsPath);
		Info.bsFilename = ::SysAllocString(bsFilename);		
		Info.nUserCompression = nUserCompression;
		Info.nSystemCompression = nSystemCompression;
		
		CComCritSecLock<CComAutoCriticalSection> lock(m_csStaticFileInfo);

		m_StaticFileInfo.push_back(Info);
		m_nIndex = 0;

		hr = S_OK;
	}
	catch (...)
	{
	}

	return hr;
}

void CEnumStaticFileInfo::CopyStaticFileInfo(const StaticFileInfo& Source, StaticFileInfo& Target) 
{
	Target.bsPath = ::SysAllocString(Source.bsPath);
	Target.bsFilename = ::SysAllocString(Source.bsFilename);	
	Target.nUserCompression = Source.nUserCompression;
	Target.nSystemCompression = Source.nSystemCompression;
}

void CEnumStaticFileInfo::DestroyStaticFileInfo(StaticFileInfo& Info)
{
	if (Info.bsPath != NULL)
	{
		::SysFreeString(Info.bsPath);
		Info.bsPath = NULL;
	}

	if (Info.bsFilename != NULL)
	{
		::SysFreeString(Info.bsFilename);
		Info.bsFilename = NULL;
	}	
}