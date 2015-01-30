#include "StdAfx.h"
#include "iisxpressregsettingsex.h"

#include "AutoArray.h"

#include "RegistryKeys.h"

#ifndef SF_NOTIFY_ORDER_LOW
#pragma message("SF_NOTIFY_ORDER_LOW has not been defined, setting to 0")
#define SF_NOTIFY_ORDER_LOW 0
#endif

CIISxpressRegSettingsEx::CIISxpressRegSettingsEx(void)
{
	m_dwCompressLocalhost = 0;

	m_dwCacheEnabled = IISXPRESS_CACHE_ENABLED_DEFAULT;
	m_dwCacheMaxEntries = IISXPRESS_CACHE_MAXENTRIES_DEFAULT;
	m_dwCacheMaxSizeKB = IISXPRESS_CACHE_MAXSIZEKB_DEFAULT;
	m_dwCacheStateCookie = IISXPRESS_CACHE_STATECOOKIE_DEFAULT;

	m_dwISAPILoadPriority = SF_NOTIFY_ORDER_LOW;

	m_dwPerfCountersEnabled = IISXPRESS_PERFCOUNTERS_ENABLED_DEFAULT;

	m_dwHandlePOSTResponses = IISXPRESS_HANDLEPOSTRESPONSES_DEFAULT;

	m_dwUserAgentExclusionEnabled = IISXPRESS_ENABLEUSERAGENTEXCLUSION_DEFAULT;
}

CIISxpressRegSettingsEx::~CIISxpressRegSettingsEx(void)
{
}

void CIISxpressRegSettingsEx::Init(LPCTSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	CIISxpressRegSettings::Init(pszRegPath, bLoad, bRegisterChangeHandler);
}

bool CIISxpressRegSettingsEx::Load(void)
{
	bool bStatus = CIISxpressRegSettings::Load();

	if (bStatus == true)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_READ | m_dwOpenFlags) == ERROR_SUCCESS)
		{								
			DWORD dwCompressLocalhost = 0;			
			Config.QueryDWORDValue(IISXPRESS_COMPRESSLOCALHOST, dwCompressLocalhost);
			::InterlockedExchange((volatile LONG*) &m_dwCompressLocalhost, dwCompressLocalhost);

			DWORD dwCacheEnabled = IISXPRESS_CACHE_ENABLED_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_CACHE_ENABLED, dwCacheEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwCacheEnabled, dwCacheEnabled);

			DWORD dwCacheMaxEntries = IISXPRESS_CACHE_MAXENTRIES_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_CACHE_MAXENTRIES, dwCacheMaxEntries);
			::InterlockedExchange((volatile LONG*) &m_dwCacheMaxEntries, dwCacheMaxEntries);

			DWORD dwCacheMaxSizeKB = IISXPRESS_CACHE_MAXSIZEKB_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_CACHE_MAXSIZEKB, dwCacheMaxSizeKB);
			::InterlockedExchange((volatile LONG*) &m_dwCacheMaxSizeKB, dwCacheMaxSizeKB);

			DWORD dwCacheStateCookie = IISXPRESS_CACHE_STATECOOKIE_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_CACHE_STATECOOKIE, dwCacheStateCookie);
			::InterlockedExchange((volatile LONG*) &m_dwCacheStateCookie, dwCacheStateCookie);

			DWORD dwISAPILoadPriority = SF_NOTIFY_ORDER_LOW;
			Config.QueryDWORDValue(IISXPRESS_ISAPILOADPRIORITY, dwISAPILoadPriority);
			::InterlockedExchange((volatile LONG*) &m_dwISAPILoadPriority, dwISAPILoadPriority);

			DWORD dwPerfCountersEnabled = IISXPRESS_PERFCOUNTERS_ENABLED_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_PERFCOUNTERS_ENABLED, dwPerfCountersEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwPerfCountersEnabled, dwPerfCountersEnabled);			

			DWORD dwHandlePOSTResponses = IISXPRESS_HANDLEPOSTRESPONSES_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_HANDLEPOSTRESPONSES, dwHandlePOSTResponses);
			::InterlockedExchange((volatile LONG*) &m_dwHandlePOSTResponses, dwHandlePOSTResponses);			

			DWORD dwUserAgentExclusionEnabled = IISXPRESS_ENABLEUSERAGENTEXCLUSION_DEFAULT;
			Config.QueryDWORDValue(IISXPRESS_ENABLEUSERAGENTEXCLUSION, dwUserAgentExclusionEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwUserAgentExclusionEnabled, dwUserAgentExclusionEnabled);			

			TCHAR szUserAgentBuffer[4096] = _T("");
			DWORD dwUserAgentBufferSize = _countof(szUserAgentBuffer) - 1;
			CAtlArray<CAtlStringA> excludedUserAgents;
			if (Config.QueryMultiStringValue(IISXPRESS_EXCLUDEDUSERAGENTS, szUserAgentBuffer, &dwUserAgentBufferSize) == ERROR_SUCCESS && dwUserAgentBufferSize > 0)
			{
				TCHAR* pszUserAgent = szUserAgentBuffer;

				while (*pszUserAgent != '\0')
				{
					int userAgentLength = (int) _tcslen(pszUserAgent);

					if (userAgentLength > 0)
					{
						excludedUserAgents.Add(CAtlStringA(pszUserAgent));
					}

					// skip to the beginning of the next string
					pszUserAgent += userAgentLength + 1;
				}
			}

			m_ExcludedUserAgents.Copy(excludedUserAgents);

			PopulateUserAgentRules(m_ExcludedUserAgents, m_AgentRules);
		}
	}

	return bStatus;
}

