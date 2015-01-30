// CompressionOptimizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "CompressionOptimizerDlg.h"

// CCompressionOptimizerDlg dialog



IMPLEMENT_DYNAMIC(CCompressionOptimizerDlg, CDialog)

CCompressionOptimizerDlg::CCompressionOptimizerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompressionOptimizerDlg::IDD, pParent), m_nBlankIconOffset(-1), m_nPerformanceIconOffset(-1)
{

}

CCompressionOptimizerDlg::~CCompressionOptimizerDlg()
{
}

void CCompressionOptimizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_cFiles);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_cApply);
	DDX_Control(pDX, IDC_DEFAULT_SIZE, m_cDefaultSize);
	DDX_Control(pDX, IDC_FASTEST_SIZE, m_cFastestSize);
	DDX_Control(pDX, IDC_SMALLEST_SIZE, m_cSmallestSize);
	DDX_Control(pDX, IDC_DEFAULT_TIME, m_cDefaultTime);
	DDX_Control(pDX, IDC_FASTEST_TIME, m_cFastestTime);
	DDX_Control(pDX, IDC_SMALLEST_TIME, m_cSmallestTime);
	DDX_Control(pDX, IDC_DEFAULT_RESPONSESSEC, m_cDefaultResponsesSec);
	DDX_Control(pDX, IDC_FASTEST_RESPONSESSEC, m_cFastestResponsesSec);
	DDX_Control(pDX, IDC_SMALLEST_RESPONSESSEC, m_cSmallestResponsesSec);
	DDX_Control(pDX, IDC_OPTDEFAULT_SIZE, m_cOptDefaultSize);
	DDX_Control(pDX, IDC_OPTDEFAULT_TIME, m_cOptDefaultTime);
	DDX_Control(pDX, IDC_OPTDEFAULT_RESPONSESSEC, m_cOptDefaultResponsesSec);
	DDX_Control(pDX, IDC_OPTFASTEST_SIZE, m_cOptFastestSize);
	DDX_Control(pDX, IDC_OPTFASTEST_TIME, m_cOptFastestTime);
	DDX_Control(pDX, IDC_OPTFASTEST_RESPONSESSEC, m_cOptFastestResponsesSec);
	DDX_Control(pDX, IDC_OPTSMALLEST_SIZE, m_cOptSmallestSize);
	DDX_Control(pDX, IDC_OPTSMALLEST_TIME, m_cOptSmallestTime);
	DDX_Control(pDX, IDC_OPTSMALLEST_RESPONSESSEC, m_cOptSmallestResponsesSec);
	DDX_Control(pDX, IDC_PROGRESS_OPTIMIZE, m_cOptProgress);
	DDX_Control(pDX, IDC_DEFAULT_RATIO, m_cDefaultRatio);
	DDX_Control(pDX, IDC_FASTEST_RATIO, m_cFastestRatio);
	DDX_Control(pDX, IDC_SMALLEST_RATIO, m_cSmallestRatio);
	DDX_Control(pDX, IDC_OPTDEFAULT_RATIO, m_cOptDefaultRatio);
	DDX_Control(pDX, IDC_OPTFASTEST_RATIO, m_cOptFastestRatio);
	DDX_Control(pDX, IDC_OPTSMALLEST_RATIO, m_cOptSmallestRatio);
	DDX_Control(pDX, IDC_SIZE_LABEL, m_cSizeLabel);
	DDX_Control(pDX, IDC_RATIO_LABEL, m_cRatioLabel);
	DDX_Control(pDX, IDC_TIME_LABEL, m_cTimeLabel);
	DDX_Control(pDX, IDC_RESPONSESSEC_LABEL, m_cResponsesSecLabel);
	DDX_Control(pDX, IDC_DEFAULT_LABEL, m_cDefaultLabel);
	DDX_Control(pDX, IDC_FASTEST_LABEL, m_cFastestLabel);
	DDX_Control(pDX, IDC_SMALLEST_LABEL, m_cSmallestLabel);
	DDX_Control(pDX, IDC_STATIC_WHITEBACKGROUND, m_cWhiteBackground);
	DDX_Control(pDX, IDC_OPTSIZE_LABEL, m_cOptSizeLabel);
	DDX_Control(pDX, IDC_OPTRATIO_LABEL, m_cOptRatioLabel);
	DDX_Control(pDX, IDC_OPTTIME_LABEL, m_cOptTimeLabel);
	DDX_Control(pDX, IDC_OPTRESPONSESSEC_LABEL, m_cOptResponsesSecLabel);
	DDX_Control(pDX, IDC_OPTDEFAULT_LABEL, m_cOptDefaultLabel);
	DDX_Control(pDX, IDC_OPTFASTEST_LABEL, m_cOptFastestLabel);
	DDX_Control(pDX, IDC_OPTSMALLEST_LABEL, m_cOptSmallestLabel);
	DDX_Control(pDX, IDC_STATIC_WHITEBACKGROUND2, m_cOptWhiteBackground);
	DDX_Control(pDX, IDC_BUTTON_OPTCALCULATE, m_cOptimizeCalculate);
	DDX_Control(pDX, IDC_STATIC_HEADERTEXTBITMAP, m_cHeaderTextBitmap);
	DDX_Control(pDX, IDC_RADIO_USESTANDARD, m_cUseStandardCompression);
	DDX_Control(pDX, IDC_RADIO_USEOPTIMIZED, m_cUseOptimizedCompression);
	DDX_Control(pDX, IDC_COMBO_OPTIMIZEDMODE, m_cOptimizedCompressionMode);
	DDX_Control(pDX, IDC_STATIC_OPTIMIZEDTIMINGSHELP, m_cOptimizedTimingsHelp);
	DDX_Control(pDX, IDC_STATIC_OPTIMIZEDTIMINGSHELPTEXT, m_cOptimizedTimingsHelpText);
}


