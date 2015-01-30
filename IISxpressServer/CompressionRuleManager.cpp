// CompressionRuleManager.cpp : Implementation of CCompressionRuleManager

#include "stdafx.h"
#include "CompressionRuleManager.h"
#include "RegistryKeys.h"
#include "EnumStaticFileInfo.h"

#include <atlpath.h>

// CCompressionRuleManager

HRESULT CCompressionRuleManager::FinalConstruct()
{	
	return S_OK;
}

void CCompressionRuleManager::FinalRelease() 
{
}

STDMETHODIMP CCompressionRuleManager::Init(IStorage* pStorage, IComIISxpressRegSettings* pRegSettings)
{
	HRESULT hr = E_FAIL;	

	try
	{
		THROW_HR_ON_NULL(pStorage, hr, E_POINTER);

		CComPtr<IStorage> pRulesStorage;
		hr = pStorage->OpenStorage(CComBSTR(L"Rules"), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pRulesStorage);
		if (hr != S_OK || pRulesStorage == NULL)
		{
			hr = pStorage->CreateStorage(CComBSTR(L"Rules"), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pRulesStorage);

			if (hr == S_OK)
			{
				pStorage->Commit(STGC_DEFAULT);
			}
		}

		THROW_HR_ON_FAIL(hr);

		hr = INeverCompressRulesImpl::Init(pRulesStorage);
		THROW_HR_ON_FAIL(hr);

		hr = IStaticFileRulesImpl::Init(pRulesStorage);
		THROW_HR_ON_FAIL(hr);

		CComPtr<IStorage> pHistoryStorage;
		hr = pStorage->OpenStorage(CComBSTR(L"History"), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pHistoryStorage);
		if (hr != S_OK || pHistoryStorage == NULL)
		{
			hr = pStorage->CreateStorage(CComBSTR(L"History"), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pHistoryStorage);

			if (hr == S_OK)
			{
				pStorage->Commit(STGC_DEFAULT);
			}
		}

		THROW_HR_ON_FAIL(hr);

		hr = ICompressionHistoryImpl::Init(pHistoryStorage);
		THROW_HR_ON_FAIL(hr);				

		// upgrade the never rules if there is an upgrade file present
		UpgradeNeverRules(this);

		// open or create the Settings stream
		CComPtr<IStream> pSettingsStream;
		hr = pStorage->OpenStream(CComBSTR(L"Settings"), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, &pSettingsStream);
		if (hr != S_OK || pSettingsStream == NULL)
		{
			hr = pStorage->CreateStream(CComBSTR(L"Settings"), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pSettingsStream);

			// commit the root storage item
			if (hr == S_OK)
			{
				pStorage->Commit(STGC_DEFAULT);
			}
		}

		THROW_HR_ON_FAIL(hr);

		hr = IIISxpressSettingsImpl::Init(pSettingsStream);
	}
	catch (HRESULT)
	{
	}

	return S_OK;
}