bool CIISxpressRegSettingsEx::GetCompressLocalhost(void) 
{ 
	// we don't need an interlock since the set/load will do it for us
	//DWORD dwCompressLocalhost = (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwCompressLocalhost, 0);

	return m_dwCompressLocalhost != 0 ? true : false;
}

DWORD CIISxpressRegSettingsEx::GetISAPILoadPriority(void)
{
	return m_dwISAPILoadPriority;
}

BOOL CIISxpressRegSettingsEx::GetCacheEnabled()
{
	return m_dwCacheEnabled != 0 ? TRUE : FALSE;
}

void CIISxpressRegSettingsEx::SetCacheEnabled(BOOL bEnable)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{								
		DWORD dwCacheEnabled = (DWORD) bEnable;
		if (Config.SetDWORDValue(IISXPRESS_CACHE_ENABLED, dwCacheEnabled) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheEnabled, dwCacheEnabled);
		}
	}
}

DWORD CIISxpressRegSettingsEx::GetCacheMaxEntries()
{
	return m_dwCacheMaxEntries;
}

void CIISxpressRegSettingsEx::SetCacheMaxEntries(DWORD dwMaxEntries)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISXPRESS_CACHE_MAXENTRIES, dwMaxEntries) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheMaxEntries, dwMaxEntries);
		}
	}
}

DWORD CIISxpressRegSettingsEx::GetCacheMaxSizeKB()
{
	return m_dwCacheMaxSizeKB;
}

void CIISxpressRegSettingsEx::SetCacheMaxSizeKB(DWORD dwMaxSizeKB)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISXPRESS_CACHE_MAXSIZEKB, dwMaxSizeKB) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheMaxSizeKB, dwMaxSizeKB);
		}
	}
}

DWORD CIISxpressRegSettingsEx::GetCacheStateCookie()
{
	return m_dwCacheStateCookie;
}

void CIISxpressRegSettingsEx::SetCacheStateCookie(DWORD dwCookie)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISXPRESS_CACHE_STATECOOKIE, dwCookie) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheStateCookie, dwCookie);
		}
	}
}

bool CIISxpressRegSettingsEx::GetPerfCountersEnabled(void)
{
	return m_dwPerfCountersEnabled != 0 ? true : false;
}

