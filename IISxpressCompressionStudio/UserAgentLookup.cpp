#include "StdAfx.h"
#include "UserAgentLookup.h"

struct UserAgentInitData g_UAInitData[] = 
{ 
	// *** Google Chrome (needs to go above Safari)
	{ "Mozilla/5 Chrome/5.",					"Google Chrome 5.x", NULL },
	{ "Mozilla/5 Chrome/4.",					"Google Chrome 4.x", NULL },
	{ "Mozilla/5 Chrome/3.",					"Google Chrome 3.x", NULL },
	{ "Mozilla/5 Chrome/2.",					"Google Chrome 2.x", NULL },
	{ "Mozilla/5 Chrome/1.",					"Google Chrome 1.x", NULL },
	{ "Mozilla/5 Chrome/",						"Google Chrome", NULL },

	// Safari (Apple)
	{ "Mozilla/5 Version/3 Safari/ Mobile/",	"Safari 3.0 on iPhone", NULL },
	{ "Mozilla/ Safari/ Mobile/",				"Safari on iPhone", NULL },
	{ "Mozilla/5 Version/5 Safari/",			"Safari 5.0", NULL },
	{ "Mozilla/5 Version/4 Safari/",			"Safari 4.0", NULL },
	{ "Mozilla/5 Version/3 Safari/",			"Safari 3.0", NULL },
	{ "Mozilla/5 Safari/41",					"Safari 2.0", NULL },
	{ "Mozilla/5 Safari/312",					"Safari 1.3", NULL },
	{ "Mozilla/5 Safari/125",					"Safari 1.2", NULL },
	{ "Mozilla/5 Safari/100",					"Safari 1.1", NULL },
	{ "Mozilla/5 Safari/85",					"Safari 1.0", NULL },	
	{ "Mozilla/ Safari/",						"Safari", NULL },

	// *** Opera in IE emulation mode
	{ "Mozilla/ (MSIE 8.) Opera",				"Opera as MSIE 8", "Opera as Internet Explorer 8" },
	{ "Mozilla/ (MSIE 7.) Opera",				"Opera as MSIE 7", "Opera as Internet Explorer 7" },
	{ "Mozilla/4 (MSIE 6.) Opera",				"Opera as MSIE 6", "Opera as Internet Explorer 6" },
	{ "Mozilla/4 (MSIE 5.) Opera",				"Opera as MSIE", "Opera as Internet Explorer" },

	// *** IE
	{ "Mozilla/ (MSIE 9.)",						"MSIE 9.x", "Internet Explorer 9" },
	{ "Mozilla/ (MSIE 8.)",						"MSIE 8.x", "Internet Explorer 8" },
	{ "Mozilla/ (MSIE 7.)",						"MSIE 7.x", "Internet Explorer 7" },
	{ "Mozilla/4 (MSIE 6.; SV1)",				"MSIE 6.0 SV1", "Internet Explorer 6 SV1" },
	{ "Mozilla/4 (MSIE 6.)",					"MSIE 6.0", "Internet Explorer 6" },
	{ "Mozilla/4 (MSIE 5.5)",					"MSIE 5.5", "Internet Explorer 5.5" },
	{ "Mozilla/4 (MSIE 5.)",					"MSIE 5.x", "Internet Explorer 5" },
	{ "Mozilla/ (MSIE 4.)",						"MSIE 4.", "Internet Explorer 4" },
	{ "Mozilla/ (MSIE)",						"MSIE", "Internet Explorer" },

	// *** Netscape Navigator
	{ "Mozilla/5 Navigator/9",					"Netscape 9.x", NULL },
	{ "Mozilla/5 Netscape/8.1",					"Netscape 8.1", NULL },
	{ "Mozilla/5 Netscape/8.0",					"Netscape 8.0", NULL },
	{ "Mozilla/5 Netscape/8.",					"Netscape 8.x", NULL },
	{ "Mozilla/5 Netscape/7.",					"Netscape 7.x", NULL },
	{ "Mozilla/5 Netscape6/",					"Netscape 6.x", NULL },
	{ "Mozilla/ Netscape",						"Netscape", NULL },

	// *** Firefox	
	{ "Mozilla/ Gecko/ Firefox/4.",				"Firefox 4.x", NULL },
	{ "Mozilla/ Gecko/ Firefox/3.6",			"Firefox 3.6", NULL },
	{ "Mozilla/ Gecko/ Firefox/3.5",			"Firefox 3.5", NULL },
	{ "Mozilla/ Gecko/ Firefox/3.0",			"Firefox 3.0", NULL },
	{ "Mozilla/ Gecko/ Firefox/3.",				"Firefox 3.x", NULL },
	{ "Mozilla/ Gecko/ Firefox/2.",				"Firefox 2.x", NULL },	
	{ "Mozilla/5 Gecko/ Firefox/1.",			"Firefox 1.x", NULL },
	{ "Mozilla/5 Gecko/ Firefox/",				"Firefox", NULL },	

