#pragma once

#include "UserAgent.h"

class CUserAgentLookup
{
public:
	CUserAgentLookup(void);	
	
	bool GetUserAgentName(LPCSTR pszUserAgent, CString& sUserAgentName);

private:

	void InitUserAgents(void);

	std::vector<HttpUserAgent::UserAgentProducts<std::string> >	UserAgents;
	std::vector<CStringA>										UserAgentNames;
};

struct UserAgentInitData
{
	LPCSTR		pszUserAgentString;
	LPCSTR		pszUserAgentName;
	LPCSTR		pszUserAgentLongName;
};

extern struct UserAgentInitData g_UAInitData[];

extern const int g_numberOfUserAgents;