#pragma once

// CBorderedSplitterWnd command target

class CBorderedSplitterWnd : public CSplitterWnd
{
public:
	CBorderedSplitterWnd();
	CBorderedSplitterWnd(int cxBorder, int cyBorder);
	virtual ~CBorderedSplitterWnd();
};