BEGIN_MESSAGE_MAP(CCompressionOptimizerDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILES, &CCompressionOptimizerDlg::OnLvnItemChangedListFiles)
	ON_BN_CLICKED(IDC_BUTTON_OPTCALCULATE, &CCompressionOptimizerDlg::OnBnClickedButtonOptCalculate)
	ON_BN_CLICKED(IDC_BUTTON_STANDARDCALCULATE, &CCompressionOptimizerDlg::OnBnClickedButtonStandardCalculate)
	ON_BN_CLICKED(IDC_RADIO_USESTANDARD, &CCompressionOptimizerDlg::OnBnClickedRadioUseStandardCompression)
	ON_BN_CLICKED(IDC_RADIO_USEOPTIMIZED, &CCompressionOptimizerDlg::OnBnClickedRadioUseOptimizedCompression)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTIMIZEDMODE, &CCompressionOptimizerDlg::OnCbnSelChangeComboOptimizedMode)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CCompressionOptimizerDlg::OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CCompressionOptimizerDlg message handlers

BOOL CCompressionOptimizerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString sCustomCompMode;
	sCustomCompMode.LoadString(IDS_CUSTOMCOMPMODE_DEFAULT);
	int nItem = m_cOptimizedCompressionMode.InsertString(0, sCustomCompMode);
	m_cOptimizedCompressionMode.SetItemData(nItem, CompressionModeDefault);

	sCustomCompMode.LoadString(IDS_CUSTOMCOMPMODE_FASTEST);
	nItem = m_cOptimizedCompressionMode.InsertString(1, sCustomCompMode);
	m_cOptimizedCompressionMode.SetItemData(nItem, CompressionModeSpeed);

	sCustomCompMode.LoadString(IDS_CUSTOMCOMPMODE_SMALLEST);
	nItem = m_cOptimizedCompressionMode.InsertString(2, sCustomCompMode);
	m_cOptimizedCompressionMode.SetItemData(nItem, CompressionModeSize);

	if (CPNGHelper::LoadPNG(_T("WhiteBackground.PNG"), m_imgWhiteBackground) == true)
	{
		CPNGHelper::ApplyAlpha(m_imgWhiteBackground);
		m_cWhiteBackground.SetWindowPos(NULL, 0, 0, m_imgWhiteBackground.GetWidth(), m_imgWhiteBackground.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		m_cWhiteBackground.SetImage(&m_imgWhiteBackground);
	}	

	if (CPNGHelper::LoadPNG(_T("WhiteBackground.PNG"), m_imgOptWhiteBackground) == true)
	{
		CPNGHelper::ApplyAlpha(m_imgOptWhiteBackground);
		m_cOptWhiteBackground.SetWindowPos(NULL, 0, 0, m_imgOptWhiteBackground.GetWidth(), m_imgOptWhiteBackground.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		m_cOptWhiteBackground.SetImage(&m_imgOptWhiteBackground);
	}	

	if (CPNGHelper::LoadPNG(_T("Globe48.PNG"), m_imgHeaderText) == true)
	{
		CPNGHelper::ApplyAlpha(m_imgHeaderText);
		m_cHeaderTextBitmap.SetWindowPos(NULL, 0, 0, m_imgHeaderText.GetWidth(), m_imgHeaderText.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		m_cHeaderTextBitmap.SetImage(&m_imgHeaderText);
	}		

	if (CPNGHelper::LoadPNG(_T("HELP16.PNG"), m_imgOptimizedTimingsHelp) == true)
	{
		CPNGHelper::ApplyAlpha(m_imgOptimizedTimingsHelp);
		m_cOptimizedTimingsHelp.SetWindowPos(NULL, 0, 0, m_imgOptimizedTimingsHelp.GetWidth(), m_imgOptimizedTimingsHelp.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		m_cOptimizedTimingsHelp.SetImage(&m_imgOptimizedTimingsHelp);
	}	

	{
		CFont* pftLabel = m_cSizeLabel.GetFont();
		if (pftLabel != NULL)
		{ 
			LOGFONT lftFont;
			if (pftLabel->GetLogFont(&lftFont) != 0)
			{
				lftFont.lfWeight = FW_SEMIBOLD;				
				m_ftBold.CreateFontIndirect(&lftFont);

				lftFont.lfUnderline = TRUE;
				m_ftBoldUnderlined.CreateFontIndirect(&lftFont);

				m_cSizeLabel.SetFont(&m_ftBoldUnderlined);
				m_cRatioLabel.SetFont(&m_ftBoldUnderlined);
				m_cTimeLabel.SetFont(&m_ftBoldUnderlined);
				m_cResponsesSecLabel.SetFont(&m_ftBoldUnderlined);
				m_cOptSizeLabel.SetFont(&m_ftBoldUnderlined);
				m_cOptRatioLabel.SetFont(&m_ftBoldUnderlined);
				m_cOptTimeLabel.SetFont(&m_ftBoldUnderlined);
				m_cOptResponsesSecLabel.SetFont(&m_ftBoldUnderlined);

				m_cDefaultLabel.SetFont(&m_ftBold);
				m_cFastestLabel.SetFont(&m_ftBold);
				m_cSmallestLabel.SetFont(&m_ftBold);
				m_cOptDefaultLabel.SetFont(&m_ftBold);
				m_cOptFastestLabel.SetFont(&m_ftBold);
				m_cOptSmallestLabel.SetFont(&m_ftBold);
			}
		}
	}

	m_cFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_cFiles.SetImageList(g_ShellImageList, LVSIL_SMALL);

	m_ImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	HICON hIconBlank = ::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BLANK16));
	ATLASSERT(hIconBlank != NULL);
	HICON hIconPerformance = ::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PERFORMANCE));
	ATLASSERT(hIconPerformance != NULL);
	m_nBlankIconOffset = m_ImageList.Add(hIconBlank);	
	m_nPerformanceIconOffset = m_ImageList.Add(hIconPerformance);
	m_cFiles.SetImageList(&m_ImageList, LVSIL_SMALL);

	CRect rcFiles;
	m_cFiles.GetClientRect(rcFiles);

	m_cFiles.InsertColumn(0, _T(""), LVCFMT_LEFT, rcFiles.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - 4);

	for (int i = 0; i < (int) m_Files.size(); i++)
	{
		int nItemIcon = m_nBlankIconOffset;

		FileItemData data;

		if (m_pCustomCompressionMode != NULL)
		{			
			std::auto_ptr<CustomCompressionFileData> pCustomCompressionData(new CustomCompressionFileData);
			if (pCustomCompressionData.get() != NULL)
			{
				CPath path(m_Files[i]->m_sPhysicalPath);
				path.Append(m_Files[i]->m_sFileName);

				bool status = pCustomCompressionData->ReadSettings(m_pCustomCompressionMode, path);
				if (status == true && pCustomCompressionData->currentSettings.bEnabled == TRUE)
				{
					nItemIcon = m_nPerformanceIconOffset;
					data.pCustomCompressionData = pCustomCompressionData;
				}
			}
		}

		int nItem = m_cFiles.InsertItem(i, m_Files[i]->m_sFileName, nItemIcon);
		m_cFiles.SetItemData(nItem, (DWORD_PTR) m_Files[i]);

		m_FileItemData[m_Files[i]->m_sFileName] = data;
	}

	m_cFiles.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	return TRUE;
}

