// ListViewEd.cpp : implementation file
//

#include "stdafx.h"
#include "ListViewEd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListViewEd

IMPLEMENT_DYNCREATE(CListViewEd, CListView)

CListViewEd::CListViewEd()
{
   m_nSubitemClick = 0;
}

CListViewEd::~CListViewEd()
{
}


BEGIN_MESSAGE_MAP(CListViewEd, CListView)
   //{{AFX_MSG_MAP(CListViewEd)
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
   ON_MESSAGE(WMY_UPDATEFROMEDIT, OnUpdateFromEdit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListViewEd message handlers

BOOL CListViewEd::PreCreateWindow(CREATESTRUCT& cs) 
{
   cs.style &= ~LVS_TYPEMASK;
   cs.style |= LVS_SINGLESEL | LVS_SHOWSELALWAYS 
               | LVS_REPORT | LVS_SHAREIMAGELISTS;
   
   return CListView::PreCreateWindow(cs);
}

void CListViewEd::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // NM_DBLCLK
   DWORD t = GetMessagePos();
   POINT p = { LOWORD(t), HIWORD(t) };
   ScreenToClient(&p);

   CListCtrlEx &a = GetListCtrl();

   int nsel = a.GetCurSel();
   if ( nsel == -1 ) return;

   CRect rca;
   a.GetItemRect(nsel, rca, LVIR_BOUNDS);
   

   int n = a.GetColumnCount();
   for ( int i=0; i<n ;i++ ) {
      int k = a.GetColumnWidth(i);
      rca.right = rca.left+k;

      if ( rca.PtInRect(p) ) {

         /////////////////////////////////////////////////
         NMLISTVIEW nm;
         nm.hdr.code = LVN_ITEMCHANGING;
         nm.hdr.hwndFrom = m_hWnd;
         nm.hdr.idFrom = GetDlgCtrlID();
         nm.iItem = nsel;
         nm.iSubItem = i;
         nm.uChanged = LVIF_EDIT; // MY
         
         if ( SendMessage(WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm) != 0 ) 
            return;

         /////////////////////////////////////////////////
         EditCurSel(i);
         break;
//          a.GetSubItemRect(nsel, i, LVIR_LABEL, rca);
//                                    
//          rca.top--;
//          rca.right++;
// 
// //          LVCOLUMN lcol;
// //          lcol.mask = LVCF_FMT;
// //          a.GetColumn(i, &lcol);
// // 
// //          DWORD style = WS_BORDER;
// //          if ( (lcol.fmt & LVCFMT_JUSTIFYMASK ) == LVCFMT_RIGHT ) {
// //           //  rca.top++;
// //             //style |= ES_RIGHT;
// //          }
//          //VERIFY(m_Edit.Create(WS_BORDER , rca, this, 1));
//          m_Edit.Create(nsel, i, rca, this, WS_BORDER);
// 
//          m_nSubitemClick = i;
// //          m_Edit.m_nItem = nsel;
// //          m_Edit.m_nSubItem = i;
// //          m_Edit.m_ldata = a.GetItemData(nsel);
//          m_Edit.SetFont(GetFont());
//          //m_Edit.SetMargins(4, 4); //can use
//          m_Edit.SetWindowText(a.GetItemText(nsel, i));
//          m_Edit.LimitText(59);
//          m_Edit.SetSel(0xFFFF0000); //all
//          m_Edit.SetFocus();
//          /////////////////////////////////////////////////
//          break;
      }
      rca.left=rca.right;
   }
      
   *pResult = 0;
}

LONG CListViewEd::OnUpdateFromEdit(UINT wParam, LONG lParam)
{
   CListCtrlEx &a = GetListCtrl();

   int nItem = GetFocusedItem();// m_Edit.m_nItem;
   ASSERT ( wParam == MAKEWPARAM(nItem, m_nSubitemClick));
   int nSubItem = m_nSubitemClick;

   a.SetItemText(nItem, nSubItem, (LPCTSTR)lParam);

   NMLISTVIEW nm;
   nm.hdr.code = LVN_ITEMCHANGED;
   nm.hdr.hwndFrom = m_hWnd;
   nm.hdr.idFrom = GetDlgCtrlID();
   nm.iItem = nItem;
   nm.iSubItem = nSubItem;
   nm.uChanged = LVIF_EDIT; // MY
   
   SendMessage(WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm );
   
   return TRUE;
}

void CListViewEd::EditCurSelCheck(int iSubItem, LPCTSTR szOverrideText/*=NULL*/)
{
   CListCtrlEx &a = GetListCtrl();
   
   int nsel = a.GetCurSel();
   if ( nsel < 0 ) return;

   NMLISTVIEW nm;
   nm.hdr.code = LVN_ITEMCHANGING;
   nm.hdr.hwndFrom = m_hWnd;
   nm.hdr.idFrom = GetDlgCtrlID();
   nm.iItem = nsel;
   nm.iSubItem = iSubItem;
   nm.uChanged = LVIF_EDIT; // MY
   
   if ( SendMessage(WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm) != 0 ) 
      return;
   
   EditCurSel(iSubItem, szOverrideText);
}

void CListViewEd::EditCurSel(int iSubItem, LPCTSTR szOverrideText/*=NULL*/)
{
   CListCtrlEx &a = GetListCtrl();

   int nsel = a.GetCurSel();
   if ( nsel < 0 ) return;

   /*
   закомментирован запрос проверки на разрешение редактирования
   можно редактировать любую ячейку
         NMLISTVIEW nm;
         nm.hdr.code = LVN_ITEMCHANGING;
         nm.hdr.hwndFrom = m_hWnd;
         nm.hdr.idFrom = GetDlgCtrlID();
         nm.iItem = nsel;
         nm.iSubItem = iSubItem;
         nm.uChanged = LVIF_EDIT; // MY
         
         if ( SendMessage(WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm) != 0 ) 
            return;
   */
   /////////////////////////////////////////////////
   CRect rc;
   a.GetSubItemRect(nsel, iSubItem, LVIR_BOUNDS, rc);
               
   rc.top--;
   rc.right++;

   // для Win7:
   rc.top--;
   rc.bottom++;  

//   VERIFY(m_Edit.Create(WS_BORDER , rc, this, 1));
   m_Edit.Create(nsel, iSubItem, rc, this, WS_BORDER);
                            
//    {
//       CRect r2;
//       m_Edit.GetRect(r2);
//    }

   m_nSubitemClick = iSubItem;

//    m_Edit.m_nItem = nsel;
//    m_Edit.m_nSubItem = iSubItem;
//    m_Edit.m_ldata = a.GetItemData(nsel);
   m_Edit.SetFont(GetFont());
   m_Edit.SetMargins(4, 4); //can use
   m_Edit.SetWindowText(szOverrideText ? szOverrideText : a.GetItemText(nsel, iSubItem));
   m_Edit.LimitText(59);
   if ( szOverrideText ) m_Edit.SetSel(~0u); 
   else  m_Edit.SetSel(0xFFFF0000); //all
   m_Edit.SetFocus();

}

void CListViewEd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   m_Edit.DestroyWindow();
	CListView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListViewEd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_Edit.DestroyWindow();
	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
}
