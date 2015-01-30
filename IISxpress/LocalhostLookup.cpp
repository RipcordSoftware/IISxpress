#include "StdAfx.h"
#include "LocalhostLookup.h"

CLocalhostLookup::CLocalhostLookup(void) :
	m_hIPHelpAPI(NULL), m_pfnGetIpAddrTable(NULL), m_nLastIPAddressUpdate(0)
{
	BindToIPLookup();
}

CLocalhostLookup::~CLocalhostLookup(void)
{
	CComCritSecLock<CComAutoCriticalSection> cs(m_csIPAddresses);

	if (m_hIPHelpAPI != NULL)
	{
		m_pfnGetIpAddrTable = NULL;
		::FreeLibrary(m_hIPHelpAPI);
		m_hIPHelpAPI = NULL;
	}
}

void CLocalhostLookup::BindToIPLookup()
{
	CComCritSecLock<CComAutoCriticalSection> cs(m_csIPAddresses);

	if (m_hIPHelpAPI == NULL)
	{		
		// bind to GetIpAddrTable in iphlpapi.dll
		m_hIPHelpAPI = ::LoadLibraryA("Iphlpapi.dll");	
		if (m_hIPHelpAPI != NULL)
		{	
			m_pfnGetIpAddrTable = (FNGETIPADDRTABLE*) ::GetProcAddress(m_hIPHelpAPI, "GetIpAddrTable");		
		}
	}
}

bool CLocalhostLookup::GetIPAddresses()
{
	if (m_pfnGetIpAddrTable == NULL)
	{
		return false;		
	}

	bool status = false;

	__int64 nNow = g_Timer.GetTimerCount();

	__int64 nLastIPAddressUpdate = m_nLastIPAddressUpdate;

	// load the IP Addresses if the interval has passed
	if ((nLastIPAddressUpdate + IPADDRESS_UPDATE_MAXINTERVAL) < nNow)
	{
		CComCritSecLock<CComAutoCriticalSection> cs(m_csIPAddresses);

		if ((nLastIPAddressUpdate + IPADDRESS_UPDATE_MAXINTERVAL) < nNow)
		{
			m_nLastIPAddressUpdate = nNow;

			m_IPAddresses.clear();

			BYTE byIPAddrTable[512];
			PMIB_IPADDRTABLE pIPAddrTable = (PMIB_IPADDRTABLE) byIPAddrTable;
			ULONG nIPAddrTableSize = sizeof(byIPAddrTable);
			if (m_pfnGetIpAddrTable(pIPAddrTable, &nIPAddrTableSize, FALSE) == NO_ERROR)
			{
				for (DWORD i = 0; i < pIPAddrTable->dwNumEntries; i++)
				{
					CStringA sIPAddress;
					sIPAddress.Format("%u.%u.%u.%u", 
						pIPAddrTable->table[i].dwAddr & 0x000000ff,
						(pIPAddrTable->table[i].dwAddr & 0x0000ff00) >> 8,
						(pIPAddrTable->table[i].dwAddr & 0x00ff0000) >> 16,
						(pIPAddrTable->table[i].dwAddr & 0xff000000) >> 24);

					m_IPAddresses.insert((const char*) sIPAddress);
				}	

				status = true;
			}		
		}		
	}

	return status;
}

bool CLocalhostLookup::MatchIPAddress(LPCSTR pszAddress)
{
	if (pszAddress == NULL || !GetIPAddresses())
	{
		return false;
	}

	bool found = false;
	{
		CComCritSecLock<CComAutoCriticalSection> cs(m_csIPAddresses);

		IPAddressCollection::const_iterator iter = m_IPAddresses.find(pszAddress);
		found = (iter != m_IPAddresses.end());
	}

	return found;
}