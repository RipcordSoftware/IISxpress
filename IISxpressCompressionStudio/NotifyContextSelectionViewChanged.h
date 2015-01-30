#pragma once

// CNotifyContextSelectionViewChanged command target

class CNotifyContextSelectionViewChanged : public CObject
{
	DECLARE_DYNAMIC(CNotifyContextSelectionViewChanged)

public:
	CNotifyContextSelectionViewChanged(StackedViewTypes newView);

	StackedViewTypes GetSelectedView();

private:

	StackedViewTypes m_NewView;

};