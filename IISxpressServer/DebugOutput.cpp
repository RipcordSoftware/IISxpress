#include "StdAfx.h"
#include "DebugOutput.h"

CDebugOutput::CDebugOutput(void)
{
}

CDebugOutput::~CDebugOutput(void)
{
}

void CDebugOutput::OutputDebugString(bool bEnabled, const char* pszMsg, ...)
{
	if (bEnabled == true)
	{
		va_list args;
		va_start(args, pszMsg);

		static char szBuffer[16 * 1024];
		vsprintf_s(szBuffer, _countof(szBuffer), pszMsg, args);

		::OutputDebugStringA(szBuffer);

		va_end(args);
	}
}

void CDebugOutput::OutputDebugString(bool bEnabled, const WCHAR* pszMsg, ...)
{
	if (bEnabled == true)
	{
		va_list args;
		va_start(args, pszMsg);

		static WCHAR szBuffer[16 * 1024];
		vswprintf_s(szBuffer, _countof(szBuffer), pszMsg, args);

		::OutputDebugStringW(szBuffer);

		va_end(args);
	}
}

void CDebugOutput::OutputDebugString(bool bEnabled, const WCHAR* pszMsg, va_list args)
{
	if (bEnabled == true)
	{
		static WCHAR szBuffer[16 * 1024];
		vswprintf_s(szBuffer, _countof(szBuffer), pszMsg, args);

		::OutputDebugStringW(szBuffer);		
	}
}