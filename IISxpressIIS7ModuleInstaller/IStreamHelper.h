#pragma once

class CIStreamHelper
{

public:

	static HRESULT GetFileDataAsStream(HANDLE hFile, IStream** ppStream);
	static HRESULT CommitStreamToFile(IStream* pStream, DWORD dwStreamLength, HANDLE hFile);
	static HRESULT CommitXMLToStream(IXMLDOMDocument* pDoc, IStream* pStream, DWORD& dwStreamLength);
};
