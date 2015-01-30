#pragma once



// CAddContentTypeWizardSheet

class CAddContentTypeWizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAddContentTypeWizardSheet)

public:

	//CAddContentTypeWizardSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	//CAddContentTypeWizardSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	CAddContentTypeWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, HBITMAP hbmWatermark, 
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

	virtual ~CAddContentTypeWizardSheet();

	enum AddContentTypeWizardMode
	{
		Auto,
		Manual
	};

	void EnableHelp(BOOL bEnable);

	void Init(INeverCompressRules* pNeverRules, DWORD dwUpdateCookie) 
	{ 
		m_pNeverRules = pNeverRules; 
		m_dwUpdateCookie = dwUpdateCookie;
	}

	void SetAddContentTypeWizardMode(AddContentTypeWizardMode Mode) { m_Mode = Mode; }
	AddContentTypeWizardMode GetAddContentTypeWizardMode(void) { return m_Mode; }

	void SetAutoSearchInstances(const CArray<CString, LPCTSTR>& saInstancesToSearch) { m_saInstancesToSearch.Copy(saInstancesToSearch); }
	const CArray<CString, LPCTSTR>& GetAutoSearchInstances(void) { return m_saInstancesToSearch; }

	void SetSelectedContentTypes(const CArray<CString, LPCTSTR>& saSelectedContentTypes) { m_saSelectedContentTypes.Copy(saSelectedContentTypes); }
	const CArray<CString, LPCTSTR>& GetSelectedContentTypes(void) { return m_saSelectedContentTypes; }

	void SetCandidateContentTypes(const CArray<CString, LPCTSTR>& saCandidateContentTypes) { m_saCandidateContentTypes.Copy(saCandidateContentTypes); }
	const CArray<CString, LPCTSTR>& GetCandidateContentTypes(void) { return m_saCandidateContentTypes; }

	CFont* GetHeaderFont(HDC hDC = NULL);

	operator CIISMetaBase*(void) { return &m_Metabase; }

	HRESULT GetXMLConfigDoc(IXMLDOMDocument** ppDoc);

	bool GetNeverRules(INeverCompressRules** ppNeverRules);

	DWORD GetUpdateCookie() { return m_dwUpdateCookie; }

protected:

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog(void);

private:

	AddContentTypeWizardMode			m_Mode;

	CIISMetaBase						m_Metabase;

	CComPtr<INeverCompressRules>		m_pNeverRules;

	CArray<CString, LPCTSTR>			m_saInstancesToSearch;

	CArray<CString, LPCTSTR>			m_saCandidateContentTypes;

	CArray<CString, LPCTSTR>			m_saSelectedContentTypes;	

	auto_ptr<CFont>						m_pftHeader;
	
	CButton								m_cDisabledHelp;

	CComPtr<IXMLDOMDocument>			m_pConfigXML;

	DWORD								m_dwUpdateCookie;
};