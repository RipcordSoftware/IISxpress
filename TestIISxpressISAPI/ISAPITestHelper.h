#pragma once

#include <vector>
#include <map>
#include <string>

#include "AutoArray.h"

class CISAPITestHelper
{

public:

	CISAPITestHelper(HTTP_FILTER_CONTEXT& context);
	~CISAPITestHelper();

	static void Attach(HTTP_FILTER_SEND_RESPONSE& response);

	// HTTP_FILTER_CONTEXT methods
	static BOOL WINAPI GetServerVariable(struct _HTTP_FILTER_CONTEXT * pfc, LPSTR lpszVariableName, LPVOID lpvBuffer, LPDWORD lpdwSize);
	static BOOL WINAPI AddResponseHeaders(struct _HTTP_FILTER_CONTEXT * pfc, LPSTR lpszHeaders, DWORD dwReserved);
	static BOOL WINAPI WriteClient(struct _HTTP_FILTER_CONTEXT * pfc, LPVOID Buffer, LPDWORD lpdwBytes, DWORD dwReserved);
	static void* WINAPI AllocMem(struct _HTTP_FILTER_CONTEXT* pfc, ULONG_PTR cbSize, ULONG_PTR dwReserved);
	static BOOL WINAPI ServerSupportFunction(struct _HTTP_FILTER_CONTEXT* pfc, enum SF_REQ_TYPE sfReq, PVOID pData, ULONG_PTR ul1, ULONG_PTR ul2);

	// HTTP_FILTER_SEND_RESPONSE methods
	static BOOL WINAPI GetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPVOID lpvBuffer, LPDWORD lpdwSize);
    static BOOL WINAPI SetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPSTR lpszValue);
    static BOOL WINAPI AddHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPSTR lpszValue);

	// const versions of HTTP_FILTER_SEND_RESPONSE methods
	static BOOL WINAPI GetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPVOID lpvBuffer, LPDWORD lpdwSize);
    static BOOL WINAPI SetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPCSTR lpszValue);
    static BOOL WINAPI AddHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPCSTR lpszValue);

	// helpers
	void SetServerVariable(LPSTR lpszVariableName, const char* pszValue);
	void SetServerVariable(LPSTR lpszVariableName, LPVOID lpvBuffer);

	static void GetWriteClientResponse(HTTP_FILTER_CONTEXT* pCtxt, std::vector<BYTE>& data);
	static void SetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt);
	static void SetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt, DWORD dwDisableNotifications);
	static DWORD GetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt);

	// Load file
	static bool LoadFileData(LPCSTR pszFileName, AutoArray<BYTE>& fileData, DWORD& dwFileLength);

private:

	// state for all the helpers
	static CComAutoCriticalSection m_csState;
	static std::map<HTTP_FILTER_CONTEXT*, CISAPITestHelper*> m_State;

	// the context object the instance is attached to
	HTTP_FILTER_CONTEXT* m_pContext;

	// state for AllocMem
	BYTE* AllocateBlock(int nSize);	
	std::vector<std::vector<BYTE> > m_AllocatedBlocks;

	// state for GetHeader/SetHeader/AddHeader
	std::map<std::string, std::string> m_Headers;

	// state for GetServerVariable
	std::map<std::string, std::string> m_ServerVariables;

	// state for WriteClient
	std::vector<BYTE> m_WriteClientResponse;

	// state for ServerSupportFunction/SF_REQ_DISABLE_NOTIFICATIONS
	DWORD m_dwDisableNotifications;

	// convert a sting to lower case
	static void LowerString(std::string& text);	

	static CComAutoCriticalSection m_csFileDataCache;
	typedef std::map<std::string, std::vector<BYTE> > FileCacheType;
	static FileCacheType m_FileDataCache;	
};
