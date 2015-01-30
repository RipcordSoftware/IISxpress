#pragma once

#include "IISxpressServer.h"

class IStaticFileRulesImpl :
	public IStaticFileRules,
	public IStaticFileRules2,
	public ICustomCompressionMode
{

protected:

	IStaticFileRulesImpl(void);
	~IStaticFileRulesImpl(void);

	// IStaticFileRules
	STDMETHOD(Init)(IStorage* pStorage);
	STDMETHOD(IsKnownFile)(LPCSTR pszFullFilePath);
	STDMETHOD(AddFileRule)(LPCSTR pszFullFilePath, int nUserCompress, int nSystemCompress);
	STDMETHOD(RemoveFileRule)(LPCSTR pszFullFilePath);
	STDMETHOD(GetFileRule)(LPCSTR pszFullFilePath, int* pnUserCompress, int* pnSystemCompress);
	STDMETHOD(SetFileRule)(LPCSTR pszFullFilePath, int nUserCompress, int nSystemCompress);
	STDMETHOD(EnumByPath)(LPCSTR pszPath, IEnumStaticFileInfo** ppEnum);

	// IStaticFileRules2
	STDMETHOD(GetFileRule)(LPCSTR pszFullFilePath, int* pnUserCompress, int* pnSystemCompress, CustomCompressionSettings* pCustomCompressionSettings);

	// ICustomCompressionMode
	STDMETHOD(GetFileCompressionParams)(LPCSTR pszFullFilePath, CustomCompressionSettings* pCustomCompressionSettings);
	STDMETHOD(SetFileCompressionParams)(LPCSTR pszFullFilePath, CustomCompressionSettings* pCustomCompressionSettings);
	STDMETHOD(RemoveFileCompressionParams)(LPCSTR pszFullFilePath);

private:	

	bool CompressionStateToString(const CComBSTR& bsState, int* pnState);
	bool StateToCompressionString(int nState, CComBSTR& bsState);

	HRESULT GetFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode);
	HRESULT EnsureFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode);
	HRESULT CreateFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode);

	HRESULT Load(void);
	HRESULT Save(void);

	CComPtr<IStorage>			m_pConfigStorage;

	CComAutoCriticalSection		m_csDOM;
	CComPtr<IXMLDOMDocument>	m_pDOM;
	
};
