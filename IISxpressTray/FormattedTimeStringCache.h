#pragma once

#include "AtlCache.h"

class CSystemTimeElementTraits
{
public:

	typedef const SYSTEMTIME& INARGTYPE;
	typedef SYSTEMTIME& OUTARGTYPE;

	static ULONG Hash(const SYSTEMTIME& stTime)
	{
		ULONG hash = (ULONG) stTime.wDay << 27;
		hash |= (ULONG) stTime.wHour << 22;
		hash |= (ULONG) stTime.wMinute << 17;
		hash |= (ULONG) stTime.wSecond << 11;
		hash |= (ULONG) stTime.wMilliseconds;

		return hash;
	}

	static bool CompareElements(const SYSTEMTIME& stTime1, const SYSTEMTIME& stTime2)
	{
		return memcmp(&stTime1, &stTime2, sizeof(stTime1)) == 0;
	}
};

typedef TCHAR FormattedTimeStringChar;
typedef FormattedTimeStringChar* FormattedTimeStringPtr;
typedef CMemoryCache<FormattedTimeStringPtr, CNoStatClass, COldFlusher, SYSTEMTIME, CSystemTimeElementTraits> FormattedTimeStringCacheBase;

class CFormattedTimeStringCache : public FormattedTimeStringCacheBase
{
public:
	CFormattedTimeStringCache(void);
	virtual ~CFormattedTimeStringCache(void);

	void AddEntry(const SYSTEMTIME& stTime, const CString& sTime);
	bool GetEntry(const SYSTEMTIME& stTime, CString& sTime);

protected:

	virtual void OnDestroyEntry(const NodeType* pEntry);

};
