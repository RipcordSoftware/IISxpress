// NotifyContextSelectionViewChanged.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "NotifyContextSelectionViewChanged.h"

IMPLEMENT_DYNAMIC(CNotifyContextSelectionViewChanged, CObject)

// CNotifyContextSelectionViewChanged

CNotifyContextSelectionViewChanged::CNotifyContextSelectionViewChanged(StackedViewTypes newView) : m_NewView(newView)
{
}


// CNotifyContextSelectionViewChanged member functions

StackedViewTypes CNotifyContextSelectionViewChanged::GetSelectedView() 
{ 
	return m_NewView; 
}