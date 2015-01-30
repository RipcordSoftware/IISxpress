#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CEnterProductKeyDlg dialog

class CEnterProductKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnterProductKeyDlg)

public:
	CEnterProductKeyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnterProductKeyDlg();

	void Init(IProductLicenseManager* pProductLicenseManager, IComIISxpressRegSettings* pRegSettings);	

// Dialog Data
	enum { IDD = IDD_DIALOG_ENTERPRODKEY };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void OnOK(void);
	LRESULT OnHelp(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

private:

	static DWORD CALLBACK StreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

	CComPtr<IProductLicenseManager>		m_pProductLicenseManager;
	CComPtr<IComIISxpressRegSettings>	m_pRegSettings;

	CEdit				m_cProductKey;
	CRichEditCtrl		m_cLicense;
};
