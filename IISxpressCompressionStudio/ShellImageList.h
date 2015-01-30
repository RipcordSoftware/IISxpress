#pragma once

// CShellImageList command target

class CShellImageList
{

public:

	CShellImageList();

	virtual ~CShellImageList();

	operator CImageList*(void) { return &m_ImageList; }

	int GetExtensionOffset(LPCTSTR pszExtension);
	bool GetExtensionDescription(LPCTSTR pszExtension, CString& sDescription);

private:

	CMap<CString, LPCTSTR, int, int> m_Offsets;
	CMap<CString, LPCTSTR, CString, CString> m_Descriptions;

	CImageList		m_ImageList;

};