bool CIISxpressRegSettingsEx::HandlePOSTResponses(void)
{
	return m_dwHandlePOSTResponses != 0 ? true : false;
}

bool CIISxpressRegSettingsEx::GetExcludedUserAgents(CAtlArray<CAtlStringA>& agents)
{
	agents.Copy(m_ExcludedUserAgents);
	return true;
}

bool CIISxpressRegSettingsEx::SetExcludedUserAgents(const CAtlArray<CAtlStringA>& agents)
{
	bool status = false;

	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{	
		int bufferSize = 0;
		for (int i = 0; i < (int) agents.GetCount(); i++)
		{
			bufferSize += agents[i].GetLength();
			bufferSize++;
		}

		bufferSize++;

		// there must be at least 2 characters for the "\0\0"
		if (bufferSize < 2)
		{
			bufferSize = 2;
		}

		// keep track of the amount of buffer available
		int bufferAvailable = bufferSize;

		AutoArray<TCHAR> buffer(bufferSize);
		if (!!buffer)
		{
			// we will at least have two NULLs
			buffer[0] = '\0';
			buffer[1] = '\0';

			TCHAR* pbyBuffer = buffer;

			// copy the exceptions first
			for (int i = 0; i < (int) agents.GetCount(); i++)
			{
				CAtlString agent(agents[i]);
				agent.Trim();

				const TCHAR* pszAgent = agent;
				if (pszAgent[0] == '!')
				{
					_tcscpy_s(pbyBuffer, bufferAvailable, agent);

					int agentLength = agent.GetLength() + 1;
					pbyBuffer += agentLength;
					bufferAvailable -= agentLength;
				}
			}

			// copy the exclusions now
			for (int i = 0; i < (int) agents.GetCount(); i++)
			{
				CAtlString agent(agents[i]);
				agent.Trim();

				const TCHAR* pszAgent = agent;
				if (pszAgent[0] != '!')
				{
					_tcscpy_s(pbyBuffer, bufferAvailable, agent);					

					int agentLength = agent.GetLength() + 1;
					pbyBuffer += agentLength;
					bufferAvailable -= agentLength;
				}
			}

			// terminate the string with a double NULL (there will already be one there from the last strcpy)
			*pbyBuffer = '\0';
			
			if (Config.SetMultiStringValue(IISXPRESS_EXCLUDEDUSERAGENTS, buffer) == ERROR_SUCCESS)
			{
				m_ExcludedUserAgents.Copy(agents);
				PopulateUserAgentRules(m_ExcludedUserAgents, m_AgentRules);

				status = true;
			}
		}
	}
	
	return status;
}

const HttpUserAgent::RuleUserAgents<std::string>& CIISxpressRegSettingsEx::GetExcludedUserAgents()
{
	return m_AgentRules;	
}

bool CIISxpressRegSettingsEx::GetUserAgentExclusionEnabled()
{
	return m_dwUserAgentExclusionEnabled != 0;
}

bool CIISxpressRegSettingsEx::SetUserAgentExclusionEnabled(bool enable)
{
	bool status = false;

	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{								
		DWORD dwEnabled = (DWORD) enable;
		if (Config.SetDWORDValue(IISXPRESS_ENABLEUSERAGENTEXCLUSION, dwEnabled) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwUserAgentExclusionEnabled, dwEnabled);
			status = true;
		}
	}

	return status;
}

void CIISxpressRegSettingsEx::PopulateUserAgentRules(CAtlArray<CAtlStringA>& excludedAgents, HttpUserAgent::RuleUserAgents<std::string>& agentRules)
{
	agentRules.Clear();
	for (DWORD i = 0; i < excludedAgents.GetCount(); i++)
	{
		HttpUserAgent::UserAgentProducts<std::string> agent;
		if (agent.ParseUserAgentString(excludedAgents[i]) == S_OK)
		{
			agentRules.AddUserAgent(agent);
		}
	}
}