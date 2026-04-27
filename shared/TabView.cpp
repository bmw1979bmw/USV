// TabView.cpp : implementation file
//

#include "stdafx.h"
#include "TabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _MSC_VER < 1300
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTabView
IMPLEMENT_DYNAMIC(CTabView, CWnd)

CTabView::CTabView()
{
   m_bUpdateFormViews = FALSE;
   m_bResizeParentToFit = FALSE;
}

CTabView::~CTabView()
{
}

BEGIN_MESSAGE_MAP(CTabView, CWnd)
   //{{AFX_MSG_MAP(CTabView)
   ON_WM_SYSCOMMAND()
   ON_WM_SIZE()
   ON_MESSAGE_VOID(WM_DISPLAYCHANGE, OnDisplayChange)
   ON_MESSAGE_VOID(WM_WININICHANGE, OnDisplayChange)
   ON_MESSAGE_VOID(WM_SETTINGCHANGE, OnDisplayChange)
   ON_WM_NCCREATE()
   //}}AFX_MSG_MAP
   ON_WM_STYLECHANGING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabView message handlers

BOOL CTabView::Create(CWnd * pParentWnd, DWORD dwStyle, UINT nID, DWORD dwExStyle, DWORD dwTabStyle)
{  // для дочерних dwStyle=0 !
              
   if (!AfxDeferRegisterClass(AFX_WNDMDIFRAME_REG))
      return FALSE;

   // create with the same wnd-class as MDI-Frame (no erase bkgnd)
   dwStyle |= WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
   if (!CreateEx(dwExStyle,
      AFX_WNDMDIFRAME,
      NULL, dwStyle, 0, 0, 0, 0,
      pParentWnd->m_hWnd, (HMENU)nID, NULL))
      return FALSE;       // create invisible

   // now this->m_hWnd > 0
   if ( !m_tabCtrl.Create(
                     dwTabStyle|
                     WS_CHILD|
                     WS_CLIPSIBLINGS|
                     WS_VISIBLE|
                     TCS_FOCUSNEVER
                     , CRect(0, 0, 0, 0), this, 55) )
      return FALSE;
   /////////////////////////////////////////////
	{   // if 01.12.2021 10:28
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
      info.lfMenuFont.lfCharSet = RUSSIAN_CHARSET;// 0|1
      info.lfMenuFont.lfHeight = info.lfMenuFont.lfHeight*7/8;
      m_tabFnt.DeleteObject();
      if ( m_tabFnt.CreateFontIndirect(&info.lfMenuFont) ) {
         m_tabCtrl.SetFont(&m_tabFnt);
      }
	}

   return TRUE;
}

BOOL CTabView::SetImageList(UINT nBitmapID, int cx, COLORREF crMask)
{ 
 	ASSERT(HIWORD(nBitmapID) == 0);
	HINSTANCE hInst = AfxFindResourceHandle((LPCTSTR)(DWORD_PTR)nBitmapID, RT_BITMAP);
	ASSERT(hInst != NULL);
	m_IL.Attach(ImageList_LoadImage(hInst, 
		(LPCTSTR)(DWORD_PTR)nBitmapID, cx, 0, crMask, IMAGE_BITMAP, LR_CREATEDIBSECTION
   ));

   m_tabCtrl.SendMessage(TCM_SETIMAGELIST, 0, (LPARAM)m_IL.m_hImageList);
   m_tabCtrl.SetMinTabWidth(cx);
   return TRUE;
}
	
/////////////////////////////////////////////////////////////////////////////

CWnd *CTabView::CreateView(int nStrId, CRuntimeClass * pViewClass, CCreateContext * pContext, DWORD dwStyle)
{
   CString s;
   if ( !s.LoadString(nStrId) ) s = _T("?"); 
   return CreateView(s, pViewClass, pContext, dwStyle);
}

