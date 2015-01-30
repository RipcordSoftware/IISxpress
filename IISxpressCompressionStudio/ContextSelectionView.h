#pragma once


// CContextSelectionView view

class CContextSelectionView : public CListView
{
	DECLARE_DYNCREATE(CContextSelectionView)

protected:
	CContextSelectionView();           // protected constructor used by dynamic creation
	virtual ~CContextSelectionView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLvnItemChanged(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnCustomDraw(NMHDR* pNotifyStruct, LRESULT* result);

	BOOL PreCreateWindow(CREATESTRUCT& cs);

	void OnInitialUpdate();

private:

	CImageList m_images;

	CImage	m_bmpIcons[5];
};