STDMETHODIMP CCompressionRuleManager::CanCompressFile(LPCSTR pszPath, LPCSTR pszFilename, DWORD dwFileSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCompressionRuleManager::UpgradeNeverRules(INeverCompressRules* pNeverRules)
{
	if (pNeverRules == NULL)
		return E_POINTER;

	// use the module filename as the basis for the path to the upgrade file
	TCHAR szModuleFileName[512];
	::GetModuleFileName(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(szModuleFileName[0]));

	// make the upgrade filename
	CPathT<CString> UpgradeFilePath(szModuleFileName);
	UpgradeFilePath.RemoveFileSpec();
	UpgradeFilePath.Append(_T("Rules.NeverRules.Upgrade2.xml"));	

	// get the file's attributes, if we fail then don't continue
	WIN32_FILE_ATTRIBUTE_DATA Attribs;
	if (::GetFileAttributesEx(UpgradeFilePath, GetFileExInfoStandard, &Attribs) == FALSE)
		return S_FALSE;

	// if the file is zero length then it's already been processed, so don't continue
	if (Attribs.nFileSizeLow == 0)
		return S_FALSE;

	// we need a DOM
	CComPtr<IXMLDOMDocument> pDOM;
	HRESULT hr = pDOM.CoCreateInstance(L"MSXML.DOMDocument");
	if (hr != S_OK)
		return hr;

	// load the upgrade XML file
	VARIANT_BOOL vbSuccess = 0;
	hr = pDOM->load(CComVariant((LPCTSTR) UpgradeFilePath), &vbSuccess);
	if (hr != S_OK)
		return hr;

	// find the upgrade/add nodes
	CComPtr<IXMLDOMNodeList> pUpgradeNodes;
	hr = pDOM->selectNodes(L"/upgrade/add/*", &pUpgradeNodes);
	if (hr == S_OK)
	{
		long nUpgradeNodes = 0;
		hr = pUpgradeNodes->get_length(&nUpgradeNodes);

		// loop through the nodes to add
		for (int i = 0; i < nUpgradeNodes; i++)
		{
			CComPtr<IXMLDOMNode> pUpgradeNode;
			hr = pUpgradeNodes->get_item(i, &pUpgradeNode);
			if (pUpgradeNode == NULL)
				continue;

			CComBSTR bsNodeName;
			hr = pUpgradeNode->get_nodeName(&bsNodeName);
			if (hr != S_OK)
				continue;

			// we're only supporting extnrule at the moment
			if (bsNodeName == L"extnrule")
			{
				CComPtr<IXMLDOMNode> pRuleTextNode;
				hr = pUpgradeNode->selectSingleNode(L"text()", &pRuleTextNode);

				if (pRuleTextNode != NULL)
				{
					CComBSTR bsExtn;
					hr = pRuleTextNode->get_text(&bsExtn);

					if (hr == S_OK)
					{
						hr = pNeverRules->AddExtensionRule(CStringA(bsExtn), "", "", 0);
					}
				}
			}
		}

		pUpgradeNodes.Release();
	}

	// find the upgrade/remove nodes	
	hr = pDOM->selectNodes(L"/upgrade/remove/*", &pUpgradeNodes);
	if (hr == S_OK)
	{
		long nUpgradeNodes = 0;
		hr = pUpgradeNodes->get_length(&nUpgradeNodes);

		// loop through the nodes to remove
		for (int i = 0; i < nUpgradeNodes; i++)
		{
			CComPtr<IXMLDOMNode> pUpgradeNode;
			hr = pUpgradeNodes->get_item(i, &pUpgradeNode);
			if (pUpgradeNode == NULL)
				continue;

			CComBSTR bsNodeName;
			hr = pUpgradeNode->get_nodeName(&bsNodeName);
			if (hr != S_OK)
				continue;

			// we're only supporting extnrule at the moment
			if (bsNodeName == L"extnrule")
			{
				CComPtr<IXMLDOMNode> pRuleTextNode;
				hr = pUpgradeNode->selectSingleNode(L"text()", &pRuleTextNode);

				if (pRuleTextNode != NULL)
				{
					CComBSTR bsExtn;
					hr = pRuleTextNode->get_text(&bsExtn);

					if (hr == S_OK)
					{
						hr = pNeverRules->RemoveExtensionRule(CStringA(bsExtn), "", 0);
					}
				}
			}
		}
	}

	// we need to release the DOM to make sure we can overwrite the file next
	pDOM.Release();

	// empty the upgrade file since we don't want to run it again	
	FILE* pUpgradeFile = NULL;
	_tfopen_s(&pUpgradeFile, UpgradeFilePath, _T("wt"));
	if (pUpgradeFile != NULL)
	{		
		fclose(pUpgradeFile);
	}
	else
	{
		// we couldn't empty it, so panic and try and delete it instead
		::DeleteFile(UpgradeFilePath);
	}

	return S_OK;
}

STDMETHODIMP CCompressionRuleManager::Fire_OnRuleChanged(NeverRuleChangeHint hint, DWORD dwSource)
{
	int nConnections = m_vec.GetSize();
	for (int i = 0; i < nConnections; i++)
	{
		CComQIPtr<INeverCompressRulesNotify> pNotify = m_vec.GetAt(i);
		if (pNotify != NULL)
		{
			pNotify->OnRuleChanged(hint, dwSource);
		}
	}

	return S_OK;
}

STDMETHODIMP CCompressionRuleManager::GetCookie(DWORD* pdwCookie)
{
	if (pdwCookie == NULL)
	{
		return E_POINTER;
	}

	*pdwCookie = (DWORD) ::InterlockedIncrement((LONG*) &m_dwNotifyCookie);
	return S_OK;
}