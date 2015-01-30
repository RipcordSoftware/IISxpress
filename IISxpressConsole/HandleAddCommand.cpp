#include "stdafx.h"

#include "HandleAddCommand.h"

void HandleAddCommand(const CStringArray& saArgs)
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

	if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("extn")) == 0)
	{
		// keep track of the number of extensions added
		int nExtns = 0;

		// loop round the supplied extensions
		for (int i = 1; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszExtn = saArgs[i];

			_tprintf(_T("Adding '%s': "), pszExtn);

			hr = pNeverCompressRules->AddExtensionRule(CStringA(pszExtn), "", "", 0);

			if (hr == S_OK)
			{
				_tprintf(_T("OK\n"));
				nExtns++;
			}
			else
			{
				_tprintf(_T("FAILED\n"));
			}
		}

		_tprintf(_T("%sAdded %d extension(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nExtns);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("ct")) == 0)
	{
		// keep track of the number of content types added
		int nCTs = 0;

		// loop around the passed in content types
		for (int i = 1; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszContentType = saArgs[i];

			_tprintf(_T("Adding '%s': "), pszContentType);

			hr = pNeverCompressRules->AddContentTypeRule(CStringA(pszContentType), "", "", 0);

			if (hr == S_OK)
			{
				_tprintf(_T("OK\n"));
				nCTs++;
			}
			else
			{
				_tprintf(_T("FAILED\n"));
			}
		}

		_tprintf(_T("%sAdded %d content type(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nCTs);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("uri")) == 0)
	{
		// the user must supply the instance
		if (saArgs.GetCount() < 2)
		{			
			CString sMsg;
			sMsg.LoadString(IDS_ERROR_COMMANDLINEINCORRECT);		
			throw sMsg;
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

		// loop around the supplied URIs
		for (int i = 2; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszURI = saArgs[i];

			// the string must begin with a /
			if (pszURI[0] != '/')
			{
				CString sMsg;
				sMsg.LoadString(IDS_ERROR_BADURIFORMAT);		
				throw sMsg;
			}

			DWORD dwMode = RULEFLAGS_FOLDER_EXACTMATCH;

			_tprintf(_T("Adding '%s': "), pszURI);			

			CStringA sURI(pszURI);			
			int nURILength = sURI.GetLength();
			if (sURI.Compare("/*") == 0)
			{
				sURI = "/";
				dwMode = RULEFLAGS_FOLDER_WILDCARDMATCH;
			}
			else if (nURILength > 1 && 
				pszURI[nURILength - 2] == '/' &&
				pszURI[nURILength - 1] == '*')
			{
				// remove the /* from the end of the string and set the type as wildcard
				sURI = CString(sURI, nURILength - 2);
				dwMode = RULEFLAGS_FOLDER_WILDCARDMATCH;
			}

			hr = pNeverCompressRules->AddFolderRule(CStringA(sInstance), sURI, dwMode, 0);

			if (hr == S_OK)
			{
				_tprintf(_T("OK\n"));
				nURIs++;
			}
			else
			{
				_tprintf(_T("FAILED\n"));
			}
		}

		_tprintf(_T("%sAdded %d URI(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nURIs);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("ip")) == 0)
	{
		// keep track of the number of IPs added
		int nIPs = 0;

		// loop round the supplied IPs
		for (int i = 1; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszIP = saArgs[i];

			_tprintf(_T("Adding '%s': "), pszIP);

			hr = pNeverCompressRules->AddIPRule(CStringA(pszIP), 0);

			if (hr == S_OK)
			{
				_tprintf(_T("OK\n"));
				nIPs++;
			}
			else
			{
				_tprintf(_T("FAILED\n"));
			}
		}

		_tprintf(_T("%sAdded %d IP(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nIPs);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("file")) == 0)
	{
		CComQIPtr<IStaticFileRules> pStaticFileRules = pNeverCompressRules;
		if (pStaticFileRules != NULL)
		{
			// keep track of the number of files added
			int nFiles = 0;

			// loop round the supplied files
			for (int i = 1; i < saArgs.GetCount(); i++)
			{				
				const TCHAR* pszFilename = saArgs[i];

				_tprintf(_T("Adding '%s': "), pszFilename);

				hr = pStaticFileRules->AddFileRule(CStringA(pszFilename), 0, -1);

				// TODO: fix this problem in the server
				if (hr == S_OK)
				{
					hr = pStaticFileRules->SetFileRule(CStringA(pszFilename), 0, -1);
				}

				if (hr == S_OK)
				{
					_tprintf(_T("OK\n"));
					nFiles++;
				}
				else
				{
					_tprintf(_T("FAILED\n"));
				}
			}

			_tprintf(_T("%sAdded %d files\n"), 
				saArgs.GetCount() > 1 ? _T("\n") : _T(""),
				nFiles);
		}
	}
	else
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_COMMANDLINEINCORRECT);		
		throw sMsg;
	}	
}