HBRUSH CCompressionOptimizerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor != CTLCOLOR_STATIC /*&& nCtlColor != CTLCOLOR_BTN*/)
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	CRect rcWnd;
	if (pWnd != NULL && pWnd->m_hWnd != this->m_hWnd)
	{
		pWnd->GetWindowRect(&rcWnd);
	}

	CRect rcWhiteBackground;
	m_cWhiteBackground.GetWindowRect(&rcWhiteBackground);

	CRect rcOptWhiteBackground;
	m_cOptWhiteBackground.GetWindowRect(&rcOptWhiteBackground);
	
	if (pDC != NULL && rcWhiteBackground.PtInRect(rcWnd.TopLeft()) == TRUE)
	{	
		// handle the ALT key causing the control to partially update
		if ((GetKeyState(VK_MENU) & 0x8000) != 0)
		{
			pWnd->Invalidate();
			return (HBRUSH) ::GetStockObject(NULL_BRUSH);
		}

		CRect rcDest;
		pWnd->GetClientRect(rcDest);

		m_cWhiteBackground.ScreenToClient(rcWnd);
		
		m_imgWhiteBackground.Draw(pDC->GetSafeHdc(), rcDest, rcWnd);

		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH) ::GetStockObject(NULL_BRUSH);
	}
	else if (pDC != NULL && rcOptWhiteBackground.PtInRect(rcWnd.TopLeft()) == TRUE)
	{	
		// handle the ALT key causing the control to partially update
		if ((GetKeyState(VK_MENU) & 0x8000) != 0)
		{
			pWnd->Invalidate();
			return (HBRUSH) ::GetStockObject(NULL_BRUSH);
		}

		CRect rcClip;
		pDC->GetClipBox(&rcClip);

		CRect rcDest;
		pWnd->GetClientRect(rcDest);

		m_cOptWhiteBackground.ScreenToClient(rcWnd);
		
		m_imgOptWhiteBackground.Draw(pDC->GetSafeHdc(), rcDest, rcWnd);

		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH) ::GetStockObject(NULL_BRUSH);
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

void CCompressionOptimizerDlg::OnLvnItemChangedListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
		if (pData != NULL)
		{
			PopulateDefaultTimingControls(pData, FALSE);
			PopulateOptimizedTimingControls(pData, FALSE);
			PopulateCustomCompressionModeControls(pData);
		}
	}

	*pResult = 0;
}

void CCompressionOptimizerDlg::PopulateDefaultTimingControls(WebDirectoryContentsViewRowData* pData, BOOL bRefresh)
{
	ClearDefaultTimingControls();

	if (pData == NULL || m_FileItemData.Lookup(pData->m_sFileName) == NULL)
	{
		return;
	}

	CWaitCursor cursor;	

	FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];
	if (bRefresh == TRUE || fileItemData.pStandardResults.get() == NULL)
	{
		if (fileItemData.pStandardResults.get() == NULL)
		{
			fileItemData.pStandardResults = std::auto_ptr<CompressionResultSet>(new CompressionResultSet());
		}

		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);

		DWORD dwDefaultCompressionSize = 0;
		__int64 nDefaultCompressionTime = 0;
		GetFileCompressionStats(FullPhysicalPath, 6, -15, 9, dwDefaultCompressionSize, nDefaultCompressionTime);
		fileItemData.pStandardResults->pDefaultResults = std::auto_ptr<OptimizationResults>(new OptimizationResults(6, 15, 9, dwDefaultCompressionSize, nDefaultCompressionTime));

		DWORD dwFastestCompressionSize = 0;
		__int64 nFastestCompressionTime = 0;
		GetFileCompressionStats(FullPhysicalPath, 1, -15, 9, dwFastestCompressionSize, nFastestCompressionTime);
		fileItemData.pStandardResults->pFastestResults = std::auto_ptr<OptimizationResults>(new OptimizationResults(1, 15, 9, dwFastestCompressionSize, nFastestCompressionTime));

		DWORD dwSmallestCompressionSize = 0;
		__int64 nSmallestCompressionTime = 0;
		GetFileCompressionStats(FullPhysicalPath, 9, -15, 9, dwSmallestCompressionSize, nSmallestCompressionTime);
		fileItemData.pStandardResults->pSmallestResults = std::auto_ptr<OptimizationResults>(new OptimizationResults(9, 15, 9, dwSmallestCompressionSize, nSmallestCompressionTime));
	}

	if (fileItemData.pStandardResults->pDefaultResults.get() != NULL)
	{
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pStandardResults->pDefaultResults, m_cDefaultSize, m_cDefaultRatio, m_cDefaultTime, m_cDefaultResponsesSec);
	}

	if (fileItemData.pStandardResults->pFastestResults.get() != NULL)
	{
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pStandardResults->pFastestResults, m_cFastestSize, m_cFastestRatio, m_cFastestTime, m_cFastestResponsesSec);
	}

	if (fileItemData.pStandardResults->pSmallestResults.get() != NULL)
	{
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pStandardResults->pSmallestResults, m_cSmallestSize, m_cSmallestRatio, m_cSmallestTime, m_cSmallestResponsesSec);	
	}
}

