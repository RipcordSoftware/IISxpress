// GenProductKeyDlg.cpp : 
//

#include "stdafx.h"
#include "GenProductKey.h"
#include "GenProductKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ProductLicense.h"
#include "ProductKeyRegistryGuids.h"

#include <atlbase.h>

// CGenProductKeyDlg 

const static TCHAR* g_szRegistryKeys[] = 
{
	PRODUCTLICENCE_REGKEY1, PRODUCTLICENCE_REGKEY2, PRODUCTLICENCE_REGKEY3, PRODUCTLICENCE_REGKEY4, PRODUCTLICENCE_REGKEY5,
	PRODUCTLICENCE_REGKEY6, PRODUCTLICENCE_REGKEY7, PRODUCTLICENCE_REGKEY8, PRODUCTLICENCE_REGKEY9, PRODUCTLICENCE_REGKEY10
};

CGenProductKeyDlg::CGenProductKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenProductKeyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGenProductKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECORDID, m_cRecordID);
	DDX_Control(pDX, IDC_EDIT_USERID, m_cUserID);
	DDX_Control(pDX, IDC_EDIT_PRODUCTVERSION, m_cProductVersion);
	DDX_Control(pDX, IDC_EDIT_MONTH, m_cMonth);
	DDX_Control(pDX, IDC_EDIT_YEAR, m_cYear);
	DDX_Control(pDX, IDC_EDIT_FRIENDLYKEY, m_cFriendlyKey);
	DDX_Control(pDX, IDC_CHECK_RANDOMIZEFLAGFIELD, m_cRandomizeFlagField);
	DDX_Control(pDX, IDC_CHECK_REGISTERPRODUCT, m_cRegisterProduct);
	DDX_Control(pDX, IDC_EDIT_ENCRYPTIONKEY, m_cEncryptionKey);
	DDX_Control(pDX, IDC_CHECKSTRONGKEY, m_cStrongKeys);
	DDX_Control(pDX, IDC_CHECK_BIGUSERIDS, m_cBigUserIds);
}

BEGIN_MESSAGE_MAP(CGenProductKeyDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_REGISTERPRODUCT, OnBnClickedCheckRegisterProduct)
	ON_BN_CLICKED(IDC_CHECK_RANDOMIZEFLAGFIELD, OnBnClickedCheckRandomizeFlagField)
	ON_BN_CLICKED(IDC_BUTTON_DELETEREGKEYS, OnBnClickedButtonDeleteRegKeys)
END_MESSAGE_MAP()


BOOL CGenProductKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);

	m_cRecordID.SetWindowText(CString(PRODUCTKEY_ID));

	m_cRandomizeFlagField.SetCheck(BST_CHECKED);

	m_cUserID.SetWindowText(_T("0"));

	m_cProductVersion.SetWindowText(_T("1.0"));

	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);

	stLocal.wMonth += 3;
	if (stLocal.wMonth > 12)
	{
		stLocal.wMonth -= 12;
		stLocal.wYear++;
	}

	CString sMonth;
	sMonth.Format(_T("%u"), stLocal.wMonth);
	m_cMonth.SetWindowText(sMonth);

	CString sYear;
	sYear.Format(_T("%u"), stLocal.wYear);
	m_cYear.SetWindowText(sYear);
	
	return TRUE;  
}


void CGenProductKeyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CGenProductKeyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGenProductKeyDlg::OnBnClickedCheckRegisterProduct()
{
	if ((m_cRegisterProduct.GetCheck() & BST_CHECKED) == BST_CHECKED)
	{		
		m_cMonth.EnableWindow(FALSE);
		m_cYear.EnableWindow(FALSE);
	}
	else
	{
		m_cMonth.EnableWindow(TRUE);
		m_cYear.EnableWindow(TRUE);	
	}
}

