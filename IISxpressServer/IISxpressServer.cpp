// IISxpressServer.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "IISxpressServer.h"

#include "IISxpressHTTPRequest.h"
#include "CompressionRuleManager.h"
#include "ProductLicenseManager.h"

#include "ServiceNames.h"

#include <atlpath.h>

#include <Tlhelp32.h>
#include <psapi.h>

class CIISxpressServerModule : public CAtlServiceModuleT< CIISxpressServerModule, IDS_SERVICENAME >
{

public:

	DECLARE_LIBID(LIBID_IISxpressServerLib)

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IISXPRESSSERVER, "{092EAFF9-46C7-4704-ADAF-F98EACE0BC85}")

	CIISxpressServerModule() : m_pszServiceCommandLine(NULL) {}

	HRESULT InitializeSecurity() throw()
	{				
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for 
		// your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.

		HRESULT hRes = CoInitializeSecurity(
			NULL, 
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_NONE,
			//0,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			0);

		return S_OK;
	}

	int WinMain(int nShowCmd)
	{
		_tcscpy_s(m_szServiceName, _countof(m_szServiceName), IISXPRESS_SERVICENAME);		

		LoadToolhelp32Procs();

		if (IsAlreadyRunning() == true)
		{
			return 0;
		}
		
		// get the OS version
		g_VerInfo.dwOSVersionInfoSize = sizeof(g_VerInfo);
		::GetVersionEx((OSVERSIONINFO*) &g_VerInfo);

		return CAtlServiceModuleT<CIISxpressServerModule, IDS_SERVICENAME>::WinMain(nShowCmd);
	}

	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) throw( )
	{
		// we need to capture the command line, just get the first arg for now
		if (dwArgc >= 2)
			m_pszServiceCommandLine = lpszArgv[1];

		CAtlServiceModuleT::ServiceMain(dwArgc, lpszArgv);
	}


	HRESULT Run(int nShowCmd)
	{
		InitializeSecurity();

		// get hold of the registry settings
		CComPtr<IUnknown> pTempRegSettings;
		CComObject<CComIISxpressRegSettings>* pRegSettings;
		CComObject<CComIISxpressRegSettings>::CreateInstance(&pRegSettings);
		if (pRegSettings != NULL)
		{			
			pRegSettings->QueryInterface(IID_IUnknown, (void**) &pTempRegSettings);

			pRegSettings->Init(IISXPRESSSERVER_REGKEY);
		}

		MyOutputDebugString(pRegSettings, _T("CIISxpressServerModule::Run()\n"));

		TCHAR szModuleFileName[512];
		::GetModuleFileName(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(szModuleFileName[0]));

		CPathT<CString> ModulePath(szModuleFileName);
		ModulePath.RemoveFileSpec();

		// chuck the path to the module into the registry so we can find it later
		HKEY hModulePathReg = NULL;
		if (::RegOpenKey(HKEY_LOCAL_MACHINE, IISXPRESSSERVER_REGKEY, &hModulePathReg) == ERROR_SUCCESS)
		{
			::RegSetValueEx(hModulePathReg, IISXPRESS_INSTALLPATH, 0, REG_SZ, 
				(const BYTE*) (const TCHAR*) ModulePath, 
				(DWORD) (_tcslen(ModulePath) + 1) * sizeof(TCHAR));

			::RegCloseKey(hModulePathReg);
			hModulePathReg = NULL;
		}

		DWORD dwLoggingLevel = IISXPRESS_LOGGINGLEVEL_NONE;
		if (pRegSettings != NULL)
		{
			pRegSettings->get_LoggingLevel(&dwLoggingLevel);
			if (dwLoggingLevel >= IISXPRESS_LOGGINGLEVEL_BASIC)
			{
				CString sLogPath = ModulePath;
				sLogPath += _T("\\Logs");

				::CreateDirectory(sLogPath, NULL);	

				g_Log.Init(sLogPath, _T("IISxpressServer-"), _T("log"));
				g_Log.Append(true, _T("IISxpressServer started\n"));
				g_Log.DumpSysInfo(_T("IISxpressServer"));
			}
		}

		CComBSTR bsStorageFileSpec(ModulePath);
		bsStorageFileSpec += _T("\\");
		bsStorageFileSpec += IISXPRESS_STORAGEFILENAME;		

		CComPtr<IStorage> pConfigStorage;

		HRESULT hr = ::StgOpenStorageEx(bsStorageFileSpec, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
			STGFMT_STORAGE, 0, NULL, NULL, IID_IStorage, (void**) &pConfigStorage);

		MyOutputDebugString(pRegSettings, 
			_T("CIISxpressServerModule::Run() - StgOpenStorageEx(%ls) hr=0x%08x\n"), 
			bsStorageFileSpec, hr);

		if (hr != S_OK)
		{
			hr = ::StgCreateStorageEx(bsStorageFileSpec, STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
				STGFMT_STORAGE, 0, NULL, NULL, IID_IStorage, (void**) &pConfigStorage);

			MyOutputDebugString(pRegSettings, 
				_T("CIISxpressServerModule::Run() - StgCreateStorageEx(%ls) hr=0x%08x\n"), 
				bsStorageFileSpec, hr);
		}				

		// if we haven't been able to open the storage then we should exit
		if (hr != S_OK)
		{
			MyOutputDebugString(pRegSettings, 
					_T("CIISxpressServerModule::Run() - unable to open storage file, exiting app.\n"), 
					bsStorageFileSpec, hr);

			return 0;
		}

		DWORD dwHTTPRequestRegister = (DWORD) -1;
		DWORD dwCompressionManagerRegister = (DWORD) -1;
		DWORD dwRegSettingsRegister = (DWORD) -1;
		DWORD dwProductLicenseRegister = (DWORD) -1;
		CComPtr<IRunningObjectTable> pROT;

		CComObject<CCompressionRuleManager>* pCompressionManagerObject = NULL;
		CComObject<CCompressionRuleManager>::CreateInstance(&pCompressionManagerObject);
		CComPtr<IUnknown> pTempCompressionManagerObject;
		pCompressionManagerObject->QueryInterface(IID_IUnknown, (void**) &pTempCompressionManagerObject);		

		CComObject<CIISxpressHTTPRequest>* pIISxpressHTTPRequestObject = NULL;
		CComObject<CIISxpressHTTPRequest>::CreateInstance(&pIISxpressHTTPRequestObject);
		CComPtr<IUnknown> pTempIISxpressHTTPRequestObject;
		pIISxpressHTTPRequestObject->QueryInterface(IID_IUnknown, (void**) &pTempIISxpressHTTPRequestObject);

		CComObject<CProductLicenseManager>* pProductLicenseManagerObject = NULL;
		CComObject<CProductLicenseManager>::CreateInstance(&pProductLicenseManagerObject);
		CComPtr<IUnknown> pTempProductLicenseManagerObject;
		pProductLicenseManagerObject->QueryInterface(IID_IUnknown, (void**) &pTempProductLicenseManagerObject);

		if (pRegSettings != NULL && 
			pCompressionManagerObject != NULL && 
			pIISxpressHTTPRequestObject != NULL &&
			pProductLicenseManagerObject != NULL)
		{
			CComPtr<ICompressionRuleManager> pCompressionRuleManager;
			HRESULT hr = pCompressionManagerObject->QueryInterface(
				IID_ICompressionRuleManager, (void**) &pCompressionRuleManager);

			CComPtr<IIISxpressHTTPRequest> pIISxpressHTTPRequest;
			hr = pIISxpressHTTPRequestObject->QueryInterface(
				IID_IIISxpressHTTPRequest, (void**) &pIISxpressHTTPRequest);

			CComPtr<IProductLicenseManager> pProductLicenseManager;
			hr = pProductLicenseManagerObject->QueryInterface(
				IID_IProductLicenseManager, (void**) &pProductLicenseManager);			

			pCompressionManagerObject->Init(pConfigStorage, pRegSettings);						

			// handle a LicenseMode override from the command line (probably from the installer)
			LPCTSTR pszCommandLine = _T("");
			if (this->m_bService == TRUE)
			{
				pszCommandLine = m_pszServiceCommandLine;
			}			
			else
			{
				pszCommandLine = ::GetCommandLine();
			}

			if (pszCommandLine != NULL && _tcsstr(pszCommandLine, _T("/LicenseMode=")) != NULL)
			{				
				if (pRegSettings != NULL)
				{
					if (_tcsstr(pszCommandLine, _T("/LicenseMode=Community")) != NULL)
					{
						pRegSettings->put_LicenseMode(LicenseModeCommunity);
					}
					else if (_tcsstr(pszCommandLine, _T("/LicenseMode=Evaluation")) != NULL)
					{
						pRegSettings->put_LicenseMode(LicenseModeEvaluation);
					}
					else if (_tcsstr(pszCommandLine, _T("/LicenseMode=Full")) != NULL)
					{
						pRegSettings->put_LicenseMode(LicenseModeFull);
					}
					else if (_tcsstr(pszCommandLine, _T("/LicenseMode=AHA")) != NULL)
					{
						pRegSettings->put_LicenseMode(LicenseModeAHA);
					}
				}
			}

			CComQIPtr<IIISxpressSettings> pSettings = pCompressionRuleManager;

			pIISxpressHTTPRequest->Init(pConfigStorage, pCompressionRuleManager, pProductLicenseManager, pRegSettings, pSettings);
			
			CComPtr<IMoniker> pCompressionRuleManagerMoniker;
			GetClassMoniker(CLSID_CompressionRuleManager, &pCompressionRuleManagerMoniker);			

			CComPtr<IMoniker> pIISxpressHTTPMoniker;
			GetClassMoniker(CLSID_IISxpressHTTPRequest, &pIISxpressHTTPMoniker);

			CComPtr<IMoniker> pRegSettingsMoniker;
			GetClassMoniker(CLSID_ComIISxpressRegSettings, &pRegSettingsMoniker);

			CComPtr<IMoniker> pProductLicenseMoniker;
			GetClassMoniker(CLSID_ProductLicenseManager, &pProductLicenseMoniker);

			hr = ::GetRunningObjectTable(0, &pROT);
			if (pROT != NULL)
			{
				DWORD dwRegisterMode = ROTFLAGS_REGISTRATIONKEEPSALIVE;

				// TODO: review this crap
				// on Windows 2003 (and less) we always set ROTFLAGS_ALLOWANYCLIENT, also when
				// on Vista+ set the flag if we are running as a service
				if (g_VerInfo.dwMajorVersion < 6 || this->m_bService == TRUE)
				{
					dwRegisterMode |= ROTFLAGS_ALLOWANYCLIENT;
				}

				ICompressionRuleManager* pTemp1 = pCompressionRuleManager;
				hr = pROT->Register(dwRegisterMode, 
					pTemp1, pCompressionRuleManagerMoniker, &dwCompressionManagerRegister);

				IIISxpressHTTPRequest* pTemp2 = pIISxpressHTTPRequest;
				hr = pROT->Register(dwRegisterMode, 
					pTemp2, pIISxpressHTTPMoniker, &dwHTTPRequestRegister);

				IComIISxpressRegSettings* pTemp3 = pRegSettings;
				hr = pROT->Register(dwRegisterMode, 
					pTemp3, pRegSettingsMoniker, &dwRegSettingsRegister);

				IProductLicenseManager* pTemp4 = pProductLicenseManager;
				hr = pROT->Register(dwRegisterMode, 
					pTemp4, pProductLicenseMoniker, &dwProductLicenseRegister);
			}
			
			/*hr = ::RegisterActiveObject(pCompressionRuleManager, CLSID_CompressionRuleManager, 
				ACTIVEOBJECT_STRONG, &dwCompressionManagerRegister);

			hr = ::RegisterActiveObject(pIISxpressHTTPRequest, CLSID_IISxpressHTTPRequest, 
				ACTIVEOBJECT_STRONG, &dwHTTPRequestRegister);*/

			// TODO: review this
			/*pCompressionRuleManager.Release();
			pIISxpressHTTPRequest.Release();*/
		}
		else
		{
			MyOutputDebugString(pRegSettings, 
				_T("CIISxpressServerModule::Run() - either HTTP or CompressionManager failed to create, %08x or %08x\n"), 
				pIISxpressHTTPRequestObject, pCompressionManagerObject);
		}

		int nRet = CAtlServiceModuleT<CIISxpressServerModule, IDS_SERVICENAME>::Run(nShowCmd);

		if (dwCompressionManagerRegister != (DWORD) -1)
		{
			pROT->Revoke(dwCompressionManagerRegister);
			//::RevokeActiveObject(dwCompressionManagerRegister, NULL);
		}

		if (dwHTTPRequestRegister != (DWORD) -1)
		{
			pROT->Revoke(dwHTTPRequestRegister);
			//::RevokeActiveObject(dwHTTPRequestRegister, NULL);
		}

		if (dwRegSettingsRegister != (DWORD) -1)
		{
			pROT->Revoke(dwRegSettingsRegister);
		}

		if (dwProductLicenseRegister != (DWORD) -1)
		{
			pROT->Revoke(dwProductLicenseRegister);
		}
		
		if (dwLoggingLevel >= IISXPRESS_LOGGINGLEVEL_BASIC)
		{			
			g_Log.Append(true, _T("IISxpressServer stopped\n"));
		}

		// TODO: review this
		//if (pRegSettings != NULL)
		//	pRegSettings->Release();

		return nRet;
	}

	bool GetClassMoniker(CLSID classid, IMoniker** ppMoniker)
	{
		if (ppMoniker == NULL)
			return false;

		LPOLESTR pszCLSID;
		::StringFromCLSID(classid, &pszCLSID);

		CComPtr<IMoniker> pMoniker;
		::CreateItemMoniker(OLESTR("!"), pszCLSID, &pMoniker);

		::CoTaskMemFree(pszCLSID);

		*ppMoniker = pMoniker.Detach();

		return true;
	}

	bool IsAlreadyRunning(void)
	{
		// we can't proceed unless these have loaded up, so return false
		if (g_pfnCreateToolhelp32Snapshot != NULL && g_pfnProcess32First != NULL && g_pfnProcess32Next != NULL)
		{
			return IsAlreadyRunning_TH();
		}
		else
		{
			return IsAlreadyRunning_EP();
		}
	}

	// Toolhelp version, does work on NT
	bool IsAlreadyRunning_TH(void)
	{		
		if (g_pfnCreateToolhelp32Snapshot == NULL || g_pfnProcess32First == NULL || g_pfnProcess32Next == NULL)
		{
			return false;
		}

		HANDLE hSnapshot = g_pfnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			return true;
		}

		TCHAR szModuleFilename[512];
		::GetModuleFileName(NULL, szModuleFilename, sizeof(szModuleFilename) / sizeof(szModuleFilename[0]));		

		CPathT<CString> ModuleFilename(szModuleFilename);
		ModuleFilename.StripPath();
		CString sModuleFilename = ModuleFilename;

		int nFound = 0;

		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		BOOL bEnum = g_pfnProcess32First(hSnapshot, &ProcessEntry);
		while (bEnum == TRUE)
		{
			CPathT<CString> ProcessFilename(ProcessEntry.szExeFile);
			ProcessFilename.StripPath();
			CString sProcessFilename = ProcessFilename;

			if (sModuleFilename == sProcessFilename)
			{
				nFound++;
			}

			bEnum = g_pfnProcess32Next(hSnapshot, &ProcessEntry);
		}

		::CloseHandle(hSnapshot);		

		return nFound > 1 ? true : false;
	}

	// EnumProcess version, works on NT/W2K/XP, etc.
	bool IsAlreadyRunning_EP(void)
	{
		int nFound = 0;

		TCHAR szModuleFilename[512];
		::GetModuleFileName(NULL, szModuleFilename, sizeof(szModuleFilename) / sizeof(szModuleFilename[0]));

		CPathT<CString> ModuleFilename(szModuleFilename);
		ModuleFilename.StripPath();
		CString sModuleFilename = ModuleFilename;

		DWORD dwProcessIds[1024];

		DWORD dwProcesses = 0;
		if (::EnumProcesses(dwProcessIds, sizeof(dwProcessIds), &dwProcesses) == FALSE)
			return false;

		dwProcesses /= sizeof(DWORD);

		for (DWORD i = 0; i < dwProcesses; i++)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessIds[i]);
			if (hProcess == NULL)
				continue;

			HMODULE hModule = NULL;
			DWORD dwModules = 0;
			if (::EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwModules) == TRUE)
			{
				TCHAR szModuleName[512];
				if (::GetModuleBaseName(hProcess, hModule, szModuleName, sizeof(szModuleName)) != 0)
				{
					if (sModuleFilename == szModuleName)
					{
						nFound++;
					}
				}
			}

			::CloseHandle(hProcess);		
		}

		return nFound > 1 ? true : false;
	}

	HRESULT RegisterAppId(bool bService = false)
	{
		HRESULT hr = CAtlServiceModuleT<CIISxpressServerModule, IDS_SERVICENAME>::RegisterAppId(bService);

		// do stuff now the service is installed
		if (hr == S_OK && bService == true)
		{			
			SC_HANDLE hSM = ::OpenSCManager(NULL, NULL, GENERIC_WRITE);
			if (hSM != NULL)
			{
				SC_HANDLE hService = ::OpenService(hSM, m_szServiceName, GENERIC_WRITE);

				// add the service description
				if (hService != NULL)
				{
					TCHAR szDesc[256];
					if (::LoadString(::GetModuleHandle(NULL), IDS_SERVICEDESC, szDesc, sizeof(szDesc) / sizeof(szDesc[0])) > 0)
					{
						SERVICE_DESCRIPTION Desc;
						Desc.lpDescription = szDesc;
					
						::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Desc);
					}					
				}

				// set the startup to auto if asked to
				TCHAR* pszCommandLine = ::GetCommandLine();
				if (hService != NULL && pszCommandLine != NULL && _tcsstr(pszCommandLine, _T(" /start=auto")) != 0)
				{
					::ChangeServiceConfig(hService, 
						SERVICE_NO_CHANGE, 
						SERVICE_AUTO_START, 
						SERVICE_NO_CHANGE,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL);
				}

				if (hService != NULL)
				{
					::CloseServiceHandle(hService); 
				}

				::CloseServiceHandle(hSM);
			}			
		}	

		return hr;
	}

private:

	LPCTSTR m_pszServiceCommandLine;
};

CIISxpressServerModule _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

