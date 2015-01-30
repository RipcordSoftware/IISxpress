#include "StdAfx.h"
#include "icompressionhistoryimpl.h"

#include <atlpath.h>
#include <atlcomtime.h>

#define COMPRESSION_RECORD_VERSION	1

ICompressionHistoryImpl::ICompressionHistoryImpl(void)
{
	ResetStats();
}

ICompressionHistoryImpl::~ICompressionHistoryImpl(void)
{
}

HRESULT ICompressionHistoryImpl::Init(IStorage* pStorage)
{
	if (pStorage == NULL)
		return E_POINTER;

	m_pStorage = pStorage;	

	return S_OK;
}

STDMETHODIMP ICompressionHistoryImpl::UpdateEntry(LPCSTR pszFullFilePath, DWORD dwRawSize, BOOL bCompressed, DWORD dwCompressedSize)
{
	return UpdateEntry(pszFullFilePath, dwRawSize, bCompressed, dwCompressedSize, FALSE);
}

STDMETHODIMP ICompressionHistoryImpl::UpdateEntry(LPCSTR pszFullFilePath, DWORD dwRawSize, BOOL bCompressed, DWORD dwCompressedSize, BOOL bCacheHit)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);

		if (PathHasLongNames(pszFullFilePath))
		{
			char* pszShortFilePath = (char*) alloca(1024);
			::GetShortPathNameA(pszFullFilePath, pszShortFilePath, 1024);
			pszFullFilePath = pszShortFilePath;
		}

		DWORD dwDownloads = 0;

		FileCompressionHistory History;
		hr = QueryEntry(pszFullFilePath, &History);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			dwDownloads = History.dwDownloads;
		}		

		CPathT<CStringA> Path(pszFullFilePath);
		Path.RemoveFileSpec();

		CPathT<CStringA> Filename(pszFullFilePath);	
		Filename.StripPath();

		CComPtr<IStream> pStream;		
		hr = GetEntry(Path, NULL, Filename, &pStream);
		THROW_HR_ON_FAIL(hr);		

		FILETIME ftNow;
		::GetSystemTimeAsFileTime(&ftNow);

		__int64 nNow = ftNow.dwHighDateTime;
		nNow <<= 32;
		nNow |= ftNow.dwLowDateTime;

		CStringA sData;
		sData.Format("%u, %u, %I64u, %u, %u, %u",
			COMPRESSION_RECORD_VERSION, 
			++dwDownloads, 
			nNow,
			dwRawSize,
			bCompressed,
			dwCompressedSize);

		hr = pStream->Write((const char*) sData, sData.GetLength(), NULL);
		THROW_HR_ON_FAIL(hr);

		//hr = pStream->Commit(STGC_DEFAULT);
		hr = pStream->Commit(STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE);
	}
	catch (HRESULT)
	{
	}

	if (bCompressed == TRUE)
	{			
		::InterlockedIncrement((volatile LONG*) &m_dwCompressedFiles);
		m_Interlocked64.InterlockedAdd64(&m_nRawSize, dwRawSize);
		m_Interlocked64.InterlockedAdd64(&m_nCompressedSize, dwCompressedSize);

		if (bCacheHit == TRUE)
		{
			::InterlockedIncrement((volatile LONG*) &m_dwCacheHits);
			m_Interlocked64.InterlockedAdd64(&m_nCacheRawSize, dwRawSize);
			m_Interlocked64.InterlockedAdd64(&m_nCacheCompressedSize, dwCompressedSize);
		}
	}

	return hr;
}

