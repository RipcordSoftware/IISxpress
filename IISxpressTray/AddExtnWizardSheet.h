#pragma once

// CAddExtnWizardSheet

class CAddExtnWizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAddExtnWizardSheet)

public:
	//CAddExtnWizardSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	//CAddExtnWizardSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	CAddExtnWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, HBITMAP hbmWatermark, 
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

	virtual ~CAddExtnWizardSheet();

	void Init(INeverCompressRules* pNeverRules, DWORD dwUpdateCookie) 
	{ 
		m_pNeverRules = pNeverRules; 
		m_dwUpdateCookie = dwUpdateCookie;
	}

	enum AddExtnWizardMode
	{
		Auto,
		Manual
	};

	void EnableHelp(BOOL bEnable);

	void SetAddExtnWizardMode(AddExtnWizardMode Mode) { m_Mode = Mode; }
	AddExtnWizardMode GetAddExtnWizardMode(void) { return m_Mode; }

	void SetAutoSearchInstances(const CArray<CString, LPCTSTR>& saInstancesToSearch) { m_saInstancesToSearch.Copy(saInstancesToSearch); }
	const CArray<CString, LPCTSTR>& GetAutoSearchInstances(void) { return m_saInstancesToSearch; }

	void SetSelectedExtensions(const CArray<CString, LPCTSTR>& saSelectedExtns) { m_saSelectedExtns.Copy(saSelectedExtns); }
	const CArray<CString, LPCTSTR>& GetSelectedExtensions(void) { return m_saSelectedExtns; }

	operator CIISMetaBase*(void) { return &m_Metabase; }
	HRESULT GetXMLConfigDoc(IXMLDOMDocument** ppDoc);

	bool GetNeverRules(INeverCompressRules** ppNeverRules);

	DWORD GetUpdateCookie() { return m_dwUpdateCookie; }

	CFont* GetHeaderFont(HDC hDC = NULL);

protected:

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog(void);

private:

	AddExtnWizardMode					m_Mode;

	CArray<CString, LPCTSTR>			m_saInstancesToSearch;

	CArray<CString, LPCTSTR>			m_saSelectedExtns;

	CIISMetaBase						m_Metabase;

	CComPtr<INeverCompressRules>		m_pNeverRules;

	auto_ptr<CFont>						m_pftHeader;

	CButton								m_cDisabledHelp;

	CComPtr<IXMLDOMDocument>			m_pConfigXML;

	DWORD								m_dwUpdateCookie;
};
