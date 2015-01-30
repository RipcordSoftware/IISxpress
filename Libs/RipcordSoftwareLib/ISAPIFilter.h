#pragma once

#include <HttpFilt.h>

namespace Ripcord { namespace IIS
{	
	class ISAPIFilterContent;

	class RIPCORDSOFTWARELIB_DLL ISAPIFilter
	{
	public:
		ISAPIFilter(void);
		virtual ~ISAPIFilter(void);

		BOOL GetFilterVersion(PHTTP_FILTER_VERSION pVer);
		DWORD HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD notificationType, LPVOID pvNotification);
		BOOL TerminateFilter(DWORD dwFlags);

		void virtual OnGetFilterVersion(PHTTP_FILTER_VERSION pVer) = 0;
		void virtual OnTerminateFilter() = 0;

		DWORD virtual OnUrlMap(ISAPIFilterContent& fc, PHTTP_FILTER_URL_MAP pUrlMap) = 0;
		DWORD virtual OnSendResponse(ISAPIFilterContent& fc, PHTTP_FILTER_SEND_RESPONSE pvResponse) = 0;
		DWORD virtual OnSendRawData(ISAPIFilterContent& fc, PHTTP_FILTER_RAW_DATA pvData) = 0;
		DWORD virtual OnEndOfRequest(ISAPIFilterContent& fc) = 0;
		DWORD virtual OnEndOfNetSession(ISAPIFilterContent& fc) = 0;
	};

	class ISAPIFilterContent
	{
	public:
		
		ISAPIFilterContent(PHTTP_FILTER_CONTEXT pfc) : m_pfc(pfc) { }	 

		BOOL GetServerVariable(LPSTR lpszVariableName, LPVOID lpvBuffer, LPDWORD lpdwSizeofBuffer)
		{
			return m_pfc->GetServerVariable(m_pfc, lpszVariableName, lpvBuffer, lpdwSizeofBuffer);
		}

		BOOL AddResponseHeaders(LPSTR lpszHeaders)
		{
			return m_pfc->AddResponseHeaders(m_pfc, lpszHeaders, 0);
		}

		BOOL WriteClient(LPVOID Buffer, LPDWORD lpdwBytes)
		{
			return m_pfc->WriteClient(m_pfc, Buffer, lpdwBytes, 0);
		}

		VOID* AllocMem(DWORD cbSize)
		{
			return m_pfc->AllocMem(m_pfc, cbSize, 0);
		}

		BOOL ServerSupportFunction(enum SF_REQ_TYPE sfReq, PVOID pData, DWORD ul1, DWORD ul2)
		{
			return m_pfc->ServerSupportFunction(m_pfc, sfReq, pData, ul1, ul2);
		}

	    operator PHTTP_FILTER_CONTEXT()
		{
			return m_pfc;
		}

		PHTTP_FILTER_CONTEXT GetContext()
		{
			return m_pfc;
		}

	private:
		PHTTP_FILTER_CONTEXT m_pfc;
	};
}}