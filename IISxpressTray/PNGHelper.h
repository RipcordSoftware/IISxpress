#pragma once

class CPNGHelper
{
private:
	CPNGHelper(void) {}	

public:
	static bool LoadPNG(LPCTSTR pszImageName, CImage& Image);

	static void ApplyAlpha(CImage& image, DWORD dwModifyAlpha = 0xff);
	static void ApplyGreyScale(CImage& image);
};
