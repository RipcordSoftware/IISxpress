#pragma once

typedef struct tagRepairProcData
{
	tagRepairProcData(void)
	{
		hwndDlg = NULL;
	}

	CStringA	sPath;
	HWND		hwndDlg;
} RepairProcData;

// CRepairingFilterDlg dialog

class CRepairingFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CRepairingFilterDlg)

public:
	CRepairingFilterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepairingFilterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOGREPAIRINGFILTER };

	void Init(const CStringA& sPath) { m_sPath = sPath; }

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);	
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	static unsigned __stdcall RepairProc(void*);	
	RepairProcData	m_RepairProcData;

	HANDLE			m_hRepairThread;

	CStringA		m_sPath;
};