STDMETHODIMP ICompressionHistoryImpl::QueryEntry(LPCSTR pszFullFilePath, FileCompressionHistory* pHistory)												 
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);
		THROW_HR_ON_NULL(pHistory, hr, E_POINTER);

		if (PathHasLongNames(pszFullFilePath))
		{
			char* pszShortFilePath = (char*) alloca(1024);
			::GetShortPathNameA(pszFullFilePath, pszShortFilePath, 1024);
			pszFullFilePath = pszShortFilePath;
		}

		CPathT<CStringA> Path(pszFullFilePath);
		Path.RemoveFileSpec();

		CPathT<CStringA> Filename(pszFullFilePath);	
		Filename.StripPath();

		CComPtr<IStream> pStream;		
		hr = GetEntry(Path, NULL, Filename, &pStream);
		THROW_HR_ON_FAIL(hr);

		STATSTG Stat;
		hr = pStream->Stat(&Stat, STATFLAG_NONAME);
		THROW_HR_ON_FAIL(hr);

		if (Stat.cbSize.LowPart == 0)
		{
			hr = S_FALSE;
			throw hr;
		}

		char* pszBuffer = (char*) alloca(Stat.cbSize.LowPart + 1);
		THROW_HR_ON_NULL(pszBuffer, hr, E_OUTOFMEMORY);

		pszBuffer[Stat.cbSize.LowPart] = '\0';

		hr = pStream->Read(pszBuffer, Stat.cbSize.LowPart, NULL);
		THROW_HR_ON_FAIL(hr);

		pStream.Release();

		char* pszContext = NULL;
		char* pszVersion = strtok_s(pszBuffer, ", ", &pszContext);
		THROW_HR_ON_NULL(pszVersion, hr, E_FAIL);

		DWORD dwVersion = atoi(pszVersion);
		if (dwVersion != COMPRESSION_RECORD_VERSION)
		{
			hr = E_FAIL;
			throw hr;
		}

		char* pszDownloads = strtok_s(NULL, ", ", &pszContext);
		THROW_HR_ON_NULL(pszDownloads, hr, E_FAIL);

		DWORD dwDownloads = atoi(pszDownloads);

		char* pszLastDownload = strtok_s(NULL, ", ", &pszContext);
		THROW_HR_ON_NULL(pszLastDownload, hr, E_FAIL);

		__int64 nLastDownload = _atoi64(pszLastDownload);

		FILETIME ftLastDownload;
		ftLastDownload.dwLowDateTime = (DWORD) nLastDownload;
		ftLastDownload.dwHighDateTime = (DWORD) (nLastDownload >> 32);

		COleDateTime LastDownload(ftLastDownload);
		CComVariant vLastDownload(LastDownload);

		char* pszRawSize = strtok_s(NULL, ", ", &pszContext);
		THROW_HR_ON_NULL(pszRawSize, hr, E_FAIL);

		DWORD dwRawSize = atoi(pszRawSize);

		char* pszCompressed = strtok_s(NULL, ", ", &pszContext);
		THROW_HR_ON_NULL(pszCompressed, hr, E_FAIL);

		BOOL bCompressed = atoi(pszCompressed);

		char* pszCompressedSize = strtok_s(NULL, ", ", &pszContext);
		THROW_HR_ON_NULL(pszCompressedSize, hr, E_FAIL);

		DWORD dwCompressedSize = atoi(pszCompressedSize);

		pHistory->dwDownloads = dwDownloads;
		pHistory->vLastDownload = vLastDownload;
		pHistory->dwRawSize = dwRawSize;
		pHistory->bCompressed = bCompressed;
		pHistory->dwCompressedSize = dwCompressedSize;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP ICompressionHistoryImpl::RemoveEntry(LPCSTR pszFullFilePath)
{
	CComPtr<IStorage> pStorage;

	if (PathHasLongNames(pszFullFilePath))
	{
		char* pszShortFilePath = (char*) alloca(1024);
		::GetShortPathNameA(pszFullFilePath, pszShortFilePath, 1024);
		pszFullFilePath = pszShortFilePath;
	}

	CPathT<CStringA> Path(pszFullFilePath);
	Path.RemoveFileSpec();

	HRESULT hr = GetEntry(Path, &pStorage, NULL, NULL);
	if (hr == S_OK)
	{
		CPathT<CStringA> Filename(pszFullFilePath);	
		Filename.StripPath();

		hr = pStorage->DestroyElement(CComBSTR(Filename));
		if (hr == S_OK)
		{
			pStorage->Commit(STGC_DEFAULT);
		}
	}

	return hr;
}

