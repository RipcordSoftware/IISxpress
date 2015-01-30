#pragma once

#ifdef _MANAGED
typedef unsigned int UINT32;
typedef wchar_t* STRINGPTR;
#define _T(x) L ## x
#else
typedef unsigned int UINT32;
typedef TCHAR* STRINGPTR;
#endif

struct IPData { DWORD dwAddress; DWORD dwMask; int nCountry; };

extern const TCHAR* const g_pszCountryCodes[203];
extern const TCHAR* const g_pszCountryNames[203];
extern const IPData g_IPData[88225];
extern const int g_nFirstOctetOffsets[256];
