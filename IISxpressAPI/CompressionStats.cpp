#include "StdAfx.h"

#include "CompressionStats.h"

IISxpressAPI::CompressionStats::CompressionStats() : m_pCompressionStats(nullptr)
{
	CComPtr<ICompressionStats> pCompressionStats;

	HRESULT hr = GetCompressionStatsObject(&pCompressionStats);
	if (hr != S_OK)
	{
		throw gcnew IISxpressAPI::IISxpressAPIException(hr);
	}

	m_pCompressionStats = pCompressionStats.Detach();
}

IISxpressAPI::CompressionStats::!CompressionStats()
{
	if (m_pCompressionStats != nullptr)
	{
		m_pCompressionStats->Release();
		m_pCompressionStats = nullptr;
	}
}

IISxpressAPI::CompressionStats::~CompressionStats()
{
	if (m_pCompressionStats != nullptr)
	{
		m_pCompressionStats->Release();
		m_pCompressionStats = nullptr;
	}
}

void IISxpressAPI::CompressionStats::ResetStats()
{
	if (m_pCompressionStats == nullptr)
		throw gcnew System::NullReferenceException();

	HRESULT hr = m_pCompressionStats->ResetStats();
	if (hr != S_OK)
		throw gcnew IISxpressAPI::IISxpressAPIException(hr);
}

void IISxpressAPI::CompressionStats::GetStats(
		[System::Runtime::InteropServices::Out] unsigned% filesCompressed, 
		[System::Runtime::InteropServices::Out] unsigned% rawSize, 
		[System::Runtime::InteropServices::Out] unsigned% compressedSize)
{
	if (m_pCompressionStats == nullptr)
		throw gcnew System::NullReferenceException();

	DWORD dwFilesCompressed = 0;
	DWORD dwRawSize = 0;
	DWORD dwCompressedSize = 0;

	HRESULT hr = m_pCompressionStats->GetStats(&dwFilesCompressed, &dwRawSize, &dwCompressedSize);
	if (hr != S_OK)
		throw gcnew IISxpressAPI::IISxpressAPIException(hr);

	filesCompressed = dwFilesCompressed;
	rawSize = dwRawSize;
	compressedSize = dwCompressedSize;
}

HRESULT IISxpressAPI::CompressionStats::GetCompressionStatsObject(ICompressionStats** ppCompressionStats)
{
	if (ppCompressionStats == NULL)
		return false;

	CComPtr<IUnknown> pUnk;
	HRESULT hr = ::GetActiveObject(CLSID_CompressionRuleManager, NULL, &pUnk);
	if (hr != S_OK)
	{				
		return hr;
	}
	
	CComPtr<ICompressionStats> pCompressionStats;
	hr = pUnk->QueryInterface(IID_ICompressionStats, (void**) &pCompressionStats);		
	if (hr != S_OK || pCompressionStats == NULL)
	{				
		return hr;
	}
	
	*ppCompressionStats = pCompressionStats.Detach();
	return S_OK;
}