STDMETHODIMP ICompressionHistoryImpl::EnumEntriesByPath(LPCSTR pszPath, IEnumString** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		typedef CComObject<CComEnum<IEnumString, &IID_IEnumString, BSTR, _Copy<BSTR> > > MyEnumString;
		MyEnumString* pStringEnum = new MyEnumString();		

		if (PathHasLongNames(pszPath))
		{
			char* pszShortFilePath = (char*) alloca(1024);
			::GetShortPathNameA(pszPath, pszShortFilePath, 1024);
			pszPath = pszShortFilePath;
		}

		CComPtr<IStorage> pStorage;
		hr = GetEntry(pszPath, &pStorage, NULL, NULL, false);
		THROW_HR_ON_FAIL(hr);

		CComPtr<IEnumSTATSTG> pEnumStat;
		hr = pStorage->EnumElements(NULL, NULL, NULL, &pEnumStat);
		THROW_HR_ON_FAIL(hr);

		int nStreamEntries = 0;

		STATSTG Stat;
		ULONG nFetched = 0;
		while (pEnumStat->Next(1, &Stat, &nFetched) == S_OK)
		{
			::CoTaskMemFree(Stat.pwcsName);

			if (Stat.type != STGTY_STREAM)
				continue;

			nStreamEntries++;
		}

		pEnumStat->Reset();

		BSTR* pbsStreams = new BSTR[nStreamEntries];
		THROW_HR_ON_NULL(pbsStreams, hr, E_OUTOFMEMORY);

		int i = 0;
		while (pEnumStat->Next(1, &Stat, &nFetched) == S_OK)
		{
			if (Stat.type == STGTY_STREAM)
			{
				pbsStreams[i] = ::SysAllocString(Stat.pwcsName);				
				i++;
			}

			::CoTaskMemFree(Stat.pwcsName);
		}

		hr = pStringEnum->Init(pbsStreams, pbsStreams + nStreamEntries, NULL, AtlFlagCopy);

		delete[] pbsStreams;

		THROW_HR_ON_FAIL(hr);

		hr = pStringEnum->QueryInterface(IID_IEnumString, (void**) ppEnum);
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT ICompressionHistoryImpl::GetEntry(LPCSTR pszPath, IStorage** ppStorage, LPCSTR pszFilename, IStream** ppStream, bool bCreate)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pStorage, hr, E_FAIL);
		THROW_HR_ON_NULL(pszPath, hr, E_POINTER);		

		CComPtr<IStorage> pStorage = m_pStorage;

		// force the path to be lowercase for consistency
		CStringA sLowerPath = pszPath;
		sLowerPath.MakeLower();
		LPCSTR pszLowerPath = sLowerPath;

		CStringA sPath;

		CComCritSecLock<CComAutoCriticalSection> lock(m_csPathToStorage);

		while (1)
		{
			CString sEntry;

			if (*pszLowerPath == '\0')
				break;

			// eat leading \ or : characters
			while (*pszLowerPath != '\0' && (*pszLowerPath == '\\' || *pszLowerPath == ':'))
			{				
				pszLowerPath++;
			}

			// get the useful stuff
			while (*pszLowerPath != '\0' && *pszLowerPath != '\\' && *pszLowerPath != ':')
			{
				sEntry.AppendChar(*pszLowerPath);
				pszLowerPath++;
			}


			// if we don't have any useful data then break out
			if (sEntry.GetLength() <= 0)
				break;

			sPath += sEntry;

			map<string, CComPtr<IStorage> >::iterator iter = m_PathToStorage.find((const char*) sPath);
			if (iter != m_PathToStorage.end())
			{
				// we have found a storage item, so we can return S_OK from here
				hr = S_OK;

				pStorage.Release();
				pStorage = iter->second;				
				continue;
			}

			// attempt to open the storage item
			CComPtr<IStorage> pChildStorage;
			hr = pStorage->OpenStorage(CComBSTR(sEntry), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pChildStorage);

			if (hr != S_OK && bCreate == true)
			{
				// we failed to open the storage item, try creating it instead
				hr = pStorage->CreateStorage(CComBSTR(sEntry), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
						NULL, NULL, &pChildStorage);

				if (hr == S_OK)
				{
					pStorage->Commit(STGC_DEFAULT);
				}
			}

			THROW_HR_ON_FAIL(hr);

			m_PathToStorage[(const char*) sPath] = pChildStorage;
			
			pStorage = pChildStorage;
		}

		if (ppStream != NULL && pszFilename != NULL)
		{
			// force the filename to be lowercase for consistency
			CStringA sLowerFilename = pszFilename;
			sLowerFilename.MakeLower();

			// if we got here then we have either opening the storage item or created it, so try and open
			// the stream now
			hr = pStorage->OpenStream(CComBSTR(sLowerFilename), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, ppStream);

			if (hr != S_OK && bCreate == true)
			{
				// we failed to open the stream, try creating it
				hr = pStorage->CreateStream(CComBSTR(sLowerFilename), STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, NULL, ppStream);

				if (hr == S_OK)
				{
					pStorage->Commit(STGC_DEFAULT);
				}
			}
		}

		// if the user requsted the parent storage item then return it too
		if (hr == S_OK && ppStorage != NULL)
		{
			*ppStorage = pStorage.Detach();
		}
	}
	catch (HRESULT)
	{
	}

	return hr;
}

