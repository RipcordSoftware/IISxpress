#pragma once
#include "afxcmn.h"

#include <wininet.h>

#include "CStringCompareNoCase.h"

class CAutoHINTERNET
{

public:

	CAutoHINTERNET(void)
	{
		m_hInet = NULL;
	}

	CAutoHINTERNET(HINTERNET hInet)
	{
		m_hInet = hInet;
	}

	~CAutoHINTERNET(void)
	{
		if (m_hInet != NULL)
		{
			::InternetCloseHandle(m_hInet);
			m_hInet = NULL;
		}
	}

	void operator = (HINTERNET hInet) { m_hInet = hInet; }

	operator HINTERNET(void) { return m_hInet; }

private:

	HINTERNET m_hInet;

};

// CAddContentTypeAutoAddSearchPage dialog

typedef struct _tagInstanceContentTypeSearch
{
	_tagInstanceContentTypeSearch(HINTERNET hInet) : Connect(hInet)
	{		
		Clear();
	}

	// the connection to the web server (Internet)
	CAutoHINTERNET								Connect;	

	// keep track of which content types we have found
	set<CString, CStringCompareNoCase>			ContentTypes;

	// keep track of file system paths we have already searched
	set<CString, CStringCompareNoCase>			FileSystemPathsSearched;	

	// keep track of the file extensions we have already searched
	set<CString, CStringCompareNoCase>			FileExtensionsSearched;

	void Clear(void)
	{		
		FileSystemPathsSearched.clear();		
	}
} InstanceContentTypeSearch;

class CAddContentTypeAutoAddSearchPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeAutoAddSearchPage)

public:
	CAddContentTypeAutoAddSearchPage();
	virtual ~CAddContentTypeAutoAddSearchPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_AUTOADD_SEARCH };	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	void OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	BOOL OnSetActive(void);
	void OnCancel(void);
	LRESULT OnWizardBack(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	// *********** WORKER THREAD STUFF : START ************************* //

	static UINT __cdecl SearchThread(void*);
	UINT SearchThread(void);	

	void StopSearchThread(void);

	bool GetWebServerPathsFromMetaBase(IMSAdminBase* pAdminBase, LPCWSTR pszMBPath, LPCWSTR pszURLPath, map<wstring, wstring>& WebSitePaths);
	bool GetWebServerPathsFromXML(IXMLDOMNode* pSite, map<wstring, wstring>& WebSitePaths);
	bool GetPathContentTypes(InstanceContentTypeSearch& Search, LPCTSTR pszFileSystemPath, LPCTSTR pszHostname, LPCTSTR pszURLPath);
	bool GetFileContentType(HINTERNET hConnect, LPCTSTR pszHostname, LPCTSTR pszURL, CString& sContentType);

	CAddContentTypeWizardSheet*		m_pSheet;	

	CAutoHINTERNET					m_Inet;

	CWinThread*						m_pSearchThread;
	::CEvent						m_SearchStop;

	// *********** WORKER THREAD STUFF : END *************************** //

	CListCtrl						m_cDirectoriesSearched;

	HCURSOR							m_hAppStarting;

	CArray<CString, LPCTSTR>		m_KnownInstances;
};
