#include "StdAfx.h"
#include "GeoIPHelper.h"

#include "GeoIPData.h"

LPCTSTR CGeoIPHelper::LookupCountry(DWORD dwAddress)
{
	BYTE byFirstOctet = (BYTE) (dwAddress >> 24);

	int nStartSearch = g_nFirstOctetOffsets[byFirstOctet];
	if (nStartSearch >= 0)
	{					
		for (int i = nStartSearch; i < _countof(g_IPData); i++)
		{
			DWORD dwMaskedAddress = dwAddress & g_IPData[i].dwMask;
			if (dwMaskedAddress == g_IPData[i].dwAddress)
			{
				int nCountry = g_IPData[i].nCountry;							
				return g_pszCountryNames[nCountry];
			}
		}
	}

	return NULL;
}

LPCTSTR CGeoIPHelper::LookupCountry(LPCSTR pszAddress)
{
	if (pszAddress == NULL || strchr(pszAddress, '.') == NULL)
	{
		return NULL;
	}

	CString sClientIP(pszAddress);

	int nStart = 0;
	CString Octets[4];
	Octets[0] = sClientIP.Tokenize(_T("."), nStart);
	Octets[1] = sClientIP.Tokenize(_T("."), nStart);
	Octets[2] = sClientIP.Tokenize(_T("."), nStart);
	Octets[3] = sClientIP.Tokenize(_T(""), nStart);

	DWORD dwFirstOctet = _ttoi(Octets[0]);

	DWORD dwAddress = dwFirstOctet; dwAddress <<= 8;
	dwAddress |= _ttoi(Octets[1]); dwAddress <<= 8;
	dwAddress |= _ttoi(Octets[2]); dwAddress <<= 8;
	dwAddress |= _ttoi(Octets[3]);

	return LookupCountry(dwAddress);
}