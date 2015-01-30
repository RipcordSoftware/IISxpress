#pragma once

#include <atlcoll.h>

#include "IISxpressRegSettings.h"

#include "UserAgent.h"

namespace Ripcord { namespace IISxpress {

class IISXPRESSLIB_DLL IISxpressFilterDefaultSettingsEx
{
public:
	static const int IISXPRESS_CACHE_ENABLED_DEFAULT = 1;
	static const int IISXPRESS_CACHE_MAXENTRIES_DEFAULT = 200;
	static const int IISXPRESS_CACHE_MAXSIZEKB_DEFAULT = (16 * 1024);
	static const int IISXPRESS_CACHE_STATECOOKIE_DEFAULT = 0;
	static const int IISXPRESS_PERFCOUNTERS_ENABLED_DEFAULT = 1;
	static const int IISXPRESS_HANDLEPOSTRESPONSES_DEFAULT = 0;
	static const int IISXPRESS_ENABLEUSERAGENTEXCLUSION_DEFAULT = 1;
};

class IISXPRESSLIB_DLL IISxpressRegSettingsEx :
	public IISxpressRegSettings
{

public:

	IISxpressRegSettingsEx(void);
	virtual ~IISxpressRegSettingsEx(void);

	virtual void Init(LPCSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler);
	virtual void Init(LPCWSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler);

	virtual bool Load(void);

	bool GetCompressLocalhost(void);

	BOOL GetCacheEnabled();
	void SetCacheEnabled(BOOL bEnable);

	DWORD GetCacheMaxEntries();
	void SetCacheMaxEntries(DWORD dwMaxEntries);

	DWORD GetCacheMaxSizeKB();
	void SetCacheMaxSizeKB(DWORD dwMaxSizeKB);

	DWORD GetCacheStateCookie();
	void SetCacheStateCookie(DWORD dwCookie);

	DWORD GetISAPILoadPriority(void);

	bool GetPerfCountersEnabled(void);

	bool HandlePOSTResponses(void);

	bool GetUserAgentExclusionEnabled();
	bool SetUserAgentExclusionEnabled(bool enable);

	bool GetExcludedUserAgents(CAtlArray<CAtlStringA>& agents);
	bool SetExcludedUserAgents(const CAtlArray<CAtlStringA>& agents);

	const HttpUserAgent::RuleUserAgents<std::string>& GetExcludedUserAgents();

protected:

	static void PopulateUserAgentRules(CAtlArray<CAtlStringA>& excludedAgents, HttpUserAgent::RuleUserAgents<std::string>& agentRules);

	volatile DWORD m_dwCompressLocalhost;

	volatile DWORD m_dwCacheEnabled;
	volatile DWORD m_dwCacheMaxEntries;
	volatile DWORD m_dwCacheMaxSizeKB;
	volatile DWORD m_dwCacheStateCookie;

	volatile DWORD m_dwISAPILoadPriority;

	volatile DWORD m_dwPerfCountersEnabled;

	volatile DWORD m_dwHandlePOSTResponses;

	volatile DWORD m_dwUserAgentExclusionEnabled;
	CAtlArray<CAtlStringA> m_ExcludedUserAgents;
	HttpUserAgent::RuleUserAgents<std::string> m_AgentRules;
};

}}