void CGenProductKeyDlg::OnOK(void)
{
	CString sRecordID;
	m_cRecordID.GetWindowText(sRecordID);
	if (sRecordID.GetLength() != 1)
	{
		MessageBox(_T("The record id should be 1 character"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	bool bRandomizeFlagField = (m_cRandomizeFlagField.GetCheck() == BST_CHECKED);
	bool bBigUserIds = (m_cBigUserIds.GetCheck() == BST_CHECKED);

	DWORD dwUserIdMax = 0x00010000;
	if (bRandomizeFlagField == false && bBigUserIds == true)
	{
		dwUserIdMax <<= 4;
	}

	CString sUserID;
	m_cUserID.GetWindowText(sUserID);
	DWORD dwUserID = _ttol(sUserID);
	if (dwUserID >= dwUserIdMax)
	{
		MessageBox(_T("The user id specified is too large"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	CString sMonth;
	m_cMonth.GetWindowText(sMonth);
	DWORD dwMonth = _ttol(sMonth);
	if (dwMonth > 12)
	{
		MessageBox(_T("The month is greater than 12"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	CString sYear;
	m_cYear.GetWindowText(sYear);
	DWORD dwYear = _ttol(sYear);
	if (dwYear < 2005)
	{
		MessageBox(_T("The year is less than 2005"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	CString sEncryptionKey;
	m_cEncryptionKey.GetWindowText(sEncryptionKey);
	if (sEncryptionKey.GetLength() == 0)
	{
		MessageBox(_T("You have not entered an encryption key"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	ProductKeyData ProductKey;

	ProductKey.byID = sRecordID[0];

	if (bRandomizeFlagField == true)
	{
		ProductKey.byFlags = (BYTE) (::GetTickCount() >> 4);
		ProductKey.byFlags &= 0x7f;
		ProductKey.byFlags |= 0x80;
	}
	else
	{
		ProductKey.byFlags = 0;

		// set the strong key bit
		if ((m_cStrongKeys.GetCheck() & BST_CHECKED) == BST_CHECKED)
		{
			ProductKey.byFlags |= 0x40;			
		}

		// set the big user id bit
		if (bBigUserIds == true)
		{
			ProductKey.byFlags |= 0x20;
		}
		else
		{
			// no big user id, so bottom 4 bits are random
			BYTE byRandom = (BYTE) (::GetTickCount() >> 4);
			byRandom &= 0x0f;
			ProductKey.byFlags |= byRandom;			
		}
	}
	
	ProductKey.wUserId = (WORD) dwUserID;	
	
	// we need to stuff the MSB of the user id field into the flag field
	if (bRandomizeFlagField == false && bBigUserIds == true)
	{
		BYTE byUserIDMSB = (BYTE) (dwUserID >> 16);
		byUserIDMSB &= 0x0f;
		ProductKey.byFlags |= byUserIDMSB;
	}

	CString sProductVersion;
	m_cProductVersion.GetWindowText(sProductVersion);
	double fVersion = atof(CStringA(sProductVersion));
	DWORD dwVersion = (DWORD) (fVersion * 10);
	ProductKey.byVersion = (BYTE) (dwVersion / 10);
	ProductKey.byVersion <<= 4;
	ProductKey.byVersion |= (BYTE) (dwVersion % 10);

	if ((m_cRegisterProduct.GetCheck() & BST_CHECKED) == BST_CHECKED)
	{
		ProductKey.byMonthYear = PRODUCTKEY_REGISTERED;
	}
	else
	{
		ProductKey.byMonthYear = (BYTE) dwMonth;
		ProductKey.byMonthYear <<= 4;
		ProductKey.byMonthYear |= (BYTE) (dwYear - 2005);
	}	

	m_License.Encrypt(ProductKey, (const BYTE*) (const char*) sEncryptionKey, sEncryptionKey.GetLength());

	CString sFriendlyProductKey;
	m_License.ConvertProductKeyToFriendlyString(ProductKey, sFriendlyProductKey);
	m_cFriendlyKey.SetWindowText(sFriendlyProductKey);
}

void CGenProductKeyDlg::OnBnClickedCheckRandomizeFlagField()
{
	if ((m_cRandomizeFlagField.GetCheck() & BST_CHECKED) == BST_CHECKED)
	{		
		m_cStrongKeys.EnableWindow(FALSE);
		m_cBigUserIds.EnableWindow(FALSE);		
	}
	else
	{
		m_cStrongKeys.EnableWindow(TRUE);	
		m_cBigUserIds.EnableWindow(TRUE);		
	}
}

void CGenProductKeyDlg::OnBnClickedButtonDeleteRegKeys()
{

	if (MessageBox(
		_T("Do you want to delete all local product key registry entries?"),
		_T("GenProductKey"), MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	CRegKey ProductKey;
	if (ProductKey.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_SET_VALUE) != ERROR_SUCCESS)
		return;	
	
	for (int i = 0; i < PRODUCTLICENCE_MAXKEY; i++)
	{			
		ProductKey.DeleteSubKey(g_szRegistryKeys[i]);
	}	
}
