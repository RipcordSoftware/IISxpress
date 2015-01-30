#pragma once

class CDebugOutput
{

public:

	CDebugOutput(void);
	~CDebugOutput(void);

	static void OutputDebugString(bool bEnabled, const char* pszMsg, ...);	
	static void OutputDebugString(bool bEnabled, const WCHAR* pszMsg, ...);	
	static void OutputDebugString(bool bEnabled, const WCHAR* pszMsg, va_list args);	
};
