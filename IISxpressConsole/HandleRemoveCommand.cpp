#include "stdafx.h"

#include "HandleRemoveCommand.h"

void HandleRemoveCommand(const CStringArray& saArgs)
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
		// keep track of the number of extensions removed
		int nExtns = 0;

		// loop round the supplied extensions
		for (int i = 1; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszExtn = saArgs[i];

			_tprintf(_T("Removing '%s': "), pszExtn);

			hr = pNeverCompressRules->RemoveExtensionRule(CStringA(pszExtn), "", 0);

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

		_tprintf(_T("%sRemoved %d extension(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nExtns);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("ct")) == 0)
	{
		// keep track of the number of content types removed
		int nCTs = 0;

		// loop around the passed in content types
		for (int i = 1; i < saArgs.GetCount(); i++)
		{
			const TCHAR* pszContentType = saArgs[i];

			_tprintf(_T("Removing '%s': "), pszContentType);

			hr = pNeverCompressRules->RemoveContentTypeRule(CStringA(pszContentType), "", 0);

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

		_tprintf(_T("%sRemoved %d content type(s)\n"), 
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

			// the URI must start with a /
			if (pszURI[0] != '/')
			{
				CString sMsg;
				sMsg.LoadString(IDS_ERROR_BADURIFORMAT);		
				throw sMsg;
			}

			_tprintf(_T("Removing '%s': "), pszURI);

			CStringA sURI(pszURI);

			if (sURI.Compare("/*") == 0)
			{
				// remove the *
				sURI = "/";
			}
			else if (sURI.Right(2).Compare("/*") == 0)
			{
				// remove the /*
				sURI = sURI.Left(sURI.GetLength() - 2);
			}

			hr = pNeverCompressRules->RemoveFolderRule(CStringA(sInstance), sURI, 0);

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

		_tprintf(_T("%sRemoved %d URI(s)\n"), 
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

			_tprintf(_T("Removing '%s': "), pszIP);

			hr = pNeverCompressRules->RemoveIPRule(CStringA(pszIP), 0);

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

		_tprintf(_T("%sRemoved %d IP(s)\n"), 
			saArgs.GetCount() > 1 ? _T("\n") : _T(""),
			nIPs);
	}
	else if (saArgs.GetCount() >= 1 && saArgs[0].CompareNoCase(_T("file")) == 0)
	{
		CComQIPtr<IStaticFileRules> pStaticFileRules = pNeverCompressRules;
		if (pStaticFileRules != NULL)
		{
			// keep track of the number of files removed
			int nFiles = 0;

			// loop round the supplied files
			for (int i = 1; i < saArgs.GetCount(); i++)
			{
				const TCHAR* pszFilename = saArgs[i];

				_tprintf(_T("Removing '%s': "), pszFilename);

				hr = pStaticFileRules->RemoveFileRule(CStringA(pszFilename));

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

			_tprintf(_T("%sRemoved %d file(s)\n"), 
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