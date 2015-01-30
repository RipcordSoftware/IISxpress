#pragma once

#include "IISxpressServer.h"

#include <map>
#include <string>
using namespace std;

#include "Interlocked64.h"

class ICompressionHistoryImpl : 
				public ICompressionHistory, 
				public ICompressionHistory2, 
				public ICompressionStats,
				public ICompressionStats2
{

protected:

	ICompressionHistoryImpl(void);
	virtual ~ICompressionHistoryImpl(void);

public:

	// *** ICompressionHistory
	STDMETHOD(Init)(IStorage* pStorage);
	STDMETHOD(UpdateEntry)(LPCSTR pszFullFilePath, DWORD dwRawSize, BOOL bCompressed, DWORD dwCompressedSize);
	STDMETHOD(QueryEntry)(LPCSTR pszFullFilePath, FileCompressionHistory* pHistory);
	STDMETHOD(RemoveEntry)(LPCSTR pszFullFilePath);
	STDMETHOD(EnumEntriesByPath)(LPCSTR pszPath, IEnumString** ppEnum);

	// *** ICompressionHistory2	
	STDMETHOD(UpdateEntry)(LPCSTR pszFullFilePath, DWORD dwRawSize, BOOL bCompressed, DWORD dwCompressedSize, BOOL bCacheHit);

	// *** ICompressionStats
	STDMETHOD(ResetStats)(void);
	STDMETHOD(GetStats)(DWORD* pdwFilesCompressed, DWORD* pdwRawSize, DWORD* pdwCompressedSize);

	// *** ICompressionStats2
	STDMETHOD(GetCacheStats)(DWORD* pdwCacheHits, DWORD* pdwRawSize, DWORD* pdwCompressedSize);

private:

	HRESULT GetEntry(LPCSTR pszPath, IStorage** ppStorage, LPCSTR pszFilename, IStream** ppStream, bool bCreate = true);

	bool PathHasLongNames(LPCSTR pszPath);

	CComPtr<IStorage>					m_pStorage;

	CComAutoCriticalSection				m_csPathToStorage;
	map<string, CComPtr<IStorage> >		m_PathToStorage;

	volatile DWORD						m_dwCompressedFiles;		
	volatile __int64					m_nRawSize;
	volatile __int64					m_nCompressedSize;
	volatile DWORD						m_dwCacheHits;
	volatile __int64					m_nCacheRawSize;
	volatile __int64					m_nCacheCompressedSize;

	Interlocked64						m_Interlocked64;
};
