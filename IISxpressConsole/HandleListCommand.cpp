#include "stdafx.h"

#include "HandleListCommand.h"

#include <map>

void HandleListCommand(const CStringArray& saArgs)
{
	// get the active Compression Manager interface
	CComPtr<IUnknown> pCompressionRuleManagerUnk;
	HRESULT hr = ::GetActiveObject(CLSID_CompressionRuleManager, NULL, (IUnknown**) &pCompressionRuleManagerUnk);
	if (hr != S_OK)
	{
		throw ProcessingException(hr, IDS_ERROR_NORUNNINGIISXPRESS);				
	}	

	// get the rule manager
	CComQIPtr<ICompressionRuleManager> pCompressionRuleManager = pCompressionRuleManagerUnk;
	if (pCompressionRuleManager == NULL)
	{
		throw ProcessingException(E_NOINTERFACE, IDS_INTERNALERROR);
	}

	// get the never compress rules
	CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
	if (pNeverCompressRules == NULL)
	{
		throw ProcessingException(E_NOINTERFACE, IDS_INTERNALERROR);
	}

	if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("extn")) == 0)
	{
		CComPtr<IEnumExclusionRule> pExtnRules;
		hr = pNeverCompressRules->EnumExtensions(&pExtnRules);
		if (hr != S_OK || pExtnRules == NULL)
		{
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMEXTNS);
		}

		int nExtns = 0;
		ULONG nFetched = 0;
		ExclusionRuleInfo RuleInfo;
		while (pExtnRules->Next(1, &RuleInfo, &nFetched) == S_OK)
		{
			if (nExtns == 0)
			{
				_tprintf(_T("Extensions:\n"));
			}

			_tprintf(_T(" %ls\n"), RuleInfo.bsRule);
			nExtns++;
		}

		_tprintf(_T("%sFound %d excluded extension(s)\n"), 
			nExtns > 0 ? _T("\n") : _T(""),
			nExtns);
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("ct")) == 0)
	{
		CComPtr<IEnumExclusionRule> pCTRules;
		hr = pNeverCompressRules->EnumContentTypes(&pCTRules);
		if (hr != S_OK || pCTRules == NULL)
		{
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMCTS);
		}

		int nCTs = 0;
		ULONG nFetched = 0;
		ExclusionRuleInfo RuleInfo;
		while (pCTRules->Next(1, &RuleInfo, &nFetched) == S_OK)
		{
			if (nCTs == 0)
			{
				_tprintf(_T("Content Types:\n"));
			}

			_tprintf(_T(" %ls\n"), RuleInfo.bsRule);
			nCTs++;
		}

		_tprintf(_T("%sFound %d excluded content type(s)\n"), 
			nCTs > 0 ? _T("\n") : _T(""),
			nCTs);
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("uri")) == 0)
	{
		CMap<CStringW, LPCWSTR, CString, LPCTSTR> Servers;		

		CComPtr<IEnumExclusionRule> pURIRules;
		hr = pNeverCompressRules->EnumFolders(&pURIRules);
		if (hr != S_OK || pURIRules == NULL)
		{
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMURIS);
		}

		// for Vista we need to get the config document
		CComPtr<IXMLDOMDocument> pDoc;
		if (g_VerInfo.dwMajorVersion >= 6 && CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc) != S_OK)
		{
			// TODO: throw better error message
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMURIS);
		}

		int nURIs = 0;
		ULONG nFetched = 0;
		ExclusionRuleInfo RuleInfo;
		while (pURIRules->Next(1, &RuleInfo, &nFetched) == S_OK)
		{						
			if (nURIs == 0)
			{
				_tprintf(_T("Web site/URI:\n"));
			}

			CString sInstance;

			// try to map the server name to an instance
			if (Servers.Lookup(RuleInfo.bsInstance, sInstance) == TRUE)
			{
				// we got a hit, do nothing
			}
			else if (g_VerInfo.dwMajorVersion < 6 && MapInstanceToServerName(RuleInfo.bsInstance, sInstance) == true)
			{
				// the mapping was a success, so store it
				Servers[sInstance] = RuleInfo.bsInstance;
			}
			else if (g_VerInfo.dwMajorVersion >= 6)
			{
				// for Vista we get the site node from the XML config file
				CComPtr<IXMLDOMNode> pSite;				
				if (CIIS7XMLConfigHelper::GetSiteNode(pDoc, RuleInfo.bsInstance, &pSite) == S_OK)
				{
					CAtlString sId;
					CAtlString sDesc;
					CAtlString sPhysicalPath;
					CAtlArray<CAtlString> Ports;
					CAtlArray<CAtlString> SecurePorts;

					if (CIIS7XMLConfigHelper::GetSiteInfo(pSite, sId, sDesc, sPhysicalPath, Ports, SecurePorts) == S_OK)
					{
						sInstance = sDesc;

						// the mapping was a success, so store it
						Servers[sInstance] = RuleInfo.bsInstance;
					}
				}
			}
			else
			{
				// use the raw instance number
				sInstance = RuleInfo.bsInstance;
			}

			_tprintf(_T(" %-24ls %ls%s\n"), 
				sInstance, 
				RuleInfo.bsRule,
				RuleInfo.dwFlags == RULEFLAGS_FOLDER_WILDCARDMATCH ? _T("/*") : _T(""));

			nURIs++;			
		}

		_tprintf(_T("%sFound %d excluded URIs(s)\n"), 
			nURIs > 0 ? _T("\n") : _T(""),
			nURIs);
	}
	else if (saArgs.GetCount() == 2 && saArgs[0].CompareNoCase(_T("uri")) == 0)
	{								
		CComPtr<IEnumExclusionRule> pURIRules;
		hr = pNeverCompressRules->EnumFolders(&pURIRules);
		if (hr != S_OK || pURIRules == NULL)
		{
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMURIS);
		}

		// assume the arg is an instance number
		CString sInstance = saArgs[1];

		if (g_VerInfo.dwMajorVersion < 6)
		{
			// attempt to map the arg to an instance id
			MapServerNameToInstance(saArgs[1], sInstance);
		}
		else
		{
			CComPtr<IXMLDOMDocument> pDoc;
			if (CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc) == S_OK)
			{
				// attempt to map the arg to an instance id
				MapServerNameToInstance(pDoc, saArgs[1], sInstance);
			}
		}

		int nURIs = 0;
		ULONG nFetched = 0;
		ExclusionRuleInfo RuleInfo;
		while (pURIRules->Next(1, &RuleInfo, &nFetched) == S_OK)
		{	
			if (sInstance.CompareNoCase(RuleInfo.bsInstance) != 0)
				continue;

			if (nURIs == 0)
			{
				_tprintf(_T("URI:\n"));
			}

			_tprintf(_T(" %ls%s\n"), 				
				RuleInfo.bsRule,
				RuleInfo.dwFlags == RULEFLAGS_FOLDER_WILDCARDMATCH ? _T("/*") : _T(""));

			nURIs++;			
		}

		_tprintf(_T("%sFound %d excluded URIs(s)\n"), 
			nURIs > 0 ? _T("\n") : _T(""),
			nURIs);
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("ip")) == 0)
	{		
		CComPtr<IEnumExclusionRule> pIPRules;
		hr = pNeverCompressRules->EnumIPAddresses(&pIPRules);
		if (hr != S_OK || pIPRules == NULL)
		{
			throw ProcessingException(hr, IDS_ERROR_UNABLETOENUMIPS);
		}

		int nIPs = 0;
		ULONG nFetched = 0;
		ExclusionRuleInfo RuleInfo;
		while (pIPRules->Next(1, &RuleInfo, &nFetched) == S_OK)
		{
			if (nIPs == 0)
			{
				_tprintf(_T("IP Addresses:\n"));
			}

			_tprintf(_T(" %ls\n"), RuleInfo.bsRule);
			nIPs++;
		}

		_tprintf(_T("%sFound %d excluded IP(s)\n"), 
			nIPs > 0 ? _T("\n") : _T(""),
			nIPs);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("file")) == 0)
	{
		CComQIPtr<IStaticFileRules> pStaticFileRules = pNeverCompressRules;
		if (pStaticFileRules != NULL)
		{
			// loop round the supplied filepaths
			for (int i = 1; i < saArgs.GetCount(); i++)
			{
				const TCHAR* pszFilepath = saArgs[i];

				CComPtr<IEnumStaticFileInfo> pEnumFileInfo;
				HRESULT hr = pStaticFileRules->EnumByPath(CStringA(pszFilepath), &pEnumFileInfo);
				if (hr != S_OK)
					continue;

				if (i != 1)
				{
					_tprintf(_T("\n"));
				}

				_tprintf(_T("Path: %s\n"), pszFilepath);

				int nFiles = 0;

				while (1)
				{
					ULONG nFetched = 0;
					StaticFileInfo fileInfo;
					hr = pEnumFileInfo->Next(1, &fileInfo, &nFetched);
					if (hr != S_OK || nFetched != 1)
					{
						break;
					}

					// only show user excluded files
					if (fileInfo.nUserCompression == 0)
					{						
						wprintf(L" %s\n", fileInfo.bsFilename);						

						nFiles++;
					}

					::SysFreeString(fileInfo.bsPath);
					::SysFreeString(fileInfo.bsFilename);															
				}

				_tprintf(_T("Found %d excluded file(s)\n"), nFiles);
			}
		}
	}
	else
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_COMMANDLINEINCORRECT);		
		throw sMsg;
	}	
}