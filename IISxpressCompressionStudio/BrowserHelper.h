#pragma once

class CBrowserHelper
{
public:	

	static bool IsIEInstalled();
	static bool LaunchIE(LPCTSTR pszCommandLine);

	static bool IsFirefoxInstalled();
	static bool LaunchFirefox(LPCTSTR pszCommandLine);

	static bool IsSafariInstalled();
	static bool LaunchSafari(LPCTSTR pszCommandLine);

	static bool IsOperaInstalled();
	static bool LaunchOpera(LPCTSTR pszCommandLine);

private:

	static bool GetIEPath(CString& sPath);
	static bool GetFirefoxPath(CString& sPath);
	static bool GetSafariPath(CString& sPath);
	static bool GetOperaPath(CString& sPath);

	static bool GetFixedDrives(CStringArray& saDrives);	
};