void CCompressionOptimizerDlg::PopulateOptimizedTimingControls(WebDirectoryContentsViewRowData* pData, BOOL bRefresh)
{
	ClearOptimizedTimingControls();

	if (pData == NULL || m_FileItemData.Lookup(pData->m_sFileName) == NULL)
	{
		return;
	}	

	CWaitCursor cursor;

	FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

	// we must always have standard results	
	if (fileItemData.pStandardResults.get() == NULL || 
		fileItemData.pStandardResults->pDefaultResults.get() == NULL ||
		fileItemData.pStandardResults->pSmallestResults.get() == NULL ||
		fileItemData.pStandardResults->pFastestResults.get() == NULL)
	{
		ASSERT(0);
		return;
	}
	
	if (bRefresh == TRUE)
	{
		if (fileItemData.pOptimizedResults.get() == NULL)
		{
			fileItemData.pOptimizedResults = std::auto_ptr<CompressionResultSet>(new CompressionResultSet());
		}

		std::vector<OptimizationResults> results(m_nMaxOptimizedConfigurations);

		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);

		int nIndex = 0;
		m_cOptProgress.ShowWindow(SW_SHOW);
		m_cOptProgress.SetStep(1);
		m_cOptProgress.SetRange(0, m_nMaxOptimizedConfigurations);

		for (int nLevel = m_nMinLevel; nLevel < m_nMaxLevel; nLevel++)
		{
			for (int nWindowBits = m_nMinWindowBits; nWindowBits < m_nMaxWindowBits; nWindowBits++)
			{
				for (int nMemoryLevel = m_nMinMemoryLevel; nMemoryLevel < m_nMaxMemoryLevel; nMemoryLevel++)
				{					
					DWORD dwCompressionSize = 0;
					__int64 nCompressionTime = 0;
					GetFileCompressionStats(FullPhysicalPath, nLevel, -nWindowBits, nMemoryLevel, dwCompressionSize, nCompressionTime);

					OptimizationResults result(nLevel, nWindowBits, nMemoryLevel, dwCompressionSize, nCompressionTime);
					results[nIndex] = result;

					nIndex++;
					m_cOptProgress.SetPos(nIndex);
				}
			}
		}

		m_cOptProgress.ShowWindow(SW_HIDE);

		// calculate fastest
		std::sort(results.begin(), results.end(), OptimizationResults::CompareTime);

		std::auto_ptr<OptimizationResults> pFastest = std::auto_ptr<OptimizationResults>(new OptimizationResults(*fileItemData.pStandardResults->pFastestResults));
		for (int i = 0; i < (int) results.size(); i++)
		{
			if (results[i].nTime > fileItemData.pStandardResults->pFastestResults->nTime)
			{
				break;
			}

			if (results[i].dwSize < fileItemData.pStandardResults->pFastestResults->dwSize)
			{
				*pFastest = results[i];
				break;
			}
		}

		fileItemData.pOptimizedResults->pFastestResults = pFastest;

		// calculate smallest
		std::sort(results.begin(), results.end(), OptimizationResults::CompareSize);		

		std::auto_ptr<OptimizationResults> pSmallest = std::auto_ptr<OptimizationResults>(new OptimizationResults(*fileItemData.pStandardResults->pSmallestResults));
		for (int i = 0; i < (int) results.size(); i++)
		{
			if (results[i].dwSize > fileItemData.pStandardResults->pSmallestResults->dwSize)
			{
				break;
			}

			if (results[i].nTime < fileItemData.pStandardResults->pSmallestResults->nTime)
			{
				*pSmallest = results[i];
				break;
			}
		}	

		fileItemData.pOptimizedResults->pSmallestResults = pSmallest;

		// calculate default
		std::auto_ptr<OptimizationResults> pBestDefault = std::auto_ptr<OptimizationResults>(new OptimizationResults(*fileItemData.pStandardResults->pDefaultResults));
		for (int i = 0; i < (int) results.size(); i++)
		{
			if (results[i].dwSize > fileItemData.pStandardResults->pDefaultResults->dwSize)
			{
				break;
			}

			if (results[i].nTime < fileItemData.pStandardResults->pDefaultResults->nTime)
			{
				*pBestDefault = results[i];
			}
		}	

		fileItemData.pOptimizedResults->pDefaultResults = pBestDefault;				
	}

	if (fileItemData.pCustomCompressionData.get() != NULL && 
		fileItemData.pCustomCompressionData->currentSettings.bEnabled == TRUE && 
		fileItemData.pOptimizedResults.get() == NULL)
	{
		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);		

		DWORD dwCompressionSize = 0;
		__int64 nCompressionTime = 0;
		if (GetFileCompressionStats(FullPhysicalPath, 
			fileItemData.pCustomCompressionData->currentSettings.nLevel, 
			-fileItemData.pCustomCompressionData->currentSettings.nWindowBits, 
			fileItemData.pCustomCompressionData->currentSettings.nMemoryLevel, 
			dwCompressionSize, nCompressionTime) == true)
		{
			std::auto_ptr<OptimizationResults> pOptimizedResults(new OptimizationResults(
				fileItemData.pCustomCompressionData->currentSettings.nLevel, 
				fileItemData.pCustomCompressionData->currentSettings.nWindowBits, 
				fileItemData.pCustomCompressionData->currentSettings.nMemoryLevel, 
				dwCompressionSize, nCompressionTime));

			fileItemData.pOptimizedResults = std::auto_ptr<CompressionResultSet>(new CompressionResultSet());

			switch (fileItemData.pCustomCompressionData->currentSettings.mode)
			{
				case CompressionModeSpeed:		fileItemData.pOptimizedResults->pFastestResults = pOptimizedResults; break;
				case CompressionModeSize:		fileItemData.pOptimizedResults->pSmallestResults = pOptimizedResults; break;
				case CompressionModeDefault:	fileItemData.pOptimizedResults->pDefaultResults = pOptimizedResults; break;
			}
		}
	}

	if (fileItemData.pOptimizedResults.get() != NULL &&
		fileItemData.pOptimizedResults->pFastestResults.get() != NULL &&
		fileItemData.pOptimizedResults->pSmallestResults.get() != NULL &&
		fileItemData.pOptimizedResults->pDefaultResults.get() != NULL)
	{
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pFastestResults, m_cOptFastestSize, m_cOptFastestRatio, m_cOptFastestTime, m_cOptFastestResponsesSec);
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pSmallestResults, m_cOptSmallestSize, m_cOptSmallestRatio, m_cOptSmallestTime, m_cOptSmallestResponsesSec);	
		UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pDefaultResults, m_cOptDefaultSize, m_cOptDefaultRatio, m_cOptDefaultTime, m_cOptDefaultResponsesSec);	

		CString sRefresh;
		sRefresh.LoadString(IDS_OPTIMIZEBUTTON_REFRESH);
		m_cOptimizeCalculate.SetWindowText(sRefresh);

		m_cOptimizedTimingsHelp.ShowWindow(SW_HIDE);
		m_cOptimizedTimingsHelpText.ShowWindow(SW_HIDE);
	}
	else if (fileItemData.pOptimizedResults.get() != NULL)
	{
		if (fileItemData.pOptimizedResults->pFastestResults.get() != NULL)
		{
			UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pFastestResults, m_cOptFastestSize, m_cOptFastestRatio, m_cOptFastestTime, m_cOptFastestResponsesSec);
		}

		if (fileItemData.pOptimizedResults->pSmallestResults.get() != NULL)
		{
			UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pSmallestResults, m_cOptSmallestSize, m_cOptSmallestRatio, m_cOptSmallestTime, m_cOptSmallestResponsesSec);	
		}

		if (fileItemData.pOptimizedResults->pDefaultResults.get() != NULL)
		{
			UpdateTimingControls(pData->m_nFileSize, *fileItemData.pOptimizedResults->pDefaultResults, m_cOptDefaultSize, m_cOptDefaultRatio, m_cOptDefaultTime, m_cOptDefaultResponsesSec);	
		}

		CString sCalculate;
		sCalculate.LoadString(IDS_OPTIMIZEBUTTON_CALCULATE);
		m_cOptimizeCalculate.SetWindowText(sCalculate);

		m_cOptimizedTimingsHelp.ShowWindow(SW_HIDE);
		m_cOptimizedTimingsHelpText.ShowWindow(SW_HIDE);
	}
	else
	{
		CString sCalculate;
		sCalculate.LoadString(IDS_OPTIMIZEBUTTON_CALCULATE);
		m_cOptimizeCalculate.SetWindowText(sCalculate);

		m_cOptimizedTimingsHelp.ShowWindow(SW_SHOW);
		m_cOptimizedTimingsHelpText.ShowWindow(SW_SHOW);
	}
}

