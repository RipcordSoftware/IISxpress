#include "StdAfx.h"
#include "iisxpressregsettingsex.h"

#include "RegistryKeys.h"

#ifndef SF_NOTIFY_ORDER_LOW
#pragma message("SF_NOTIFY_ORDER_LOW has not been defined, setting to 0")
#define SF_NOTIFY_ORDER_LOW 0
#endif

namespace Ripcord { namespace IISxpress {

IISxpressRegSettingsEx::IISxpressRegSettingsEx(void)
{
	m_dwCompressLocalhost = 0;

	m_dwCacheEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_ENABLED_DEFAULT;
	m_dwCacheMaxEntries = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_MAXENTRIES_DEFAULT;
	m_dwCacheMaxSizeKB = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_MAXSIZEKB_DEFAULT;
	m_dwCacheStateCookie = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_STATECOOKIE_DEFAULT;

	m_dwISAPILoadPriority = SF_NOTIFY_ORDER_LOW;

	m_dwPerfCountersEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_PERFCOUNTERS_ENABLED_DEFAULT;

	m_dwHandlePOSTResponses = IISxpressFilterDefaultSettingsEx::IISXPRESS_HANDLEPOSTRESPONSES_DEFAULT;

	m_dwUserAgentExclusionEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_ENABLEUSERAGENTEXCLUSION_DEFAULT;
}

IISxpressRegSettingsEx::~IISxpressRegSettingsEx(void)
{
}

void IISxpressRegSettingsEx::Init(LPCSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	Init(CAtlStringW(pszRegPath), bLoad, bRegisterChangeHandler);
}

void IISxpressRegSettingsEx::Init(LPCWSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	IISxpressRegSettings::Init(pszRegPath, bLoad, bRegisterChangeHandler);
}

bool IISxpressRegSettingsEx::Load(void)
{
	bool bStatus = IISxpressRegSettings::Load();

	if (bStatus == true)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_READ | m_dwOpenFlags) == ERROR_SUCCESS)
		{								
			DWORD dwCompressLocalhost = 0;			
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_COMPRESSLOCALHOST, dwCompressLocalhost);
			::InterlockedExchange((volatile LONG*) &m_dwCompressLocalhost, dwCompressLocalhost);

			DWORD dwCacheEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_ENABLED_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_ENABLED, dwCacheEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwCacheEnabled, dwCacheEnabled);

			DWORD dwCacheMaxEntries = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_MAXENTRIES_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_MAXENTRIES, dwCacheMaxEntries);
			::InterlockedExchange((volatile LONG*) &m_dwCacheMaxEntries, dwCacheMaxEntries);

			DWORD dwCacheMaxSizeKB = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_MAXSIZEKB_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_MAXSIZEKB, dwCacheMaxSizeKB);
			::InterlockedExchange((volatile LONG*) &m_dwCacheMaxSizeKB, dwCacheMaxSizeKB);

			DWORD dwCacheStateCookie = IISxpressFilterDefaultSettingsEx::IISXPRESS_CACHE_STATECOOKIE_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_STATECOOKIE, dwCacheStateCookie);
			::InterlockedExchange((volatile LONG*) &m_dwCacheStateCookie, dwCacheStateCookie);

			DWORD dwISAPILoadPriority = SF_NOTIFY_ORDER_LOW;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_ISAPILOADPRIORITY, dwISAPILoadPriority);
			::InterlockedExchange((volatile LONG*) &m_dwISAPILoadPriority, dwISAPILoadPriority);

			DWORD dwPerfCountersEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_PERFCOUNTERS_ENABLED_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_PERFCOUNTERS_ENABLED, dwPerfCountersEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwPerfCountersEnabled, dwPerfCountersEnabled);			

			DWORD dwHandlePOSTResponses = IISxpressFilterDefaultSettingsEx::IISXPRESS_HANDLEPOSTRESPONSES_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_HANDLEPOSTRESPONSES, dwHandlePOSTResponses);
			::InterlockedExchange((volatile LONG*) &m_dwHandlePOSTResponses, dwHandlePOSTResponses);			

			DWORD dwUserAgentExclusionEnabled = IISxpressFilterDefaultSettingsEx::IISXPRESS_ENABLEUSERAGENTEXCLUSION_DEFAULT;
			Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_ENABLEUSERAGENTEXCLUSION, dwUserAgentExclusionEnabled);
			::InterlockedExchange((volatile LONG*) &m_dwUserAgentExclusionEnabled, dwUserAgentExclusionEnabled);			

			TCHAR szUserAgentBuffer[4096] = _T("");
			DWORD dwUserAgentBufferSize = _countof(szUserAgentBuffer) - 1;
			CAtlArray<CAtlStringA> excludedUserAgents;
			if (Config.QueryMultiStringValue(IISxpressRegKeys::IISXPRESS_EXCLUDEDUSERAGENTS, szUserAgentBuffer, &dwUserAgentBufferSize) == ERROR_SUCCESS && dwUserAgentBufferSize > 0)
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

