#pragma once

template <typename T> class CAutoSortableHeaderListView : public T
{

public:

	CAutoSortableHeaderListView() {}

protected:

	void OnColumnClick(NMHDR* pNotifyStruct, LRESULT* result)
	{
		NMLISTVIEW* pNml = (NMLISTVIEW*) pNotifyStruct;	

		CListCtrl& cListCtrl = T::GetListCtrl();
		CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
		if (pcHeader == NULL)
		{
			return;
		}

		int nSelectedColumn = pNml->iSubItem;	

		HDITEM hdSelectedItem;
		hdSelectedItem.mask = HDI_FORMAT;
		pcHeader->GetItem(nSelectedColumn, &hdSelectedItem);	

		if ((hdSelectedItem.fmt & HDF_SORTUP) == HDF_SORTUP)
		{
			hdSelectedItem.fmt &= ~(HDF_SORTUP);
			hdSelectedItem.fmt |= HDF_SORTDOWN;
			pcHeader->SetItem(nSelectedColumn, &hdSelectedItem);

			OnSortColumn(nSelectedColumn, FALSE);
		}
		else if ((hdSelectedItem.fmt & HDF_SORTDOWN) == HDF_SORTDOWN)
		{
			hdSelectedItem.fmt &= ~(HDF_SORTDOWN);
			hdSelectedItem.fmt |= HDF_SORTUP;
			pcHeader->SetItem(nSelectedColumn, &hdSelectedItem);

			OnSortColumn(nSelectedColumn, TRUE);
		}	
		else
		{
			BOOL bSortAsc = TRUE;

			for (int i = 0; i < pcHeader->GetItemCount(); i++)
			{
				if (i == nSelectedColumn)
					continue;

				HDITEM hditem;
				hditem.mask = HDI_FORMAT;
				pcHeader->GetItem(i, &hditem);	

				if ((hditem.fmt & HDF_SORTUP) == HDF_SORTUP)
				{		
					hditem.fmt &= ~(HDF_SORTUP);
					pcHeader->SetItem(i, &hditem);

					bSortAsc = TRUE;
					break;
				}
				else if ((hditem.fmt & HDF_SORTDOWN) == HDF_SORTDOWN)
				{				
					hditem.fmt &= ~(HDF_SORTDOWN);				
					pcHeader->SetItem(i, &hditem);

					bSortAsc = FALSE;
					break;
				}	
			}	

			if (bSortAsc == TRUE)
			{			
				hdSelectedItem.fmt &= ~(HDF_SORTDOWN);
				hdSelectedItem.fmt |= HDF_SORTUP;
				pcHeader->SetItem(nSelectedColumn, &hdSelectedItem);

				OnSortColumn(nSelectedColumn, TRUE);
			}
			else
			{
				hdSelectedItem.fmt &= ~(HDF_SORTUP);
				hdSelectedItem.fmt |= HDF_SORTDOWN;
				pcHeader->SetItem(nSelectedColumn, &hdSelectedItem);

				OnSortColumn(nSelectedColumn, FALSE);
			}
		}	
	}

	virtual void OnSortColumn(int nColumn, BOOL bAscending) {}

	int GetSelectedColumn()
	{
		CListCtrl& cListCtrl = T::GetListCtrl();
		CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
		if (pcHeader == NULL)
		{
			return -1;
		}

		int nColumns = pcHeader->GetItemCount();
		for (int i = 0; i < nColumns; i++)
		{
			HDITEM hdItem;
			hdItem.mask = HDI_FORMAT;
			pcHeader->GetItem(i, &hdItem);	

			if (((hdItem.fmt & HDF_SORTUP) == HDF_SORTUP) || ((hdItem.fmt & HDF_SORTDOWN) == HDF_SORTDOWN))
				return i;
		}		

		return -1;
	}

	enum ColumnSortOrder
	{
		Unknown,
		Ascending,
		Descending
	};

	ColumnSortOrder GetColumnSort(int nColumn)
	{
		CListCtrl& cListCtrl = T::GetListCtrl();
		CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
		if (pcHeader == NULL)
		{
			return Unknown;
		}

		HDITEM hdItem;
		hdItem.mask = HDI_FORMAT;
		if (pcHeader->GetItem(nColumn, &hdItem) == TRUE)
		{			
			if ((hdItem.fmt & HDF_SORTUP) == HDF_SORTUP)
			{
				return Ascending;
			}
			else if ((hdItem.fmt & HDF_SORTDOWN) == HDF_SORTDOWN)
			{
				return Descending;
			}		
		}

		return Unknown;		
	}

};