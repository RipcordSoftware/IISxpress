// EnterProductKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "EnterProductKeyDlg.h"

struct RTFLicenseData
{
	RTFLicenseData() : nOffset(0) {}

	CStringA	sLicense;
	int			nOffset;
};


// CEnterProductKeyDlg dialog

IMPLEMENT_DYNAMIC(CEnterProductKeyDlg, CDialog)

CEnterProductKeyDlg::CEnterProductKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterProductKeyDlg::IDD, pParent)
{
}

CEnterProductKeyDlg::~CEnterProductKeyDlg()
{
}

void CEnterProductKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRODUCTKEY, m_cProductKey);
	DDX_Control(pDX, IDC_RICHEDIT2_LICENSE, m_cLicense);
}


BEGIN_MESSAGE_MAP(CEnterProductKeyDlg, CDialog)		
	ON_MESSAGE(WM_HELP, OnHelp)
END_MESSAGE_MAP()


// CEnterProductKeyDlg message handlers

BOOL CEnterProductKeyDlg::OnInitDialog()
{
	BOOL bStatus = CDialog::OnInitDialog();

	// load the RTF license data from the resources
	RTFLicenseData data;
	LoadRTF(_T("License.rtf"), data.sLicense);	
	data.nOffset = 0;			

	// stream the RTF data into the rich edit control
	EDITSTREAM es;
	es.dwCookie = (DWORD_PTR) &data;
	es.dwError = 0;
	es.pfnCallback = StreamInCallback;
	m_cLicense.StreamIn(SF_RTF, es);

	return bStatus;
}

DWORD CALLBACK CEnterProductKeyDlg::StreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	RTFLicenseData* pData = reinterpret_cast<RTFLicenseData*>(dwCookie);

	int nLicenseSize = pData->sLicense.GetLength();

	int nBlockSize = nLicenseSize - pData->nOffset;
	if (cb < nBlockSize)
		nBlockSize = cb;

	const char* pszLicense = pData->sLicense;
	memcpy(pbBuff, pszLicense + pData->nOffset, nBlockSize);

	pData->nOffset += nBlockSize;
	*pcb = nBlockSize;
	
	return 0;
}

void CEnterProductKeyDlg::OnOK(void)
{
	// we must have a product manager interface here
	if (m_pProductLicenseManager == NULL)
		return;

	// load the caption for error dialogs
	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	// get the key string from the user
	CString sProductKey;
	m_cProductKey.GetWindowText(sProductKey);

	// check the length
	if (sProductKey.GetLength() <= 0)
	{
		CString sError;
		sError.LoadString(IDS_ERROR_ZEROLENGTHPRODUCTKEY);

		MessageBox(sError, sCaption, MB_OK | MB_ICONWARNING);
		m_cProductKey.SetFocus();
		return;
	}

	HRESULT hr = E_FAIL;

	try
	{
		// attempt to set the key (don't allow trials)
		hr = m_pProductLicenseManager->SetKey(CStringA(sProductKey), FALSE);		
	}
	catch (...)
	{
		// some kind of fatal COM error
		CString sError;
		sError.LoadString(IDS_ERROR_FATALPRODUCTKEYERR);
		MessageBox(sError, sCaption, MB_OK | MB_ICONERROR);
		m_cProductKey.SetFocus();
		return;
	}

	if (FAILED(hr) == TRUE)
	{		
		// the string was badly formed
		CString sError;
		sError.LoadString(IDS_ERROR_PRODUCTKEYBADFORMAT);		
		MessageBox(sError, sCaption, MB_OK | MB_ICONERROR);
		m_cProductKey.SetFocus();		
	}
	else if (hr == S_FALSE)
	{		
		// NOTE: this isn't used anymore - only left here incase it is ever enabled again
		// the key has already expired (or maybe it's already been trialed)		
		CString sError;
		sError.LoadString(IDS_ERROR_PROUCTKEYALREADYEXPIRED);		
		MessageBox(sError, sCaption, MB_OK | MB_ICONERROR);
		m_cProductKey.SetFocus();		
	}
	else
	{
		if (m_pRegSettings != NULL)
		{
			hr = m_pRegSettings->put_LicenseMode(LicenseModeFull);
		}

		// yeha!!!!
		CString sMsg;
		sMsg.LoadString(IDS_STRING_GOODPRODUCTKEY);
		MessageBox(sMsg, sCaption, MB_OK | MB_ICONINFORMATION);

		// set the caption
		CWnd* pMainWnd = ::AfxGetMainWnd();
		if (pMainWnd->IsKindOf(RUNTIME_CLASS(CIISxpressTrayPropertySheet)) == TRUE)
		{
			CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) pMainWnd;
			pSheet->SetCaptionEdition();
		}
		else
		{
			ASSERT("The root window is not a PropSheet");
		}

		CDialog::OnOK();
	}
}

void CEnterProductKeyDlg::Init(IProductLicenseManager* pProductLicenseManager, IComIISxpressRegSettings* pRegSettings)
{
	if (pProductLicenseManager != NULL)
	{
		m_pProductLicenseManager = pProductLicenseManager;
	}

	if (pRegSettings != NULL)
	{
		m_pRegSettings = pRegSettings;
	}
}

LRESULT CEnterProductKeyDlg::OnHelp(WPARAM, LPARAM)
{
	return 0;
}