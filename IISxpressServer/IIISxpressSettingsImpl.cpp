#include "stdafx.h"

#include "IIISxpressSettingsImpl.h"

IIISxpressSettingsImpl::IIISxpressSettingsImpl() :
	m_CompressionMode(CompressionModeDefault),
	m_dwProcessorAffinity(IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO),
	m_LicenseMode(LicenseModeCommunity)
{
}

HRESULT IIISxpressSettingsImpl::Init(IStream* pSettings)
{
	HRESULT hr = S_OK;	

	try
	{
		if (pSettings == NULL)
			THROW_HR_ON_FAIL(E_POINTER);
		
		m_pSettingsStream = pSettings;		

		// we need an XML DOM for the settings info
		hr = m_pSettingsDOM.CoCreateInstance(L"MSXML.DOMDocument");
		THROW_HR_ON_FAIL(hr);

		// load the settings stream into the DOM
		VARIANT_BOOL vbSuccess = -1;
		hr = m_pSettingsDOM->load(CComVariant(m_pSettingsStream), &vbSuccess);		
		if (hr != S_OK || vbSuccess != VARIANT_TRUE)
		{			
			// the load failed, get a clean DOM
			m_pSettingsDOM.Release();
			hr = m_pSettingsDOM.CoCreateInstance(L"MSXML.DOMDocument");
			THROW_HR_ON_FAIL(hr);

			// the stream on the disk is bad, so overwrite with the blank data
			CommitDOM(m_pSettingsDOM, m_pSettingsStream);
		}

		// initialise the DOM and load the data from the stream
		hr = InitSettingsDOM(m_pSettingsDOM);				
		hr = LoadSettings(m_pSettingsDOM);
	}
	catch (HRESULT)
	{		
	}

	return hr;
}

STDMETHODIMP IIISxpressSettingsImpl::get_CompressionMode(CompressionMode* pMode)
{
	if (pMode == NULL)
		return E_POINTER;	

	*pMode = m_CompressionMode;
	return S_OK;
}

STDMETHODIMP IIISxpressSettingsImpl::put_CompressionMode(CompressionMode Mode)
{
	if (m_CompressionMode != Mode)
	{
		m_CompressionMode = Mode;

		if (m_pSettingsDOM != NULL)
		{
			HRESULT hr = put_CompressionMode(m_pSettingsDOM, m_CompressionMode);			
			if (hr == S_OK)
				hr = CommitDOM(m_pSettingsDOM, m_pSettingsStream);			
		}
	}

	return S_OK;
}

STDMETHODIMP IIISxpressSettingsImpl::get_ProcessorAffinity(DWORD* pdwProcessorAffinity)
{
	if (pdwProcessorAffinity == NULL)
		return E_POINTER;

	*pdwProcessorAffinity = m_dwProcessorAffinity;
	return S_OK;
}

STDMETHODIMP IIISxpressSettingsImpl::put_ProcessorAffinity(DWORD dwProcessorAffinity)
{
	if (m_dwProcessorAffinity != dwProcessorAffinity)
	{
		m_dwProcessorAffinity = dwProcessorAffinity;

		if (m_pSettingsDOM != NULL)
		{
			HRESULT hr = put_ProcessorAffinity(m_pSettingsDOM, m_dwProcessorAffinity);			
			if (hr == S_OK)
				hr = CommitDOM(m_pSettingsDOM, m_pSettingsStream);			
		}
	}

	return S_OK;
}

HRESULT IIISxpressSettingsImpl::InitSettingsDOM(IXMLDOMDocument* pSettingsDOM)
{
	if (pSettingsDOM == NULL)
		return E_POINTER;	
	
	CComPtr<IXMLDOMNode> pSettingsNode;
	HRESULT hr = pSettingsDOM->selectSingleNode(L"/settings", &pSettingsNode);
	if (hr != S_OK || pSettingsNode == NULL)
	{
		CComPtr<IXMLDOMNode> pTempNode;
		hr = pSettingsDOM->createNode(CComVariant(NODE_ELEMENT), L"settings", L"", &pTempNode);
		hr = pSettingsDOM->appendChild(pTempNode, &pSettingsNode);		
	}	

	return S_OK;
}

