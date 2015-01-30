#pragma once
#include "afxcmn.h"

#include "CStringCompareNoCase.h"

typedef struct _tagInstanceExtensionSearch
{
	_tagInstanceExtensionSearch(void)
	{		
		Clear();
	}	

	// keep track of the file extensions we have found
	set<CStringW, CStringWCompareNoCase>			FileExtensionsFound;

	// keep track of file system paths we have already searched
	set<CStringW, CStringWCompareNoCase>			FileSystemPathsSearched;		

	void Clear(void)
	{		
		FileSystemPathsSearched.clear();
	}
} InstanceExtensionSearch;

// CAddExtnAutoAddSelectExtnsPage dialog

class CAddExtnAutoAddSelectExtnsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddExtnAutoAddSelectExtnsPage)

public:
	CAddExtnAutoAddSelectExtnsPage();
	virtual ~CAddExtnAutoAddSelectExtnsPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDEXTN_AUTOADD_SELECTEXTN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnLvnItemChangedListSelectExtns(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	void UpdateControls();

	bool GetWebServerPathsFromMetaBase(CIISMetaBase* pMetaBase, LPCWSTR pszMBPath, set<wstring>& WebSitePaths, bool bClear = true);

	bool GetPathExtensions(InstanceExtensionSearch& Search, LPCWSTR pszPath);

	bool GetSelectedExtensions(CArray<CString, LPCTSTR>& saExtensions);

	CColouredListCtrl m_cExtensions;	
};
