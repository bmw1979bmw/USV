#pragma once

#include <afxpriv.h>
#if _MSC_VER > 1200
#include <..\src\mfc\afximpl.h>
#else
#include <..\mfc\src\afximpl.h>
#endif

// TabView.h : header file
/////////////////////////////////////////////////////////////////////////////
// CTabView window     

class CTabView : public CWnd
{
	DECLARE_DYNAMIC(CTabView)

   CTabCtrl m_tabCtrl;
   CFont m_tabFnt;
   CImageList m_IL;
public:
   // будет вызываться UpdateData для FormView, и, если FALSE, не переключаться
   BOOL m_bUpdateFormViews; 
   // будет вызываться ResizeParentToFit для FormView
   BOOL m_bResizeParentToFit; 
   //mb BOOL m_bIconClick;

// Construction
public:
   CTabView();

// Attributes
public:
   virtual void RecalcLayout();    // call after changing sizes   
   BOOL SwitchView(int index, CView *pView); // переключить другое view
                                             // 

// Operations
public:

// Overrides
   //{{AFX_VIRTUAL(CTabView)
   protected:
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetTabIcon(int nTab, int iconIndex);
// 	BOOL UpdateDescendants();
   BOOL SetImageList(UINT nBitmapID, int cx, COLORREF crMask=0x00C0C0C0);
	BOOL SetItemText(int nItem, LPTSTR szText);
   UINT AddPane(UINT nIdStr);
   UINT AddPane(LPCTSTR szStr);
   BOOL ShowTab(int nId, BOOL bShow);
   CView * operator [] (int idx) const;
   void ShowCurSel();
   // TRUE if OK (hidden)
   BOOL HideCurSel(void);
   int GetCurSel (void) const ;
   int SetCurSel(int n);
   void SetCurView(CWnd *pWnd);

   CWnd *CreateView(int nStrId, 
      CRuntimeClass* pViewClass, 
      CCreateContext* pContext, 
      DWORD dwStyle=WS_BORDER);
   CWnd *CreateView(LPCTSTR szStr, 
      CRuntimeClass* pViewClass, 
      CCreateContext* pContext, 
      DWORD dwStyle=WS_BORDER);
   CWnd *CreateSubView( //создаёт View с id=0 для SwitchView
      CRuntimeClass* pViewClass, 
      CCreateContext* pContext, 
      DWORD dwStyle=WS_BORDER);
   BOOL Create(CWnd* pParentWnd,
               DWORD dwStyle = WS_VISIBLE,
               UINT nID = AFX_IDW_PANE_FIRST,
               DWORD dwExStyle = WS_EX_CLIENTEDGE,
               DWORD dwTabStyle = 0
               );
   BOOL Create2(CWnd* pParentWnd, DWORD dwTabStyle)
   { 
      return Create(pParentWnd, WS_VISIBLE, AFX_IDW_PANE_FIRST, 
         WS_EX_CLIENTEDGE, dwTabStyle);
   }

   virtual ~CTabView();

   UINT GetLastId(void ){ return m_tabCtrl.GetItemCount() - 1;}

   void SwitchToNext();
   // Generated message map functions
protected:
   //{{AFX_MSG(CTabView)
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDisplayChange();
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

