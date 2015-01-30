// stdafx.cpp : source file that includes just the standard includes
// IISxpressServer.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CAppLog										g_Log;

OSVERSIONINFOEX								g_VerInfo;

void MyOutputDebugString(BOOL bDebugEnabled, const TCHAR* pszMsg, ...)
{	
	if (bDebugEnabled == TRUE)
	{
		va_list args;
		va_start(args, pszMsg);
		CDebugOutput::OutputDebugString(true, pszMsg, args);

		va_end(args);
	}
}

void MyOutputDebugString(IComIISxpressRegSettings* pRegSettings, const TCHAR* pszMsg, ...)
{
	BOOL bEnabled = FALSE;	
	if (pRegSettings != NULL && pRegSettings->get_DebugEnabled(&bEnabled) == S_OK && bEnabled == TRUE)
	{
		va_list args;
		va_start(args, pszMsg);
		CDebugOutput::OutputDebugString(true, pszMsg, args);

		va_end(args);
	}
}

void MyOutputLoggingString(DWORD dwCurrentLevel, DWORD dwRequiredLevel, const TCHAR* pszMsg, ...)
{	
	if (dwCurrentLevel >= dwRequiredLevel)
	{		
		va_list args;
		va_start(args, pszMsg);
		g_Log.AppendV(true, pszMsg, args);

		va_end(args);
	}
}

void MyOutputLoggingString(IComIISxpressRegSettings* pRegSettings, DWORD dwRequiredLevel, const TCHAR* pszMsg, ...)
{
	DWORD dwLevel = IISXPRESS_LOGGINGLEVEL_NONE;
	if (pRegSettings != NULL && pRegSettings->get_LoggingLevel(&dwLevel) == S_OK && dwLevel >= dwRequiredLevel)
	{
		va_list args;
		va_start(args, pszMsg);
		g_Log.AppendV(true, pszMsg, args);

		va_end(args);
	}
}