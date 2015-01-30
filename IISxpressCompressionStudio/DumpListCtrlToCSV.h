#pragma once

class CDumpListCtrlToCSV
{
private:
	CDumpListCtrlToCSV(void) {}

public:
	static void Dump(CListCtrl& cListCtrl, CWnd* pParent);
};
