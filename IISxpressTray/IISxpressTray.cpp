// IISxpressTray.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "IISxpressTrayDlg.h"
#include <initguid.h>

#include "IISxpressTray_i.c"

#include <Tlhelp32.h>
#include <psapi.h>

#include <atlpath.h>

#include "IISxpressInstallerExtensions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIISxpressTrayApp


class CIISxpressTrayModule :
	public CAtlMfcModule
{

public:

	DECLARE_LIBID(LIBID_IISxpressTrayLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IISXPRESSTRAY, "{248A0297-CCD9-4FB1-8F25-D53EAFA1259C}");

};

CIISxpressTrayModule _AtlModule;

BEGIN_MESSAGE_MAP(CIISxpressTrayApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIISxpressTrayApp construction

CIISxpressTrayApp::CIISxpressTrayApp()
{
	EnableHtmlHelp();

	m_pSheet = NULL;
	m_pGeneralPage = NULL;
	m_pHistoryPage = NULL;
	m_pExclusionsPage = NULL;
	m_pConfigPage = NULL;
	m_pSystemPage = NULL;
	m_pAboutPage = NULL;
}


// The one and only CIISxpressTrayApp object

CIISxpressTrayApp theApp;


// CIISxpressTrayApp initialization

BOOL CIISxpressTrayApp::InitInstance()
{
	// dynamically link to toolhelp32
	LoadToolhelp32Procs();

	// if already running then don't go any further
	if (IsAlreadyRunning() == true)
		return FALSE;

	// set the language (if it has been overridden)
	SetLanguage();

	AfxOleInit();

	// get the OS version info
	g_VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	::GetVersionEx((OSVERSIONINFO*) &g_VerInfo);

	// *****************************************************************************

	HMODULE hPSAPI = ::LoadLibrary(_T("psapi.dll"));
	if (hPSAPI != NULL)
	{	
#ifndef _UNICODE
		g_fnGetProcessImageFileName = (FNGETPROCESSIMAGEFILENAME*) ::GetProcAddress(hPSAPI, "GetProcessImageFileNameA");
#else
		g_fnGetProcessImageFileName = (FNGETPROCESSIMAGEFILENAME*) ::GetProcAddress(hPSAPI, "GetProcessImageFileNameW");
#endif	
	}

	// *****************************************************************************

#if 0
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;

	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}

	// App was launched with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;
	}

	// App was launched with /Register or /Regserver switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
	{
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		return FALSE;
	}
#endif

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	SetRegistryKey(RIPCORD_SOFTWARE);	

	/*CIISxpressTrayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}*/

	m_pSheet = new CIISxpressTrayPropertySheet(IDS_IISXPRESSSTATUS);	

	// determine whether the user is admin or not
	BOOL bAdmin = IsUserAdmin() == 1 ? TRUE : FALSE;

	m_pGeneralPage = new CGeneralPropertyPage();
	m_pSheet->AddPage(m_pGeneralPage);

	m_pHistoryPage = new CHistoryPage();
	m_pSheet->AddPage(m_pHistoryPage);

	// only display config/system pages to admin users
	if (bAdmin == TRUE)
	{
		m_pExclusionsPage = new CExclusionsPage();
		m_pSheet->AddPage(m_pExclusionsPage);

		m_pConfigPage = new CConfigPage();
		m_pSheet->AddPage(m_pConfigPage);

		m_pSystemPage = new CSystemPage();
		m_pSheet->AddPage(m_pSystemPage);
	}

	m_pAboutPage = new CAboutPage();	
	m_pSheet->AddPage(m_pAboutPage);

	m_pSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	m_pMainWnd = m_pSheet;

	m_pSheet->Create(NULL, 
		//WS_VISIBLE |
		DS_MODALFRAME | DS_3DLOOK | DS_CONTEXTHELP | DS_SETFONT | 
		WS_POPUP | WS_CAPTION | WS_SYSMENU);	
	
	return TRUE;
}

BOOL CIISxpressTrayApp::ExitInstance(void)
{
	delete m_pAboutPage;
	delete m_pSystemPage;
	delete m_pConfigPage;
	delete m_pExclusionsPage;
	delete m_pHistoryPage;
	delete m_pGeneralPage;
	delete m_pSheet;	

	_AtlModule.RevokeClassObjects();
	return CWinApp::ExitInstance();
}


bool CIISxpressTrayApp::IsAlreadyRunning(void)
{	
	// get a lowercase version of the command line
	CString sCmdLine(this->m_lpCmdLine);
	sCmdLine.MakeLower();

	// if the user wants multiple instances then pretend it isn't already running
	if (sCmdLine.Find(_T("/allowmulti")) >= 0)
	{
		return false;
	}

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

bool CIISxpressTrayApp::IsAlreadyRunning_TH(void)
{		
	// we can't proceed unless these have loaded up, so return false
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

	// get the name of the currently executing process from the module name (stripping the path and extn)
	CPathT<CString> ModuleFilename(szModuleFilename);
	ModuleFilename.StripPath();
	ModuleFilename.RemoveExtension();
	CString sModuleFilename = ModuleFilename;

	int nFound = 0;

	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bEnum = g_pfnProcess32First(hSnapshot, &ProcessEntry);
	while (bEnum == TRUE)
	{
		// strip the path and the extension from the process entry name (needed for incompatibilities
		// between different versions of Windows
		CPathT<CString> ProcessFilename(ProcessEntry.szExeFile);
		ProcessFilename.StripPath();
		ProcessFilename.RemoveExtension();
		CString sProcessFilename = ProcessFilename;

		if (sModuleFilename.CompareNoCase(sProcessFilename) == 0)
		{			
			// if we can open the process with all access then assume we must own it, so count it as ours
			// (it's a bit brute force but seems to work)
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);
			if (hProcess != NULL)
			{				
				::CloseHandle(hProcess);

				nFound++;
			}			
		}

		bEnum = g_pfnProcess32Next(hSnapshot, &ProcessEntry);
	}

	::CloseHandle(hSnapshot);			

	return nFound > 1 ? true : false;
}

bool CIISxpressTrayApp::IsAlreadyRunning_EP(void)
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
		// use an approach the same as in IsAlreadyRunning_TH() above, i.e. ask for all rights possible,
		// if the process is the current users this will succeed, otherwise it will fail - we are
		// only interested in the current users processes after all!
		//HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessIds[i]);
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessIds[i]);
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

void CIISxpressTrayApp::SetLanguage(void)
{
	// get a lowercase version of the command line
	CString sCmdLine(this->m_lpCmdLine);
	sCmdLine.MakeLower();

	if (sCmdLine.Find(_T("/lang=fr")) >= 0)
	{
		// force French
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH), SORT_DEFAULT));
	}
	else if (sCmdLine.Find(_T("/lang=en")) >= 0)
	{
		// force English
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
	}
	else if (sCmdLine.Find(_T("/lang=")) >= 0)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_BADLANGUAGE);

		::MessageBox(::GetDesktopWindow(), sMsg, sCaption, MB_ICONEXCLAMATION | MB_OK);
	}
}