void CCompressionOptimizerDlg::PopulateCustomCompressionModeControls(WebDirectoryContentsViewRowData* pData)
{	
	SelectCustomCompressionMode(m_cOptimizedCompressionMode, CompressionModeDefault);

	if (pData == NULL || m_pCustomCompressionMode == NULL || m_FileItemData.Lookup(pData->m_sFileName) == NULL)
	{
		m_cUseStandardCompression.SetCheck(FALSE);
		m_cUseStandardCompression.EnableWindow(FALSE);
		m_cUseOptimizedCompression.SetCheck(FALSE);
		m_cUseOptimizedCompression.EnableWindow(FALSE);
		m_cOptimizedCompressionMode.EnableWindow(FALSE);
		return;
	}

	FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

	bool bGotCustomData = fileItemData.pCustomCompressionData.get() != NULL;
	if (bGotCustomData == false)
	{
		CPath fullPath(pData->m_sPhysicalPath);
		fullPath.Append(pData->m_sFileName);

		if (fileItemData.pCustomCompressionData.get() == NULL)
		{
			std::auto_ptr<CustomCompressionFileData> pCustomCompressionData(new CustomCompressionFileData());
			bGotCustomData = pCustomCompressionData->ReadSettings(m_pCustomCompressionMode, fullPath);		
			if (bGotCustomData == true)
			{
				fileItemData.pCustomCompressionData = pCustomCompressionData;
			}
		}			
	}	

	bool bGotOptimizedResults = fileItemData.pOptimizedResults.get() != NULL;
	if (bGotCustomData == false && bGotOptimizedResults == false)
	{
		m_cUseStandardCompression.SetCheck(FALSE);
		m_cUseStandardCompression.EnableWindow(FALSE);
		m_cUseOptimizedCompression.SetCheck(FALSE);
		m_cUseOptimizedCompression.EnableWindow(FALSE);
		m_cOptimizedCompressionMode.EnableWindow(FALSE);
		return;
	}

	if (bGotCustomData == false && bGotOptimizedResults == true && fileItemData.pOptimizedResults->pDefaultResults.get() != NULL)
	{
		CustomCompressionSettings settings;
		settings.bEnabled = FALSE;
		settings.mode = CompressionModeDefault;			
		settings.nLevel = fileItemData.pOptimizedResults->pDefaultResults->nLevel;
		settings.nMemoryLevel = fileItemData.pOptimizedResults->pDefaultResults->nMemoryLevel;
		settings.nWindowBits = fileItemData.pOptimizedResults->pDefaultResults->nWindowBits;

		fileItemData.pCustomCompressionData = std::auto_ptr<CustomCompressionFileData>(new CustomCompressionFileData());
		fileItemData.pCustomCompressionData->currentSettings = settings;
		fileItemData.pCustomCompressionData->newSettings = settings;
	}	

	m_cUseStandardCompression.EnableWindow(TRUE);
	m_cUseOptimizedCompression.EnableWindow(TRUE);		
	
	SelectCustomCompressionMode(m_cOptimizedCompressionMode, fileItemData.pCustomCompressionData->newSettings.mode);

	if (fileItemData.pCustomCompressionData->newSettings.bEnabled == TRUE)
	{
		m_cUseStandardCompression.SetCheck(BST_UNCHECKED);
		m_cUseOptimizedCompression.SetCheck(BST_CHECKED);

		if (bGotOptimizedResults == true && 
			fileItemData.pOptimizedResults->pDefaultResults.get() != NULL &&
			fileItemData.pOptimizedResults->pFastestResults.get() != NULL &&
			fileItemData.pOptimizedResults->pSmallestResults.get() != NULL)
		{
			m_cOptimizedCompressionMode.EnableWindow(TRUE);		
		}
		else
		{
			m_cOptimizedCompressionMode.EnableWindow(FALSE);
		}
	}
	else
	{
		m_cUseStandardCompression.SetCheck(BST_CHECKED);
		m_cUseOptimizedCompression.SetCheck(BST_UNCHECKED);
		m_cOptimizedCompressionMode.EnableWindow(FALSE);
	}
}

