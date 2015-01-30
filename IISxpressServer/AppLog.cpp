#include "StdAfx.h"

#include "AppLog.h"

CAppLog::CAppLog(void)
{
	memset(&m_stFileOpened, 0, sizeof(m_stFileOpened));

	m_bInit = false;

	m_dwCookie = 0;
}

CAppLog::~CAppLog(void)
{	
}

bool CAppLog::Init(LPCTSTR pszPath, LPCTSTR pszFilenamePrefix, LPCTSTR pszFileExtension)
{
	if (pszPath == NULL || pszFilenamePrefix == NULL || pszFileExtension == NULL)
		return false;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);

	m_sPath = pszPath;

	int nPathLength = (int) _tcslen(pszPath);
	if (nPathLength > 0 && pszPath[nPathLength - 1] == '\\')
	{
		m_sPath.resize(nPathLength - 1);
	}

	m_sFilenamePrefix = pszFilenamePrefix;

	if (pszFileExtension[0] == '.')
	{
		pszFileExtension++;
	}

	m_sFileExtension = pszFileExtension;

	if (IsFileOpen())
	{
		m_File.Close();
	}

	m_bInit = true;

	return true;
}

bool CAppLog::Append(bool bTimestamp, LPCTSTR pszFormat, ...)
{
	if (GetInitFlag() == false)
		return false;

	va_list args;
	va_start(args, pszFormat);
	bool bStatus = _Append(bTimestamp, pszFormat, args);
	va_end(args);
	return bStatus;
}

bool CAppLog::AppendV(bool bTimestamp, LPCTSTR pszFormat, va_list args)
{
	return _Append(bTimestamp, pszFormat, args);
}

bool CAppLog::_Append(bool bTimestamp, LPCTSTR pszFormat, va_list args)
{
	if (GetInitFlag() == false || pszFormat == NULL)
		return false;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);

	if (!IsFileOpen())
	{
		OpenFile();
	}

	if (IsFileOpen())
	{
		if (bTimestamp == true)
		{
			SYSTEMTIME stLocalTime;
			::GetLocalTime(&stLocalTime);

			CString timestamp;
			timestamp.Format(_T("%04u-%02u-%02u %02u:%02u:%02u - "), 
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

			WriteString(timestamp);
		}

		CString data;
		data.FormatV(pszFormat, args);
		WriteString(data);		
	}	

	return true;
}

bool CAppLog::AppendFromResource(HINSTANCE hInst, int nResourceID, bool bTimestamp, ...)
{
	if (GetInitFlag() == false)
		return false;

	TCHAR szBuffer[1024];
	if (::LoadString(hInst, nResourceID, szBuffer, sizeof(szBuffer)) <= 0)
		return false;

	va_list args;
	va_start(args, bTimestamp);
	bool bStatus = _Append(bTimestamp, szBuffer, args);
	va_end(args);
	return bStatus;
}

bool CAppLog::Flush(void)
{
	bool bStatus = false;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);
	
	if (IsFileOpen())
	{
		m_File.Flush();
		bStatus = true;
	}	

	return bStatus;
}

bool CAppLog::OpenFile(void)
{
	if (GetInitFlag() == false)
		return false;	

	SYSTEMTIME stLocalTime;
	::GetLocalTime(&stLocalTime);	

	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);

	if (IsFileOpen() &&
		stLocalTime.wDay != m_stFileOpened.wDay)
	{
		m_File.Close();
	}

	if (!IsFileOpen())
	{
		TCHAR szTimestamp[64];
		_stprintf_s(szTimestamp, _T("%04u%02u%02u"), 
			stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay);

		APPLOGSTRING sLogFile = m_sPath;
		sLogFile += '\\';
		sLogFile += m_sFilenamePrefix;
		sLogFile += szTimestamp;
		sLogFile += '.';
		sLogFile += m_sFileExtension;

		if (m_File.Create(sLogFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS) == S_OK)
		{
			m_File.Seek(0, FILE_END);

			m_stFileOpened = stLocalTime;
		}
		else
		{
			memset(&m_stFileOpened, 0, sizeof(m_stFileOpened));
		}
	}

	return true;
}

