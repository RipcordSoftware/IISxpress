// GenProductKey.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "GenProductKey.h"
#include "GenProductKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGenProductKeyApp

BEGIN_MESSAGE_MAP(CGenProductKeyApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGenProductKeyApp 建構

CGenProductKeyApp::CGenProductKeyApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CGenProductKeyApp 物件

CGenProductKeyApp theApp;


// CGenProductKeyApp 初始設定

BOOL CGenProductKeyApp::InitInstance()
{
	// 假如應用程式資訊清單指定使用 ComCtl32.dll 6.0 (含) 以後版本
	// 以啟用視覺化樣式，則 Windows XP 需要 InitCommonControls()。否則的話，
	// 任何視窗的建立將失敗。
	InitCommonControls();

	CWinApp::InitInstance();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少最後完成的可執行檔大小，您可以從下列
	// 程式碼移除不需要的初始化常式，變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串 (例如，公司名稱或組織名稱)
	SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	CGenProductKeyDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置於使用 [確定] 來停止使用對話方塊時
		// 處理的程式碼
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置於使用 [取消] 來停止使用對話方塊時
		// 處理的程式碼
	}

	// 因為已經關閉對話方塊，傳回 FALSE，所以我們會結束應用程式，
	// 而非提示開始應用程式的訊息。
	return FALSE;
}
