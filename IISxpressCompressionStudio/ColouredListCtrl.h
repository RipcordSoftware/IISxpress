#pragma once

// CColouredListCtrl

class CColouredListCtrlBase
{	
public:	

	CColouredListCtrlBase() : m_pfnCallback(NULL) {}

	BOOL DeleteItem(CListCtrl& ctrl, int nItem);

	typedef void OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult);
	BOOL RegisterCallback(OnCustomDrawCallback pfnCallback);	

	void OnCustomDraw(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult);

private:

	OnCustomDrawCallback* m_pfnCallback;
};

class CColouredListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CColouredListCtrl)

public:

	BOOL DeleteItem(int nItem) { return m_ColouredBase.DeleteItem(*this, nItem); }

	BOOL RegisterCallback(CColouredListCtrlBase::OnCustomDrawCallback pfnCallback) { return m_ColouredBase.RegisterCallback(pfnCallback); }

protected:
	DECLARE_MESSAGE_MAP()

	void OnCustomDraw(NMHDR* pNmHdr, LRESULT* pResult) { m_ColouredBase.OnCustomDraw(this, pNmHdr, pResult); }

private:

	CColouredListCtrlBase m_ColouredBase;
	
};

#ifdef __AFXCVIEW_H__
class CColouredListView : public CListView
{
	DECLARE_DYNAMIC(CColouredListView)

public:

	BOOL DeleteItem(int nItem) { return m_ColouredBase.DeleteItem(GetListCtrl(), nItem); }

	BOOL RegisterCallback(CColouredListCtrlBase::OnCustomDrawCallback pfnCallback) { return m_ColouredBase.RegisterCallback(pfnCallback); }

protected:
	DECLARE_MESSAGE_MAP()

	void OnCustomDraw(NMHDR* pNmHdr, LRESULT* pResult) { m_ColouredBase.OnCustomDraw(&(GetListCtrl()), pNmHdr, pResult); }

private:

	CColouredListCtrlBase m_ColouredBase;
	
};
#endif