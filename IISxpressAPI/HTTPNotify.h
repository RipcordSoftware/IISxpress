// HTTPNotify.h : Declaration of the CHTTPNotify

#pragma once
#include "resource.h"       // main symbols

#include <vcclr.h>

#include <string>
#include <map>
#include <vector>

// TODO: remove this
#include <atlstr.h>

#include "ResponseDetails.h"

namespace IISxpressAPI
{	

	class ATL_NO_VTABLE CHTTPNotify : 
		public CComObjectRootEx<CComMultiThreadModel>,	
		public IIISxpressHTTPNotify
	{

		template <typename T> class AutoLock
		{
			friend typename T;

			AutoLock(T& parent) : m_parent(parent) {  m_parent.Lock(); }
			~AutoLock() { m_parent.Unlock(); }	
			T& m_parent;
		};

		class AutoTrace
		{
		public:

#ifdef _DEBUG
			AutoTrace(const char* pszFunction, DWORD dwContext, const void* pThis) :
			  m_sFunction(pszFunction), m_dwContext(dwContext), m_pThis(pThis)
			{
				CAtlStringA sMsg;
				sMsg.Format("%s(%u), thread=%u, this=0x%08x\n", m_sFunction.c_str(), m_dwContext, 
					::GetCurrentThreadId(), m_pThis);
				::OutputDebugStringA(sMsg);
			}

			~AutoTrace()
			{
				std::string sMsg = m_sFunction + " exit\n";
				::OutputDebugStringA(sMsg.c_str());
			}

		private:

			std::string m_sFunction;
			DWORD m_dwContext;
			const void* m_pThis;
#else
			AutoTrace(const char* pszFunction, DWORD dwContext, const void* pThis) {}
#endif
		};

		struct NativeResponseDetails
		{
			NativeResponseDetails() : bCompressed(FALSE), dwRawSize(0), dwCompressedSize(0)
			{
				::GetSystemTime(&stTime);
				
			}

			SYSTEMTIME stTime;
			std::string sServerName;
			std::string sServerPort;
			std::string sInstanceId;
			std::string sURLMapPath;
			std::string sUserAgent;
			std::string sHostname;
			std::string sURI;
			std::string sQueryString;	
			std::string sRemoteAddress;
			std::string sContentType;
			BOOL bCompressed;
			DWORD dwRawSize;
			DWORD dwCompressedSize;

			operator ResponseDetails^()
			{
				ResponseDetailHolder^ details = gcnew ResponseDetailHolder();

				details->time = gcnew System::DateTime(
					stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, System::DateTimeKind::Utc);

				details->serverName = GetManagedString(sServerName.c_str());
				details->serverPort = GetManagedString(sServerPort.c_str());
				details->instanceId = GetManagedString(sInstanceId.c_str());
				details->URLMapPath = GetManagedString(sURLMapPath.c_str());
				details->userAgent = GetManagedString(sUserAgent.c_str());
				details->hostname = GetManagedString(sHostname.c_str());
				details->URI = GetManagedString(sURI.c_str());
				details->queryString = GetManagedString(sQueryString.c_str());
				details->remoteAddress = GetManagedString(sRemoteAddress.c_str());
				details->contentType = GetManagedString(sContentType.c_str());				
				details->compressed = bCompressed == TRUE ? true : false;
				details->rawSize = dwRawSize;
				details->compressedSize = dwCompressedSize;

				return gcnew ResponseDetails(details);
			}

			template <typename T> System::String^ GetManagedString(const T* pszString)
			{
				return pszString == NULL ? nullptr : gcnew System::String(pszString);
			}
		};

	public:

		CHTTPNotify()
		{
		}

	DECLARE_NOT_AGGREGATABLE(CHTTPNotify)

	BEGIN_COM_MAP(CHTTPNotify)
		COM_INTERFACE_ENTRY(IIISxpressHTTPNotify)
	END_COM_MAP()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}
		
		void FinalRelease() 
		{
		}

