#include "stdafx.h"

class IIISxpressSettingsImpl : public IIISxpressSettings
{

public:

	IIISxpressSettingsImpl();	

	HRESULT Init(IStream* pSettings);

	// IIISxpressSettings
	STDMETHOD(get_CompressionMode)(CompressionMode* pMode);
	STDMETHOD(put_CompressionMode)(CompressionMode Mode);
	STDMETHOD(get_ProcessorAffinity)(DWORD* pdwProcessorAffinity);
	STDMETHOD(put_ProcessorAffinity)(DWORD dwProcessorAffinity);	

private:

	HRESULT InitSettingsDOM(IXMLDOMDocument* pSettingsDOM);	
	HRESULT LoadSettings(IXMLDOMDocument* pSettingsDOM);
	HRESULT put_CompressionMode(IXMLDOMDocument* pSettingsDOM, CompressionMode Mode);	
	HRESULT put_ProcessorAffinity(IXMLDOMDocument* pSettingsDOM, DWORD dwProcessorAffinity);	
	HRESULT put_LicenseMode(IXMLDOMDocument* pSettingsDOM, LicenseMode Mode);

	HRESULT CommitDOM(IXMLDOMDocument* pSettingsDOM, IStream* pStream);

	CComPtr<IStream>			m_pSettingsStream;
	CComPtr<IXMLDOMDocument>	m_pSettingsDOM;
	volatile CompressionMode	m_CompressionMode;
	volatile DWORD				m_dwProcessorAffinity;
	volatile LicenseMode		m_LicenseMode;

};