// NeverCompressRulesNotify.h : Declaration of the CNeverCompressRulesNotify

#pragma once
#include "resource.h"       // main symbols

#ifdef _MANAGED
#include <vcclr.h>
#endif

// CNeverCompressRulesNotify

template <typename T> 
class ATL_NO_VTABLE CNeverCompressRulesNotify : 
	public CComObjectRootEx<CComMultiThreadModel>,	
	public INeverCompressRulesNotify
{

#ifdef _MANAGED
	typedef gcroot<T^> NotifyPtr;
#else
	typedef T* NotifyPtr;
#endif

public:

	CNeverCompressRulesNotify()
	{
#ifdef _MANAGED
		m_pNotify = nullptr;
#else
		m_pNotify = NULL;
#endif		
	}

DECLARE_NOT_AGGREGATABLE(CNeverCompressRulesNotify)

BEGIN_COM_MAP(CNeverCompressRulesNotify)
	COM_INTERFACE_ENTRY(INeverCompressRulesNotify)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	// INeverCompressRulesNotify
	STDMETHOD(OnRuleChanged)(NeverRuleChangeHint hint, DWORD dwUpdateCookie)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnRuleChanged(hint, dwUpdateCookie);
		else
			return E_FAIL;
	}	

	void Init(NotifyPtr pNotify) { m_pNotify = pNotify; }

private:

#ifdef _MANAGED
	bool IsNull(System::Object^ o) { return o == nullptr ? true : false; }
#else
	bool IsNull(void* p) { return p == NULL ? true : false; }
#endif

	NotifyPtr m_pNotify;

};
