#pragma once

// ListViewEd.h : header file
/////////////////////////////////////////////////////////////////////////////
// CListViewEd view
// расширение ListView до редактирования полей - нотификация паренту

//#include "ListEditWnd.h"
#include "GRidEdit2.h"
#include "ListCtrlEx.h"

#define LVIF_EDIT 0x20

class CListViewEd : public CListView
{
protected:
	CListViewEd();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListViewEd)

   CListCtrlEx& GetListCtrl() const
        { return *(CListCtrlEx*)this; }


	CGridEdit2 m_Edit;
   int m_nSubitemClick;
   //CListEditWnd m_Edit;

   void EditCurSelCheck(int iSubItem, LPCTSTR szOverrideText=NULL);
   void EditCurSel(int iSubItem, LPCTSTR szOverrideText=NULL);
   int GetFocusedItem(){ return GetListCtrl().GetNextItem(-1,LVNI_FOCUSED);}

	//{{AFX_VIRTUAL(CListViewEd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	virtual ~CListViewEd();

protected:
	//{{AFX_MSG(CListViewEd)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LONG OnUpdateFromEdit( UINT wParam, LONG lParam);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

