// ValidateIISxpress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ValidateIISxpress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

void PrintBanner();
void ValidateIISSetup();
void ValidateIISxpressFiles();

int warning = 0;
int error = 0;
void ReportInfo(LPCTSTR pszMsg, ...);
void ReportWarning(LPCTSTR pszMsg, ...);
void ReportError(LPCTSTR pszMsg, ...);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;	

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0) || ::OleInitialize(NULL) != S_OK)
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC or COM initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		PrintBanner();
		ValidateIISSetup();
		ValidateIISxpressFiles();
	}

	::OleUninitialize();

	return nRetCode;
}

void PrintBanner()
{
	TCHAR szModuleName[512];
	VERIFY(::GetModuleFileName(NULL, szModuleName, _countof(szModuleName)) > 0);

	DWORD dwVersionHandle = 0;
	DWORD dwVersionInfoSize = ::GetFileVersionInfoSize(szModuleName, &dwVersionHandle);
	ASSERT(dwVersionInfoSize > 0);
	
	AutoArray<BYTE> pVersionInfo(dwVersionInfoSize);

	VERIFY(::GetFileVersionInfo(szModuleName, dwVersionHandle, dwVersionInfoSize, pVersionInfo));

	VS_FIXEDFILEINFO* pFixedFileInfo = NULL;
	UINT nFixedFileInfoSize = 0;
	VERIFY(::VerQueryValue(pVersionInfo, _T("\\"), (void**) &pFixedFileInfo, &nFixedFileInfoSize));	

	_tprintf(_T("\nValidateIISxpress v%u.%u.%u.%u (c) Ripcord Software 2005-2009\n\n"), 
		pFixedFileInfo->dwProductVersionMS >> 16, 
		pFixedFileInfo->dwProductVersionMS & 0xffff,
		pFixedFileInfo->dwProductVersionLS >> 16, 
		pFixedFileInfo->dwProductVersionLS & 0xffff);
}