		STDMETHOD(OnResponseStart)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext)
		{
			AutoLock<CHTTPNotify> autolock(*this);

			AutoTrace autotrace(__FUNCTION__, dwFilterContext, this);

			NativeResponseDetails details;		
			details.sServerName = pIISInfo->pszServerName;
			details.sServerPort = pIISInfo->pszServerPort;
			details.sInstanceId = pIISInfo->pszInstanceId;
			details.sURLMapPath = pIISInfo->pszURLMapPath;
			details.sUserAgent = pRequestInfo->pszUserAgent;
			details.sHostname = pRequestInfo->pszHostname;
			details.sURI = pRequestInfo->pszURI;
			details.sQueryString = pRequestInfo->pszQueryString;
			details.sRemoteAddress = pRequestInfo->pszRemoteAddress;
			details.sContentType = pResponseInfo->pszContentType;			
			details.dwRawSize = pResponseInfo->dwContentLength;

			m_PendingResponses[dwFilterContext] = details;					

			return S_OK;
		}

		STDMETHOD(OnResponseComplete)(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
		{
			AutoLock<CHTTPNotify> autolock(*this);

			AutoTrace autotrace(__FUNCTION__, dwFilterContext, this);

			PendingResponseColl::iterator iter = m_PendingResponses.find(dwFilterContext);
			if (iter != m_PendingResponses.end())
			{
				iter->second.bCompressed = bCompressed;
				iter->second.dwCompressedSize = dwCompressedSize;

				m_Responses.push_back(iter->second);
				m_PendingResponses.erase(iter);							
			}			

			return S_OK;
		}

		STDMETHOD(OnResponseEnd)(DWORD dwFilterContext)
		{
			AutoLock<CHTTPNotify> autolock(*this);

			AutoTrace autotrace(__FUNCTION__, dwFilterContext, this);

			PendingResponseColl::iterator iter = m_PendingResponses.find(dwFilterContext);
			if (iter != m_PendingResponses.end())
			{
				m_PendingResponses.erase(dwFilterContext);
			}		

			return S_OK;
		}

		STDMETHOD(OnResponseAbort)(DWORD dwFilterContext)
		{
			AutoLock<CHTTPNotify> autolock(*this);

			AutoTrace autotrace(__FUNCTION__, dwFilterContext, this);

			PendingResponseColl::iterator iter = m_PendingResponses.find(dwFilterContext);
			if (iter != m_PendingResponses.end())
			{
				m_PendingResponses.erase(dwFilterContext);
			}

			return S_OK;
		}	

		STDMETHOD(OnCacheHit)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize)
		{
			AutoTrace autotrace(__FUNCTION__, 0, this);

			NativeResponseDetails details;		
			details.bCompressed = true;
			details.dwCompressedSize = dwCompressedSize;
			details.sServerName = pIISInfo->pszServerName;
			details.sServerPort = pIISInfo->pszServerPort;
			details.sInstanceId = pIISInfo->pszInstanceId;
			details.sURLMapPath = pIISInfo->pszURLMapPath;
			details.sUserAgent = pRequestInfo->pszUserAgent;
			details.sHostname = pRequestInfo->pszHostname;
			details.sURI = pRequestInfo->pszURI;
			details.sQueryString = pRequestInfo->pszQueryString;
			details.sRemoteAddress = pRequestInfo->pszRemoteAddress;
			details.sContentType = pResponseInfo->pszContentType;			
			details.dwRawSize = pResponseInfo->dwContentLength;

			AutoLock<CHTTPNotify> autolock(*this);
			m_Responses.push_back(details);
			
			return S_OK;
		}

		array<ResponseDetails^>^ GetResponses()
		{
			AutoLock<CHTTPNotify> autolock(*this);

			size_t nResponses = m_Responses.size();

			array<ResponseDetails^>^ responses = gcnew array<ResponseDetails^>(nResponses);

			for (size_t i = 0; i < nResponses; i++)
			{
				responses[i] = m_Responses[i];
			}

			return responses;
		}

		void ClearResponses()
		{
			AutoLock<CHTTPNotify> autolock(*this);
			m_Responses.clear();
		}

	private:

		typedef std::map<DWORD, NativeResponseDetails> PendingResponseColl;
		PendingResponseColl m_PendingResponses;

		std::vector<NativeResponseDetails> m_Responses;		

	};
}