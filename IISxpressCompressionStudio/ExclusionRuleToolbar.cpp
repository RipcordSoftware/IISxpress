// ExclusionRuleToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "ExclusionRuleToolbar.h"


// CExclusionRuleToolbar dialog

IMPLEMENT_DYNAMIC(CExclusionRuleToolbar, CDialogBar)

CExclusionRuleToolbar::CExclusionRuleToolbar(CWnd* pParent /*=NULL*/)
	: m_hTheme(NULL)
{
	m_hTheme = ::OpenThemeData(GetSafeHwnd(), _T("REBAR"));
}

CExclusionRuleToolbar::~CExclusionRuleToolbar()
{
	if (m_hTheme != NULL)
	{
		::CloseThemeData(m_hTheme);
	}
}

void CExclusionRuleToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExclusionRuleToolbar, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

// CExclusionRuleToolbar message handlers

LRESULT CExclusionRuleToolbar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT nStatus = CDialogBar::HandleInitDialog(wParam, lParam);	

	CString sTitle;
	sTitle.LoadString(IDS_TOOLBARTITLE_EXCLUSIONRULES);
	SetWindowText(sTitle);

	m_cExcludeExtn.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDEEXTN, this);
	m_cExcludeExtn.LoadPNG(_T("STAMP32.PNG"), 2);

	m_cExcludeCT.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDECT, this);
	m_cExcludeCT.LoadPNG(_T("DOCUMENT32.PNG"), 2);

	m_cExcludeMatchedCT.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDECT_MATCH, this);
	m_cExcludeMatchedCT.LoadPNG(_T("DOCUMENTGROUP32.PNG"), 2);

	m_cExcludeDir.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDEDIR, this);
	m_cExcludeDir.LoadPNG(_T("WEBDIRECTORY32.PNG"), 2);

	m_cExcludeDirTree.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDEDIRTREE, this);
	m_cExcludeDirTree.LoadPNG(_T("WEBHIERARCHY32.PNG"), 2);

	m_cExcludeWebSite.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDEWEBSITE, this);
	m_cExcludeWebSite.LoadPNG(_T("DETAILEDGLOBE32.PNG"), 2);

	m_cExcludeClientIP.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDECLIENTIP, this);
	m_cExcludeClientIP.LoadPNG(_T("FIGURE32.PNG"), 2);

	m_cExcludeNetworkIP.SubclassDlgItem(IDC_BUTTON_TOOLBAR_EXCLUDENETWORK, this);
	m_cExcludeNetworkIP.LoadPNG(_T("FIGUREGROUP32.PNG"), 2);

	SetBarStyle(GetBarStyle() | CBRS_FLYBY | CBRS_TOOLTIPS);
	EnableToolTips(TRUE);

	return nStatus;
}

BOOL CExclusionRuleToolbar::OnEraseBkgnd(CDC* pDC)
{	
	if (m_hTheme != NULL)
	{	
		CRect rcClient;
		GetClientRect(rcClient);				

		::DrawThemeBackground(m_hTheme, pDC->GetSafeHdc(), -1, 0, &rcClient, NULL);			

		return TRUE;
	}
	else
	{
		return CDialogBar::OnEraseBkgnd(pDC);
	}
}

BOOL CExclusionRuleToolbar::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*) pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*) pNMHDR;   

   UINT_PTR nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND) nID);
   }   

   if (pNMHDR->code == TTN_NEEDTEXTA)
   {
		CStringA strTipText;
		strTipText.LoadString((UINT) nID);

	   strcpy_s(pTTTA->szText, strTipText);
   }
   else
   {
		CStringW strTipText;
		strTipText.LoadString((UINT) nID);

	   wcscpy_s(pTTTW->szText, strTipText);
   }   

   *pResult = 0;

   return TRUE;    // message was handled
}