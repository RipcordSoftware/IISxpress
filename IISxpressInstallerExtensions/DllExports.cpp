// DllExports.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DllExports.h"
#include "IISxpressInstallerExtensions.h"

#include <objbase.h>
#include <atlstr.h>

#include "IISMetabase.h"
#include "ServiceNames.h"

#include "resource.h"

#define IISXPRESS_FILTERNAMEW			L"IISxpress"

// Add the IISxpress filter to the IIS configuration
// 
// incoming:
//  IValue				 : set to 0 to ignore service manager logic
//                         set to 1 to stop IIS
//                         set to 2 to stop then restart IIS if it was already running
//  lpszValue            : the full path to the filter DLL
//
// returns:
//  -1 : fatal
//   0 : failed but can continue
//   1 : succeeded
LONG APIENTRY InstallIISxpressFilter(HWND hwnd, LONG IValue, LPSTR lpszValue)
{
	::CoInitialize(NULL);

	LONG nStatus = Internal_InstallIISxpressFilter(hwnd, IValue, lpszValue);

	::CoUninitialize();

	return nStatus;
}

// Remove the IISxpress filter from the IIS configuration
//
// incoming:
//  IValue               : set to 0 to ignore service manager logic
//                         set to 1 to stop IIS
//                         set to 2 to stop then restart IIS if it was already running
//
// returns:
//  -1 : fatal
//   0 : failed but can continue
//   1 : succeeded
LONG APIENTRY RemoveIISxpressFilter(HWND hwnd, LONG IValue)
{
	::CoInitialize(NULL);

	LONG nStatus = Internal_RemoveIISxpressFilter(hwnd, IValue);

	::CoUninitialize();

	return nStatus;
}