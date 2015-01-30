#pragma once

#include <string>
#include <map>
#include <vector>
using namespace std;

#define HTTPREQUEST_BLOCKSIZE				(64 * 1024)

// TODO: change class name to CMyHTTPResponseData
class CMyHTTPResponseData
{

public:

	CMyHTTPResponseData(void);
	~CMyHTTPResponseData(void);

	bool Init(bool bResetBuffers = false, DWORD dwInitialAllocSize = HTTPREQUEST_BLOCKSIZE, DWORD dwAllocBlockSize = HTTPREQUEST_BLOCKSIZE);	

	bool GotHeader(void);

	bool AppendData(const char* pszData, DWORD dwSize, bool bHasHeader);
	bool AppendData(const BYTE* pbyData, DWORD dwSize, bool bHasHeader);
	HRESULT AppendData(IStream* pStream, bool bHasHeader);
	HRESULT AppendData(IStream* pStream, DWORD dwSize, bool bHasHeader);	

	bool GetHTTPType(string& sHTTPType);	

	int GetResponseCode(void);
	bool GetResponseCode(string& sResponseCode);
	bool GetResponseString(string& sResponseString);

	bool GetParam(const char* pszParam, string& sValue);
	bool SetParam(const char* pszParam, const char* pszValue);

	const BYTE* GetBody(void) const;
	DWORD GetBodySize(void) const;
	bool SetBody(const BYTE* pbyData, DWORD dwSize);

	bool GetHTTPHeader(string& sHeader);
	bool GetHTTPData(BYTE* pbyData, DWORD* pdwSize);

	DWORD GetAllocatedBufferSize(void);

private:

	bool AppendHeader(const char* pszData, DWORD dwSize);
	HRESULT AppendHeader(IStream* pStream, DWORD dwSize);

	bool AppendBody(const BYTE* pbyData, DWORD dwSize);
	HRESULT AppendBody(IStream* pStream, DWORD dwSize);

	bool AppendBlock(const BYTE* pbyData, DWORD dwSize);
	bool ReallocBlock(DWORD dwSize = HTTPREQUEST_BLOCKSIZE);

	bool ParseHeader(void);

	bool							m_bGotHeader;
	string							m_sHeader;

	string							m_sResponseCode;
	string							m_sResponseString;

	string							m_sHTTPType;

	map<string, vector<string> >	m_Params;

	BYTE*							m_pbyData;					// the data block
	DWORD							m_dwDataSize;				// the amount of data in the block
	DWORD							m_dwAllocated;				// the amount of memory allocated for the block

	DWORD							m_dwInitialAllocSize;
	DWORD							m_dwAllocBlockSize;
};