HRESULT IIISxpressSettingsImpl::LoadSettings(IXMLDOMDocument* pSettingsDOM)
{
	if (pSettingsDOM == NULL)
		return E_POINTER;	
	
	CComPtr<IXMLDOMNode> pSettingsNode;
	HRESULT hr = pSettingsDOM->selectSingleNode(L"/settings", &pSettingsNode);
	if (hr != S_OK || pSettingsNode == NULL)
		return E_FAIL;	

	m_CompressionMode = CompressionModeDefault;
	m_dwProcessorAffinity = IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO;
	m_LicenseMode = LicenseModeCommunity;

	CComPtr<IXMLDOMNode> pCompressionModeNode;
	hr = pSettingsNode->selectSingleNode(L"compressionMode", &pCompressionModeNode);
	if (hr == S_OK && pCompressionModeNode != NULL)
	{
		CComBSTR bsValue;	
		pCompressionModeNode->get_text(&bsValue);
		if (bsValue.Length() > 0)
		{
			m_CompressionMode = (CompressionMode) _wtoi(bsValue);
		}
	}	

	CComPtr<IXMLDOMNode> pProcessorAffinityNode;
	hr = pSettingsNode->selectSingleNode(L"processorAffinity", &pProcessorAffinityNode);
	if (hr == S_OK && pProcessorAffinityNode != NULL)
	{
		CComBSTR bsValue;	
		pProcessorAffinityNode->get_text(&bsValue);
		if (bsValue.Length() > 0)
		{
			WCHAR* pszEnd = NULL;
			m_dwProcessorAffinity = (DWORD) wcstoul(bsValue, &pszEnd, 10);
		}
	}		

	return S_OK;
}

HRESULT IIISxpressSettingsImpl::put_CompressionMode(IXMLDOMDocument* pSettingsDOM, CompressionMode Mode)
{
	if (pSettingsDOM == NULL)
		return E_POINTER;

	CComPtr<IXMLDOMNode> pSettingsNode;
	HRESULT hr = pSettingsDOM->selectSingleNode(L"/settings", &pSettingsNode);
	if (pSettingsNode == NULL)
		return E_FAIL;

	CComPtr<IXMLDOMNode> pCompressionModeNode;
	hr = pSettingsNode->selectSingleNode(L"compressionMode", &pCompressionModeNode);
	if (hr != S_OK || pCompressionModeNode == NULL)
	{		
		CComPtr<IXMLDOMNode> pTempNode;
		hr = pSettingsDOM->createNode(CComVariant(NODE_ELEMENT), L"compressionMode", L"", &pTempNode);
		hr = pSettingsNode->appendChild(pTempNode, &pCompressionModeNode);				
	}

	CAtlStringW sValue;
	sValue.Format(L"%u", Mode);
	pCompressionModeNode->put_text(CComBSTR(sValue));

	return S_OK;
}

HRESULT IIISxpressSettingsImpl::put_ProcessorAffinity(IXMLDOMDocument* pSettingsDOM, DWORD dwProcessorAffinity)
{
	if (pSettingsDOM == NULL)
		return E_POINTER;

	CComPtr<IXMLDOMNode> pSettingsNode;
	HRESULT hr = pSettingsDOM->selectSingleNode(L"/settings", &pSettingsNode);
	if (pSettingsNode == NULL)
		return E_FAIL;

	CComPtr<IXMLDOMNode> pProcessorAffinityNode;
	hr = pSettingsNode->selectSingleNode(L"processorAffinity", &pProcessorAffinityNode);
	if (hr != S_OK || pProcessorAffinityNode == NULL)
	{		
		CComPtr<IXMLDOMNode> pTempNode;
		hr = pSettingsDOM->createNode(CComVariant(NODE_ELEMENT), L"processorAffinity", L"", &pTempNode);
		hr = pSettingsNode->appendChild(pTempNode, &pProcessorAffinityNode);
	}

	CAtlStringW sValue;
	sValue.Format(L"%u", dwProcessorAffinity);
	pProcessorAffinityNode->put_text(CComBSTR(sValue));		

	return S_OK;
}

HRESULT IIISxpressSettingsImpl::CommitDOM(IXMLDOMDocument* pSettingsDOM, IStream* pStream)
{
	if (pSettingsDOM == NULL || pStream == NULL)
		return E_POINTER;

	LARGE_INTEGER nOffset = { 0, 0 };
	HRESULT hr = pStream->Seek(nOffset, STREAM_SEEK_SET, NULL);

	ULARGE_INTEGER nNewSize = { 0, 0 };
	hr = pStream->SetSize(nNewSize);

	hr = pSettingsDOM->save(CComVariant(pStream));
	hr = pStream->Commit(STGC_DEFAULT);

	return S_OK;
}