CWnd *CTabView::CreateView(LPCTSTR szStr, CRuntimeClass * pViewClass, CCreateContext * pContext, DWORD dwStyle)
{
   int nId = m_tabCtrl.GetItemCount();

   TC_ITEM ti;
   if ( szStr ) { 
      ti.mask = TCIF_TEXT | TCIF_IMAGE ;
      ti.pszText = (LPTSTR)szStr;
   } else {
      ti.mask = TCIF_IMAGE ;
   }   
   if ( m_IL.m_hImageList ) ti.iImage = nId;
   else                     ti.iImage = -1;

   if ( m_tabCtrl.InsertItem(nId, &ti) == -1 ) {
      return NULL;
   }
      
   CWnd* pWnd;
   TRY
   {
      pWnd = (CWnd*)pViewClass->CreateObject();
      if (pWnd == NULL)
         AfxThrowMemoryException();
   }
   CATCH_ALL(e)
   {
      TRACE0("Out of memory creating a tab pane.\n");
      m_tabCtrl.DeleteItem(nId);
      return NULL;
   }
   END_CATCH_ALL

   ASSERT_KINDOF(CWnd, pWnd);
   ASSERT(pWnd->m_hWnd == NULL);       // not yet created

   
   if (!pWnd->Create(NULL, NULL, WS_CHILD | dwStyle, CRect(0, 0, 10, 10),
      this, AFX_IDW_PANE_FIRST + nId, pContext))
   {
      TRACE0("Warning: couldn't create client pane.\n");
         // pWnd will be cleaned up by PostNcDestroy
      m_tabCtrl.DeleteItem(nId);
      return NULL;
   }
      
   SetCurSel(0);
   return pWnd;  
}

void CTabView::SetCurView(CWnd *pWnd)
{
   ASSERT(pWnd);
   if ( !pWnd ) return;
   int nID = pWnd->GetDlgCtrlID();
   if ( nID >= AFX_IDW_PANE_FIRST ) {
      SetCurSel(nID-AFX_IDW_PANE_FIRST);
   }
}

CWnd *CTabView::CreateSubView(CRuntimeClass * pViewClass, CCreateContext * pContext, DWORD dwStyle)
{
   CWnd* pWnd;
   TRY
   {
      pWnd = (CWnd*)pViewClass->CreateObject();
      if (pWnd == NULL)
         AfxThrowMemoryException();
   }
   CATCH_ALL(e)
   {
      TRACE0("Out of memory creating a subview.\n");
      return NULL;
   }
   END_CATCH_ALL

   ASSERT_KINDOF(CWnd, pWnd);
   ASSERT(pWnd->m_hWnd == NULL);       // not yet created
   
   if (!pWnd->Create(NULL, NULL, (WS_CHILD | dwStyle)&~WS_VISIBLE, CRect(0, 0, 10, 10),
      this, 0, pContext))
   {
      TRACE0("Warning: couldn't create subview.\n");
         // pWnd will be cleaned up by PostNcDestroy
      return NULL;
   }
      
   return pWnd;  
}

UINT CTabView::AddPane(UINT nIdStr)
{
   CString s;
   if ( !s.LoadString(nIdStr) ) s = _T("?"); 
   return AddPane(s);
}

UINT CTabView::AddPane(LPCTSTR szStr)
{
   int nId = m_tabCtrl.GetItemCount();
   TC_ITEM ti;
   if ( szStr ) {
      ti.mask = TCIF_TEXT | TCIF_IMAGE ;
      ti.pszText = (LPTSTR)szStr;
   } else {
      ti.mask = TCIF_IMAGE ;
   }
   if ( m_IL.m_hImageList ) ti.iImage = nId;
   else                     ti.iImage = -1;

   if ( m_tabCtrl.InsertItem(nId, &ti) == -1 ) {
      return 0;
   }
//?   SetCurSel(0);
   return AFX_IDW_PANE_FIRST+nId;
}


BOOL CTabView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
   if (CWnd::OnCommand(wParam, lParam))
      return TRUE;
   // route commands to the parent frame window
   return GetParentFrame()->SendMessage(WM_COMMAND, wParam, lParam);
}

