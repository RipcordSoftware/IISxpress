// EnumExclusionRule.cpp : Implementation of CEnumExclusionRule

#include "stdafx.h"
#include "EnumExclusionRule.h"


// CEnumExclusionRule

HRESULT CEnumExclusionRule::FinalConstruct()
{
	return S_OK;
}

void CEnumExclusionRule::FinalRelease() 
{	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csExclusionRules);

	int nSize = (int) m_ExclusionRules.size();
	for (int i = 0; i < nSize; i++)
	{
		CEnumExclusionRuleHelper::DestroyExclusionRuleInfo(m_ExclusionRules[i]);
	}

	m_ExclusionRules.clear();	
}

STDMETHODIMP CEnumExclusionRule::Next(ULONG celt, ExclusionRuleInfo* rgelt, ULONG* pceltFetched)
{
	HRESULT hr = S_FALSE;

	try
	{
		THROW_HR_ON_NULL(rgelt, hr, E_POINTER);
		THROW_HR_ON_NULL(pceltFetched, hr, E_POINTER);

		if (celt != 1)
		{
			hr = E_INVALIDARG;
			throw hr;
		}

		memset(rgelt, 0, sizeof(ExclusionRuleInfo) * celt);
		
		CComCritSecLock<CComAutoCriticalSection> lock(m_csExclusionRules);

		if (m_nIndex < 0)
			m_nIndex = 0;

		int nSize = (int) m_ExclusionRules.size();

		if (nSize > 0 && m_nIndex < nSize)
		{			
			CEnumExclusionRuleHelper::CopyExclusionRuleInfo(m_ExclusionRules[m_nIndex], *rgelt);
			m_nIndex++;

			hr = S_OK;
		}

		*pceltFetched = 1;
	}
	catch (...)
	{
	}	

	return hr;
}

STDMETHODIMP CEnumExclusionRule::Skip(ULONG celt)
{
	HRESULT hr = S_FALSE;
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csExclusionRules);

	if (m_nIndex < 0)
		m_nIndex = 0;

	if (m_nIndex >= 0 && (m_nIndex + celt) < (int) m_ExclusionRules.size())
	{
		m_nIndex += celt;
		hr = S_OK;
	}	

	return hr;
}

STDMETHODIMP CEnumExclusionRule::Reset(void)
{
	HRESULT hr = S_FALSE;
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csExclusionRules);

	if (m_ExclusionRules.size() > 0)
	{
		m_nIndex = 0;
		hr = S_OK;
	}	

	return hr;
}

STDMETHODIMP CEnumExclusionRule::Clone(IEnumExclusionRule** ppenum)
{
	return E_NOTIMPL;
}



HRESULT CEnumExclusionRule::AddRule(BSTR bsRuleData, BSTR bsInstance, DWORD dwFlags, int nUserAgents, BSTR* pbsUserAgentID)
{
	if (bsRuleData == NULL || (nUserAgents > 0 && pbsUserAgentID == NULL))
		return E_POINTER;

	HRESULT hr = S_OK;
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csExclusionRules);

	int nRules = (int) m_ExclusionRules.size();
	for (int i = 0; i < nRules; i++)
	{
		if (wcscmp(bsRuleData, m_ExclusionRules[i].bsRule) == 0)
		{
			hr =  E_POINTER;
			break;
		}
	}

	if (hr == S_OK)
	{
		try
		{
			ExclusionRuleInfo Info;

			Info.bsRule = ::SysAllocString(bsRuleData);					
			THROW_HR_ON_NULL(Info.bsRule, hr, E_OUTOFMEMORY);

			if (bsInstance != NULL)
			{
				Info.bsInstance = ::SysAllocString(bsInstance);
				THROW_HR_ON_NULL(Info.bsInstance, hr, E_OUTOFMEMORY);
			}
			else
			{
				Info.bsInstance = NULL;
			}

			Info.dwFlags = dwFlags;

			Info.nUserAgents = nUserAgents;

			if (nUserAgents == 0)
			{
				Info.pbsUserAgentIDs = NULL;
			}
			else
			{
				Info.pbsUserAgentIDs = (BSTR*) ::CoTaskMemAlloc(sizeof(BSTR) * nUserAgents);
				THROW_HR_ON_NULL(Info.pbsUserAgentIDs, hr, E_OUTOFMEMORY);

				for (int i = 0; i < nUserAgents; i++)
				{
					Info.pbsUserAgentIDs[i] = ::SysAllocString(pbsUserAgentID[i]);
				}
			}			

			m_ExclusionRules.push_back(Info);
		}
		catch (...)
		{
		}
	}	

	return hr;
}