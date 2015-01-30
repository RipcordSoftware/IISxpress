#include "stdafx.h"

#include "ResponseStream.h"
#include "ResponseStreamMagicNumbers.h"

HRESULT CResponseStream::FinalConstruct()
{
	m_bSelfAllocated = false;
	m_pData = 0;
	m_dwSize = 0;
	m_dwOffset = 0;

	return S_OK;
}

void CResponseStream::FinalRelease()
{
	// lock the object
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	FreeSelfAllocatedMemoryBlock();
}

STDMETHODIMP CResponseStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	if (pv == NULL)
		return E_POINTER;

	if (cb == 0)
		return S_OK;

	// lock the object
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	// make sure the offset isn't outside the size of the buffer
	if (m_dwOffset > m_dwSize)
		return E_FAIL;

	// calculate how many bytes are in the array
	DWORD dwAvailable = m_dwSize - m_dwOffset;

	// adjust the size of the requested bytes
	if (dwAvailable < cb)
		cb = dwAvailable;

	// copy the data
	memcpy(pv, m_pData, cb);

	// adjust the offset
	m_dwOffset += cb;

	// if the client wants to know how many bytes were read then tell them
	if (pcbRead != NULL)
		*pcbRead = cb;

	return S_OK;
}

STDMETHODIMP CResponseStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	if (pv == NULL)
		return E_POINTER;

	if (cb == 0)
		return S_OK;

	// lock the object
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	// if the user is going to go out of bounds then calculate how much we can write
	if ((m_dwOffset + cb) > m_dwSize)
	{		
		cb = m_dwSize - m_dwOffset;
	}

	// copy the data
	memcpy((BYTE*) m_pData + m_dwOffset, pv, cb);

	// inform the user what we did
	if (pcbWritten != NULL)
	{
		*pcbWritten = cb;
	}

	m_dwOffset += cb;

	return S_OK;
}

STDMETHODIMP CResponseStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	// we must have a plibNewPosition
	if (plibNewPosition == NULL)
	{
		return STG_E_INVALIDPOINTER;
	}

	// lock the object
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	// it's an internal error
	if (m_pData == NULL)
		return E_FAIL;

	switch (dwOrigin)
	{

	case STREAM_SEEK_CUR:

		// translate into a SEEK_SET equivalent
		dlibMove.LowPart += m_dwOffset;

	case STREAM_SEEK_SET:

		// check for out of bounds
		if (dlibMove.LowPart < 0 || dlibMove.LowPart > m_dwSize)
			return E_INVALIDARG;

		// move the offset
		m_dwOffset = dlibMove.LowPart;

		// tell the caller
		plibNewPosition->HighPart = 0;
		plibNewPosition->LowPart = m_dwOffset;

		// life is good
		return S_OK;	

	case STREAM_SEEK_END:

		// we don't need this
		return E_NOTIMPL;		
	}

	return E_FAIL;
}

STDMETHODIMP CResponseStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	if (pstatstg == NULL)
		return E_POINTER;

	// we don't return a name
	if (grfStatFlag == STATFLAG_DEFAULT)
		return E_INVALIDARG;

	// lock the object
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	pstatstg->pwcsName = NULL;
	pstatstg->type = STGTY_STREAM;
	pstatstg->cbSize.HighPart = 0;
	pstatstg->cbSize.LowPart = m_dwSize;
	pstatstg->mtime = FILETIME();
	pstatstg->ctime = pstatstg->mtime;
	pstatstg->atime = pstatstg->mtime;
	pstatstg->grfMode = STGM_READWRITE | STGM_SHARE_DENY_NONE;
	pstatstg->clsid = CLSID_NULL;

	return S_OK;
}

STDMETHODIMP CResponseStream::SetSize(ULARGE_INTEGER libNewSize)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);	

	// if we have been asked to reset then set the stream offset to zero
	if (libNewSize.HighPart == RESPONSESTREAM_SETSIZE_RESETANDALLOC)
	{
		m_dwOffset = 0;
	}

	// if the caller is asking for a smaller or same sized buffer then just fake it
	if (libNewSize.LowPart <= m_dwSize)
		return S_OK;

	// allocate a new memory block	
	void* pData = (void*) ::SysAllocStringByteLen(NULL, libNewSize.LowPart);
	if (pData == NULL)
		return E_OUTOFMEMORY;		

	// if we haven't been asked for a reset then copy the existing data
	if (libNewSize.HighPart != RESPONSESTREAM_SETSIZE_RESETANDALLOC)
	{		
		// copy the old data
		memcpy(pData, m_pData, m_dwSize);
	}		

	// before we assign the new memory block get rid of the old one
	FreeSelfAllocatedMemoryBlock();

	// we now have a new buffer
	m_pData = pData;

	// adjust the size
	m_dwSize = libNewSize.LowPart;

	// mark as self allocated
	m_bSelfAllocated = true;

	return S_OK;	
}

bool CResponseStream::AttachBuffer(void* pData, DWORD dwSize, DWORD dwOffset)
{
	if (pData == NULL || dwSize == 0)
		return false;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	// get rid of the old memory block
	FreeSelfAllocatedMemoryBlock();

	m_pData = pData;
	m_dwSize = dwSize;
	m_dwOffset = dwOffset;	
	m_bSelfAllocated = false;

	return true;
}

bool CResponseStream::GetBuffer(void** ppData, DWORD* pdwSize, DWORD* pdwOffset)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (m_pData == NULL)
	{		
		return false;
	}

	if (ppData != NULL)
		*ppData = m_pData;

	if (pdwSize != NULL)
		*pdwSize = m_dwSize;

	if (pdwOffset != NULL)
		*pdwOffset = m_dwOffset;

	return true;	
}

DWORD CResponseStream::GetOffset(void)
{	
	return (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwOffset, 0);
}

void CResponseStream::ResetOffset(void)
{
	::InterlockedExchange((LONG*) &m_dwOffset, 0);
}

void CResponseStream::FreeSelfAllocatedMemoryBlock(void)
{
	// free the old memory block if we allocated it
	if (m_bSelfAllocated == true && m_pData != NULL)
	{
		::SysFreeString((BSTR) m_pData);
		m_pData = NULL;
		m_dwSize = 0;
		m_dwOffset = 0;
		m_bSelfAllocated = false;
	}
}