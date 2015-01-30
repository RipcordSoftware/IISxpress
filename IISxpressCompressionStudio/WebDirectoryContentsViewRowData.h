#pragma once

#include <algorithm>

struct WebDirectoryContentsViewRowData
{
	enum CompressState
	{
		Unknown,
		Automatic,
		Always,
		Never
	};

	WebDirectoryContentsViewRowData() : 
		m_dwSelectionCookie(0), m_nFileSize(0), m_nRawSize(0), m_nCompressedSize(0), 
			m_nDownloads(0), m_fRatio(0.0), m_CompressState(Unknown)
	{
		memset(&m_stLastDownload, 0, sizeof(m_stLastDownload));		
	}

	WebDirectoryContentsViewRowData(LPCTSTR pszPhysicalPath, LPCTSTR pszFileName, __int64 nFileSize, __int64 nRawSize, 		
		__int64 nCompressedSize, __int64 nDownloads, float fRatio, const SYSTEMTIME& stLastDownload, CompressState compressState) : 
			m_dwSelectionCookie(0), m_sPhysicalPath(pszPhysicalPath), m_sFileName(pszFileName), m_nFileSize(nFileSize), 
				m_nRawSize(nRawSize), m_nCompressedSize(nCompressedSize), m_nDownloads(nDownloads), m_fRatio(fRatio), 
				m_stLastDownload(stLastDownload), m_CompressState(compressState)
	{		
	}

	WebDirectoryContentsViewRowData(const WebDirectoryContentsViewRowData& other) : 
		m_dwSelectionCookie(other.m_dwSelectionCookie), m_sPhysicalPath(other.m_sPhysicalPath),	m_sFileName(other.m_sFileName), 
			m_nFileSize(other.m_nFileSize), m_nRawSize(other.m_nRawSize), m_nCompressedSize(other.m_nCompressedSize), 
			m_nDownloads(other.m_nDownloads), m_fRatio(other.m_fRatio), m_stLastDownload(other.m_stLastDownload),
			m_CompressState(other.m_CompressState)
	{		
	}

	WebDirectoryContentsViewRowData& operator=(const WebDirectoryContentsViewRowData& other)
	{
		m_dwSelectionCookie = other.m_dwSelectionCookie;
		m_sPhysicalPath = other.m_sPhysicalPath;
		m_sFileName = other.m_sFileName;
		m_nFileSize = other.m_nFileSize;
		m_nRawSize = other.m_nRawSize;
		m_nCompressedSize = other.m_nCompressedSize;
		m_nDownloads = other.m_nDownloads;
		m_fRatio = other.m_fRatio;
		m_stLastDownload = other.m_stLastDownload;
		m_CompressState = other.m_CompressState;

		return *this;
	}

	static bool CompareFileName(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_sFileName.CompareNoCase(r->m_sFileName) < 0;
	}

	static bool CompareContentType(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_sContentType.CompareNoCase(r->m_sContentType) < 0;
	}

	static bool CompareFileSize(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_nFileSize < r->m_nFileSize;
	}

	static bool CompareRawSize(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_nRawSize < r->m_nRawSize;
	}

	static bool CompareCompressedSize(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_nCompressedSize < r->m_nCompressedSize;
	}

	static bool CompareDownloads(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_nDownloads < r->m_nDownloads;
	}

	static bool CompareRatio(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_fRatio < r->m_fRatio;
	}

	static bool CompareLastDownload(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		if (l->m_stLastDownload.wYear < r->m_stLastDownload.wYear)
			return true;

		if (l->m_stLastDownload.wYear > r->m_stLastDownload.wYear)
			return false;

		if (l->m_stLastDownload.wMonth < r->m_stLastDownload.wMonth)
			return true;

		if (l->m_stLastDownload.wMonth > r->m_stLastDownload.wMonth)
			return false;

		if (l->m_stLastDownload.wDay < r->m_stLastDownload.wDay)
			return true;

		if (l->m_stLastDownload.wDay > r->m_stLastDownload.wDay)
			return false;

		if (l->m_stLastDownload.wHour < r->m_stLastDownload.wHour)
			return true;

		if (l->m_stLastDownload.wHour > r->m_stLastDownload.wHour)
			return false;

		if (l->m_stLastDownload.wMinute < r->m_stLastDownload.wMinute)
			return true;

		if (l->m_stLastDownload.wMinute > r->m_stLastDownload.wMinute)
			return false;

		if (l->m_stLastDownload.wSecond < r->m_stLastDownload.wSecond)
			return true;

		if (l->m_stLastDownload.wSecond > r->m_stLastDownload.wSecond)
			return false;

		if (l->m_stLastDownload.wMilliseconds < r->m_stLastDownload.wMilliseconds)
			return true;

		return false;						
	}

	static bool CompareCompress(const WebDirectoryContentsViewRowData* l, const WebDirectoryContentsViewRowData* r)
	{
		return l->m_CompressState < r->m_CompressState;			
	}

	typedef void SortMethod(std::vector<WebDirectoryContentsViewRowData*>& items);

	static void SortByFileName(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareFileName);
	}

	static void SortByFileSize(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareFileSize);
	}

	static void SortByRawSize(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareRawSize);
	}

	static void SortByCompressedSize(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareCompressedSize);
	}

	static void SortByDownloads(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareDownloads);
	}

	static void SortByRatio(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareRatio);
	}

	static void SortByLastDownload(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareLastDownload);
	}

	static void SortByContentType(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareContentType);
	}

	static void SortByCompress
		(std::vector<WebDirectoryContentsViewRowData*>& items)
	{
		std::sort(items.begin(), items.end(), CompareCompress);
	}

	DWORD		m_dwSelectionCookie;
	CString		m_sPhysicalPath;

	CString		m_sFileName;
	__int64		m_nFileSize;
	__int64		m_nRawSize;
	__int64		m_nCompressedSize;
	__int64		m_nDownloads;
	float		m_fRatio;
	CString		m_sContentType;
	SYSTEMTIME	m_stLastDownload;	

	CompressState	m_CompressState;
};