#pragma once

#include "ResponseStream.h"

#define IISXPRESSFILTER_INVALID_CONTEXT		((DWORD) -1)

// we need a structure to keep track of the response context
struct IISxpressFilterContext
{	

private:

	IISxpressFilterContext(void) : 
		state(Default),
		dwContext(IISXPRESSFILTER_INVALID_CONTEXT),		// no server context yet
		nCompressionStart(0),							// we haven't embarked on compression yet
		pResponseStream(NULL),							// we don't have a stream object to start with
		hCacheItem(NULL)
	{							
		dwRawBlockCount = 0;			
		dwRemainingDataBytes = 0;
		dwContentLength = 0;		
	}

	~IISxpressFilterContext(void)
	{
		if (pResponseStream != NULL)
		{
			pResponseStream->Release();
			pResponseStream = NULL;
		}		
	}

public:	

	HRESULT AllocateStream(void)
	{
		if (pResponseStream != NULL)
		{
			return E_FAIL;
		}

		// create and lock the stream object
		HRESULT hr = CComObject<CResponseStream>::CreateInstance(&pResponseStream);		
		if (hr == S_OK)
		{
			pResponseStream->AddRef();
		}

		return hr;
	}	

	static IISxpressFilterContext* AllocateContext(void)
	{
		void* pVoid = (IISxpressFilterContext*) ::SysAllocStringByteLen(NULL, sizeof(IISxpressFilterContext));
		if (pVoid == NULL)
			return NULL;

		// construct the context object
		IISxpressFilterContext* pContext = new(pVoid) IISxpressFilterContext;
		return pContext;
	}

	static void FreeContext(IISxpressFilterContext* pContext)
	{
		if (pContext == NULL)
			return;

		// call the destructor
		pContext->~IISxpressFilterContext();

		// free the context 
		::SysFreeString((BSTR) pContext);
	}

	enum ContextState
	{
		Default = 0,
		Ignore = 1,											// set if the raw handler is to ignore the response
		SentToClient = 2,									// true if we have sent the response to the client
		Cached = 3
	};

	ContextState					state;

	DWORD							dwContext;				// the context cookie handed to us by the server		

	__int64							nCompressionStart;		// the time at the start of the compression	

	DWORD							dwRawBlockCount;		// the number of raw blocks received		
	DWORD							dwContentLength;		// the size of the response
	DWORD							dwRemainingDataBytes;	// the number of bytes remaining in this response		

	CComObject<CResponseStream>*	pResponseStream;		// the response stream we talk to	

	CAtlStringA						sCacheKey;
	HCACHEITEM						hCacheItem;

};

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************s

// we need a structure to hold the response context object and destroy it
// when it goes out of scope
class IISxpressFilterContextHolder
{

public:

	IISxpressFilterContextHolder(IISxpressFilterContext* pFilterContext) : m_pFilterContext(pFilterContext)
	{
	}

	~IISxpressFilterContextHolder()
	{
		if (m_pFilterContext != NULL)
		{
			IISxpressFilterContext::FreeContext(m_pFilterContext);
			m_pFilterContext = NULL;
		}
	}

	IISxpressFilterContext* Detach()
	{
		IISxpressFilterContext* pFilterContext = m_pFilterContext;
		m_pFilterContext = NULL;
		return pFilterContext;
	}

	bool operator !() { return m_pFilterContext == NULL; }

	IISxpressFilterContext* operator ->() { return m_pFilterContext; }

private:

	IISxpressFilterContext* m_pFilterContext;

};

// ***************************************************************************************************
// ***************************************************************************************************
// ***************************************************************************************************s

struct FilterContextContainer
{
	char*							pszPhysicalPath;		// the path to the file the user has requested
															// this is filled in by OnUrlMap before we start 
															// any processing	
	DWORD							dwPhysicalPathSize;

	FilterContextContainer(void)
	{
		pszPhysicalPath = NULL;
		dwPhysicalPathSize = 0;
		m_pIISxpressContext = NULL;
	}

	IISxpressFilterContext* GetContext(void) { return m_pIISxpressContext; }
	void SetContext(IISxpressFilterContext* pIISxpressContext) { m_pIISxpressContext = pIISxpressContext; }

	void FreeContext() 
	{ 
		if (m_pIISxpressContext != NULL)
		{
			IISxpressFilterContext::FreeContext(m_pIISxpressContext);
			m_pIISxpressContext = NULL;
		}
	}

private:

	IISxpressFilterContext*			m_pIISxpressContext;

};