void CCompressionOptimizerDlg::SelectCustomCompressionMode(CComboBox& cCombo, CompressionMode mode)
{
	for (int i = 0; i < m_cOptimizedCompressionMode.GetCount(); i++)
	{
		if (mode == m_cOptimizedCompressionMode.GetItemData(i))
		{
			m_cOptimizedCompressionMode.SetCurSel(i);
			return;
		}
	}

	m_cOptimizedCompressionMode.SetCurSel(-1);
}

void CCompressionOptimizerDlg::GetCustomCompressionMode(CComboBox& cCombo, CompressionMode& mode)
{
	int nSel = cCombo.GetCurSel();
	if (nSel == CB_ERR)
	{
		mode = CompressionModeNone;
		return;
	}

	mode = (CompressionMode) cCombo.GetItemData(nSel);	
}

void CCompressionOptimizerDlg::UpdateTimingControls(__int64 nOriginalSize, const OptimizationResults& result, CStatic& cSize, CStatic& cRatio, CStatic& cTime, CStatic& cResponsesSec)
{
	UpdateTimingControls(nOriginalSize, result.dwSize, result.nTime, cSize, cRatio, cTime, cResponsesSec);
}

void CCompressionOptimizerDlg::UpdateTimingControls(__int64 nOriginalSize, DWORD dwCompressedSize, __int64 nTime, CStatic& cSize, CStatic& cRatio, CStatic& cTime, CStatic& cResponsesSec)
{
	CString sCompressedSize;
	sCompressedSize.Format(_T("%u"), dwCompressedSize);
	CInternationalHelper::FormatIntegerNumber(sCompressedSize);
	cSize.SetWindowText(sCompressedSize);

	double fRatio = nOriginalSize - dwCompressedSize;
	fRatio *= 100;
	fRatio /= nOriginalSize;

	CString sRatio;
	sRatio.Format(_T("%.1f"), fRatio);
	CInternationalHelper::FormatFloatNumber(sRatio, 1);
	cRatio.SetWindowText(sRatio + _T("%"));

	CString sTime;
	sTime.Format(_T("%I64d"), nTime);
	CInternationalHelper::FormatIntegerNumber(sTime);
	cTime.SetWindowText(sTime);

	CString sResponsesSec;
	sResponsesSec.Format(_T("%.1f"), 1000000.0 / nTime);
	CInternationalHelper::FormatFloatNumber(sResponsesSec, 1);
	cResponsesSec.SetWindowText(sResponsesSec);
}

void CCompressionOptimizerDlg::ClearTimingControls()
{
	ClearDefaultTimingControls();
	ClearOptimizedTimingControls();
}

void CCompressionOptimizerDlg::ClearDefaultTimingControls()
{
	m_cSmallestSize.SetWindowText(_T(""));
	m_cSmallestRatio.SetWindowText(_T(""));
	m_cSmallestTime.SetWindowText(_T(""));
	m_cSmallestResponsesSec.SetWindowText(_T(""));

	m_cFastestSize.SetWindowText(_T(""));
	m_cFastestRatio.SetWindowText(_T(""));
	m_cFastestTime.SetWindowText(_T(""));
	m_cFastestResponsesSec.SetWindowText(_T(""));

	m_cDefaultSize.SetWindowText(_T(""));
	m_cDefaultRatio.SetWindowText(_T(""));
	m_cDefaultTime.SetWindowText(_T(""));
	m_cDefaultResponsesSec.SetWindowText(_T(""));	
}

void CCompressionOptimizerDlg::ClearOptimizedTimingControls()
{
	TCHAR* pszEmptyString = _T("-");

	m_cOptSmallestSize.SetWindowText(pszEmptyString);
	m_cOptSmallestRatio.SetWindowText(pszEmptyString);
	m_cOptSmallestTime.SetWindowText(pszEmptyString);
	m_cOptSmallestResponsesSec.SetWindowText(pszEmptyString);

	m_cOptFastestSize.SetWindowText(pszEmptyString);
	m_cOptFastestRatio.SetWindowText(pszEmptyString);
	m_cOptFastestTime.SetWindowText(pszEmptyString);
	m_cOptFastestResponsesSec.SetWindowText(pszEmptyString);

	m_cOptDefaultSize.SetWindowText(pszEmptyString);
	m_cOptDefaultRatio.SetWindowText(pszEmptyString);
	m_cOptDefaultTime.SetWindowText(pszEmptyString);
	m_cOptDefaultResponsesSec.SetWindowText(pszEmptyString);
}

