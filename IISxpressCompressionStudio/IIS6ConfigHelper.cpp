#include "StdAfx.h"
#include "IIS6ConfigHelper.h"

bool CIIS6ConfigHelper::GetWebServerHostnameAndPort(
												IMSAdminBase* pAdminBase, 
												LPCTSTR pszMBPath, 
												CString& sHostname,
												DWORD& dwPort)												
{
	if (pAdminBase == NULL)
		return false;

	CIISMetaBaseData PortData;
	std::vector<std::wstring> saPorts;
	if (PortData.ReadData(pAdminBase, CStringW(pszMBPath), MD_SERVER_BINDINGS) == false ||
		PortData.GetAsStringVector(saPorts) == false)
		return false;
	
	for (int i = 0; i < (int) saPorts.size(); i++)
	{
		CString sPort(saPorts[i].c_str());		

		if (sPort.GetLength() == 0 || sPort[0] != ':')
			continue;		

		LPCTSTR pszToken = sPort;

		// skip the leading ':'
		pszToken++;

		// get the port
		dwPort = _ttoi(pszToken);

		// skip the number characters
		while (*pszToken != '\0' && _istdigit(*pszToken) != 0)
			pszToken++;

		// we must find the ':' here
		if (*pszToken != ':')
			continue;

		// skip the ':'
		pszToken++;

		// get the hostname
		sHostname = pszToken;

		return true;
	}

	return false;
}