STDMETHODIMP ICompressionHistoryImpl::ResetStats(void)
{
	// all response members
	m_dwCompressedFiles = 0;
	m_nRawSize = 0;
	m_nCompressedSize = 0;	

	// cache members
	m_dwCacheHits = 0;
	m_nCacheRawSize = 0;
	m_nCacheCompressedSize = 0;

	return S_OK;
}

STDMETHODIMP ICompressionHistoryImpl::GetStats(DWORD* pdwFilesCompressed, DWORD* pdwRawSize, DWORD* pdwCompressedSize)
{
	HRESULT hr = E_FAIL;
	
	try
	{
		THROW_HR_ON_NULL(pdwFilesCompressed, hr, E_POINTER);
		THROW_HR_ON_NULL(pdwRawSize, hr, E_POINTER);
		THROW_HR_ON_NULL(pdwCompressedSize, hr, E_POINTER);

		*pdwFilesCompressed = m_dwCompressedFiles;

		// return raw and compressed sizes in Kb so it will fit a DWORD ok
		*pdwRawSize = (DWORD) ((m_nRawSize + 512) >> 10);
		*pdwCompressedSize = (DWORD) ((m_nCompressedSize + 512) >> 10);		

		hr = S_OK;
	}
	catch (HRESULT)
	{
	}		

	return hr;
}

STDMETHODIMP ICompressionHistoryImpl::GetCacheStats(DWORD* pdwCacheHits, DWORD* pdwRawSize, DWORD* pdwCompressedSize)
{
	HRESULT hr = E_FAIL;
	
	try
	{
		THROW_HR_ON_NULL(pdwCacheHits, hr, E_POINTER);
		THROW_HR_ON_NULL(pdwRawSize, hr, E_POINTER);
		THROW_HR_ON_NULL(pdwCompressedSize, hr, E_POINTER);

		*pdwCacheHits = m_dwCacheHits;

		// return raw and compressed sizes in Kb so it will fit a DWORD ok
		*pdwRawSize = (DWORD) ((m_nCacheRawSize + 512) >> 10);
		*pdwCompressedSize = (DWORD) ((m_nCacheCompressedSize + 512) >> 10);		

		hr = S_OK;
	}
	catch (HRESULT)
	{
	}		

	return hr;
}

bool ICompressionHistoryImpl::PathHasLongNames(LPCSTR pszPath)
{
	if (pszPath == NULL)
	{
		return false;
	}

	// search the path portion for names longer than 31 characters
	LPCSTR pszStart = NULL;
	LPCSTR pszEnd = pszPath;
	while ((pszStart = strchr(pszEnd, '\\')) != NULL && (pszEnd = strchr(pszStart + 1, '\\')) != NULL)
	{	
		// skip to the next character
		++pszStart;

		DWORD_PTR length = pszEnd - pszStart;
		if (length > 31)
		{
			return true;
		}
	}

	// search the filename portion for names longer than 31 characters
	if (pszStart != NULL)
	{
		// skip to the next character
		if (*pszStart == '\\')
		{
			++pszStart;
		}

		pszEnd = strchr(pszStart, '\0');

		DWORD_PTR length = pszEnd - pszStart;
		if (length > 31)
		{
			return true;
		}
	}

	return false;
}