bool CCompressionOptimizerDlg::GetFileCompressionStats(LPCTSTR pszFilePath, int nLevel, int nWindowBits, int nMemoryLevel, DWORD& dwCompressedSize, __int64& nTime)
{
	if (pszFilePath == NULL)
	{
		return false;
	}

	CFile file;
	if (file.Open(pszFilePath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan) == FALSE)
	{
		// TODO: handle this better
		return false;
	}

	z_stream deflateStream;
	memset(&deflateStream, 0, sizeof(deflateStream));
	::deflateInit2(&deflateStream, nLevel, Z_DEFLATED, nWindowBits, nMemoryLevel, Z_DEFAULT_STRATEGY);						

	DWORD dwFileSize = (DWORD) file.GetLength();
	DWORD dwOutputBufferSize = dwFileSize * 2;
	std::auto_ptr<BYTE> pbyOutputBuffer(new BYTE[dwOutputBufferSize]);

	HiResTimer timer;	
	__int64 nDuration = 0;

	while (1)
	{
		const int nDefaultSize = 256 * 1024;
		BYTE byData[nDefaultSize];
		UINT nBytesRead = file.Read(byData, _countof(byData));

		deflateStream.next_in = &byData[0];
		deflateStream.avail_in = nBytesRead;
		
		deflateStream.next_out = pbyOutputBuffer.get();
		deflateStream.avail_out = dwOutputBufferSize;

		if (nBytesRead == 0)
		{
			int nOldPriority = ::GetThreadPriority(::GetCurrentThread());
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
			__int64 nStart = timer.GetMicroSecTimerCount();
			int nCompressedStatus = ::deflate(&deflateStream, Z_FULL_FLUSH);
			__int64 nEnd = timer.GetMicroSecTimerCount();
			::SetThreadPriority(::GetCurrentThread(), nOldPriority);
			nDuration += nEnd - nStart;
			break;
		}				

		int nOldPriority = ::GetThreadPriority(::GetCurrentThread());
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		__int64 nStart = timer.GetMicroSecTimerCount();
		int nCompressedStatus = ::deflate(&deflateStream, Z_NO_FLUSH);
		__int64 nEnd = timer.GetMicroSecTimerCount();
		::SetThreadPriority(::GetCurrentThread(), nOldPriority);
		nDuration += nEnd - nStart;

		::Sleep(1);
	}					

	dwCompressedSize = deflateStream.total_out;
	nTime = nDuration;

	::deflateEnd(&deflateStream);

	return true;
}

void CCompressionOptimizerDlg::OnBnClickedButtonStandardCalculate()
{
	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
	if (pData != NULL)
	{
		PopulateDefaultTimingControls(pData, TRUE);
	}
}

void CCompressionOptimizerDlg::OnBnClickedButtonOptCalculate()
{	
	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
	if (pData != NULL)
	{
		PopulateOptimizedTimingControls(pData, TRUE);
		PopulateCustomCompressionModeControls(pData);
	}
}

void CCompressionOptimizerDlg::OnBnClickedRadioUseStandardCompression()
{	
	m_cUseOptimizedCompression.SetCheck(FALSE);	
	m_cOptimizedCompressionMode.EnableWindow(FALSE);

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
	if (pData != NULL && m_FileItemData.Lookup(pData->m_sFileName) != NULL)
	{		
		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		bool bLookup = fileItemData.pCustomCompressionData.get() != NULL;
		if (bLookup == true)
		{
			fileItemData.pCustomCompressionData->newSettings.bEnabled = FALSE;

			SetApplyButtonState();

			int nSelected = CListCtrlHelper::GetSelectedItem(m_cFiles);
			if (nSelected >= 0)
			{
				ShowFileItemDirtyStatus(nSelected);
			}
		}
	}
}

void CCompressionOptimizerDlg::OnBnClickedRadioUseOptimizedCompression()
{
	m_cUseStandardCompression.SetCheck(FALSE);
	m_cOptimizedCompressionMode.EnableWindow(FALSE);

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
	if (pData != NULL && m_FileItemData.Lookup(pData->m_sFileName) != NULL)
	{
		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		if (fileItemData.pOptimizedResults.get() != NULL &&
			fileItemData.pOptimizedResults->pDefaultResults.get() != NULL &&
			fileItemData.pOptimizedResults->pFastestResults.get() != NULL &&
			fileItemData.pOptimizedResults->pSmallestResults.get() != NULL)
		{
			m_cOptimizedCompressionMode.EnableWindow(TRUE);
		}

		bool bLookup = fileItemData.pCustomCompressionData.get() != NULL;
		if (bLookup == true)
		{
			fileItemData.pCustomCompressionData->newSettings.bEnabled = TRUE;

			SetApplyButtonState();

			int nSelected = CListCtrlHelper::GetSelectedItem(m_cFiles);
			if (nSelected >= 0)
			{
				ShowFileItemDirtyStatus(nSelected);
			}
		}
	}
}

