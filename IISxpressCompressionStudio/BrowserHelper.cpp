#include "StdAfx.h"
#include "BrowserHelper.h"

bool CBrowserHelper::IsIEInstalled()
{
	return true;
}

bool CBrowserHelper::LaunchIE(LPCTSTR pszCommandLine)
{
	HINSTANCE hInst = ::ShellExecute(NULL, _T("open"), _T("iexplore.exe"), pszCommandLine, NULL, SW_SHOW);
	if (hInst <= (HINSTANCE) HINSTANCE_ERROR)
	{
		CString sPath;
		if (GetIEPath(sPath) == false)
		{
			return false;
		}

		hInst = ::ShellExecute(NULL, _T("open"), sPath, pszCommandLine, NULL, SW_SHOW);
	}

	return hInst > (HINSTANCE) HINSTANCE_ERROR;
}

bool CBrowserHelper::IsFirefoxInstalled()
{	
	CString sPath;
	return GetFirefoxPath(sPath);
}

bool CBrowserHelper::LaunchFirefox(LPCTSTR pszCommandLine)
{
	CString sPath;
	if (GetFirefoxPath(sPath) == false)
	{
		return false;
	}

	HINSTANCE hInst = ::ShellExecute(NULL, _T("open"), sPath, pszCommandLine, NULL, SW_SHOW);

	return hInst > (HINSTANCE) HINSTANCE_ERROR;
}

bool CBrowserHelper::IsSafariInstalled()
{
	CString sPath;
	return GetSafariPath(sPath);
}

bool CBrowserHelper::LaunchSafari(LPCTSTR pszCommandLine)
{
	CString sPath;
	if (GetSafariPath(sPath) == false)
	{
		return false;
	}	

	CString sCommandLine(pszCommandLine);
	if (_tcsstr(pszCommandLine, _T("http")) == pszCommandLine)
	{
		sCommandLine = _T("-url ") + sCommandLine;
	}

	HINSTANCE hInst = ::ShellExecute(NULL, _T("open"), sPath, sCommandLine, NULL, SW_SHOW);

	return hInst > (HINSTANCE) HINSTANCE_ERROR;
}

bool CBrowserHelper::IsOperaInstalled()
{
	CString sPath;
	return GetOperaPath(sPath);
}

bool CBrowserHelper::LaunchOpera(LPCTSTR pszCommandLine)
{
	CString sPath;
	if (GetOperaPath(sPath) == false)
	{
		return false;
	}	

	HINSTANCE hInst = ::ShellExecute(NULL, _T("open"), sPath, pszCommandLine, NULL, SW_SHOW);

	return hInst > (HINSTANCE) HINSTANCE_ERROR;
}

bool CBrowserHelper::GetFixedDrives(CStringArray& saDrives)
{
	saDrives.RemoveAll();

	TCHAR szDriveStrings[256] = _T("");
	DWORD dwDriveStringLength = ::GetLogicalDriveStrings(_countof(szDriveStrings), szDriveStrings);	
	if (dwDriveStringLength == 0 || dwDriveStringLength > _countof(szDriveStrings))
	{
		return false;
	}

	const TCHAR* pszDrive = szDriveStrings;
	while (dwDriveStringLength > 0)
	{
		DWORD dwDriveLength = (DWORD) _tcslen(pszDrive) + 1;

		UINT nDriveType = ::GetDriveType(pszDrive);
		if (nDriveType == DRIVE_FIXED)
		{
			saDrives.Add(pszDrive);
		}

		pszDrive += dwDriveLength;
		dwDriveStringLength -= dwDriveLength;
	}

	return true;
}

bool CBrowserHelper::GetIEPath(CString& sPath)
{
	CStringArray saDrives;
	GetFixedDrives(saDrives);

	for (int i = 0; i < saDrives.GetCount(); i++)
	{
		CPath path(saDrives[i]);
		path.Append(_T("\\Program Files\\Internet Explorer\\iexplore.exe"));		

		DWORD dwAttributes = ::GetFileAttributes(path);
		if (dwAttributes == INVALID_FILE_ATTRIBUTES)
			continue;

		sPath = CString(path);
		return true;
	}

	return false;
}

bool CBrowserHelper::GetFirefoxPath(CString& sPath)
{
	CRegKey reg;
	if (reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Mozilla\\Mozilla Firefox"), KEY_READ) != ERROR_SUCCESS)
	{
		return false;
	}

	TCHAR szCurrentVersion[64] = _T("");
	ULONG nCurrentVersionChars = _countof(szCurrentVersion);
	if (reg.QueryStringValue(_T("CurrentVersion"), szCurrentVersion, &nCurrentVersionChars) != ERROR_SUCCESS)
	{
		return false;
	}
	
	CString sMainKey(szCurrentVersion);
	sMainKey += _T("\\Main");

	CRegKey mainReg;
	if (mainReg.Open(reg, sMainKey, KEY_READ) != ERROR_SUCCESS)
	{
		return false;
	}

	TCHAR szPath[MAX_PATH] = _T("");
	ULONG nPathChars = _countof(szPath);
	if (mainReg.QueryStringValue(_T("PathToExe"), szPath, &nPathChars) != ERROR_SUCCESS)
	{
		return false;
	}

	sPath = szPath;

	return true;
}

bool CBrowserHelper::GetSafariPath(CString& sPath)
{
	CStringArray saDrives;
	GetFixedDrives(saDrives);

	for (int i = 0; i < saDrives.GetCount(); i++)
	{
		CPath path(saDrives[i]);
		path.Append(_T("\\Program Files\\Safari\\Safari.exe"));		

		DWORD dwAttributes = ::GetFileAttributes(path);
		if (dwAttributes == INVALID_FILE_ATTRIBUTES)
			continue;

		sPath = CString(path);
		return true;
	}

	return false;
}

bool CBrowserHelper::GetOperaPath(CString& sPath)
{
	CStringArray saDrives;
	GetFixedDrives(saDrives);

	for (int i = 0; i < saDrives.GetCount(); i++)
	{
		CPath path(saDrives[i]);
		path.Append(_T("\\Program Files\\Opera\\Opera.exe"));		

		DWORD dwAttributes = ::GetFileAttributes(path);
		if (dwAttributes == INVALID_FILE_ATTRIBUTES)
			continue;

		sPath = CString(path);
		return true;
	}

	return false;
}