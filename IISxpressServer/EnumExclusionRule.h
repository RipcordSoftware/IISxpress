// EnumExclusionRule.h : Declaration of the CEnumExclusionRule

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"

#include <vector>
using namespace std;

#include "EnumExclusionRuleHelper.h"

// CEnumExclusionRule

class ATL_NO_VTABLE CEnumExclusionRule : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEnumExclusionRule, &CLSID_EnumExclusionRule>,
	public IEnumExclusionRule
{

public:

	CEnumExclusionRule()
	{
		m_nIndex = -1;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ENUMEXCLUSIONRULE)

DECLARE_NOT_AGGREGATABLE(CEnumExclusionRule)

BEGIN_COM_MAP(CEnumExclusionRule)
	COM_INTERFACE_ENTRY(IEnumExclusionRule)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();	

	HRESULT AddRule(BSTR bsRuleData, BSTR bsInstance, DWORD dwFlags, int nUserAgents, BSTR* pbsUserAgentID);	

protected:

	STDMETHOD(Next)(ULONG celt, ExclusionRuleInfo* rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(IEnumExclusionRule** ppenum);

private:

	CComAutoCriticalSection		m_csExclusionRules;
	vector<ExclusionRuleInfo>	m_ExclusionRules;
	int							m_nIndex;

};

OBJECT_ENTRY_AUTO(__uuidof(EnumExclusionRule), CEnumExclusionRule)