void CCompressionOptimizerDlg::OnCbnSelChangeComboOptimizedMode()
{
	int nSel = m_cOptimizedCompressionMode.GetCurSel();
	if (nSel == CB_ERR)
	{
		return;
	}

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(m_cFiles);
	if (pData != NULL && m_FileItemData.Lookup(pData->m_sFileName) != NULL)
	{
		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		if (fileItemData.pOptimizedResults.get() == NULL)
		{
			ASSERT(0);
			return;
		}

		bool bLookup = fileItemData.pCustomCompressionData.get() != NULL;
		if (bLookup == false)
		{
			ASSERT(0);
			return;
		}		

		CompressionMode mode = CompressionModeNone;
		GetCustomCompressionMode(m_cOptimizedCompressionMode, mode);

		if (fileItemData.pCustomCompressionData->newSettings.mode != mode)
		{
			fileItemData.pCustomCompressionData->newSettings.mode = mode;

			const OptimizationResults* pResults = NULL;			
			switch (fileItemData.pCustomCompressionData->newSettings.mode)
			{
			case CompressionModeDefault:	pResults = fileItemData.pOptimizedResults->pDefaultResults.get(); break;
			case CompressionModeSpeed:		pResults = fileItemData.pOptimizedResults->pFastestResults.get(); break;
			case CompressionModeSize:		pResults = fileItemData.pOptimizedResults->pSmallestResults.get(); break;			
			}

			ASSERT(pResults != NULL);

			if (pResults != NULL)
			{
				fileItemData.pCustomCompressionData->newSettings.nLevel = pResults->nLevel;
				fileItemData.pCustomCompressionData->newSettings.nMemoryLevel = pResults->nMemoryLevel;
				fileItemData.pCustomCompressionData->newSettings.nWindowBits = pResults->nWindowBits;
			}

			SetApplyButtonState();

			int nSelected = CListCtrlHelper::GetSelectedItem(m_cFiles);
			if (nSelected >= 0)
			{
				ShowFileItemDirtyStatus(nSelected);
			}
		}
	}
}

bool CCompressionOptimizerDlg::HasDirtyCompressionData()
{
	POSITION pos = m_FileItemData.GetStartPosition();
	while (pos != NULL)
	{
		const FileItemData& data = m_FileItemData.GetNextValue(pos);
		if (data.pCustomCompressionData.get() != NULL && data.pCustomCompressionData->IsDirty() == true)
		{
			return true;
		}
	}

	return false;
}

bool CCompressionOptimizerDlg::HasDirtyCompressionData(LPCTSTR pszFilename)
{
	if (m_FileItemData.Lookup(CAtlString(pszFilename)) == NULL)
	{
		return false;
	}

	const FileItemData& data = m_FileItemData[CAtlString(pszFilename)];

	if (data.pCustomCompressionData.get() == NULL)
	{
		return false;
	}

	return data.pCustomCompressionData->IsDirty();
}

void CCompressionOptimizerDlg::SetApplyButtonState()
{
	if (HasDirtyCompressionData() == true)
	{
		m_cApply.EnableWindow(TRUE);
	}
	else
	{
		m_cApply.EnableWindow(FALSE);
	}
}

void CCompressionOptimizerDlg::OnBnClickedButtonApply()
{
	int nFiles = m_cFiles.GetItemCount();
	for (int i = 0; i < nFiles; i++)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(m_cFiles, i);
		if (pData == NULL || m_FileItemData.Lookup(pData->m_sFileName) == NULL)
		{
			continue;
		}

		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		if (fileItemData.pCustomCompressionData.get() == NULL)
		{
			continue;
		}

		if (fileItemData.pCustomCompressionData->IsDirty() == false)
		{
			continue;
		}

		CPath filePath(pData->m_sPhysicalPath);
		filePath.Append(pData->m_sFileName);

		fileItemData.pCustomCompressionData->WriteSettings(m_pCustomCompressionMode, filePath);

		ShowFileItemDirtyStatus(i);
		SetFileItemIcon(i);
	}

	SetApplyButtonState();	
}

void CCompressionOptimizerDlg::ShowFileItemDirtyStatus(int nItem)
{
	if (nItem < 0)
	{
		return;
	}

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(m_cFiles, nItem);
	if (pData == NULL)
	{
		ASSERT(0);
		return;
	}

	CString sItemText = m_cFiles.GetItemText(nItem, 0);
	int nLength = sItemText.GetLength();
	if (nLength > 0)
	{
		CString sDirtyChar = sItemText.Right(1);

		bool bDirty = HasDirtyCompressionData(pData->m_sFileName);
		if (bDirty == true && sDirtyChar != '*')
		{
			sItemText += '*';
			m_cFiles.SetItemText(nItem, 0, sItemText);
		}
		else if (bDirty == false && sDirtyChar == '*')
		{
			sItemText = sItemText.Left(nLength - 1);
			m_cFiles.SetItemText(nItem, 0, sItemText);
		}
	}
}

void CCompressionOptimizerDlg::RemoveDisabledCustomCompressionEntries()
{
	int nFiles = m_cFiles.GetItemCount();
	for (int i = 0; i < nFiles; i++)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(m_cFiles, i);
		if (pData == NULL || m_FileItemData.Lookup(pData->m_sFileName) == NULL)
		{
			continue;
		}

		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		if (fileItemData.pCustomCompressionData.get() == NULL)
		{
			continue;
		}

		CPath filePath(pData->m_sPhysicalPath);
		filePath.Append(pData->m_sFileName);

		if (fileItemData.pCustomCompressionData->currentSettings.bEnabled == FALSE)
		{
			fileItemData.pCustomCompressionData->RemoveSettings(m_pCustomCompressionMode, filePath);
		}
	}	
}

void CCompressionOptimizerDlg::OnDestroy()
{
	RemoveDisabledCustomCompressionEntries();
}

void CCompressionOptimizerDlg::SetFileItemIcon(int nItem)
{
	if (nItem < 0)
	{
		return;
	}

	int nIcon = m_nBlankIconOffset;
	
	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(m_cFiles, nItem);
	if (pData != NULL && m_FileItemData.Lookup(pData->m_sFileName) != NULL)
	{
		FileItemData& fileItemData = m_FileItemData[pData->m_sFileName];

		if (fileItemData.pCustomCompressionData.get() != NULL)
		{		
			if (fileItemData.pCustomCompressionData->currentSettings.bEnabled == TRUE)
			{
				nIcon = m_nPerformanceIconOffset;
			}
		}
	}

	LVITEM item;
	item.iItem = nItem;
	item.iSubItem = 0;
	item.mask = LVIF_IMAGE;
	item.iImage = nIcon;
	m_cFiles.SetItem(&item);
}