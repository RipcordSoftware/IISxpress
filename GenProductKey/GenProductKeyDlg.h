#pragma once
#include "afxwin.h"

#include "ProductLicense.h"

class CGenProductKeyDlg : public CDialog
{

public:
	CGenProductKeyDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_GENPRODUCTKEY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedCheckRegisterProduct();
	afx_msg void OnBnClickedCheckRandomizeFlagField();
	void OnOK(void);

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_cRecordID;
	CEdit m_cUserID;
	CEdit m_cProductVersion;
	CEdit m_cMonth;
	CEdit m_cYear;
	CEdit m_cFriendlyKey;
	CButton m_cRandomizeFlagField;
	CButton m_cStrongKeys;
	CButton m_cBigUserIds;
	CButton m_cRegisterProduct;
	CEdit m_cEncryptionKey;

	CProductLicense		m_License;
	
public:
	afx_msg void OnBnClickedButtonDeleteRegKeys();
};
