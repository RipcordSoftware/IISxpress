// IISxpressHTTPRequest.h : Declaration of the CIISxpressHTTPRequest

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"

#include "MyHTTPResponseData.h"
#include "IISxpressRegSettings.h"

#include "IISMetabase.h"

#include "IISxpressHTTPNotifyHelper.h"

#include "TransientResponseHistory.h"

#include "Compressor.h"

#include "AHA363Connector.h"

#include <stack>
#include <map>
#include <vector>
using namespace std;

#include "AutoArray.h"

// CIISxpressHTTPRequest

struct IISxpressRequestContext
{
	IISxpressRequestContext(void)
	{
		dwContentLength = 0;
		dwResponseCode = 0;
		nLastUsed = 0;
		dwProcessorAffinityMask = 0;
	}

	CStringA			sUserAgent;
	CStringA			sURI;
	CStringA			sAcceptEncoding;	
	CStringA			sContentType;
	CStringA			sURLMapPath;
	CStringA			sLastModified;
	DWORD				dwContentLength;
	DWORD				dwResponseCode;
	CMyHTTPResponseData	ResponseData;	

	__int64				nLastUsed;

	DWORD				dwProcessorAffinityMask;
};

class ATL_NO_VTABLE CIISxpressHTTPRequest : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CIISxpressHTTPRequest, &CLSID_IISxpressHTTPRequest>,
	public IIISxpressHTTPRequest,
	public IConnectionPointContainerImpl<CIISxpressHTTPRequest>,
	public IConnectionPointImpl<CIISxpressHTTPRequest, &IID_IIISxpressHTTPNotify>,
	public IIISxpressSettings,
	public IAHAState
{

private:

	static const DWORD MAX_CONTEXTS = 128;

	static const DWORD DEFAULT_COMPRESSION_BUFFER = (64 * 1024);
	static const DWORD COMPRESSION_BUFFER_MEMLIMIT = (32 * 1024 * 1024);
	static const DWORD COMPRESSION_BUFFER_RESET_PERIOD = 30000;

	static const DWORD REQUEST_STACK_MIN_SCAVENGE_PERIOD = 2000;
	static const DWORD REQUEST_STACK_RESET_PERIOD = 10000;
	static const DWORD REQUEST_STACK_MAXDEADLIFETIME = 60000;
	static const DWORD REQUEST_STACK_FORCEKILLLIFETIME = (REQUEST_STACK_MAXDEADLIFETIME * 2);

	static const DWORD HOUSEKEEPING_CYCLE_PERIOD = 10000;

	static const DWORD WM_HOUSEKEEPRESPONSE = (WM_USER + 1);

public:

	CIISxpressHTTPRequest() : m_dwContext(0), m_dwCompressorOffset(0), m_pAHA363StatusComp(NULL), m_pAHA363Status(NULL), m_dwAHATempCheck(0)
	{		
		AHA::SGL_FLAGS sglFlags;
		AHA::STRM_FLAGS strmFlags;	
		ULONG dmaBlockSize = MAX_DMA_BUFFER_SIZE;
		BOOL zLib = FALSE;	

		sglFlags.u.Value = 0;
		strmFlags.u.Value = 2;
		m_pAHA363StatusComp = m_AHA363StatusConnector.CreateAHACompressor(1, 0, dmaBlockSize, strmFlags, sglFlags, zLib);
		if (m_pAHA363StatusComp != NULL)
		{
			m_pAHA363Status = dynamic_cast<AHA::IAHAStatus*>(m_pAHA363StatusComp);
		}
	}

	~CIISxpressHTTPRequest()
	{		
		m_AHA363StatusConnector.FreeAHACompressor(m_pAHA363StatusComp);
		m_pAHA363StatusComp = NULL;
		m_pAHA363Status = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_IISXPRESSHTTPREQUEST)

DECLARE_NOT_AGGREGATABLE(CIISxpressHTTPRequest)

BEGIN_COM_MAP(CIISxpressHTTPRequest)
	COM_INTERFACE_ENTRY(IIISxpressHTTPRequest)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)	
	COM_INTERFACE_ENTRY(IIISxpressSettings)
	COM_INTERFACE_ENTRY(IAHAState)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CIISxpressHTTPRequest)
	CONNECTION_POINT_ENTRY(IID_IIISxpressHTTPNotify)
END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

