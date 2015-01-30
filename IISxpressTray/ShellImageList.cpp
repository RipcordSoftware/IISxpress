// ShellImageList.cpp : implementation file
//

#include "stdafx.h"
#include "ShellImageList.h"


// CShellImageList

CShellImageList::CShellImageList()
{
	SHFILEINFO FileInfo;
	HIMAGELIST hImageList = (HIMAGELIST) ::SHGetFileInfo(
								_T(".exe"), FILE_ATTRIBUTE_NORMAL, 
								&FileInfo, sizeof(FileInfo), 
								SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);	

	// we got the system image list, attach to it
	if (hImageList != 0)
	{						
		m_ImageList.Attach(hImageList);					
	}	
}

CShellImageList::~CShellImageList()
{
}


// CShellImageList member functions


int CShellImageList::GetExtensionOffset(LPCTSTR pszExtension)
{
	if (pszExtension == 0)
		return -1;

	int nLength = (int) _tcslen(pszExtension);
	if (nLength <= 0)
		return -1;

	if (pszExtension[0] != '.')
	{
		int nTempLength = nLength + 1 + 1;
		LPTSTR pszTemp = (LPTSTR) alloca(nTempLength * sizeof(pszExtension[0]));
		pszTemp[0] = '.';
		_tcscpy_s(pszTemp + 1, nTempLength - 1, pszExtension);
		pszExtension = pszTemp;
	}

	int nOffset = 0;
	if (m_Offsets.Lookup(pszExtension, nOffset) == TRUE)
		return nOffset;

	SHFILEINFO FileInfo;
	::SHGetFileInfo(pszExtension, FILE_ATTRIBUTE_NORMAL, &FileInfo, sizeof(FileInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	m_Offsets[pszExtension] = FileInfo.iIcon;	

	return FileInfo.iIcon;
}

bool CShellImageList::GetExtensionDescription(LPCTSTR pszExtension, CString& sDescription)
{
	if (pszExtension == 0)
		return false;

	int nLength = (int) _tcslen(pszExtension);
	if (nLength <= 0)
		return false;

	if (pszExtension[0] != '.')
	{
		int nTempLength = nLength + 1 + 1;
		LPTSTR pszTemp = (LPTSTR) alloca(nTempLength * sizeof(pszExtension[0]));
		pszTemp[0] = '.';
		_tcscpy_s(pszTemp + 1, nTempLength - 1, pszExtension);
		pszExtension = pszTemp;
	}

	CString sTemp;
	if (m_Descriptions.Lookup(pszExtension, sTemp) == TRUE)
	{
		sDescription = sTemp;
		return true;
	}

	SHFILEINFO FileInfo;
	if (::SHGetFileInfo(pszExtension, FILE_ATTRIBUTE_NORMAL, &FileInfo, sizeof(FileInfo), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME) == 0)
	{
		return false;
	}

	// store the description for later
	m_Descriptions[pszExtension] = FileInfo.szTypeName;

	// return the typename to the caller
	sDescription = FileInfo.szTypeName;

	return false;
}