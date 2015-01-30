#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include <atlcoll.h>

#include "WebDirectoryContentsViewRowData.h"

#include "OwnerDrawStatic.h"

struct OptimizationResults
{
	OptimizationResults() : nLevel(0), nWindowBits(0), nMemoryLevel(0), dwSize(0), nTime(0) {}

	OptimizationResults(int level, int windowBits, int memoryLevel, DWORD size, __int64 time) 
		: nLevel(level), nWindowBits(windowBits), nMemoryLevel(memoryLevel), dwSize(size), nTime(time) {}

	int nLevel;
	int nWindowBits;
	int nMemoryLevel;
	DWORD dwSize;
	__int64 nTime;

	static bool CompareSize(const OptimizationResults& l, const OptimizationResults& r)
	{
		return l.dwSize < r.dwSize;
	}

	static bool CompareTime(const OptimizationResults& l, const OptimizationResults& r)
	{
		return l.nTime < r.nTime;
	}
};

struct CompressionResultSet
{
	std::auto_ptr<OptimizationResults> pDefaultResults;
	std::auto_ptr<OptimizationResults> pFastestResults;
	std::auto_ptr<OptimizationResults> pSmallestResults;
};

struct CustomCompressionFileData
{
	CustomCompressionFileData()
	{
		CustomCompressionSettings settings;
		settings.bEnabled = FALSE;
		settings.mode = CompressionModeNone;
		settings.nLevel = -1;
		settings.nMemoryLevel = -1;
		settings.nWindowBits = -1;

		currentSettings = settings;
		newSettings = settings;
	}

	bool IsDirty() const
	{
		if (currentSettings.bEnabled != newSettings.bEnabled)
		{
			return true;
		}

		if (currentSettings.mode != newSettings.mode)
		{
			return true;
		}

		if (currentSettings.nLevel != newSettings.nLevel)
		{
			return true;
		}

		if (currentSettings.nWindowBits != newSettings.nWindowBits)
		{
			return true;
		}

		if (currentSettings.nMemoryLevel != newSettings.nMemoryLevel)
		{
			return true;
		}

		return false;
	}

	bool ReadSettings(ICustomCompressionMode* pCustomCompressionMode, const CString& sFilePath)
	{
		if (pCustomCompressionMode == NULL)
		{
			return false;
		}

		CustomCompressionSettings settings;
		HRESULT hr = pCustomCompressionMode->GetFileCompressionParams(CStringA(sFilePath), &settings);
		if (hr != S_OK)
		{
			return false;
		}

		currentSettings = settings;
		newSettings = settings;

		return true;
	}

	bool WriteSettings(ICustomCompressionMode* pCustomCompressionMode, const CString& sFilePath)
	{
		if (pCustomCompressionMode == NULL)
		{
			return false;
		}

		if (IsDirty() == true)
		{		
			HRESULT hr = pCustomCompressionMode->SetFileCompressionParams(CStringA(sFilePath), &newSettings);
			if (hr != S_OK)
			{
				return false;
			}

			currentSettings = newSettings;
		}

		return true;
	}

	bool RemoveSettings(ICustomCompressionMode* pCustomCompressionMode, const CString& sFilePath)
	{
		if (pCustomCompressionMode == NULL)
		{
			return false;
		}

		HRESULT hr = pCustomCompressionMode->RemoveFileCompressionParams(CStringA(sFilePath));
		if (hr != S_OK)
		{
			return false;
		}

		return true;
	}
	
	CustomCompressionSettings currentSettings;
	CustomCompressionSettings newSettings;
};

struct FileItemData
{
	std::auto_ptr<CompressionResultSet>			pStandardResults;
	std::auto_ptr<CompressionResultSet>			pOptimizedResults;
	std::auto_ptr<CustomCompressionFileData>	pCustomCompressionData;
};

// CCompressionOptimizerDlg dialog

class CCompressionOptimizerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompressionOptimizerDlg)

public:
	CCompressionOptimizerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCompressionOptimizerDlg();

	void SetFiles(const std::vector<WebDirectoryContentsViewRowData*>& files) { m_Files = files; }
	void SetCustomCompressionInterface(ICustomCompressionMode* pCustomCompressionMode) { m_pCustomCompressionMode = pCustomCompressionMode; }

