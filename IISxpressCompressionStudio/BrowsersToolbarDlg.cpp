// BrowsersToolbarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "BrowsersToolbarDlg.h"

// CBrowsersToolbarDlg dialog

IMPLEMENT_DYNAMIC(CBrowsersToolbarDlg, CDialogBar)

CBrowsersToolbarDlg::CBrowsersToolbarDlg(CWnd* pParent /*=NULL*/)
	: m_hTheme(NULL)
{
	m_hTheme = ::OpenThemeData(GetSafeHwnd(), _T("REBAR"));
}

CBrowsersToolbarDlg::~CBrowsersToolbarDlg()
{
	if (m_hTheme != NULL)
	{
		::CloseThemeData(m_hTheme);
	}
}

void CBrowsersToolbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_TOOLBAR_IE, m_cIE);
	DDX_Control(pDX, IDC_BUTTON_TOOLBAR_FIREFOX, m_cFirefox);
	DDX_Control(pDX, IDC_BUTTON_TOOLBAR_OPERA, m_cOpera);
	DDX_Control(pDX, IDC_BUTTON_TOOLBAR_SAFARI, m_cSafari);
}


BEGIN_MESSAGE_MAP(CBrowsersToolbarDlg, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()


// CBrowsersToolbarDlg message handlers

LRESULT CBrowsersToolbarDlg::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT nStatus = CDialogBar::HandleInitDialog(wParam, lParam);	

	CString sTitle;
	sTitle.LoadString(IDS_TOOLBARTITLE_BROWSERS);
	SetWindowText(sTitle);

	m_cIE.SubclassDlgItem(IDC_BUTTON_TOOLBAR_IE, this);
	m_cIE.LoadPNG(_T("IEXPLORE32.PNG"), 2);

	m_cFirefox.SubclassDlgItem(IDC_BUTTON_TOOLBAR_FIREFOX, this);
	m_cFirefox.LoadPNG(_T("FIREFOX32.PNG"), 2);

	m_cOpera.SubclassDlgItem(IDC_BUTTON_TOOLBAR_OPERA, this);
	m_cOpera.LoadPNG(_T("OPERA32.PNG"), 2);

	m_cSafari.SubclassDlgItem(IDC_BUTTON_TOOLBAR_SAFARI, this);
	m_cSafari.LoadPNG(_T("SAFARI32.PNG"), 2);

	SetBarStyle(GetBarStyle() | CBRS_FLYBY | CBRS_TOOLTIPS);
	EnableToolTips(TRUE);

	return nStatus;
}

BOOL CBrowsersToolbarDlg::OnEraseBkgnd(CDC* pDC)
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

BOOL CBrowsersToolbarDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
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