	// *** SeaMonkey
	{ "Mozilla/5 SeaMonkey/1.1",				"SeaMonkey 1.1", NULL },
	{ "Mozilla/5 SeaMonkey/1.",					"SeaMonkey 1.x", NULL },
	{ "Mozilla/5 SeaMonkey/",					"SeaMonkey", NULL },

	// *** Mozilla
	{ "Mozilla/ (rv:2.) Gecko/2",				"Mozilla 2.x", NULL },
	{ "Mozilla/5 (rv:1.9.) Gecko/2",			"Mozilla 1.9.x", NULL },
	{ "Mozilla/5 (rv:1.8.) Gecko/2",			"Mozilla 1.8.x", NULL },
	{ "Mozilla/5 (rv:1.7.) Gecko/2",			"Mozilla 1.7.x", NULL },
	{ "Mozilla/5 (rv:1.6.) Gecko/2",			"Mozilla 1.6.x", NULL },
	{ "Mozilla/5 (rv:1.5.) Gecko/2",			"Mozilla 1.5.x", NULL },
	{ "Mozilla/5 (rv:1.4.) Gecko/2",			"Mozilla 1.4.x", NULL },
	{ "Mozilla/5 (rv:1.3.) Gecko/2",			"Mozilla 1.3.x", NULL },
	{ "Mozilla/5 (rv:1.2.) Gecko/2",			"Mozilla 1.2.x", NULL },
	{ "Mozilla/5 (rv:1.1.) Gecko/2",			"Mozilla 1.1.x", NULL },
	{ "Mozilla/5 (rv:1.) Gecko/2",				"Mozilla 1.x", NULL },
	{ "Mozilla/5 (rv:0.) Gecko/2",				"Mozilla 0.x", NULL },
	{ "Mozilla/5 Gecko/2",						"Mozilla", NULL },		

	// *** Opera
	{ "Opera/10.",								"Opera 10.x", NULL },
	{ "Opera/9.",								"Opera 9.x", NULL },
	{ "Opera/8.5",								"Opera 8.5.x", NULL },
	{ "Opera/8.",								"Opera 8.x", NULL },
	{ "Opera/7.",								"Opera 7.x", NULL },
	{ "Opera/6.",								"Opera 6.x", NULL },
	{ "Opera",									"Opera", NULL },

	// *** Yahoo! Slurp
	{ "Mozilla/ (Yahoo!)",						"Yahoo! Slurp", NULL },

	// *** Googlebot
	{ "Mozilla/ (Googlebot/)",					"Googlebot", NULL },

	// *** MSNbot
	{ "msnbot/",								"MSNBot", NULL },

	// *** Ask Jeeves
	{ "Mozilla/ (Ask Jeeves)",					"Ask Jeeves", NULL },
};

const int g_numberOfUserAgents = sizeof(g_UAInitData) / sizeof(g_UAInitData[0]);

CUserAgentLookup::CUserAgentLookup(void)
{
	InitUserAgents();
}

void CUserAgentLookup::InitUserAgents(void)
{
	UserAgents.clear();
	UserAgentNames.clear();

	int nUserAgentInitDataSize = g_numberOfUserAgents;
	for (int i = 0; i < nUserAgentInitDataSize; i++)
	{
		HttpUserAgent::UserAgentProducts<std::string> UserAgent;
		if (UserAgent.ParseUserAgentString(g_UAInitData[i].pszUserAgentString) == S_OK)
		{
			UserAgents.push_back(UserAgent);
			UserAgentNames.push_back(g_UAInitData[i].pszUserAgentName);
		}
	}
}

bool CUserAgentLookup::GetUserAgentName(LPCSTR pszUserAgent, CString& sUserAgentName)
{
	sUserAgentName.Empty();

	if (pszUserAgent == NULL)
		return false;

	HttpUserAgent::UserAgentProducts<std::string> UserAgent;
	if (UserAgent.ParseUserAgentString(pszUserAgent) != S_OK)
		return false;

	int nUserAgents = (int) UserAgents.size();
	for (int i = 0; i < nUserAgents; i++)
	{
		if (UserAgents[i].Compare(UserAgent) == true)
		{
			sUserAgentName = UserAgentNames[i];
			return true;
		}
	}

	return false;
}