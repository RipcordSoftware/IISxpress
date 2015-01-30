#pragma once

#include "AtlCache.h"

class CNumberElementTraits
{
public:

	typedef const DWORD INARGTYPE;
	typedef DWORD OUTARGTYPE;

	static ULONG Hash(const DWORD dwNumber)
	{	
		return dwNumber;
	}

	static bool CompareElements(const DWORD& dwNum1, const DWORD& dwNum2)
	{
		return dwNum1 == dwNum2;
	}
};

typedef TCHAR FormattedNumberStringChar;
typedef FormattedNumberStringChar* FormattedNumberStringPtr;
typedef CMemoryCache<FormattedNumberStringPtr, CNoStatClass, COldFlusher, DWORD, CNumberElementTraits> FormattedNumberStringCacheBase;

class CFormattedNumberStringCache : public FormattedNumberStringCacheBase
{
public:
	CFormattedNumberStringCache(void);
	virtual ~CFormattedNumberStringCache(void);

	void AddEntry(DWORD dwNumber, const CString& sText);
	bool GetEntry(DWORD dwNumber, CString& sText);

protected:

	virtual void OnDestroyEntry(const NodeType* pEntry);
};