BOOL CTabView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   if (CWnd::OnNotify(wParam, lParam, pResult))
      return TRUE;

   if ( wParam == 55 ) {
      UINT code = ((NMHDR *)lParam)->code;
      if ( code == TCN_SELCHANGING ) {
         SetRedraw(FALSE);
         *pResult = !HideCurSel();
         SetRedraw(TRUE);
      } else if ( code == TCN_SELCHANGE ) {
         ShowCurSel();
      } 
      return TRUE;
   }

   // route commands to the parent frame window
   *pResult = GetParentFrame()->SendMessage(WM_NOTIFY, wParam, lParam);
   return TRUE;
}

void CTabView::OnSysCommand(UINT nID, LPARAM lParam) 
{
   if ((nID & 0xFFF0) == SC_SIZE)
   {
      CWnd* pParent = GetParent();
      if (pParent != NULL)
      {
         pParent->SendMessage(WM_SYSCOMMAND, (WPARAM)nID, lParam);
         return;
      }
   }
   CWnd::OnSysCommand(nID, lParam);
}

void CTabView::OnSize(UINT nType, int cx, int cy) 
{
   if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
      RecalcLayout();

   CWnd::OnSize(nType, cx, cy);
}

void CTabView::RecalcLayout()
{
   CRect rc;
   GetClientRect(rc);

   
   int nCount = m_tabCtrl.GetItemCount();
   // Size the tab control to fit the client area. 
   HDWP hdwp = BeginDeferWindowPos(nCount+1); 
          
   // сам таб
   DeferWindowPos(hdwp, m_tabCtrl.m_hWnd , NULL, 0, 0, 
      rc.right, rc.bottom, 
      SWP_NOMOVE | SWP_NOZORDER 
      ); 
 
   m_tabCtrl.AdjustRect(FALSE, &rc);
   //rc.left--;
   rc.top++;

   for ( int i = 0; i<nCount ;i++ ) {
      CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST + i); 
      if ( pWnd ) {
         // Position and size the static control to fit the 
         // tab control's display area, and make sure the 
         // static control is in front of the tab control. 
         DeferWindowPos(hdwp, 
            pWnd->m_hWnd, 
            HWND_TOP, rc.left, rc.top, 
            rc.right - rc.left, rc.bottom - rc.top, 0 
            ); 
      }
   }
   EndDeferWindowPos(hdwp); 
}

BOOL CTabView::SwitchView(int index, CView *pView)
{
   CView *pCur = (CView *)GetDlgItem(AFX_IDW_PANE_FIRST+index);
   if ( pCur != pView ) {
      pCur->SetDlgCtrlID(0);
      pCur->ShowWindow(SW_HIDE);
      ASSERT(pView);
      pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST+index);
      pView->ShowWindow(SW_SHOW);
      RecalcLayout();
      return TRUE;
   }
   return FALSE;
}

void CTabView::OnDisplayChange()
{
   if (!IsIconic() && IsWindowVisible())
      RecalcLayout();
}

BOOL CTabView::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (!CWnd::OnNcCreate(lpCreateStruct))
      return FALSE;
   
   // remove WS_EX_CLIENTEDGE style from parent window
   //  (the tabwnd itself will provide the 3d look)
   CWnd* pParent = GetParent();
   ASSERT_VALID(pParent);
   if ( pParent == GetParentFrame() ) {
      pParent->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);
   }
   
   return TRUE;
}

int CTabView::SetCurSel(int n)
{
   int t;
   HideCurSel();
   t = m_tabCtrl.SetCurSel(n);
   ShowCurSel();
   return t;
}

int CTabView::GetCurSel() const 
{
   return m_tabCtrl.GetCurSel();
}

