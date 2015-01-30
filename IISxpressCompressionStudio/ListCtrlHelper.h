#pragma once

#include <vector>

class CListCtrlHelper
{
private:
	CListCtrlHelper(void) {}

public:
	static int GetSelectedItem(CListCtrl& cListCtrl);
	static void GetSelectedItems(CListCtrl& cListCtrl, std::vector<int>& items);

	template <typename T> static T* GetItemData(CListCtrl& cListCtrl, int nItem)
	{
		void* pData = (void*) cListCtrl.GetItemData(nItem);
		return reinterpret_cast<T*>(pData);		
	}

	template <typename T> static T* GetSelectedItemData(CListCtrl& cListCtrl)
	{
		int nItem = GetSelectedItem(cListCtrl);
		if (nItem < 0)
		{
			return NULL;
		}

		return GetItemData<T>(cListCtrl, nItem);		
	}

	template <typename T> static void GetSelectedItemsData(CListCtrl& cListCtrl, std::vector<T*>& itemsData)
	{
		itemsData.clear();

		std::vector<int> items;
		GetSelectedItems(cListCtrl, items);

		size_t nItems = items.size();

		itemsData.resize(nItems);

		for (size_t i = 0; i < nItems; i++)
		{			
			int nItem = items[i];
			itemsData[i] = GetItemData<T>(cListCtrl, nItem);
		}
	}
};
