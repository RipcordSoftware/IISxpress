#include "stdafx.h"

#include "IISxpressApp.h"

#include <atlstr.h>
#include <atlpath.h>

// The one and only CWinApp object
IISxpressHTTPApp theApp;

BOOL IISxpressHTTPApp::InitInstance(void)
{
	if (CWinApp::InitInstance() == FALSE)
	{
		return FALSE;
	}	

	// chuck the path to the module into the registry so we can find it later
	TCHAR szModuleFileName[512];
	if (::GetModuleFileName(m_hInstance, szModuleFileName, 
		sizeof(szModuleFileName) / sizeof(szModuleFileName[0])) != 0)
	{
		HKEY hReg = NULL;
		if (::RegOpenKeyW(HKEY_LOCAL_MACHINE, Ripcord::IISxpress::IISxpressRegKeys::IISXPRESSFILTER_REGKEY, &hReg) == ERROR_SUCCESS)
		{
			CPathT<CString> ModulePath(szModuleFileName);			
			ModulePath.RemoveFileSpec();

			::RegSetValueExW(hReg, Ripcord::IISxpress::IISxpressRegKeys::IISXPRESS_INSTALLPATH, 0, REG_SZ, 
				(const BYTE*) (const TCHAR*) ModulePath, 
				(DWORD) (_tcslen(ModulePath) + 1) * sizeof(TCHAR));

			::RegCloseKey(hReg);
			hReg = NULL;
		}
	}

	return TRUE;
}

int IISxpressHTTPApp::ExitInstance(void)
{		
	return CWinApp::ExitInstance();
}

// If your extension will not use MFC, you'll need this code to make
// sure the extension objects can find the resource handle for the
// module.  If you convert your extension to not be dependent on MFC,
// remove the comments around the following AfxGetResourceHandle()
// and DllMain() functions, as well as the g_hInstance global.

/****

static HINSTANCE g_hInstance;

HINSTANCE AFXISAPI AfxGetResourceHandle()
{
	return g_hInstance;
}

BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ulReason,
					LPVOID lpReserved)
{
	if (ulReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst;
	}

	return TRUE;
}

****/
