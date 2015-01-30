#include "StdAfx.h"
#include "FormattedTimeStringCache.h"

CFormattedTimeStringCache::CFormattedTimeStringCache(void)
{
	Initialize(NULL);
	SetMaxAllowedEntries(64);
}

CFormattedTimeStringCache::~CFormattedTimeStringCache(void)
{
	FormattedTimeStringCacheBase::RemoveAllEntries();
}

void CFormattedTimeStringCache::AddEntry(const SYSTEMTIME& stTime, const CString& sTime)
{
	int length = sTime.GetLength() + 1;
	FormattedTimeStringPtr buffer = new FormattedTimeStringChar[length];
	if (buffer != NULL)
	{
		_tcscpy_s(buffer, length, sTime);
		FormattedTimeStringCacheBase::AddEntry(stTime, buffer, length);
	}
}

bool CFormattedTimeStringCache::GetEntry(const SYSTEMTIME& stTime, CString& sTime)
{
	bool status = false;

	HCACHEITEM hItem = NULL;
	if (FormattedTimeStringCacheBase::LookupEntry(stTime, &hItem) == S_OK && hItem != NULL)
	{
		FormattedTimeStringPtr buffer = NULL;
		DWORD length = 0;
		if (FormattedTimeStringCacheBase::GetEntryData(hItem, &buffer, &length) == S_OK)
		{
			sTime = buffer;
			status = true;
		}

		FormattedTimeStringCacheBase::ReleaseEntry(hItem);
	}

	return status;
}

void CFormattedTimeStringCache::OnDestroyEntry(const NodeType* pEntry)
{
	if (pEntry != NULL && pEntry->Data != NULL)
	{
		delete pEntry->Data;
	}
}