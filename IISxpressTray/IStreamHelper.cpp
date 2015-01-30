#include "stdafx.h"

#include "IStreamHelper.h"

HRESULT CIStreamHelper::GetFileDataAsStream(HANDLE hFile, IStream** ppStream)
{
	if (hFile == INVALID_HANDLE_VALUE)
		return E_INVALIDARG;

	if (ppStream == NULL)
		return E_POINTER;

	// get the size of the file
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	
	// allocate an array to hold the xml data
	Ripcord::Types::AutoArray<BYTE> pbyData(dwFileSize);
	if (pbyData == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// read the data from the file into memory
	DWORD dwRead = 0;
	::ReadFile(hFile, pbyData, dwFileSize, &dwRead, NULL);
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	// put the xml data into a stream
	CComPtr<IStream> pStream;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	if (hr != S_OK)
	{
		return hr;
	}

	// write the xml to the stream
	ULONG nWritten = 0;
	hr = pStream->Write(pbyData, dwFileSize, &nWritten);	

	// put the file pointer back to the beginning of the stream
	LARGE_INTEGER nZero = { 0, 0 };
	hr = pStream->Seek(nZero, STREAM_SEEK_SET, NULL);

	pStream.CopyTo(ppStream);

	return S_OK;
}

HRESULT CIStreamHelper::CommitStreamToFile(IStream* pStream, DWORD dwStreamLength, HANDLE hFile)
{
	if (pStream == NULL)
		return E_POINTER;

	if (dwStreamLength == 0 || hFile == INVALID_HANDLE_VALUE)
		return E_INVALIDARG;

	// allocate a buffer to hold the outgoing data
	Ripcord::Types::AutoArray<BYTE> pbyData(dwStreamLength);
	if (pbyData == NULL)
		return E_OUTOFMEMORY;

	// get back to the beginning of the stream object
	LARGE_INTEGER nZero = { 0, 0 };
	HRESULT hr = pStream->Seek(nZero, STREAM_SEEK_SET, NULL);	

	// read the XML data from the stream
	hr = pStream->Read(pbyData, dwStreamLength, NULL);

	// write the XML data back to the file
	DWORD dwBytesWritten = 0;
	::WriteFile(hFile, pbyData, dwStreamLength, &dwBytesWritten, NULL);
	::SetEndOfFile(hFile);	

	return S_OK;
}

HRESULT CIStreamHelper::CommitXMLToStream(IXMLDOMDocument* pDoc, IStream* pStream, DWORD& dwStreamLength)
{
	if (pDoc == NULL || pStream == NULL)
		return E_POINTER;

	// get back to the beginning of the stream object
	LARGE_INTEGER nZero = { 0, 0 };
	HRESULT hr = pStream->Seek(nZero, STREAM_SEEK_SET, NULL);	

	// write the XML into the stream
	hr = pDoc->save(CComVariant(pStream));

	// get the new length of the stream
	ULARGE_INTEGER nOffset;
	hr = pStream->Seek(nZero, STREAM_SEEK_CUR, &nOffset);

	dwStreamLength = nOffset.LowPart;

	return S_OK;
}