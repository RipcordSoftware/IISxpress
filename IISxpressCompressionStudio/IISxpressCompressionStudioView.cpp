// IISxpressCompressionStudioView.cpp : implementation of the CIISxpressCompressionStudioView class
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"

#include "IISxpressCompressionStudioDoc.h"
#include "IISxpressCompressionStudioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIISxpressCompressionStudioView

IMPLEMENT_DYNCREATE(CIISxpressCompressionStudioView, CView)

BEGIN_MESSAGE_MAP(CIISxpressCompressionStudioView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CIISxpressCompressionStudioView construction/destruction

CIISxpressCompressionStudioView::CIISxpressCompressionStudioView()
{
	// TODO: add construction code here

}

CIISxpressCompressionStudioView::~CIISxpressCompressionStudioView()
{
}

BOOL CIISxpressCompressionStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIISxpressCompressionStudioView drawing

void CIISxpressCompressionStudioView::OnDraw(CDC* /*pDC*/)
{
	CIISxpressCompressionStudioDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CIISxpressCompressionStudioView printing

BOOL CIISxpressCompressionStudioView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIISxpressCompressionStudioView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIISxpressCompressionStudioView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CIISxpressCompressionStudioView diagnostics

#ifdef _DEBUG
void CIISxpressCompressionStudioView::AssertValid() const
{
	CView::AssertValid();
}

void CIISxpressCompressionStudioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIISxpressCompressionStudioDoc* CIISxpressCompressionStudioView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIISxpressCompressionStudioDoc)));
	return (CIISxpressCompressionStudioDoc*)m_pDocument;
}
#endif //_DEBUG


// CIISxpressCompressionStudioView message handlers

void CIISxpressCompressionStudioView::OnInitialUpdate()
{
}