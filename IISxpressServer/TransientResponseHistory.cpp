#include "StdAfx.h"
#include "TransientResponseHistory.h"

#include "AutoMemberCriticalSection.h"

CTransientResponseHistory::CTransientResponseHistory(void)
{	
}

CTransientResponseHistory::~CTransientResponseHistory(void)
{
}

bool CTransientResponseHistory::SetEntry(LPCSTR pszFilepath, LPCSTR pszContentType, DWORD dwContentLength)
{
	if (pszFilepath == NULL || pszContentType == NULL)
		return false;

	ResponseHistory History;
	History.sContentType = pszContentType;
	History.dwContentLength = dwContentLength;

	AutoMemberCriticalSection lock(&m_csHistory);
	m_History[pszFilepath] = History;
	return true;
}

bool CTransientResponseHistory::GetEntry(LPCSTR pszFilepath, ResponseHistory& History)
{
	if (pszFilepath == NULL)
		return false;

	if (m_History.Lookup(pszFilepath, History) == FALSE)	
		return false;

	return true;
}
