#include "StdAfx.h"
#include "PNGHelper.h"

bool CPNGHelper::LoadPNG(LPCTSTR pszImageName, CImage& Image)
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

void CPNGHelper::ApplyAlpha(CImage& image, DWORD dwModifyAlpha)
{
	for (int y = 0; y < image.GetHeight(); y++)
	{
		for (int x = 0; x < image.GetWidth(); x++)
		{
			COLORREF* pcrCol = (COLORREF*) image.GetPixelAddress(x, y);

			DWORD dwAlpha = *pcrCol >> 24;
			dwAlpha *= dwModifyAlpha;
			dwAlpha /= 0xff;

			DWORD red = *pcrCol & 0xff;			
			red *= dwAlpha;
			red /= 0xff;

			DWORD green = (*pcrCol >> 8) & 0xff;			
			green *= dwAlpha;
			green /= 0xff;

			DWORD blue = (*pcrCol >> 16) & 0xff;									
			blue *= dwAlpha;
			blue /= 0xff;
		
			COLORREF crNewCol = RGB(red, green, blue);
			crNewCol |= dwAlpha << 24;

			*pcrCol = crNewCol;
		}
	}
}

void CPNGHelper::ApplyGreyScale(CImage& image)
{
	for (int y = 0; y < image.GetHeight(); y++)
	{
		for (int x = 0; x < image.GetWidth(); x++)
		{
			COLORREF* pcrCol = (COLORREF*) image.GetPixelAddress(x, y);

			double red = *pcrCol & 0xff;
			red *= 0.2125;

			double green = (*pcrCol >> 8) & 0xff;
			green *= 0.7154;

			double blue = (*pcrCol >> 16) & 0xff;
			blue *= 0.0721;			

			double colour = red + green + blue;		

			COLORREF dwAlpha = *pcrCol & 0xff000000;
		
			COLORREF crNewCol = RGB(colour, colour, colour);
			
			crNewCol |= dwAlpha;
			*pcrCol = crNewCol;
		}
	}
}