void ValidateIISSetup()
{
	CIISMetaBase MetaBase;
	CComPtr<IMSAdminBase> pAdminBase;
	if (MetaBase.Init() != true || MetaBase.GetAdminBase(&pAdminBase) != true)
	{
		ReportError(_T("Unable to establish a connection to the IIS metabase"));
		return;
	}
	
	CIISMetaBaseData LoadOrderData;
	if (LoadOrderData.ReadData(pAdminBase, L"/LM/W3SVC/Filters", MD_FILTER_LOAD_ORDER))
	{
		std::wstring sLoadOrder;
		if (LoadOrderData.GetAsString(sLoadOrder))
		{
			std::wstring::size_type nPos = sLoadOrder.find(IISXPRESS_FILTERNAMEW);
			if (nPos != std::wstring::npos)
			{
				std::wstring::size_type expectedLocation = sLoadOrder.size();
				expectedLocation -= _countof(IISXPRESS_FILTERNAMEW) - 1;
				if (nPos != expectedLocation)
				{
					ReportWarning(_T("The IISxpress ISAPI filter entry should be the last defined module, please run repair"));
				}	
			}
			else
			{
				ReportError(_T("Unable to find the IISxpress ISAPI entry in the filter load order"));	
			}
		}
		else
		{
			ReportWarning(_T("Unable to read the ISAPI filter load order"));		
		}
	}
	else
	{
		ReportError(_T("Unable to query the ISAPI filter data"));		
	}				

	const std::wstring sMBFilterPath = L"/LM/W3SVC/Filters/" IISXPRESS_FILTERNAMEW;

	CIISMetaBaseData FilterPathData;
	if (FilterPathData.ReadData(pAdminBase, sMBFilterPath.c_str(), MD_FILTER_IMAGE_PATH))
	{
		std::wstring sFilterPath;
		if (FilterPathData.GetAsString(sFilterPath))
		{
			CFile file;
			if (!file.Open(sFilterPath.c_str(), CFile::modeRead | CFile::shareDenyNone))
			{
				ReportWarning(_T("Unable to open the IISxpress ISAPI filter DLL ('%s'), check that the file exists"), sFilterPath.c_str());		
			}
			file.Close();
		}
		else
		{
			ReportWarning(_T("Unable to read the IISxpress ISAPI filter path"));		
		}		
	}
	else
	{
		ReportWarning(_T("Unable to query the IISxpress ISAPI filter path"));		
	}		

	CIISMetaBaseData FilterStatusData;
	if (FilterStatusData.ReadData(pAdminBase, sMBFilterPath.c_str(), MD_FILTER_STATE) == true)
	{
		DWORD dwFilterState = 0;
		if (FilterStatusData.GetAsDWORD(&dwFilterState))
		{
			if (dwFilterState != 1)
			{
				ReportWarning(_T("IIS reports that the filter state is %d"), dwFilterState);			
			}
		}
		else
		{
			ReportWarning(_T("Unable to read the IISxpress ISAPI filter state"));			
		}
	}
	else
	{
		ReportError(_T("Unable to query the IISxpress ISAPI filter state"));
	}		

	CIISMetaBaseData FilterWin32StatusData;
	if (FilterWin32StatusData.ReadData(pAdminBase, sMBFilterPath.c_str(), MD_WIN32_ERROR) == true)
	{
		DWORD dwWIN32FilterState = 0;
		if (FilterWin32StatusData.GetAsDWORD(&dwWIN32FilterState))
		{
			if (dwWIN32FilterState != 0)
			{
				TCHAR szErrorMessage[512] = _T("");
				::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwWIN32FilterState, 0x0409, szErrorMessage, _countof(szErrorMessage), NULL);

				TCHAR* pchWhitespace = NULL;
				while ((pchWhitespace = _tcschr(szErrorMessage, '\n')))
				{
					*pchWhitespace = ' ';
				}

				while ((pchWhitespace = _tcschr(szErrorMessage, '\r')))
				{
					*pchWhitespace = ' ';
				}

				ReportWarning(_T("IIS reports that the filter WIN32 state is %d, the error is: '%s'"), dwWIN32FilterState, szErrorMessage);
			}
		}
		else
		{
			ReportWarning(_T("Unable to read the IISxpress ISAPI filter Win32 state"));			
		}
	}
	else
	{
		ReportError(_T("Unable to query the IISxpress ISAPI filter Win32 state"));
	}

	std::vector<std::wstring> AppPools;
	if (MetaBase.EnumKeys(L"/LM/W3SVC/AppPools", AppPools))
	{
		for (size_t i = 0; i < AppPools.size(); i++)
		{
			std::wstring appPoolPath = L"/LM/W3SVC/AppPools/" + AppPools[i];

			CIISMetaBaseData AppPoolIdentityData;
			if (AppPoolIdentityData.ReadData(pAdminBase, appPoolPath.c_str(), MD_APPPOOL_IDENTITY_TYPE) == true)
			{
				DWORD dwIdentityType = 0;
				AppPoolIdentityData.GetAsDWORD(&dwIdentityType);

				ReportInfo(_T("AppPool '%s' has identity type %d"), AppPools[i].c_str(), dwIdentityType);

				if (dwIdentityType == MD_APPPOOL_IDENTITY_TYPE_SPECIFICUSER)
				{
					CIISMetaBaseData AppPoolUserData;
					if (AppPoolUserData.ReadData(pAdminBase, appPoolPath.c_str(), MD_WAM_USER_NAME) == true)
					{
						std::wstring sUserName;
						if (AppPoolUserData.GetAsString(sUserName))
						{
							ReportWarning(
								_T("The AppPool '%s' has been configured to use user account '%s', ensure this account or IIS_WPG ")
								_T("has read/execute permissions to the IISxpress directory ")
								_T("and full permissions to the IISxpress Logs directory"),
								AppPools[i].c_str(), sUserName.c_str());
						}
						else
						{
							ReportError(_T("Unable to read the user name of the AppPool"));
						}
					}
					else
					{
						ReportError(_T("Unable to query the user name of the AppPool"));
					}
				}
			}
			else
			{
				ReportInfo(_T("AppPool '%s' does not have an identity type"), AppPools[i].c_str());
			}
		}
	}
	else
	{
		ReportError(_T("Unable to query the ISAPI AppPool data"));
	}
}