bool IISxpressRegSettingsEx::GetCompressLocalhost(void) 
{ 
	// we don't need an interlock since the set/load will do it for us
	//DWORD dwCompressLocalhost = (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwCompressLocalhost, 0);

	return m_dwCompressLocalhost != 0 ? true : false;
}

DWORD IISxpressRegSettingsEx::GetISAPILoadPriority(void)
{
	return m_dwISAPILoadPriority;
}

BOOL IISxpressRegSettingsEx::GetCacheEnabled()
{
	return m_dwCacheEnabled != 0 ? TRUE : FALSE;
}

void IISxpressRegSettingsEx::SetCacheEnabled(BOOL bEnable)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{								
		DWORD dwCacheEnabled = (DWORD) bEnable;
		if (Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_ENABLED, dwCacheEnabled) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheEnabled, dwCacheEnabled);
		}
	}
}

DWORD IISxpressRegSettingsEx::GetCacheMaxEntries()
{
	return m_dwCacheMaxEntries;
}

void IISxpressRegSettingsEx::SetCacheMaxEntries(DWORD dwMaxEntries)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_MAXENTRIES, dwMaxEntries) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheMaxEntries, dwMaxEntries);
		}
	}
}

DWORD IISxpressRegSettingsEx::GetCacheMaxSizeKB()
{
	return m_dwCacheMaxSizeKB;
}

void IISxpressRegSettingsEx::SetCacheMaxSizeKB(DWORD dwMaxSizeKB)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_MAXSIZEKB, dwMaxSizeKB) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheMaxSizeKB, dwMaxSizeKB);
		}
	}
}

DWORD IISxpressRegSettingsEx::GetCacheStateCookie()
{
	return m_dwCacheStateCookie;
}

void IISxpressRegSettingsEx::SetCacheStateCookie(DWORD dwCookie)
{
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{										
		if (Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_CACHE_STATECOOKIE, dwCookie) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwCacheStateCookie, dwCookie);
		}
	}
}

bool IISxpressRegSettingsEx::GetPerfCountersEnabled(void)
{
	return m_dwPerfCountersEnabled != 0 ? true : false;
}

bool IISxpressRegSettingsEx::HandlePOSTResponses(void)
{
	return m_dwHandlePOSTResponses != 0 ? true : false;
}

bool IISxpressRegSettingsEx::GetExcludedUserAgents(CAtlArray<CAtlStringA>& agents)
{
	agents.Copy(m_ExcludedUserAgents);
	return true;
}

bool IISxpressRegSettingsEx::SetExcludedUserAgents(const CAtlArray<CAtlStringA>& agents)
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

		Ripcord::Types::AutoArray<TCHAR> buffer(bufferSize);
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
			
			if (Config.SetMultiStringValue(IISxpressRegKeys::IISXPRESS_EXCLUDEDUSERAGENTS, buffer) == ERROR_SUCCESS)
			{
				m_ExcludedUserAgents.Copy(agents);
				PopulateUserAgentRules(m_ExcludedUserAgents, m_AgentRules);

				status = true;
			}
		}
	}
	
	return status;
}

const HttpUserAgent::RuleUserAgents<std::string>& IISxpressRegSettingsEx::GetExcludedUserAgents()
{
	return m_AgentRules;	
}

bool IISxpressRegSettingsEx::GetUserAgentExclusionEnabled()
{
	return m_dwUserAgentExclusionEnabled != 0;
}

bool IISxpressRegSettingsEx::SetUserAgentExclusionEnabled(bool enable)
{
	bool status = false;

	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) == ERROR_SUCCESS)
	{								
		DWORD dwEnabled = (DWORD) enable;
		if (Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_ENABLEUSERAGENTEXCLUSION, dwEnabled) == ERROR_SUCCESS)
		{
			::InterlockedExchange((LONG*) &m_dwUserAgentExclusionEnabled, dwEnabled);
			status = true;
		}
	}

	return status;
}

void IISxpressRegSettingsEx::PopulateUserAgentRules(CAtlArray<CAtlStringA>& excludedAgents, HttpUserAgent::RuleUserAgents<std::string>& agentRules)
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

}}