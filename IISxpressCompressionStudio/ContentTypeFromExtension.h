#pragma once

#include "AutoMemberCriticalSection.h"

class CContentTypeFromExtension
{
public:	

	template <typename T> bool GetContentTypeFromExtension(LPCTSTR pszExtension, T& sContentType);

private:

	CComAutoCriticalSection	m_csMap;

	std::map<CAdapt<CString>, CAdapt<CString> >	m_ExtensionToContentType;
};

template <typename T> bool CContentTypeFromExtension::GetContentTypeFromExtension(LPCTSTR pszExtension, T& sContentType)
{
	if (pszExtension == NULL)
	{
		return false;
	}

	if (pszExtension[0] == '.')
	{
		pszExtension++;
	}

	CString sExtension('.');
	sExtension += pszExtension;

	AutoMemberCriticalSection autoCS(&m_csMap);

	std::map<CAdapt<CString>, CAdapt<CString> >::const_iterator iter = m_ExtensionToContentType.find(sExtension);
	if (iter != m_ExtensionToContentType.end())
	{
		sContentType = T(iter->second.m_T);
		return true;
	}

	CRegKey registry;
	if (registry.Open(HKEY_CLASSES_ROOT, sExtension, KEY_READ) != ERROR_SUCCESS)
	{
		return false;
	}

	TCHAR szContentType[512];
	ULONG nChars = _countof(szContentType);
	if (registry.QueryStringValue(_T("Content Type"), szContentType, &nChars) != ERROR_SUCCESS)
	{
		return false;
	}

	sContentType = T(szContentType);

	m_ExtensionToContentType[sExtension] = szContentType;

	return true;
}