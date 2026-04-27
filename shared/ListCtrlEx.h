#pragma once

// ListCtrlEx.h : header file
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CListCtrl
{
// Construction
public:
   CListCtrlEx():CListCtrl(){}

   //{{AFX_VIRTUAL(CListCtrlEx)
	protected:
	//}}AFX_VIRTUAL

public:
	void SetSelectedArray(CDWordArray *a);
	void GetSelectedArray(CDWordArray *a);
   using CListCtrl::SetItemText;
   using CListCtrl::InsertItem;
	DWORD GetItemDword(int item, int subitem);
   DWORD GetItemDwHex(int item, int subitem);
	float GetItemFloat(int item, int subitem);
	int GetItemInt(int item, int subitem);
	int InsertItem2(int idx, LPCTSTR str);
	int InsertItem2Dwhex(int idx, LPCTSTR str, DWORD data);
	int InsertItem2Text(int idx, LPCTSTR str, LPCTSTR data);
	int InsertItem2Float(int idx, LPCTSTR str, float data, TCHAR *fmt=_T("%.3f"));
	int InsertItem2Int(int idx, LPCTSTR str, int data);
   DWORD GetItemSpacing();
   BOOL SetCurSel(int inItem);
//    BOOL SetCurFocus(int inItem);
   void SelectAll();
   int GetColumnCount(void);
   BOOL LoadColumnSizes(LPCTSTR szSection);
   BOOL LoadColumnOrder(LPCTSTR szSection);
   BOOL LoadColumnSizes(UINT nSection);
   BOOL SaveColumnSizes(LPCTSTR szSection);
   BOOL SaveColumnOrder(LPCTSTR szSection);
   BOOL SaveColumnSizes(UINT nSection);
   int GetCurSel(void);
   BOOL SetItemInt(int item, int subitem, int value);
   BOOL SetItemQuotedInt(int item, int subitem, int value);
   BOOL SetItemFloat(int item, int subitem, float value, TCHAR *fmt=_T("%.3f"));
   BOOL SetItemDword(int item, int subitem, DWORD value, TCHAR *fmt=_T("0x%08X"));
   BOOL SetItemText(int item, int subitem, UINT idstr);
   int InsertColumn(int idx, UINT idstr, int size, int fmt=LVCFMT_LEFT);
   int InsertColumn(int idx, LPCTSTR str, int size, int fmt=LVCFMT_LEFT);
   int InsertItem(int idx, int img, UINT idstr);
   int InsertItem(int idx, int img, LPCTSTR str);

   void DeleteAllColumns(void);

   void SetItemCount(int nItems, DWORD dwFlags = 0)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, dwFlags); }

   void ReduceItems(int nItems);

protected:
   //{{AFX_MSG(CListCtrlEx)
	//}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

