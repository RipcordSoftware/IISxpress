#pragma once

#include <atlcoll.h>

class CTransientResponseHistory
{

public:

	typedef struct tagResponseHistory
	{
		string	sContentType;
		DWORD	dwContentLength;
	} ResponseHistory;

	CTransientResponseHistory(void);
	~CTransientResponseHistory(void);

	bool SetEntry(LPCSTR pszFilepath, LPCSTR pszContentType, DWORD dwContentLength);
	bool GetEntry(LPCSTR pszFilepath, ResponseHistory& History);

private:	

	CComAutoCriticalSection					m_csHistory;
	CAtlMap<CStringA, ResponseHistory>		m_History;
};
