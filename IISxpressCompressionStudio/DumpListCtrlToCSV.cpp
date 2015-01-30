#include "StdAfx.h"
#include "DumpListCtrlToCSV.h"

void CDumpListCtrlToCSV::Dump(CListCtrl& cListCtrl, CWnd* pParent)
{
	CString sCSVFileFilter;
	sCSVFileFilter.LoadString(IDS_CSVFILEFILTER);

	CFileDialog fileDlg(FALSE, _T("csv"), NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN,
		sCSVFileFilter, pParent);		

	if (fileDlg.DoModal() != IDOK)
	{
		return;
	}

	CWaitCursor cursor;

	CStringArray columns;

	int nColumn = 0;
	while (1)
	{
		TCHAR szColumnName[256];

		LVCOLUMN column;
		column.mask = LVCF_TEXT;
		column.pszText = szColumnName;
		column.cchTextMax = _countof(szColumnName);

		if (cListCtrl.GetColumn(nColumn, &column) == FALSE)
		{
			break;
		}

		columns.Add(column.pszText);

		nColumn++;
	}
		
	TRY
	{
		const int nFileMode = CFile::modeCreate | CFile::modeWrite | 
			CFile::shareDenyRead | CFile::shareDenyWrite;

		CFile file(fileDlg.GetPathName(), nFileMode);		

	#ifdef _UNICODE
		const BYTE byUnicodePrefix[2] = { 0xff, 0xfe };
		file.Write(byUnicodePrefix, sizeof(byUnicodePrefix));
	#endif

		const CString sEOL(_T("\r\n"));

		int nColumns = (int) columns.GetCount();
		for (int i = 0; i < nColumns; i++)
		{
			CString sData;		
			sData.Format(_T("\"%s\""), columns[i]);		

			if (i > 0)
			{
				sData.Insert(0, ',');
			}

			file.Write(sData, sData.GetLength() * sizeof(CString::XCHAR));
		}

		file.Write(sEOL, sEOL.GetLength() * sizeof(CString::XCHAR));

		int nRows = cListCtrl.GetItemCount();
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nColumns; j++)
			{
				CString sText = cListCtrl.GetItemText(i, j);

				if (sText.FindOneOf(_T(", ")) >= 0)
				{
					CString sTemp(sText);
					sText = _T("\"");
					sText += sTemp;
					sText += _T("\"");
				}

				if (j > 0)
				{
					sText.Insert(0, ',');
				}

				file.Write(sText, sText.GetLength() * sizeof(CString::XCHAR));
			}

			file.Write(sEOL, sEOL.GetLength() * sizeof(CString::XCHAR));
		}
	}
	CATCH (CFileException, e)
	{		
		CString sMsg;
		sMsg.Format(IDS_ERROR_CSVFILEWRITE, e->m_strFileName);
		AfxMessageBox(sMsg, MB_OK | MB_ICONEXCLAMATION);
	}
	END_CATCH
}