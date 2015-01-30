#include "StdAfx.h"
#include "ListCtrlHelper.h"

int CListCtrlHelper::GetSelectedItem(CListCtrl& cListCtrl)
{
	if (cListCtrl.GetSelectedCount() != 1)
	{
		return -1;
	}

	int nItems = cListCtrl.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		if (cListCtrl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			return i;
		}
	}

	return -1;
}

void CListCtrlHelper::GetSelectedItems(CListCtrl& cListCtrl, std::vector<int>& items)
{
	items.clear();

	int nItems = cListCtrl.GetItemCount();

	items.reserve(nItems);

	for (int i = 0; i < nItems; i++)
	{
		if (cListCtrl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			items.push_back(i);
		}
	}	
}