#pragma once

class CGeoIPHelper
{
public:
	
	static LPCTSTR LookupCountry(DWORD dwAddress);
	static LPCTSTR LookupCountry(LPCSTR pszAddress);
};
