// IISxpressIIS7ModuleInstaller.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "IISxpressIIS7ModuleInstaller.h"

#include <Shellapi.h>

#include "IISxpressInstallerExtensions.h"

const TCHAR g_pszInstallIIS7NativeModule[] = _T("/installiis7nativemodule=");
const size_t g_nInstallIIS7NativeModuleLength = _countof(g_pszInstallIIS7NativeModule) - 1;

const TCHAR g_pszUninstallIIS7NativeModule[] = _T("/uninstalliis7nativemodule");
const size_t g_nUninstallIIS7NativeModuleLength = _countof(g_pszUninstallIIS7NativeModule) - 1;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);		

	OSVERSIONINFO OSVersionInfo;
	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
	::GetVersionEx(&OSVersionInfo);

	if (OSVersionInfo.dwMajorVersion >= 6)
	{	
		::CoInitialize(NULL);		

		if (_tcsncmp(lpCmdLine, g_pszInstallIIS7NativeModule, g_nInstallIIS7NativeModuleLength) == 0)
		{		
			TCHAR* pszModulePath = lpCmdLine + g_nInstallIIS7NativeModuleLength;
			if (pszModulePath[0] != '\0')
			{
				CAtlStringA sModulePath(pszModulePath);				
				LONG nStatus = Internal_InstallIISxpressFilter_IIS7(::GetDesktopWindow(), 0, sModulePath, 0);			
				return nStatus == 1 ? 0 : 2;
			}
		}
		else if (_tcsncmp(lpCmdLine, g_pszUninstallIIS7NativeModule, g_nUninstallIIS7NativeModuleLength) == 0)
		{
			return Internal_RemoveIISxpressFilter_IIS7(::GetDesktopWindow(), 0) == 1 ? 0 : 3;
		}	
	}

	return 1;
}
