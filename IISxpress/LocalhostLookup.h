#pragma once

#include <Iphlpapi.h>

#include <set>
#include <string>

class CLocalhostLookup
{

public:

	CLocalhostLookup(void);

	~CLocalhostLookup(void);	

	bool MatchIPAddress(LPCSTR pszAddress);

private:

	static const int IPADDRESS_UPDATE_MAXINTERVAL = 30000;

	typedef DWORD __stdcall FNGETIPADDRTABLE(PMIB_IPADDRTABLE, PULONG, BOOL);

	void BindToIPLookup();
	bool GetIPAddresses();	

	CComAutoCriticalSection		m_csIPAddresses;

	HMODULE m_hIPHelpAPI;
	FNGETIPADDRTABLE* m_pfnGetIpAddrTable;
	
	volatile __int64			m_nLastIPAddressUpdate;

	typedef std::set<std::string> IPAddressCollection;
	IPAddressCollection	m_IPAddresses;
};