void ValidateIISxpressFiles()
{
	RS::WinSecurity::SmartSid sidIISWPG;
	if (!RS::WinSecurity::WinSecurity::GetAccountSid(_T("IIS_WPG"), sidIISWPG))
	{
		ReportError(_T("Unable to get the SID for the IIS_WPG group, is IIS installed?"));
	}

	CRegKey regFilter;
	if (regFilter.Open(HKEY_LOCAL_MACHINE, IISXPRESSFILTER_REGKEY, KEY_READ) == ERROR_SUCCESS)
	{		
		if (sidIISWPG.IsValid())
		{
			RS::WinSecurity::SmartSecurityDescriptor SD;
			if (RS::WinSecurity::WinSecurity::GetKeySecurity(regFilter, SD))
			{
				if (!RS::WinSecurity::WinSecurity::CheckPermissons(SD, sidIISWPG, READ_CONTROL))
				{
					ReportWarning(_T("The IIS_WPG group does not have permissions for the IISxpress ISAPI filter registry entry"));
				}
			}
			else
			{
				ReportError(_T("Unable to get the SD for IISxpress ISAPI filter registry entry"));
			}
		}	
 
		TCHAR szFilterInstallPath[512] = _T("");
		ULONG nFilterInstallPathSize = _countof(szFilterInstallPath);
		if (regFilter.QueryStringValue(IISXPRESS_INSTALLPATH, szFilterInstallPath, &nFilterInstallPathSize) == ERROR_SUCCESS)
		{
			CPath ISAPIDLLPath(szFilterInstallPath);
			ISAPIDLLPath.Append(_T("IISxpress.dll"));
			if (ISAPIDLLPath.FileExists())
			{
				if (sidIISWPG.IsValid())
				{
					RS::WinSecurity::SmartSecurityDescriptor SD;
					if (RS::WinSecurity::WinSecurity::GetFileSecurity(ISAPIDLLPath, SD))
					{
						if (!RS::WinSecurity::WinSecurity::CheckPermissons(SD, sidIISWPG, STANDARD_RIGHTS_READ))
						{
							ReportWarning(_T("The IIS_WPG group does not have permissions for the IISxpress ISAPI DLL"));
						}
					}
					else
					{
						ReportError(_T("Unable to get the SD for the IISxpress ISAPI DLL"));
					}
				}
			}
			else
			{
				ReportError(_T("The file '%s' could not be found"), ISAPIDLLPath.m_strPath);
			}

			CPath ISAPIPerfDLLPath(szFilterInstallPath);
			ISAPIPerfDLLPath.Append(_T("IISxpressPerf.dll"));
			if (ISAPIPerfDLLPath.FileExists())
			{
				if (sidIISWPG.IsValid())
				{
					RS::WinSecurity::SmartSecurityDescriptor SD;
					if (RS::WinSecurity::WinSecurity::GetFileSecurity(ISAPIPerfDLLPath, SD))
					{
						if (!RS::WinSecurity::WinSecurity::CheckPermissons(SD, sidIISWPG, STANDARD_RIGHTS_READ))
						{
							ReportWarning(_T("The IIS_WPG group does not have permissions for the IISxpress Perf DLL"));
						}
					}
					else
					{
						ReportError(_T("Unable to get the SD for the IISxpress Perf DLL"));
					}
				}
			}
			else
			{
				ReportError(_T("The file '%s' could not be found"), ISAPIPerfDLLPath.m_strPath);
			}
		}
		else
		{
			ReportError(_T("Unable to locate the IISxpress ISAPI filter path registry key"));			
		}		
	}
	else
	{
		ReportError(_T("Unable to locate the IISxpress ISAPI filter registry key"));
	}	

	CRegKey regServer;
	if (regServer.Open(HKEY_LOCAL_MACHINE, IISXPRESSSERVER_REGKEY, KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szServerInstallPath[512] = _T("");
		ULONG nServerInstallPathSize = _countof(szServerInstallPath);
		if (regServer.QueryStringValue(IISXPRESS_INSTALLPATH, szServerInstallPath, &nServerInstallPathSize) == ERROR_SUCCESS)
		{
			CPath ServerPSPath(szServerInstallPath);
			ServerPSPath.Append(_T("IISxpressServerPS.dll"));
			if (ServerPSPath.FileExists())
			{
				if (sidIISWPG.IsValid())
				{
					RS::WinSecurity::SmartSecurityDescriptor SD;
					if (RS::WinSecurity::WinSecurity::GetFileSecurity(ServerPSPath, SD))
					{
						if (!RS::WinSecurity::WinSecurity::CheckPermissons(SD, sidIISWPG, STANDARD_RIGHTS_READ))
						{
							ReportWarning(_T("The IIS_WPG group does not have permissions for the IISxpress PS DLL"));
						}
					}
					else
					{
						ReportError(_T("Unable to get the SD for the IISxpress PS DLL"));
					}
				}
			}
			else
			{
				ReportError(_T("The file '%s' could not be found"), ServerPSPath.m_strPath);
			}
		}
		else
		{
			ReportError(_T("Unable to locate the IISxpress ISAPI filter path registry key"));			
		}	
	}
	else
	{
		ReportError(_T("Unable to locate the IISxpress Server registry key"));		
	}
}

void ReportWarning(LPCTSTR pszMsg, ...)
{
	++warning;

	va_list args;
	va_start(args, pszMsg);

	_tprintf(_T("Warning (%d): "), warning);
	_vtprintf(pszMsg, args);
	_tprintf(_T("\n\n"));

	va_end(args);
}

void ReportError(LPCTSTR pszMsg, ...)
{
	++error;

	va_list args;
	va_start(args, pszMsg);

	_tprintf(_T("Error (%d): "), error);
	_vtprintf(pszMsg, args);
	_tprintf(_T("\n\n"));

	va_end(args);
}
void ReportInfo(LPCTSTR pszMsg, ...)
{
	va_list args;
	va_start(args, pszMsg);

	_tprintf(_T("Info: "));
	_vtprintf(pszMsg, args);
	_tprintf(_T("\n\n"));

	va_end(args);
}