// IISxpressCompressionStudioView.h : interface of the CIISxpressCompressionStudioView class
//


#pragma once


class CIISxpressCompressionStudioView : public CView
{
protected: // create from serialization only
	CIISxpressCompressionStudioView();
	DECLARE_DYNCREATE(CIISxpressCompressionStudioView)

// Attributes
public:
	CIISxpressCompressionStudioDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CIISxpressCompressionStudioView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void OnInitialUpdate();
	
};

#ifndef _DEBUG  // debug version in IISxpressCompressionStudioView.cpp
inline CIISxpressCompressionStudioDoc* CIISxpressCompressionStudioView::GetDocument() const
   { return reinterpret_cast<CIISxpressCompressionStudioDoc*>(m_pDocument); }
#endif

