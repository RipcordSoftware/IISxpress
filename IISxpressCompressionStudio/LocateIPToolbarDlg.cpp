// LocateIPToolbarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "LocateIPToolbarDlg.h"


// CLocateIPToolbarDlg dialog

IMPLEMENT_DYNAMIC(CLocateIPToolbarDlg, CDialogBar)

CLocateIPToolbarDlg::CLocateIPToolbarDlg(CWnd* pParent /*=NULL*/)
	//: CDialogBar(CLocateIPToolbarDlg::IDD, pParent)
	: m_hTheme(NULL)
{	
	m_hTheme = ::OpenThemeData(GetSafeHwnd(), _T("REBAR"));
}

CLocateIPToolbarDlg::~CLocateIPToolbarDlg()
{
	if (m_hTheme != NULL)
	{
		::CloseThemeData(m_hTheme);
	}
}

void CLocateIPToolbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_LOCATE, m_cIPAddress);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_cLocation);
}


BEGIN_MESSAGE_MAP(CLocateIPToolbarDlg, CDialogBar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCATE, &CLocateIPToolbarDlg::OnBnClickedButtonLocate)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_LOCATE, OnCmdUIButtonLocate)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()


// CLocateIPToolbarDlg message handlers

LRESULT CLocateIPToolbarDlg::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT nStatus = CDialogBar::HandleInitDialog(wParam, lParam);	

	CString sTitle;
	sTitle.LoadString(IDS_TOOLBARTITLE_LOCATE);
	SetWindowText(sTitle);

	SetBarStyle(GetBarStyle() | CBRS_FLYBY | CBRS_TOOLTIPS);
	EnableToolTips(TRUE);

	return nStatus;
}

void CLocateIPToolbarDlg::OnBnClickedButtonLocate()
{
	UpdateData(FALSE);

	DWORD dwAddress = 0;
	m_cIPAddress.GetAddress(dwAddress);

	LPCTSTR pszCountry = CGeoIPHelper::LookupCountry(dwAddress);
	if (pszCountry != NULL)
	{
		m_cLocation.SetWindowText(pszCountry);
	}
	else
	{
		m_cLocation.SetWindowText(_T(""));	
	}
}

BOOL CLocateIPToolbarDlg::OnEraseBkgnd(CDC* pDC)
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

void CLocateIPToolbarDlg::OnCmdUIButtonLocate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

BOOL CLocateIPToolbarDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
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