#pragma once

#include <string>
using namespace std;

#include <atlfile.h>

#ifdef _UNICODE
typedef wstring APPLOGSTRING;
#else
typedef string APPLOGSTRING;
#endif

class CAppLog
{

public:

	CAppLog(void);
	~CAppLog(void);

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

	APPLOGSTRING				m_sPath;
	APPLOGSTRING				m_sFilenamePrefix;
	APPLOGSTRING				m_sFileExtension;

	DWORD						m_dwCookie;

};
