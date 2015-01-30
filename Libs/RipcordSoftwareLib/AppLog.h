#pragma once

#include <string>
#include <atlfile.h>

#include "tstring.h"

namespace Ripcord { namespace Utils
{

class RIPCORDSOFTWARELIB_DLL AppLog
{

public:

	AppLog(void);
	~AppLog(void);

	bool Init(LPCTSTR pszPath, LPCTSTR pszFilenamePrefix, LPCTSTR pszFileExtension);

	bool Append(bool bTimestamp, LPCTSTR pszFormat, ...);	
	bool AppendV(bool bTimestamp, LPCTSTR pszFormat, va_list args);	

	bool AppendFromResource(HINSTANCE hInst, int nResourceID, bool bTimestamp, ...);

	bool Flush(void);
	void Close(void);

	bool DumpSysInfo(LPCTSTR pszPrefix = NULL);

	DWORD GetCookie(void);

private:

	bool OpenFile(void);
	bool _Append(bool bTimestamp, LPCTSTR pszFormat, va_list args);	

	bool GetInitFlag(void);

	bool IsFileOpen();
	bool WriteString(const CString& data);

	CComAutoCriticalSection		m_csFile;

	CAtlFile					m_File;
	volatile bool				m_bInit;
	SYSTEMTIME					m_stFileOpened;

	Ripcord::Types::tstring		m_sPath;
	Ripcord::Types::tstring		m_sFilenamePrefix;
	Ripcord::Types::tstring		m_sFileExtension;

	DWORD						m_dwCookie;

};
}}