public:

	STDMETHOD(Init)(
		IStorage* pStorage, 
		ICompressionRuleManager* pCompressionRuleManager,
		IProductLicenseManager* pProductLicenseManager,
		IComIISxpressRegSettings* pRegSettings,
		IIISxpressSettings* pSettings
		);

	STDMETHOD(GetFilterVersion)(IStream* pFilterVersion);

	STDMETHOD(OnSendResponse)(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo,
		ResponseInfo* pResponseInfo,
		DWORD* pdwFilterContext);

	STDMETHOD(OnSendRawData)(
		DWORD dwFilterContext, 
		IStream* pRawDataStream, 
		DWORD dwStreamDataLength, 
		BOOL bHasHeader, 
		signed char* pszResponseContentEncoding,
		DWORD dwResponseContentEncodingBufferSize);

	STDMETHOD(OnEndOfRequest)(
		DWORD dwFilterContext, 
		IStream* pRawDataStream, 
		DWORD dwStreamDataLength,
		BOOL bHasHeader,
		signed char* pszResponseContentEncoding,
		DWORD dwResponseContentEncodingBufferSize);

	STDMETHOD(OnReadRawData)(IStream* pRawDataStream);

	STDMETHOD(OnEndOfNetSession)(DWORD dwFilterContext);

	STDMETHOD(IsAlive)(DWORD* pdwData);

	STDMETHOD(AbortRequest)(DWORD dwFilterContext);

	STDMETHOD(NotifyCacheHit)(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo,
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize);	

	// IIISxpressSettings
	STDMETHOD(get_CompressionMode)(CompressionMode* pMode);
	STDMETHOD(put_CompressionMode)(CompressionMode Mode);
	STDMETHOD(get_ProcessorAffinity)(DWORD* pdwProcessorAffinity);
	STDMETHOD(put_ProcessorAffinity)(DWORD dwProcessorAffinity);

	// IConnectionPoint
	STDMETHOD(Advise)(IUnknown* pUnkSink, DWORD* pdwCookie);
	STDMETHOD(Unadvise)(DWORD dwCookie);

	// IAHAState
	STDMETHOD(GetBoardTemps)(DWORD* pdwFPGA, DWORD* pdwBoard);
	STDMETHOD(GetBoardInfo)(AHABoardInfo* pBoardInfo);

