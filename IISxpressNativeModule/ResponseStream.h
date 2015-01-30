#pragma once

#include <atlsoap.h>

class ATL_NO_VTABLE CResponseStream :
	public CComObjectRootEx<CComMultiThreadModel>,	
	public CComCoClass<CResponseStream, &CLSID_NULL>,
	public IStreamImpl
{

public:

DECLARE_NOT_AGGREGATABLE(CResponseStream)

BEGIN_COM_MAP(CResponseStream)
	COM_INTERFACE_ENTRY(IStream)	
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	// IStream overrides
	STDMETHOD(Read)(void* pv, ULONG cb, ULONG* pcbRead);
	STDMETHOD(Write)(void const* pv, ULONG cb, ULONG* pcbWritten);
	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
	STDMETHOD(Stat)(STATSTG* pstatstg, DWORD grfStatFlag);
	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);

	bool AttachBuffer(void* pData, DWORD dwSize, DWORD dwOffset);	
	bool GetBuffer(void** ppData = NULL, DWORD* pdwSize = NULL, DWORD* pdwOffset = NULL);

	DWORD GetOffset(void);
	void ResetOffset(void);

	void FreeSelfAllocatedMemoryBlock(void);

private:

	CComAutoCriticalSection		m_cs;

	bool						m_bSelfAllocated;
	void*						m_pData;
	DWORD						m_dwSize;
	DWORD						m_dwOffset;
};