BOOL CTabView::HideCurSel()
{
   CWnd *pWnd;

   int t = GetCurSel();
   if ( t != -1 ) {
      pWnd = GetDlgItem(AFX_IDW_PANE_FIRST + t); 
      if ( !pWnd ) return FALSE;
      if ( m_bUpdateFormViews ) {
         // if ( !UpdateDescendants() ) return false;
         if ( pWnd->IsKindOf(RUNTIME_CLASS(CFormView)) ) {
            if ( !pWnd->UpdateData(TRUE) ) return FALSE;
         }
      }
      pWnd->ShowWindow(SW_HIDE);
   }
   CFrameWnd *w = GetParentFrame();
   if ( w ) w->SetActiveView(NULL);
         
   return TRUE;
}

void CTabView::ShowCurSel()
{
   CWnd *pWnd;

   int n = GetCurSel();
   pWnd = GetDlgItem(AFX_IDW_PANE_FIRST + n);
   
   if ( pWnd ) {
      pWnd->ShowWindow(SW_SHOW);
      if ( pWnd->IsKindOf(RUNTIME_CLASS(CView)) ) {
         CFrameWnd *w = GetParentFrame();
         if ( w ) w->SetActiveView((CView*)pWnd);
         else     pWnd->SetFocus();
         if ( m_bResizeParentToFit ) {
            if ( pWnd->IsKindOf(RUNTIME_CLASS(CFormView)) ) {
               ((CFormView*)pWnd)->ResizeParentToFit(FALSE);
            }
         }
      } else if ( pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)) ) {

         CSplitterWnd *pSplit = (CSplitterWnd*)pWnd;

         CFrameWnd *w = GetParentFrame();
         if ( w ) {
_1:
            CWnd *wc = pSplit->GetPane(0, 0);
            if ( wc->IsKindOf(RUNTIME_CLASS(CView)) ) w->SetActiveView((CView*)wc);
            else if ( wc->IsKindOf(RUNTIME_CLASS(CSplitterWnd)) ) {
               pSplit = (CSplitterWnd *)wc;
               goto _1;
            } else {
               wc->SetFocus();
            }
         }

      } else if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabView)) ) {
         ((CTabView*)pWnd)->ShowCurSel();
      } else {
         pWnd->SetFocus();
      }
      m_tabCtrl.SetCurSel(n);
   }
}

CView * CTabView::operator [](int idx) const
{
   return (CView *)GetDlgItem(AFX_IDW_PANE_FIRST + idx); 
}

BOOL CTabView::ShowTab(int nId, BOOL bShow)
{
   TC_ITEM ti;
   ti.mask = TCIF_STATE;
   ti.dwState = bShow?0:4;   //undocumended TCIS_HIDDEN 
   ti.dwStateMask = 4;
   return m_tabCtrl.SetItem(nId, &ti);
}

void CTabView::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{  // если дочерний - сплиттер, он изменяет parent в WM_NCCREATE
   if ( nStyleType == GWL_EXSTYLE ) {
      if ( (lpStyleStruct->styleOld & WS_EX_CLIENTEDGE )
         &&
          !(lpStyleStruct->styleNew & WS_EX_CLIENTEDGE )
         ) {
            lpStyleStruct->styleNew |= WS_EX_CLIENTEDGE;
      }
   }
   Default();
}

BOOL CTabView::SetItemText(int nItem, LPTSTR szText)
{
   TCITEM tci;
   memset(&tci, 0, sizeof(tci));
   tci.mask = TCIF_TEXT;
   tci.pszText = szText;
   return m_tabCtrl.SetItem(nItem, &tci);
}

BOOL CTabView::SetTabIcon(int nTab, int iconIndex)
{
   TCITEM tci;
   memset(&tci, 0, sizeof(tci));
   tci.mask = TCIF_IMAGE;
   tci.iImage = iconIndex;
   return m_tabCtrl.SetItem(nTab, &tci);
}

void CTabView::SwitchToNext()
{
   int t = GetCurSel();
   int nCount = m_tabCtrl.GetItemCount();
   t = (t+1)%nCount;
   SetCurSel(t);
 // TODO hidden items!!!
//   m_tabCtrl.GetItem(;
   
}

