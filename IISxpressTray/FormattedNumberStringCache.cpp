#include "StdAfx.h"
#include "FormattedNumberStringCache.h"

CFormattedNumberStringCache::CFormattedNumberStringCache(void)
{
	Initialize(NULL);
	SetMaxAllowedEntries(4196);
}

CFormattedNumberStringCache::~CFormattedNumberStringCache(void)
{
	FormattedNumberStringCacheBase::RemoveAllEntries();
}

void CFormattedNumberStringCache::AddEntry(DWORD dwNumber, const CString& sText)
{
	int length = sText.GetLength() + 1;
	FormattedNumberStringPtr buffer = new FormattedNumberStringChar[length];
	if (buffer != NULL)
	{
		_tcscpy_s(buffer, length, sText);
		FormattedNumberStringCacheBase::AddEntry(dwNumber, buffer, length);
	}
}

bool CFormattedNumberStringCache::GetEntry(DWORD dwNumber, CString& sText)
{
	bool status = false;

	HCACHEITEM hItem = NULL;
	if (FormattedNumberStringCacheBase::LookupEntry(dwNumber, &hItem) == S_OK && hItem != NULL)
	{
		FormattedNumberStringPtr buffer = NULL;
		DWORD length = 0;
		if (FormattedNumberStringCacheBase::GetEntryData(hItem, &buffer, &length) == S_OK)
		{
			sText = buffer;
			status = true;
		}

		FormattedNumberStringCacheBase::ReleaseEntry(hItem);
	}

	return status;
}

void CFormattedNumberStringCache::OnDestroyEntry(const NodeType* pEntry)
{
	if (pEntry != NULL && pEntry->Data != NULL)
	{
		delete pEntry->Data;
	}
}
