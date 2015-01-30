// stdafx.cpp : source file that includes just the standard includes
// IISxpressTray.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

OSVERSIONINFOEX g_VerInfo;

Ripcord::Utils::HiResTimer g_Timer;

FNGETPROCESSIMAGEFILENAME* g_fnGetProcessImageFileName = NULL;

CUserAgentLookup g_UserAgentLookup;

// we want a global shell image list for extensions
CShellImageList g_ShellImageList;

// we want common controls
#ifdef _WIN64
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='AMD64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

bool LoadPNG(LPCTSTR pszImageName, CImage& Image)
{
	if (pszImageName == NULL || ((HBITMAP) Image) != NULL)
		return false;

	// get the instance handle for loading the resource
	HINSTANCE hInst = ::AfxGetInstanceHandle();

	// find the resource
	HRSRC hRes = ::FindResource(hInst, pszImageName, _T("png"));
	if (hRes == NULL)
		return false;

	// load the resource
	HGLOBAL hLoadedImage = ::LoadResource(hInst, hRes);
	if (hLoadedImage == NULL)
		return false;

	bool bStatus = false;

	// get a pointer to the bitmap
	void* pSrcImageData = ::LockResource(hLoadedImage);
	if (pSrcImageData != NULL)
	{
		// get the size of the resource since we need to load it into memory
		DWORD dwImageSize = ::SizeofResource(hInst, hRes);

		// allocate memory for the bitmap
		HGLOBAL hDestImageData = ::GlobalAlloc(GMEM_MOVEABLE, dwImageSize);
		if (hDestImageData != NULL)
		{
			// create an IStream to allow us to load the bitmap
			CComPtr<IStream> pImageStream;
			HRESULT hr = ::CreateStreamOnHGlobal(hDestImageData, TRUE, &pImageStream);
			if (hr == S_OK)
			{
				// write the data into the stream
				ULONG nWritten = 0;
				hr = pImageStream->Write(pSrcImageData, dwImageSize, &nWritten);

				// load the bitmap now
				if (Image.Load(pImageStream) == S_OK)
				{
					bStatus = true;
				}
			}		
		}
	}

	::UnlockResource(hLoadedImage);	

	return bStatus;
}

bool LoadRTF(LPCTSTR pszRTFName, CStringA& sRTF)
{
	if (pszRTFName == NULL)
		return false;

	// get the instance handle for loading the resource
	HINSTANCE hInst = ::AfxGetInstanceHandle();

	// find the resource
	HRSRC hRes = ::FindResource(hInst, pszRTFName, _T("rtf"));
	if (hRes == NULL)
		return false;

	// load the resource
	HGLOBAL hLoadedResource = ::LoadResource(hInst, hRes);
	if (hLoadedResource == NULL)
		return false;

	bool bStatus = false;

	// get a pointer to the bitmap
	const char* pSrcRTFData = (const char*) ::LockResource(hLoadedResource);
	if (pSrcRTFData != NULL)
	{
		// get the size of the resource since we need to load it into memory
		DWORD dwRTFSize = ::SizeofResource(hInst, hRes);

		sRTF = CStringA(pSrcRTFData, dwRTFSize);

		::UnlockResource(hLoadedResource);	

		bStatus = true;
	}	

	return bStatus;
}