bool CAppLog::DumpSysInfo(LPCTSTR pszPrefix)
{
	if (GetInitFlag() == false)
		return false;

	TCHAR szComputerName[256];
	DWORD dwComputerNameSize = sizeof(szComputerName) / sizeof(szComputerName[0]);
	::GetComputerName(szComputerName, &dwComputerNameSize);

	Append(true, _T("%s Computer name: '%s'\n"), 
		pszPrefix != NULL ? pszPrefix : _T(""),	szComputerName);

	OSVERSIONINFO VerInfo;
	VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
	::GetVersionEx(&VerInfo);

	Append(true, _T("%s Windows version: %u.%u '%s'\n"), 
		pszPrefix != NULL ? pszPrefix : _T(""),
		VerInfo.dwMajorVersion, VerInfo.dwMinorVersion, VerInfo.szCSDVersion);

	MEMORYSTATUS MemoryStatus;
	::GlobalMemoryStatus(&MemoryStatus);

	Append(true, 
		_T("%s Physical memory: %u Kb, available memory: %u Kb, virtual memory: %u Kb, available virtual memory: %u Kb\n"),
		pszPrefix != NULL ? pszPrefix : _T(""),
		MemoryStatus.dwTotalPhys >> 10, MemoryStatus.dwAvailPhys >> 10, 
		MemoryStatus.dwTotalVirtual >> 10, MemoryStatus.dwAvailVirtual >> 10);

	HKEY hCPU;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\Description\\System\\CentralProcessor\\0"), 0, KEY_READ, &hCPU) == ERROR_SUCCESS)
	{
		TCHAR szIdentifier[256] = _T("Unknown");
		DWORD dwIdentifierType = 0;
		DWORD dwIdentifierSize = sizeof(szIdentifier) / sizeof(szIdentifier[0]);
		::RegQueryValueEx(hCPU, _T("Identifier"), NULL, &dwIdentifierType, (LPBYTE) &szIdentifier[0], &dwIdentifierSize);

		TCHAR szProcessorName[256] = _T("Unknown");
		DWORD dwProcessorNameType = 0;
		DWORD dwProcessorNameSize = sizeof(szProcessorName) / sizeof(szProcessorName[0]);
		::RegQueryValueEx(hCPU, _T("ProcessorNameString"), NULL, &dwProcessorNameType, (LPBYTE) &szProcessorName[0], &dwProcessorNameSize);

		DWORD dwMHz = 0;
		DWORD dwMhzType = 0;
		DWORD dwMhzSize = sizeof(dwMHz);
		::RegQueryValueEx(hCPU, _T("~Mhz"), NULL, &dwMhzType, (LPBYTE) &dwMHz, &dwMhzSize);

		::RegCloseKey(hCPU);

		Append(true, 
			_T("%s '%s', '%s', %u Mhz\n"),	
			pszPrefix != NULL ? pszPrefix : _T(""),	szIdentifier, szProcessorName, dwMHz);
	}
	else
	{
		Append(true, _T("Unable to determine CPU information\n"));
	}

	Flush();

	return true;
}

DWORD CAppLog::GetCookie(void)
{
	return ::InterlockedIncrement((LONG*) &m_dwCookie);
}

void CAppLog::Close(void)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);	

	if (IsFileOpen())
	{
		m_File.Close();
	}

	m_bInit = false;	
}

bool CAppLog::GetInitFlag(void)
{		
	bool bStatus = m_bInit;	
	return bStatus;
}

bool CAppLog::IsFileOpen()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);
	return m_File.m_h != NULL && m_File.m_h != INVALID_HANDLE_VALUE;
}

bool CAppLog::WriteString(const CString& data)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csFile);

	CString temp(data);
	temp.Replace(_T("\n"), _T("\r\n"));
	return m_File.Write((const void*) temp, temp.GetLength() * sizeof(CString::XCHAR)) == S_OK;
}