// Dialog Data
	enum { IDD = IDD_DIALOG_COMPRESSIONOPTIMIZER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void OnDestroy();

	afx_msg void OnLvnItemChangedListFiles(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonOptCalculate();
	afx_msg void OnBnClickedButtonStandardCalculate();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedRadioUseStandardCompression();
	afx_msg void OnBnClickedRadioUseOptimizedCompression();
	afx_msg void OnCbnSelChangeComboOptimizedMode();

	afx_msg void OnBnClickedButtonApply();

	DECLARE_MESSAGE_MAP()

private:

	void ClearTimingControls();
	void ClearDefaultTimingControls();
	void ClearOptimizedTimingControls();

	void PopulateDefaultTimingControls(WebDirectoryContentsViewRowData* pData, BOOL bRefresh);
	void PopulateOptimizedTimingControls(WebDirectoryContentsViewRowData* pData, BOOL bRefresh);
	void PopulateCustomCompressionModeControls(WebDirectoryContentsViewRowData* pData);

	void UpdateTimingControls(__int64 nOriginalSize, const OptimizationResults& result, CStatic& cSize, CStatic& cRatio, CStatic& cTime, CStatic& cResponsesSec);
	void UpdateTimingControls(__int64 nOriginalSize, DWORD dwCompressedSize, __int64 nTime, CStatic& cSize, CStatic& cRatio, CStatic& cTime, CStatic& cResponsesSec);

	bool GetFileCompressionStats(LPCTSTR pszFilePath, int nLevel, int nWindowBits, int nMemoryLevel, DWORD& dwCompressedSize, __int64& nTime);

	void SelectCustomCompressionMode(CComboBox& cCombo, CompressionMode mode);
	void GetCustomCompressionMode(CComboBox& cCombo, CompressionMode& mode);

	bool HasDirtyCompressionData();
	bool HasDirtyCompressionData(LPCTSTR pszFilename);

	void SetApplyButtonState();

	void ShowFileItemDirtyStatus(int nItem);
	void SetFileItemIcon(int nItem);

	void RemoveDisabledCustomCompressionEntries();

	std::vector<WebDirectoryContentsViewRowData*>	m_Files;

	CComPtr<ICustomCompressionMode> m_pCustomCompressionMode;	

	CAtlMap<CAtlString, FileItemData> m_FileItemData;

	CColouredListCtrl m_cFiles;
	CButton m_cOptimizeCalculate;
	CButton m_cApply;
	CStatic m_cDefaultSize;
	CStatic m_cFastestSize;
	CStatic m_cSmallestSize;
	CStatic m_cDefaultTime;
	CStatic m_cFastestTime;
	CStatic m_cSmallestTime;
	CStatic m_cDefaultResponsesSec;
	CStatic m_cFastestResponsesSec;
	CStatic m_cSmallestResponsesSec;
	CStatic m_cOptDefaultSize;
	CStatic m_cOptDefaultTime;
	CStatic m_cOptDefaultResponsesSec;
	CStatic m_cOptFastestSize;
	CStatic m_cOptFastestTime;
	CStatic m_cOptFastestResponsesSec;
	CStatic m_cOptSmallestSize;
	CStatic m_cOptSmallestTime;
	CStatic m_cOptSmallestResponsesSec;
	CStatic m_cDefaultRatio;
	CStatic m_cFastestRatio;
	CStatic m_cSmallestRatio;
	CStatic m_cOptDefaultRatio;
	CStatic m_cOptFastestRatio;
	CStatic m_cOptSmallestRatio;
	CProgressCtrl m_cOptProgress;

	CFont m_ftBoldUnderlined;
	CFont m_ftBold;
	CStatic m_cSizeLabel;
	CStatic m_cRatioLabel;
	CStatic m_cTimeLabel;
	CStatic m_cResponsesSecLabel;
	CStatic m_cDefaultLabel;
	CStatic m_cFastestLabel;
	CStatic m_cSmallestLabel;
	CStatic m_cOptSizeLabel;
	CStatic m_cOptRatioLabel;
	CStatic m_cOptTimeLabel;
	CStatic m_cOptResponsesSecLabel;
	CStatic m_cOptDefaultLabel;
	CStatic m_cOptFastestLabel;
	CStatic m_cOptSmallestLabel;

	CImage m_imgWhiteBackground;
	CImage m_imgOptWhiteBackground;
	COwnerDrawStatic m_cWhiteBackground;
	COwnerDrawStatic m_cOptWhiteBackground;

	CImage m_imgHeaderText;
	COwnerDrawStatic m_cHeaderTextBitmap;

	CImage m_imgOptimizedTimingsHelp;
	COwnerDrawStatic m_cOptimizedTimingsHelp;
	CStatic m_cOptimizedTimingsHelpText;

	CButton m_cUseStandardCompression;
	CButton m_cUseOptimizedCompression;
	CComboBox m_cOptimizedCompressionMode;

	static const int m_nMinLevel = 1;
	static const int m_nMaxLevel = 10;
	static const int m_nMinWindowBits = 8;
	static const int m_nMaxWindowBits = 16;
	static const int m_nMinMemoryLevel = 1;
	static const int m_nMaxMemoryLevel = 10;
	static const int m_nMaxOptimizedConfigurations = (m_nMaxLevel - m_nMinLevel) * (m_nMaxWindowBits - m_nMinWindowBits) * (m_nMaxMemoryLevel - m_nMinMemoryLevel);				

	CImageList		m_ImageList;
	int				m_nBlankIconOffset;
	int				m_nPerformanceIconOffset;
};
