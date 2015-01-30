// EnumStaticFileInfo.h : Declaration of the CEnumStaticFileInfo

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"

#include <vector>
using namespace std;

// CEnumStaticFileInfo

class ATL_NO_VTABLE CEnumStaticFileInfo : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEnumStaticFileInfo, &CLSID_EnumStaticFileInfo>,
	public IEnumStaticFileInfo
{

public:

	CEnumStaticFileInfo()
	{
		m_nIndex = -1;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ENUMSTATICFILEINFO)

DECLARE_NOT_AGGREGATABLE(CEnumStaticFileInfo)

BEGIN_COM_MAP(CEnumStaticFileInfo)
	COM_INTERFACE_ENTRY(IEnumStaticFileInfo)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	
	void FinalRelease();

	static void CopyStaticFileInfo(const StaticFileInfo& Source, StaticFileInfo& Target);
	static void DestroyStaticFileInfo(StaticFileInfo& Info);

public:

	STDMETHOD(Next)(ULONG celt, StaticFileInfo* rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(IEnumStaticFileInfo** ppenum);

	HRESULT AddElement(BSTR bsPath, BSTR bsFilename, int nUserCompression, int nSystemCompression);

private:

	CComAutoCriticalSection		m_csStaticFileInfo;
	vector<StaticFileInfo>		m_StaticFileInfo;
	int							m_nIndex;

};

OBJECT_ENTRY_AUTO(__uuidof(EnumStaticFileInfo), CEnumStaticFileInfo)
