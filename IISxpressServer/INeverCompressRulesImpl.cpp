#include "StdAfx.h"

#include "inevercompressrulesimpl.h"
#include "EnumExclusionRule.h"

#include "IISxpressServer.h"

#define IISXPRESS_NEVERRULES			_T("NeverRules")

INeverCompressRulesImpl::INeverCompressRulesImpl(void)
{
	m_pDOM = NULL;
}

INeverCompressRulesImpl::~INeverCompressRulesImpl(void)
{
}

STDMETHODIMP INeverCompressRulesImpl::Init(IStorage* pStorage)
{
	HRESULT hr = E_FAIL;	

	try
	{
		THROW_HR_ON_NULL(pStorage, hr, E_POINTER);		

		// lock the DOM for the duration of the call
		AutoMemberCriticalSection AMCS_DOM(&m_csDOM);

		if (m_pDOM != NULL)
		{
			m_pDOM.Release();
		}

		hr = m_pDOM.CoCreateInstance(L"MSXML.DOMDocument");
		
		THROW_HR_ON_FAIL(hr);

		m_pConfigStorage = pStorage; 

		hr = Load();
		THROW_HR_ON_FAIL(hr);
	}
	catch (HRESULT)
	{		
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::AddExtensionRule(LPCSTR pszExtension, LPCSTR pszUserAgent, LPCSTR pszTypeString, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszExtension, hr, E_POINTER);
		THROW_HR_ON_NULL(pszUserAgent, hr, E_POINTER);
		THROW_HR_ON_NULL(pszTypeString, hr, E_POINTER);

		// skip the '.' if present
		if (pszExtension[0] == '.')
			pszExtension++;

		DWORD dwLowerExtensionSize = (DWORD) strlen(pszExtension) + 1;
		char* pszLowerExtension = (char*) alloca(dwLowerExtensionSize);
		strcpy_s(pszLowerExtension, dwLowerExtensionSize, pszExtension);
		_strlwr_s(pszLowerExtension, dwLowerExtensionSize);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);		

		if (m_NeverExtensions.find(pszLowerExtension) != m_NeverExtensions.end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/extnrule[text()='%hs']"), pszLowerExtension);

		CComPtr<IXMLDOMNode> pExtnRuleNode;
		m_pDOM->selectSingleNode(CComBSTR(sQuery), &pExtnRuleNode);

		if (pExtnRuleNode != NULL)
		{
			hr = S_FALSE;
			throw hr;
		}
		else
		{
			CComPtr<IXMLDOMNode> pExtnRuleNode;
			hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"extnrule"), CComBSTR(), &pExtnRuleNode);
			THROW_HR_ON_FAIL(hr);

			hr = pExtnRuleNode->put_text(CComBSTR(pszLowerExtension));
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pTemp;
			hr = pNeverRulesNode->appendChild(pExtnRuleNode, &pTemp);
			THROW_HR_ON_FAIL(hr);

			pExtnRuleNode = pTemp;
		}

		m_NeverExtensions[pszLowerExtension] = RuleUserAgents();

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintExtension, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::AddContentTypeRule(LPCSTR pszContentType, LPCSTR pszUserAgent, LPCSTR pszTypeString, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszContentType, hr, E_POINTER);
		THROW_HR_ON_NULL(pszUserAgent, hr, E_POINTER);
		THROW_HR_ON_NULL(pszTypeString, hr, E_POINTER);

		DWORD dwLowerContentTypeSize = (DWORD) strlen(pszContentType);
		if (dwLowerContentTypeSize == 0)
		{
			hr = E_INVALIDARG;
			throw hr;
		}

		char* pszLowerContentType = (char*) alloca(dwLowerContentTypeSize + 1);
		strcpy_s(pszLowerContentType, dwLowerContentTypeSize + 1, pszContentType);
		_strlwr_s(pszLowerContentType, dwLowerContentTypeSize + 1);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		map<string, RuleUserAgents>* pContentTypeRules = NULL;

		// decide which rule collection to deal with
		if (pszLowerContentType[dwLowerContentTypeSize - 1] != '*')
		{
			pContentTypeRules = &m_NeverContentTypes;
		}
		else
		{
			pContentTypeRules = &m_NeverContentTypeClass;
		}

		if (pContentTypeRules->find(pszLowerContentType) != pContentTypeRules->end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/contenttyperule[text()='%hs']"), pszLowerContentType);

		CComPtr<IXMLDOMNode> pContentTypeRuleNode;
		m_pDOM->selectSingleNode(CComBSTR(sQuery), &pContentTypeRuleNode);

		if (pContentTypeRuleNode != NULL)
		{
			hr = S_FALSE;
			throw hr;
		}
		else
		{
			CComPtr<IXMLDOMNode> pContentTypeRuleNode;
			hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"contenttyperule"), CComBSTR(), &pContentTypeRuleNode);
			THROW_HR_ON_FAIL(hr);

			hr = pContentTypeRuleNode->put_text(CComBSTR(pszLowerContentType));
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pTemp;
			hr = pNeverRulesNode->appendChild(pContentTypeRuleNode, &pTemp);
			THROW_HR_ON_FAIL(hr);

			pContentTypeRuleNode = pTemp;
		}
		
		(*pContentTypeRules)[pszLowerContentType] = RuleUserAgents();		

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintContentType, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::AddFolderRule(LPCSTR pszInstance, LPCSTR pszFolder, DWORD dwFlags, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszInstance, hr, E_POINTER);
		THROW_HR_ON_NULL(pszFolder, hr, E_POINTER);

		DWORD dwLowerFolderSize = (DWORD) strlen(pszFolder) + 1;
		char* pszLowerFolder = (char*) alloca(dwLowerFolderSize);
		strcpy_s(pszLowerFolder, dwLowerFolderSize, pszFolder);
		_strlwr_s(pszLowerFolder, dwLowerFolderSize);

		DWORD dwLowerInstanceSize = (DWORD) strlen(pszInstance) + 1;
		char* pszLowerInstance = (char*) alloca(dwLowerInstanceSize);
		strcpy_s(pszLowerInstance, dwLowerInstanceSize, pszInstance);
		_strlwr_s(pszLowerInstance, dwLowerInstanceSize);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		map<string, map<string, DWORD> >::const_iterator InstanceIter = m_NeverFolders.find(pszLowerInstance);
		if (InstanceIter != m_NeverFolders.end())
		{
			map<string, DWORD>::const_iterator FolderIter = InstanceIter->second.find(pszLowerFolder);
			if (FolderIter != InstanceIter->second.end())
			{			
				hr = S_FALSE;
				throw hr;
			}
		}
		
		m_NeverFolders[pszLowerInstance][pszLowerFolder] = dwFlags;

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CComPtr<IXMLDOMNode> pFolderRuleNode;
		hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"folderrule"), CComBSTR(), &pFolderRuleNode);
		THROW_HR_ON_FAIL(hr);

		hr = pFolderRuleNode->put_text(CComBSTR(pszLowerFolder));
		THROW_HR_ON_FAIL(hr);

		CComPtr<IXMLDOMAttribute> pInstanceAttrib;
		hr = m_pDOM->createAttribute(CComBSTR(L"instance"), &pInstanceAttrib);
		
		hr = pInstanceAttrib->put_text(CComBSTR(pszLowerInstance));
		THROW_HR_ON_FAIL(hr);

		CComPtr<IXMLDOMAttribute> pFlagsAttrib;
		hr = m_pDOM->createAttribute(CComBSTR(L"flags"), &pFlagsAttrib);
		
		CStringW sFlags;
		sFlags.Format(L"%u", dwFlags);
		hr = pFlagsAttrib->put_text(CComBSTR(sFlags));
		THROW_HR_ON_FAIL(hr);

		CComPtr<IXMLDOMNamedNodeMap> pAttribs;
		hr = pFolderRuleNode->get_attributes(&pAttribs);
		THROW_HR_ON_FAIL(hr);

		hr = pAttribs->setNamedItem(pInstanceAttrib, NULL);
		THROW_HR_ON_FAIL(hr);

		hr = pAttribs->setNamedItem(pFlagsAttrib, NULL);
		THROW_HR_ON_FAIL(hr);

		CComPtr<IXMLDOMNode> pTemp;
		hr = pNeverRulesNode->appendChild(pFolderRuleNode, &pTemp);
		THROW_HR_ON_FAIL(hr);

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintURI, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::AddIPRule(LPCSTR pszIPAddress, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszIPAddress, hr, E_POINTER);						

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);		

		if (m_NeverIPAddresses.find(pszIPAddress) != m_NeverIPAddresses.end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/iprule[text()='%hs']"), pszIPAddress);

		CComPtr<IXMLDOMNode> pIPRuleNode;
		m_pDOM->selectSingleNode(CComBSTR(sQuery), &pIPRuleNode);

		if (pIPRuleNode != NULL)
		{
			hr = S_FALSE;
			throw hr;
		}
		else
		{
			CComPtr<IXMLDOMNode> pIPRuleNode;
			hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"iprule"), CComBSTR(), &pIPRuleNode);
			THROW_HR_ON_FAIL(hr);

			hr = pIPRuleNode->put_text(CComBSTR(pszIPAddress));
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pTemp;
			hr = pNeverRulesNode->appendChild(pIPRuleNode, &pTemp);
			THROW_HR_ON_FAIL(hr);

			pIPRuleNode = pTemp;
		}

		m_NeverIPAddresses.insert(pszIPAddress);

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintIP, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::RemoveExtensionRule(LPCSTR pszExtension, LPCSTR pszUserAgent, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszExtension, hr, E_POINTER);
		THROW_HR_ON_NULL(pszUserAgent, hr, E_POINTER);		

		// skip the '.' if present
		if (pszExtension[0] == '.')
			pszExtension++;

		DWORD dwLowerExtensionSize = (DWORD) strlen(pszExtension) + 1;
		char* pszLowerExtension = (char*) alloca(dwLowerExtensionSize);
		strcpy_s(pszLowerExtension, dwLowerExtensionSize, pszExtension);
		_strlwr_s(pszLowerExtension, dwLowerExtensionSize);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		if (m_NeverExtensions.find(pszLowerExtension) == m_NeverExtensions.end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/extnrule[text()='%hs']"), pszLowerExtension);

		CComPtr<IXMLDOMNode> pExtnRuleNode;
		m_pDOM->selectSingleNode(CComBSTR(sQuery), &pExtnRuleNode);

		if (pExtnRuleNode == NULL)
		{
			hr = S_FALSE;
			throw hr;
		}
		else
		{
			pNeverRulesNode->removeChild(pExtnRuleNode, NULL);
		}

		m_NeverExtensions.erase(pszLowerExtension);

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintExtension, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::RemoveContentTypeRule(LPCSTR pszContentType, LPCSTR pszUserAgent, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszContentType, hr, E_POINTER);
		THROW_HR_ON_NULL(pszUserAgent, hr, E_POINTER);		

		DWORD dwLowerContentTypeSize = (DWORD) strlen(pszContentType);
		if (dwLowerContentTypeSize == 0)
		{
			hr = E_INVALIDARG;
			throw hr;
		}

		char* pszLowerContentType = (char*) alloca(dwLowerContentTypeSize + 1);
		strcpy_s(pszLowerContentType, dwLowerContentTypeSize + 1, pszContentType);
		_strlwr_s(pszLowerContentType, dwLowerContentTypeSize + 1);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		map<string, RuleUserAgents>* pContentTypeRules = NULL;

		// decide which rule collection to deal with
		if (pszLowerContentType[dwLowerContentTypeSize - 1] != '*')
		{
			pContentTypeRules = &m_NeverContentTypes;
		}
		else
		{
			pContentTypeRules = &m_NeverContentTypeClass;
		}

		if (pContentTypeRules->find(pszLowerContentType) == pContentTypeRules->end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/contenttyperule[text()='%hs']"), pszLowerContentType);

		CComPtr<IXMLDOMNode> pContentTypeRuleNode;
		m_pDOM->selectSingleNode(CComBSTR(sQuery), &pContentTypeRuleNode);

		if (pContentTypeRuleNode == NULL)
		{
			hr = S_FALSE;
			throw hr;
		}
		else
		{
			pNeverRulesNode->removeChild(pContentTypeRuleNode, NULL);
		}
		
		pContentTypeRules->erase(pszLowerContentType);		

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintContentType, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::RemoveFolderRule(LPCSTR pszInstance, LPCSTR pszFolder, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszInstance, hr, E_POINTER);	
		THROW_HR_ON_NULL(pszFolder, hr, E_POINTER);	

		DWORD dwLowerFolder = (DWORD) strlen(pszFolder) + 1;
		char* pszLowerFolder = (char*) alloca(dwLowerFolder);
		strcpy_s(pszLowerFolder, dwLowerFolder, pszFolder);
		_strlwr_s(pszLowerFolder, dwLowerFolder);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		map<string, map<string, DWORD> >::iterator iter = m_NeverFolders.find(pszInstance);
		if (iter == m_NeverFolders.end() || iter->second.find(pszLowerFolder) == iter->second.end())
		{			
			hr = S_FALSE;
			throw hr;
		}

		iter->second.erase(pszLowerFolder);

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/folderrule[@instance='%hs' and text()='%hs']"), pszInstance, pszLowerFolder);

		CComPtr<IXMLDOMNode> pFolderRuleNode;
		hr = pNeverRulesNode->selectSingleNode(CComBSTR(sQuery), &pFolderRuleNode);
		THROW_HR_ON_FAIL(hr);

		if (pFolderRuleNode != NULL)
		{
			pNeverRulesNode->removeChild(pFolderRuleNode, NULL);
		}		

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintURI, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::RemoveIPRule(LPCSTR pszIPAddress, DWORD dwUpdateCookie)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszIPAddress, hr, E_POINTER);					

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		set<string>::iterator iter = m_NeverIPAddresses.find(pszIPAddress);
		if (iter == m_NeverIPAddresses.end())
		{			
			hr = S_FALSE;
			throw hr;
		}		

		// remove the IP address from the collection
		m_NeverIPAddresses.erase(pszIPAddress);

		CComPtr<IXMLDOMNode> pNeverRulesNode;
		hr = m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pNeverRulesNode);
		THROW_HR_ON_FAIL(hr);

		CString sQuery;
		sQuery.Format(_T("/neverrules/iprule[text()='%hs']"), pszIPAddress);

		CComPtr<IXMLDOMNode> pIPRuleNode;
		hr = pNeverRulesNode->selectSingleNode(CComBSTR(sQuery), &pIPRuleNode);
		THROW_HR_ON_FAIL(hr);

		if (pIPRuleNode != NULL)
		{
			hr = pNeverRulesNode->removeChild(pIPRuleNode, NULL);
			THROW_HR_ON_FAIL(hr);
		}		

		hr = Save();
		THROW_HR_ON_FAIL(hr);

		Fire_OnRuleChanged(NeverRuleChangeHintIP, dwUpdateCookie);
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::MatchRule(LPCSTR pszExtension, 
												LPCSTR pszContentType, 
												LPCSTR pszInstance, LPCSTR pszFolder, 
												LPCSTR pszUserAgent,
												LPCSTR pszIPAddress)
{
	HRESULT hr = S_FALSE;

	try
	{
		// lock the config for the duration of the call
		AutoMemberCriticalSection AMCS_Config(&m_csConfig);

		// if the caller has supplied the extension then they want us to compare it against the rules
		if (pszExtension != NULL)
		{
			if (pszExtension[0] == '.')
				pszExtension++;

			DWORD dwLowerExtensionSize = (DWORD) strlen(pszExtension) + 1;
			char* pszLowerExtension = (char*) alloca(dwLowerExtensionSize);
			strcpy_s(pszLowerExtension, dwLowerExtensionSize, pszExtension);
			_strlwr_s(pszLowerExtension, dwLowerExtensionSize);

			map<string, RuleUserAgents>::const_iterator iter = m_NeverExtensions.find(pszLowerExtension);
			if (iter != m_NeverExtensions.end())
			{		
				// don't support user agents at the moment
				/*if (pszUserAgent != NULL)
				{
					UserAgentProducts Products;
					Products.ParseUserAgentString(pszUserAgent);

					if (iter->second.Compare(Products) == true)
					{
						hr = S_OK;
					}
				}
				else*/
				{
					hr = S_OK;
				}
			}
		}

		// if the caller has supplied the content type then they want us to compare it against the rules
		// (unless the extension has already matched above in which case don't do anything)
		if (hr != S_OK && pszContentType != NULL)
		{			
			// copy the content string into a buffer so we can create a lower case version
			DWORD dwLowerContentTypeSize = (DWORD) strlen(pszContentType) + 1;
			char* pszLowerContentType = (char*) alloca(dwLowerContentTypeSize);
			strcpy_s(pszLowerContentType, dwLowerContentTypeSize, pszContentType);

			// remove any characters after the semi-colon if there is one
			char* pszDelim = strchr(pszLowerContentType, ';');
			if (pszDelim != NULL)
				*pszDelim = '\0';

			// now convert to lower case
			_strlwr_s(pszLowerContentType, dwLowerContentTypeSize);

			// look for an exact match
			map<string, RuleUserAgents>::const_iterator iter = m_NeverContentTypes.find(pszLowerContentType);
			if (iter != m_NeverContentTypes.end())
			{		
				// don't support user agents at the moment
				/*if (pszUserAgent != NULL)
				{
					UserAgentProducts Products;
					Products.ParseUserAgentString(pszUserAgent);

					if (iter->second.Compare(Products) == true)
					{
						hr = S_OK;
					}
				}
				else*/
				{
					// we've got a match
					hr = S_OK;
				}
			}

			// if we haven't matched yet
			if (hr != S_OK)
			{
				// look for the delimiter
				char* pszDelim = strchr(pszLowerContentType, '/');
				if (pszDelim != NULL && pszDelim[1] != '\0')
				{
					// we need to insert the * after the / delimiter
					pszDelim[1] = '*';
					pszDelim[2] = '\0';

					// look for the wildcard match
					map<string, RuleUserAgents>::const_iterator iter = m_NeverContentTypeClass.find(pszLowerContentType);
					if (iter != m_NeverContentTypeClass.end())
					{
						hr = S_OK;
					}
				}
			}
		}

		if (hr != S_OK)
		{
			hr = MatchFolderRule(pszInstance, pszFolder);
		}

		if (hr != S_OK && pszIPAddress != NULL)
		{			
			// get the length of the incoming string
			int nIPAddressLength = (int) strlen(pszIPAddress);
			if (nIPAddressLength > 0)
			{
				// allocate space for the IP address
				char* pszTemp = (char*) alloca(nIPAddressLength + 1);
				if (pszTemp != NULL)
				{
					// copy the address to a temp
					memcpy(pszTemp, pszIPAddress, nIPAddressLength + 1);

					// loop thru the string (from the end to the beginning)
					while (nIPAddressLength > 0)
					{
						// attempt to find the address in the exclusion list
						if (m_NeverIPAddresses.find(pszTemp) != m_NeverIPAddresses.end())
						{
							hr = S_OK;
							break;
						}
			
						// look for the next '.' seperator
						while (nIPAddressLength > 0 && pszTemp[nIPAddressLength] != '.')
							nIPAddressLength--;

						// get rid of the '.'
						pszTemp[nIPAddressLength] = '\0';
					}
				}
			}
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::MatchFolderRule(LPCSTR pszInstance, LPCSTR pszFolder)
{
	if (pszInstance == NULL && pszFolder == NULL)
		return E_POINTER;

	HRESULT hr = S_FALSE;
	
	DWORD dwLowerFolderSize = (DWORD) strlen(pszFolder) + 1;
	char* pszLowerFolder = (char*) alloca(dwLowerFolderSize);
	strcpy_s(pszLowerFolder, dwLowerFolderSize, pszFolder);
	_strlwr_s(pszLowerFolder, dwLowerFolderSize);
	
	DWORD dwLowerInstanceSize = (DWORD) strlen(pszInstance) + 1;
	char* pszLowerInstance = (char*) alloca(dwLowerInstanceSize);
	strcpy_s(pszLowerInstance, dwLowerInstanceSize, pszInstance);
	_strlwr_s(pszLowerInstance, dwLowerInstanceSize);

	map<string, map<string, DWORD> >::const_iterator InstanceIter = m_NeverFolders.find(pszLowerInstance);
	if (InstanceIter != m_NeverFolders.end())
	{	
		map<string, DWORD>::const_iterator FolderIter = InstanceIter->second.begin();
		while (FolderIter != InstanceIter->second.end())
		{								
			CStringA sFolderRule('\0', 512);
			sFolderRule = FolderIter->first.c_str();

			// make sure the last rule character is a '/'
			int nLength = sFolderRule.GetLength();
			if (nLength > 0 && sFolderRule[nLength - 1] != '/')
				sFolderRule += '/';

			DWORD dwMatchType = FolderIter->second;

			if (dwMatchType == RULEFLAGS_FOLDER_WILDCARDMATCH)
			{
				// match the strings
				int nMatch = strncmp(sFolderRule, pszLowerFolder, sFolderRule.GetLength());
				if (nMatch == 0)
				{
					hr = S_OK;
					break;
				}
			}
			else if (dwMatchType == RULEFLAGS_FOLDER_EXACTMATCH)
			{
				// match the strings
				int nMatch = strncmp(sFolderRule, pszLowerFolder, sFolderRule.GetLength());
				if (nMatch == 0)
				{
					nLength = sFolderRule.GetLength();

					if (strchr(pszLowerFolder + nLength, '/') == NULL)
					{
						hr = S_OK;
						break;
					}
				}
			}

			FolderIter++;
		}
	}

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::EnumExtensions(IEnumExclusionRule** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(ppEnum, hr, E_POINTER);

		*ppEnum = NULL;

		CComObject<CEnumExclusionRule>* pEnumExclusionRule = NULL;
		hr = CComObject<CEnumExclusionRule>::CreateInstance(&pEnumExclusionRule);
		THROW_HR_ON_FAIL(hr);

		pEnumExclusionRule->AddRef();

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		int nExtensions = (int) m_NeverExtensions.size();
		if (nExtensions > 0)
		{
			map<string, RuleUserAgents>::const_iterator iter = m_NeverExtensions.begin();
			while (iter != m_NeverExtensions.end())
			{
				pEnumExclusionRule->AddRule(CComBSTR(iter->first.c_str()), 0, 0, NULL, 0);	
				iter++;
			}
		}

		*ppEnum = pEnumExclusionRule;
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::EnumContentTypes(IEnumExclusionRule** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(ppEnum, hr, E_POINTER);

		*ppEnum = NULL;

		CComObject<CEnumExclusionRule>* pEnumExclusionRule = NULL;
		hr = CComObject<CEnumExclusionRule>::CreateInstance(&pEnumExclusionRule);
		THROW_HR_ON_FAIL(hr);

		pEnumExclusionRule->AddRef();

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		int nContentTypes = (int) m_NeverContentTypes.size();
		if (nContentTypes > 0)
		{
			map<string, RuleUserAgents>::const_iterator iter = m_NeverContentTypes.begin();
			while (iter != m_NeverContentTypes.end())
			{
				pEnumExclusionRule->AddRule(CComBSTR(iter->first.c_str()), 0, 0, NULL, 0);	
				iter++;
			}
		}

		nContentTypes = (int) m_NeverContentTypeClass.size();
		if (nContentTypes > 0)
		{
			map<string, RuleUserAgents>::const_iterator iter = m_NeverContentTypeClass.begin();
			while (iter != m_NeverContentTypeClass.end())
			{
				pEnumExclusionRule->AddRule(CComBSTR(iter->first.c_str()), 0, 0, NULL, 0);	
				iter++;
			}
		}

		*ppEnum = pEnumExclusionRule;
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::EnumFolders(IEnumExclusionRule** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(ppEnum, hr, E_POINTER);

		*ppEnum = NULL;

		CComObject<CEnumExclusionRule>* pEnumExclusionRule = NULL;
		hr = CComObject<CEnumExclusionRule>::CreateInstance(&pEnumExclusionRule);
		THROW_HR_ON_FAIL(hr);

		pEnumExclusionRule->AddRef();

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		map<string, map<string, DWORD> >::const_iterator InstanceIter = m_NeverFolders.begin();
		while (InstanceIter != m_NeverFolders.end())
		{
			map<string, DWORD>::const_iterator FolderIter = InstanceIter->second.begin();
			while (FolderIter != InstanceIter->second.end())
			{
				pEnumExclusionRule->AddRule(
					CComBSTR(FolderIter->first.c_str()), 
					CComBSTR(InstanceIter->first.c_str()), 
					FolderIter->second, 0, NULL);

				FolderIter++;
			}

			InstanceIter++;
		}		

		*ppEnum = pEnumExclusionRule;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::EnumIPAddresses(IEnumExclusionRule** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(ppEnum, hr, E_POINTER);

		*ppEnum = NULL;

		CComObject<CEnumExclusionRule>* pEnumExclusionRule = NULL;
		hr = CComObject<CEnumExclusionRule>::CreateInstance(&pEnumExclusionRule);
		THROW_HR_ON_FAIL(hr);

		pEnumExclusionRule->AddRef();

		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);		

		set<string>::const_iterator iter = m_NeverIPAddresses.begin();
		while (iter != m_NeverIPAddresses.end())
		{		
			pEnumExclusionRule->AddRule(
					CComBSTR((*iter).c_str()), 
					CComBSTR(L""), 0, 
					0, NULL);

			iter++;
		}		

		*ppEnum = pEnumExclusionRule;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::Load(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pConfigStorage, hr, E_FAIL);
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);		

		CComPtr<IStream> pStream;
		hr = m_pConfigStorage->OpenStream(CComBSTR(IISXPRESS_NEVERRULES), NULL, 
			STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, &pStream);

		if (hr != S_OK)
		{
			hr = m_pConfigStorage->CreateStream(CComBSTR(IISXPRESS_NEVERRULES), 
				STGM_CREATE | STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pStream);

			// if the stream has been created then commit the parent storage item to make sure
			// it won't be lost if there is an unusual termination
			if (pStream != NULL)
			{	
				m_pConfigStorage->Commit(STGC_DEFAULT);
			}
		}

		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{		
			STATSTG Stat;
			pStream->Stat(&Stat, STATFLAG_NONAME);

			// only attempt to parse if the stream has some data in it
			if (Stat.cbSize.LowPart > 0)
			{
				VARIANT_BOOL vbSuccess;
				hr = m_pDOM->load(CComVariant(pStream), &vbSuccess);
				THROW_HR_ON_FAIL(hr);					

				// if the stream failed to validate as XML we need to dump the current DOM and
				// create a new one
				if (hr != S_OK)
				{
					m_pDOM.Release();

					hr = m_pDOM.CoCreateInstance(L"MSXML.DOMDocument");
					THROW_HR_ON_FAIL(hr);
					hr = S_FALSE;
				}
			}

			// if the stream was zero length or failed to parse then we need to make sure we
			// have a processing instruction in the document otherwise it won't save later (yes
			// really!)
			if (Stat.cbSize.LowPart == 0 || hr != S_OK)
			{
				CComPtr<IXMLDOMNode> pProcessingInstruction;
				hr = m_pDOM->createNode(CComVariant(NODE_PROCESSING_INSTRUCTION), CComBSTR(L"xml"), CComBSTR(), &pProcessingInstruction);			
				THROW_HR_ON_FAIL(hr);

				hr = m_pDOM->appendChild(pProcessingInstruction, NULL);
				THROW_HR_ON_FAIL(hr);
			}			

			// make sure there is a 'neverrules' node
			CComPtr<IXMLDOMNode> pFilesNode;
			m_pDOM->selectSingleNode(CComBSTR(L"/neverrules"), &pFilesNode);
			if (pFilesNode == NULL)
			{
				hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"neverrules"), CComBSTR(), &pFilesNode);
				THROW_HR_ON_FAIL(hr);

				hr = m_pDOM->appendChild(pFilesNode, NULL);
				THROW_HR_ON_FAIL(hr);				
			}

			// lock the config for the duration of the call
			AutoMemberCriticalSection AMCS_Config(&m_csConfig);

			LoadNeverExtensions();
			LoadNeverContentTypes();
			LoadNeverFolders();
			LoadNeverIPAddresses();
		}			
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::Save(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_POINTER);		
		THROW_HR_ON_NULL(m_pConfigStorage, hr, E_POINTER);

		CComPtr<IStream> pStream;
		hr = m_pConfigStorage->OpenStream(CComBSTR(IISXPRESS_NEVERRULES), NULL, 
			STGM_WRITE | STGM_SHARE_EXCLUSIVE, NULL, &pStream);
		THROW_HR_ON_FAIL(hr);

		ULARGE_INTEGER nNewSize = { 0, 0 };
		pStream->SetSize(nNewSize);

		hr = m_pDOM->save(CComVariant(pStream));		
		THROW_HR_ON_FAIL(hr);

		hr = pStream->Commit(STGC_DEFAULT);
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::GetProductFromNode(IXMLDOMNode* pProductNode, UserAgentProduct& Product)
{
	HRESULT hr = E_FAIL;

	try
	{
		Product.clear();		

		THROW_HR_ON_NULL(pProductNode, hr, E_POINTER);

		CComPtr<IXMLDOMNamedNodeMap> pAttribs;
		pProductNode->get_attributes(&pAttribs);
		if (pAttribs != NULL)
		{
			CComPtr<IXMLDOMNode> pOpNode;
			pAttribs->getNamedItem(CComBSTR(L"op"), &pOpNode);

			if (pOpNode != NULL)
			{
				CComBSTR bsOp;
				pOpNode->get_text(&bsOp);

				if (bsOp == L"not")
				{
					Product.bNot = true;
				}
			}
		}

		CComPtr<IXMLDOMNode> pProductNodeText;
		hr = pProductNode->selectSingleNode(CComBSTR(L"text()"), &pProductNodeText);
		THROW_HR_ON_FAIL(hr);
		THROW_HR_ON_NULL(pProductNodeText, hr, E_FAIL);

		CComBSTR bsName;
		hr = pProductNodeText->get_text(&bsName);
		THROW_HR_ON_FAIL(hr);

		Product.sName = CStringA(bsName);

		CComPtr<IXMLDOMNodeList> pCommentNodes;
		hr = pProductNode->selectNodes(CComBSTR(L"comment"), &pCommentNodes);
		THROW_HR_ON_FAIL(hr);

		long nCommentNodes = 0;
		pCommentNodes->get_length(&nCommentNodes);

		for (long i = 0; i < nCommentNodes; i++)
		{
			CComPtr<IXMLDOMNode> pCommentNode;
			pCommentNodes->get_item(i, &pCommentNode);

			if (pCommentNode == NULL)
				continue;

			CComBSTR bsComment;
			hr = pCommentNode->get_text(&bsComment);
			THROW_HR_ON_FAIL(hr);

			Product.Comments.push_back((const char*) CStringA(bsComment));
		}

		hr = S_OK;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::GetUserAgentProducts(IXMLDOMNode* pNode, UserAgentProducts& Products)
{
	HRESULT hr = E_FAIL;

	try
	{
		Products.clear();

		THROW_HR_ON_NULL(pNode, hr, E_POINTER);

		CComPtr<IXMLDOMNode> pRootProductNode;
		pNode->selectSingleNode(CComBSTR(L"rootproduct"), &pRootProductNode);
		if (pRootProductNode == NULL)
			return S_FALSE;
		
		if (GetProductFromNode(pRootProductNode, Products.RootProduct) != S_OK)
		{
			hr = E_FAIL;
			throw hr;
		}

		CComPtr<IXMLDOMNodeList> pProductNodes;
		pNode->selectNodes(CComBSTR(L"product"), &pProductNodes);

		long nProductNodes = 0;
		pProductNodes->get_length(&nProductNodes);

		for (long j = 0; j < nProductNodes; j++)
		{
			CComPtr<IXMLDOMNode> pProductNode;
			pProductNodes->get_item(j, &pProductNode);

			if (pProductNode == NULL)
				continue;

			UserAgentProduct Product;
			if (GetProductFromNode(pProductNode, Product) == S_OK)
				Products.Products.push_back(Product);
		}			

		hr = S_OK;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::LoadNeverExtensions(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		// make sure we are starting from a clean sheet
		m_NeverExtensions.clear();

		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		// get all the extension rules
		CComPtr<IXMLDOMNodeList> pExtensionRules;
		hr = m_pDOM->selectNodes(CComBSTR(L"/neverrules/extnrule"), &pExtensionRules);
		THROW_HR_ON_FAIL(hr);

		if (hr != S_OK)
			throw hr;

		long nExtnRules = 0;
		pExtensionRules->get_length(&nExtnRules);

		// loop thru the extension rule nodes
		for (long i = 0; i < nExtnRules; i++)
		{
			CComPtr<IXMLDOMNode> pExtnRuleNode;
			pExtensionRules->get_item(i, &pExtnRuleNode);

			if (pExtnRuleNode == NULL)
				continue;

			// get the extension text node
			CComPtr<IXMLDOMNode> pExtnRuleTextNode;
			hr = pExtnRuleNode->selectSingleNode(CComBSTR(L"text()"), &pExtnRuleTextNode);
			THROW_HR_ON_FAIL(hr);

			if (pExtnRuleTextNode == NULL)
				continue;
			
			CComBSTR bsExtn;
			pExtnRuleTextNode->get_text(&bsExtn);

			string sExtn = (const char*) CStringA(bsExtn);

			// don't support user agents at the moment!
			/*CComPtr<IXMLDOMNodeList> pUserAgentNodes;
			pExtnRuleNode->selectNodes(CComBSTR(L"useragent"), &pUserAgentNodes);			

			// get the number of useragent nodes if there are any
			long nUserAgentNodes = 0;
			if (pUserAgentNodes != NULL)
			{
				pUserAgentNodes->get_length(&nUserAgentNodes);
			}

			// if there are useragent nodes then we need to compare the product info
			if (nUserAgentNodes > 0)
			{				
				// loop thru the nodes
				for (int j = 0; j < nUserAgentNodes; j++)
				{
					CComPtr<IXMLDOMNode> pUserAgentNode;
					pUserAgentNodes->get_item(j, &pUserAgentNode);

					if (pUserAgentNode == NULL)
						continue;

					// get the attribs of the node
					CComPtr<IXMLDOMNamedNodeMap> pAttribs;
					pUserAgentNode->get_attributes(&pAttribs);

					if (pAttribs != NULL)
					{
						// get the id so we can look it up
						CComPtr<IXMLDOMNode> pUserAgentNodeId;
						pAttribs->getNamedItem(CComBSTR(L"id"), &pUserAgentNodeId);

						if (pUserAgentNodeId != NULL)
						{
							// get the id text
							CComBSTR bsId;
							pUserAgentNodeId->get_text(&bsId);

							// form the query we need to find the product info
							CStringW sQuery;
							sQuery.Format(L"/neverrules/useragents/useragent[@id='%s']", bsId);							
	
							// get the useragent node from the lookup
							CComPtr<IXMLDOMNode> pLookupUserAgentNode;
							m_pDOM->selectSingleNode(CComBSTR(sQuery), &pLookupUserAgentNode);

							if (pLookupUserAgentNode != NULL)
							{
								// add the useragent to the collection
								UserAgentProducts UAProducts;
								if (GetUserAgentProducts(pLookupUserAgentNode, UAProducts) == S_OK)
								{
									m_NeverExtensions[sExtn].UAProducts.push_back(UAProducts);
								}
							}
						}
					}
				}
			}
			else*/
			{		
				UserAgentProducts UAProducts;
				m_NeverExtensions[sExtn].UAProducts.push_back(UAProducts);
			}
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::LoadNeverContentTypes(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		m_NeverContentTypes.clear();
		m_NeverContentTypeClass.clear();

		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CComPtr<IXMLDOMNodeList> pContentTypeRules;
		hr = m_pDOM->selectNodes(CComBSTR(L"/neverrules/contenttyperule"), &pContentTypeRules);
		THROW_HR_ON_FAIL(hr);

		if (hr != S_OK)
			throw hr;

		long nContentTypeRules = 0;
		pContentTypeRules->get_length(&nContentTypeRules);

		for (long i = 0; i < nContentTypeRules; i++)
		{
			CComPtr<IXMLDOMNode> pContentTypeRuleNode;
			pContentTypeRules->get_item(i, &pContentTypeRuleNode);

			if (pContentTypeRuleNode == NULL)
				continue;

			CComPtr<IXMLDOMNode> pContentTypeRuleTextNode;
			hr = pContentTypeRuleNode->selectSingleNode(CComBSTR(L"text()"), &pContentTypeRuleTextNode);
			THROW_HR_ON_FAIL(hr);

			if (pContentTypeRuleTextNode == NULL)
				continue;
			
			CComBSTR bsContentType;
			pContentTypeRuleTextNode->get_text(&bsContentType);

			if (bsContentType.Length() == 0)
				continue;

			string sContentType = (const char*) CStringA(bsContentType);

			// don't support user agents at the moment!
			/*CComPtr<IXMLDOMNodeList> pUserAgentNodes;
			pContentTypeRuleNode->selectNodes(CComBSTR(L"useragent"), &pUserAgentNodes);

			long nUserAgentNodes = 0;
			if (pUserAgentNodes != NULL)
			{
				pUserAgentNodes->get_length(&nUserAgentNodes);
			}

			if (nUserAgentNodes > 0)
			{
				for (int j = 0; j < nUserAgentNodes; j++)
				{
					CComPtr<IXMLDOMNode> pUserAgentNode;
					pUserAgentNodes->get_item(j, &pUserAgentNode);

					if (pUserAgentNode == NULL)
						continue;

					// get the attribs of the node
					CComPtr<IXMLDOMNamedNodeMap> pAttribs;
					pUserAgentNode->get_attributes(&pAttribs);

					if (pAttribs != NULL)
					{
						// get the id so we can look it up
						CComPtr<IXMLDOMNode> pUserAgentNodeId;
						pAttribs->getNamedItem(CComBSTR(L"id"), &pUserAgentNodeId);

						if (pUserAgentNodeId != NULL)
						{
							// get the id text
							CComBSTR bsId;
							pUserAgentNodeId->get_text(&bsId);

							// form the query we need to find the product info
							CStringW sQuery;
							sQuery.Format(L"/neverrules/useragents/useragent[@id='%s']", bsId);							
	
							// get the useragent node from the lookup
							CComPtr<IXMLDOMNode> pLookupUserAgentNode;
							m_pDOM->selectSingleNode(CComBSTR(sQuery), &pLookupUserAgentNode);

							if (pLookupUserAgentNode != NULL)
							{
								// add the useragent to the collection
								UserAgentProducts UAProducts;
								if (GetUserAgentProducts(pLookupUserAgentNode, UAProducts) == S_OK)
								{
									m_NeverContentTypes[sExtn].UAProducts.push_back(UAProducts);
								}
							}
						}
					}
				}
			}
			else*/
			{		
				UserAgentProducts UAProducts;

				if (sContentType[sContentType.size() - 1] != '*')
				{
					m_NeverContentTypes[sContentType].UAProducts.push_back(UAProducts);
				}
				else
				{
					m_NeverContentTypeClass[sContentType].UAProducts.push_back(UAProducts);
				}
			}
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::LoadNeverFolders(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		m_NeverFolders.clear();

		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CComPtr<IXMLDOMNodeList> pFolderRules;
		hr = m_pDOM->selectNodes(CComBSTR(L"/neverrules/folderrule"), &pFolderRules);
		THROW_HR_ON_FAIL(hr);

		if (hr != S_OK)
			throw hr;

		long nFolderRules = 0;
		pFolderRules->get_length(&nFolderRules);

		for (long i = 0; i < nFolderRules; i++)
		{
			CComPtr<IXMLDOMNode> pFolderRuleNode;
			pFolderRules->get_item(i, &pFolderRuleNode);

			if (pFolderRuleNode == NULL)
				continue;

			CComPtr<IXMLDOMNode> pFolderRuleTextNode;
			hr = pFolderRuleNode->selectSingleNode(CComBSTR(L"text()"), &pFolderRuleTextNode);
			THROW_HR_ON_FAIL(hr);

			if (pFolderRuleTextNode == NULL)
				continue;
			
			CComBSTR bsFolder;
			hr = pFolderRuleTextNode->get_text(&bsFolder);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFolderRuleNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pInstanceNode;
			hr = pAttribs->getNamedItem(CComBSTR(L"instance"), &pInstanceNode);
			THROW_HR_ON_FAIL(hr);

			CComBSTR bsInstance;
			hr = pInstanceNode->get_text(&bsInstance);
			THROW_HR_ON_FAIL(hr);			

			DWORD dwFlags = RULEFLAGS_FOLDER_WILDCARDMATCH;
			CComPtr<IXMLDOMNode> pFlagsNode;
			hr = pAttribs->getNamedItem(CComBSTR(L"flags"), &pFlagsNode);
			if (hr == S_OK && pFlagsNode != NULL)
			{
				CComBSTR bsFlags;
				hr = pFlagsNode->get_text(&bsFlags);
				THROW_HR_ON_FAIL(hr);

				dwFlags = _wtoi(bsFlags);				
			}
			else
			{
				hr = S_OK;
			}

			string sTempInstance = CStringA(bsInstance);
			string sTempFolder = CStringA(bsFolder);

			m_NeverFolders[sTempInstance][sTempFolder] = dwFlags;
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT INeverCompressRulesImpl::LoadNeverIPAddresses(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		// make sure we are starting from a clean sheet
		m_NeverIPAddresses.clear();

		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		// get all the ip rules
		CComPtr<IXMLDOMNodeList> pIPRules;
		hr = m_pDOM->selectNodes(CComBSTR(L"/neverrules/iprule"), &pIPRules);
		THROW_HR_ON_FAIL(hr);

		if (hr != S_OK)
			throw hr;

		long nIPRules = 0;
		pIPRules->get_length(&nIPRules);

		// loop thru the ip rule nodes
		for (long i = 0; i < nIPRules; i++)
		{
			CComPtr<IXMLDOMNode> pIPRuleNode;
			pIPRules->get_item(i, &pIPRuleNode);

			if (pIPRuleNode == NULL)
				continue;

			// get the ip text node
			CComPtr<IXMLDOMNode> pIPRuleTextNode;
			hr = pIPRuleNode->selectSingleNode(CComBSTR(L"text()"), &pIPRuleTextNode);
			THROW_HR_ON_FAIL(hr);

			if (pIPRuleTextNode == NULL)
				continue;
			
			CComBSTR bsIP;
			pIPRuleTextNode->get_text(&bsIP);

			string sIP = (const char*) CStringA(bsIP);	

			m_NeverIPAddresses.insert(sIP);
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP INeverCompressRulesImpl::Fire_OnRuleChanged(NeverRuleChangeHint hint, DWORD dwUpdateCookie)
{
	return E_NOTIMPL;
}