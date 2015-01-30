#pragma once

class CIIS6ConfigHelper
{
public:

	static bool GetWebServerHostnameAndPort(IMSAdminBase* pAdminBase, 
										LPCTSTR pszMBPath, 
										CString& sHostname,
										DWORD& dwPort);
												
};