private:

	HRESULT CompressResponse(DWORD dwFilterContext, IISxpressRequestContext* pRequestContext, IStream* pRawDataStream, char* pszResponseContentEncoding, DWORD dwResponseContentEncodingBufferSize);

	IISxpressRequestContext* LeaseContextObject(DWORD dwContext);
	void ReturnContextObject(DWORD dwContext);
	void RecoverContextObjects(void);
	IISxpressRequestContext* GetActiveContextObject(DWORD dwContext);

	bool BuildStream(CMyHTTPResponseData* pResponseData, IStream* pStream);	
	bool BuildStream(const string& sHeader, const BYTE* pbyBody, DWORD dwBodySize, IStream* pStream);

	CompressorMode MapCompressionMode(CompressionMode Mode);
	bool InitCompressorStreams(CompressorMode mode);

	DWORD GetProcessorAffinityMask(DWORD dwMode);

	static unsigned __stdcall HousekeepingProc(void*);
	DWORD HousekeepingProc(void);	

	static unsigned __stdcall StartupPerfCounterProc(void*);
	DWORD StartupPerfCounterProc(void);

	unsigned m_nHousekeepResponseDetailsThreadId;
	static unsigned __stdcall HousekeepResponseDetailsProc(void*);
	DWORD HousekeepResponseDetailsProc(void);
	
	DWORD									m_dwContext;

	IISxpressRequestContext					m_RequestContexts[MAX_CONTEXTS];

	CComAutoCriticalSection					m_csRequestStack;
	stack<IISxpressRequestContext*>			m_RequestContextStack;
	map<DWORD, IISxpressRequestContext*>	m_ActiveRequests;
	volatile __int64						m_nRequestStackLastUsedTickCount;
	volatile __int64						m_nRequestStackLastResetTickCount;

	template <int N> class CompressorBuffer
	{
	public:
		CompressorBuffer() : pbyDynBuffer(NULL), dwDynBufferSize(0), dwBufferSize(N)
		{			
		}

		~CompressorBuffer()
		{
			Free();
		}

		bool Allocate(DWORD size)
		{
			Free();

			if (size > dwBufferSize)
			{
				pbyDynBuffer = new BYTE[size];
				if (pbyDynBuffer != NULL)
				{
					dwDynBufferSize = size;
				}

				return pbyDynBuffer != NULL;
			}
			else
			{
				return true;
			}
		}

		void Free()
		{
			delete[] pbyDynBuffer;
			pbyDynBuffer = NULL;
			dwDynBufferSize = 0;
		}

		void Attach(CompressorBuffer& otherBuffer)
		{
			Free();

			if (otherBuffer.pbyDynBuffer != NULL && otherBuffer.dwDynBufferSize > 0)
			{
				pbyDynBuffer = otherBuffer.pbyDynBuffer;
				dwDynBufferSize = otherBuffer.dwDynBufferSize;

				otherBuffer.Detach();
			}
		}

		void Detach()
		{
			pbyDynBuffer = NULL;
			dwDynBufferSize = 0;
		}

		DWORD Size() { return dwDynBufferSize > 0 ? dwDynBufferSize : dwBufferSize; }

		operator BYTE*() { return pbyDynBuffer != NULL ? pbyDynBuffer : &byBuffer[0]; }

	private:

		BYTE byBuffer[N];
		const DWORD dwBufferSize;

		BYTE* pbyDynBuffer;
		DWORD dwDynBufferSize; 
	};

	struct CompressorData
	{
		CompressorData(void) : m_nCompressorLastUsedTickCount(0), 
			m_pAHA363GZIPCompressor(NULL), 
			m_pAHA363DeflateCompressor(NULL),
			m_dwAHA363DMAChannel(0)
		{	
			ULONG dmaChannel = (m_dwAHA363DMAChannel & 1) == 0 ? 0 : 2;			
			
			{				
				AHA::SGL_FLAGS sglFlags;
				AHA::STRM_FLAGS strmFlags;	
				ULONG dmaBlockSize = MAX_DMA_BUFFER_SIZE;
				BOOL zLib = FALSE;	

				sglFlags.u.Value = 0;
				strmFlags.u.Value = 0;
				m_pAHA363GZIPCompressor = m_AHA363Connector.CreateAHACompressor(1, dmaChannel, dmaBlockSize, strmFlags, sglFlags, zLib);				
			}

			{				
				AHA::SGL_FLAGS sglFlags;
				AHA::STRM_FLAGS strmFlags;	
				ULONG dmaBlockSize = MAX_DMA_BUFFER_SIZE;
				BOOL zLib = FALSE;	

				sglFlags.u.Value = 0;
				strmFlags.u.Value = 2;
				m_pAHA363DeflateCompressor = m_AHA363Connector.CreateAHACompressor(1, dmaChannel, dmaBlockSize, strmFlags, sglFlags, zLib);
			}

			++m_dwAHA363DMAChannel;
		}

		~CompressorData() 
		{ 			
			m_AHA363Connector.FreeAHACompressor(m_pAHA363GZIPCompressor);			
			m_AHA363Connector.FreeAHACompressor(m_pAHA363DeflateCompressor);			
		}

		CComAutoCriticalSection					m_csCompressor;

		CAHA363Connector						m_AHA363Connector;
		AHA::CAHA363*							m_pAHA363GZIPCompressor;		
		AHA::CAHA363*							m_pAHA363DeflateCompressor;		
		volatile DWORD							m_dwAHA363DMAChannel;

		CDeflateCompressor						m_DeflateCompressor;
		CGZipCompressor							m_GZipCompressor;
		CBZip2Compressor						m_BZip2Compressor;

		CompressorBuffer<DEFAULT_COMPRESSION_BUFFER> m_CompressedBuffer;

		volatile __int64						m_nCompressorLastUsedTickCount;

		bool InitCompressorStreams(CompressorMode mode)
		{
			m_DeflateCompressor.SetMode(mode);
			m_GZipCompressor.SetMode(mode);
			m_BZip2Compressor.SetMode(mode);
			return true;
		}
	};
	
	AutoArray<CompressorData>				m_Compressors;
	DWORD									m_dwCompressorOffset;

	CComPtr<ICompressionRuleManager>		m_pCompressionRuleManager;
	CComQIPtr<INeverCompressRules>			m_pNeverCompressRules;
	CComQIPtr<IStaticFileRules>				m_pStaticFileRules;
	CComQIPtr<ICompressionHistory>			m_pCompressionHistory;
	CComQIPtr<ICompressionHistory2>			m_pCompressionHistory2;
	CComPtr<IComIISxpressRegSettings>		m_pRegSettings;
	CComPtr<IIISxpressSettings>				m_pSettings;

	CComPtr<IProductLicenseManager>			m_pProductLicenseManager;

	CIISxpressHTTPNotifyHelper				m_NotifyHelper;

	// event handle to allow termination of threads
	HANDLE									m_hTermThreads;

	// performance counter stuff
	CComAutoCriticalSection					m_csPerfCounter;
	AutoHandle::AutoPerfQueryHandle			m_hPerfQuery;	
	AutoHandle::AutoPerfCounterHandle		m_hPerfCounter;	

	DWORD									m_dwIsAliveCookie;

	// keep a per-response transient history (server lifetime)
	CTransientResponseHistory				m_TransientHistory;

	// allow switching between cores
	DWORD									m_dwActiveCore;
	DWORD									m_dwNumberOfCores;

	// AHA363 status
	CAHA363Connector						m_AHA363StatusConnector;
	AHA::CAHA363*							m_pAHA363StatusComp;
	AHA::IAHAStatus*						m_pAHA363Status;

	volatile DWORD							m_dwAHATempCheck;
};

OBJECT_ENTRY_AUTO(__uuidof(IISxpressHTTPRequest), CIISxpressHTTPRequest)
