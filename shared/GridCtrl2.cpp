// GridCtrl2.cpp : implementation file
//

#include "stdafx.h"
#include "math2.h"
#include "GridCtrl2.h"
#include "TextArchive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl2
WNDPROC g_lpListViewWndProc = NULL;
LRESULT CALLBACK GridCtrl2WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   LPCREATESTRUCT c;
   switch ( msg ) {
   case WM_NCCREATE:
      {
      c = (LPCREATESTRUCT)lParam;
      DWORD style = (c->style&0xFFFF0000)
         |WS_VISIBLE|LVS_SINGLESEL|LVS_REPORT
         |LVS_OWNERDATA|LVS_OWNERDRAWFIXED;

      if ( c->style & GVS_SELALWAYS ) style |= LVS_SHOWSELALWAYS;

      ::SetWindowLong(hWnd,GWL_ID,0);
      HWND hw = ::CreateWindowEx(c->dwExStyle,WC_LISTVIEW,c->lpszName,
         style,c->x,c->y,c->cx,c->cy,c->hwndParent,c->hMenu,
         c->hInstance,(LPVOID)c);
      if ( hw ) {
         DWORD st = c->style&GVS_ALL;
         if (st) SetProp(hw,_T("GVS"),(HANDLE)st);
      }
      return TRUE;
      }
      //break;
   case WM_CREATE:
      PostMessage(hWnd,WM_CLOSE,0,0);
      return 0;
   }
   return CallWindowProc(g_lpListViewWndProc,hWnd,msg,wParam,lParam);
}
// это две функции только для совместимости со старыми ресурсами!
void RegisterGridCtrl2Class() 
{
   WNDCLASS wc;
   INITCOMMONCONTROLSEX icce = {sizeof(icce),ICC_LISTVIEW_CLASSES};
   InitCommonControlsEx(&icce);
   VERIFY( GetClassInfo(0,_T("SysListView32"),&wc));
   g_lpListViewWndProc = wc.lpfnWndProc;
   wc.lpfnWndProc = GridCtrl2WndProc;
   wc.lpszClassName = _T("SysGridCtrl32");
   VERIFY( AfxRegisterClass(&wc) ); 
}


/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlEx)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx message handlers

void CHeaderCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	CHeaderCtrl::OnMouseMove(nFlags, point);
   MSG msg;
   msg.hwnd = m_hWnd;
   msg.message = WM_MOUSEMOVE;
   msg.wParam = nFlags;
   msg.lParam = MAKELPARAM(point.x,point.y);
   m_ToolTip.RelayEvent(&msg);
}

void CHeaderCtrlEx::UpdateToolTips()
{
   int nitems = GetItemCount();
   int ntools = m_ToolTip.GetToolCount();
   CRect rc(0,0,0,0);
   while ( ntools < nitems ) {
      m_ToolTip.AddTool(this,"zz",&rc,ntools+1);
      ntools++;
   }
   HDITEM hdi;
   hdi.mask = HDI_LPARAM;
   for ( int n=0; n<nitems ;n++ ) {
      GetItemRect(n,rc);
      m_ToolTip.SetToolRect(this,n+1,rc);
      GetItem(n,&hdi);
      ASSERT(((TColumnInfo*)hdi.lParam)->signature == 0x77);
      m_ToolTip.UpdateTipText(((TColumnInfo*)hdi.lParam)->sToolTip,this,n+1);
   }
}

BOOL CHeaderCtrlEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
   if ( message == WM_NOTIFY ) {
      UINT code = ((NMHDR*)lParam)->code;
      switch (code) {
      case HDN_ITEMCHANGEDA:
      case HDN_ITEMCHANGEDW:
         UpdateToolTips();
         *pLResult = 0;
         return FALSE; //continue in parent
      }
   }
	return CHeaderCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

void CHeaderCtrlEx::PreSubclassWindow() 
{
   VERIFY(m_ToolTip.Create(this,TTS_ALWAYSTIP|TTS_NOPREFIX));	
   m_ToolTip.SetDelayTime(1000);
}

void CHeaderCtrlEx::OnDestroy() 
{
   int nitems = GetItemCount();
   HDITEM hdi;
   for ( int n=0; n<nitems ;n++ ) {
      hdi.mask = HDI_LPARAM;
      GetItem(n,&hdi);
      delete (TColumnInfo *)hdi.lParam;
   }
	CHeaderCtrl::OnDestroy();
}

LRESULT CHeaderCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   LRESULT res = CHeaderCtrl::WindowProc(message, wParam, lParam);
   switch ( message ) {
   case HDM_INSERTITEMA:
   case HDM_INSERTITEMW:
      UpdateToolTips();
   	break;
   }
	return res;
}
///////////////////////////////////////////////////////////////////////

CGridCtrl2::CGridCtrl2()
{
   RegisterGridCtrl2Class();
   m_nSelCol = 0;
   m_ChkList.Create(AFX_IDB_CHECKLISTBOX_95,11,1,CLR_NONE);
   m_nItemHeight = 0;
   m_bNotify = TRUE;
}

CGridCtrl2::~CGridCtrl2()
{
//    DeleteContents();
}

BEGIN_MESSAGE_MAP(CGridCtrl2, CListCtrl)
   //{{AFX_MSG_MAP(CGridCtrl2)
   ON_WM_KEYDOWN()
   ON_WM_GETDLGCODE()
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   ON_WM_CHAR()
   ON_WM_LBUTTONDBLCLK()
   ON_MESSAGE(WMY_UPDATEFROMEDIT,OnUpdateFromEdit)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void ModifySquareRoot(HWND hWnd,int nCol)
{ 
   ASSERT(::IsWindow(hWnd)); 

   WCHAR buf[128];
   LVCOLUMNW lvc;
   lvc.mask = LVCF_TEXT;
   lvc.pszText = buf;
   lvc.cchTextMax = sizeof(buf)/sizeof(buf[0]);

   if ( (BOOL)::SendMessage(hWnd, LVM_GETCOLUMNW, nCol, (LPARAM)&lvc) ) {
      WCHAR *p=buf;
      while ( *p ) {
         if ( *p == 'v' && p[1] == '~' ) {
            p[0] = 0x221A;
            p[1] = 0x2015;
         }
         p++;
      }
      ::SendMessage(hWnd, LVM_SETCOLUMNW, nCol, (LPARAM)&lvc);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl2 message handlers
UINT CGridCtrl2::OnGetDlgCode() 
{
   return CListCtrl::OnGetDlgCode()|DLGC_WANTALLKEYS;
}

void CGridCtrl2::PreSubclassWindow() 
{
   ASSERT(GetStyle()&LVS_OWNERDATA);
   m_Head.SubclassWindow(GetHeaderCtrl()->m_hWnd);
	m_dwFlags = (DWORD)GetProp(m_hWnd,_T("GVS")); // если создан из старого
   RemoveProp(m_hWnd,_T("GVS"));
}

void CGridCtrl2::DeleteAllColumns()
{
// Delete all of the columns.
   int nitems = GetColumnCount();
   HDITEM hdi;
   for ( int n=0; n<nitems ;n++ ) {
      hdi.mask = HDI_LPARAM;
      m_Head.GetItem(0,&hdi);
      delete (TColumnInfo *)hdi.lParam;
      DeleteColumn(0);
   }

}

BOOL CGridCtrl2::Init(int, int nrows, DWORD dwFlags)
{
   DeleteAllColumns();
   if ( dwFlags ) m_dwFlags = dwFlags;
   if ( dwFlags & GVS_SELALWAYS ) {
      ModifyStyle(0,LVS_SHOWSELALWAYS);
   }
   ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_OWNERDRAWFIXED);
   m_nSelCol = 0;
   SetItemCount(nrows);
//    SetExtendedStyle(LVS_EX_GRIDLINES);
   return TRUE;
}

TColumnInfo *CGridCtrl2::GetColumnInfo(int ncol)
{
   LVCOLUMN lvc;
   lvc.mask = /*LVCF_FMT |*/ /*LVCF_WIDTH |*/ LVCF_SUBITEM;
   VERIFY ( GetColumn(ncol, &lvc) );
   TColumnInfo *ci = (TColumnInfo*)lvc.iSubItem;
   ASSERT(ci->signature == 0x77);
   return ci;
}

BOOL CGridCtrl2::InitColumn(int idx, UINT IdName, int cx, DWORD dwflags, 
                           void *data, UINT nFormat, int nDataStep)
{
   CString sName;

   if ( dwflags & GRID_IDVAL ) {
      sName.Format("%g", *(float *)&IdName );
   } else {
      sName.LoadString(IdName);
   }
   return InitColumn(idx,sName, cx, dwflags,data, nFormat,nDataStep);
}

BOOL CGridCtrl2::InitColumn(int idx, LPCTSTR szName, int cx, DWORD dwflags, 
                           void *data, UINT nFormat, int nDataStep)
{
   TColumnInfo *pC;
   pC = new TColumnInfo;

   BOOL bModSqrt = FALSE;
   if ( *szName == '~' ) {
      szName++;
      bModSqrt = TRUE;
   }

   if ( cx == 0 ) dwflags |= GRID_AUTOSIZE;

   pC->signature = 0x77;  // for debug
   pC->flags = dwflags;
   pC->pData = data;
   pC->nDataStep = nDataStep;
   pC->nDTFormat = nFormat;

   CString sName = szName;

   if ( !sName.IsEmpty() ) {
      LPTSTR sz = sName.GetBuffer(sName.GetLength());
      TCHAR *cc = _tcschr(sz,'\n');
      if ( cc ) {
         pC->sToolTip = cc+1;
         sName.ReleaseBuffer(cc-sz);
      } else {
         sName.ReleaseBuffer();
         pC->sToolTip = sName;
      }
   }

   int nfmt=0;
   if ( nFormat & DT_CENTER ) nfmt = LVCFMT_CENTER;
   else if ( nFormat & DT_RIGHT ) nfmt = LVCFMT_RIGHT;
   
   InsertColumn(idx,sName,nfmt,cx,(int)pC);

   if ( !idx && (nFormat & DT_CENTER)) {
      HDITEM hdi;
      hdi.mask = HDI_FORMAT;
      m_Head.GetItem(0,&hdi);
      hdi.mask = HDI_FORMAT;
      hdi.fmt &= ~HDF_JUSTIFYMASK;
      hdi.fmt |= HDF_CENTER;
      m_Head.SetItem(0,&hdi);
   }

   if ( bModSqrt ) ModifySquareRoot(m_hWnd,idx);

   if ( dwflags & GRID_INITSIZE ) InitSize();

   return TRUE;
}

BOOL CGridCtrl2::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
   UNUSED(pLResult); // unused in release builds
   if (message == WM_DRAWITEM) {
      ASSERT(pLResult == NULL);  // no return value expected
      DrawItem((LPDRAWITEMSTRUCT)lParam);
      return TRUE;
   } else if (message == WM_MEASUREITEM) {
      MeasureItem((LPMEASUREITEMSTRUCT)lParam);
      return TRUE;
   } else if ( message == WM_NOTIFY && 
      ((NMHDR*)lParam)->code == LVN_ITEMCHANGED){
         *pLResult = 0;
         
         return !(m_bNotify && 
            (
            (((NM_LISTVIEW*)lParam)->uChanged != LVIF_STATE)||
            (((NM_LISTVIEW*)lParam)->uOldState==0)
            )
            )
            ; //continue in parent or not
   } else {
      return CWnd::OnChildNotify(message, wParam, lParam, pLResult);
   }
}

void CGridCtrl2::MeasureItem(LPMEASUREITEMSTRUCT pMIS)
{
   if ( m_nItemHeight != 0 ) pMIS->itemHeight = m_nItemHeight;
}

void CGridCtrl2::SetItemHeight(int cy)
{
   if ( m_nItemHeight == cy ) return;

   if ( cy == -1 ) {
      CRect rc;
      m_Head.GetWindowRect(rc);
      cy = rc.bottom - rc.top;
   }

   m_nItemHeight = cy;
   SetRedraw(FALSE);

   RECT rc;
   GetWindowRect(&rc);
   rc.right -= rc.left;
   rc.bottom -= rc.top;
   
   SetWindowPos(0,0,0,rc.right,rc.bottom-1,
      SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_FRAMECHANGED);
   SetWindowPos(0,0,0,rc.right,rc.bottom,
      SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_FRAMECHANGED);

   SetRedraw(TRUE);
}

void CGridCtrl2::DrawItem(LPDRAWITEMSTRUCT pDIS)
{
   int nItem = pDIS->itemID;
   CRect rci(pDIS->rcItem);

   CDC* pDC = CDC::FromHandle(pDIS->hDC);

   LVCOLUMN lvc;
   lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;

   BOOL bFocus = (::GetFocus() == m_hWnd);

   COLORREF clrbk = GetTextBkColor();
   if ( clrbk == CLR_DEFAULT ) clrbk = GetSysColor(COLOR_WINDOW);

   COLORREF cTextColor = GetTextColor();
   if ( cTextColor == CLR_DEFAULT ) cTextColor = GetSysColor(COLOR_WINDOWTEXT);

   ///////////////////////////////////////////////////////////
   CString s;
   
   pDC->SetBkColor(clrbk);
   pDC->SelectStockObject(BLACK_PEN);
   pDC->SetTextColor(cTextColor);

   for(int n = 0; GetColumn(n, &lvc); n++)
   {
      TColumnInfo *ci = (TColumnInfo*)lvc.iSubItem;
      ASSERT(ci->signature == 0x77);

      rci.right = rci.left+lvc.cx;

      CRect rc(rci.left,rci.top,rci.right-1,rci.bottom-1);

#define rci $
      pDC->MoveTo(rc.left,rc.bottom);
      pDC->LineTo(rc.right,rc.bottom);
      pDC->LineTo(rc.right,rc.top-1);
      
      BOOL bSelected = FALSE;

      if ( n == 0  && (m_dwFlags & GVS_FIXCOL) ) {
         CBrush br(GetSysColor(COLOR_BTNFACE));
         pDC->FillRect( rc, &br);
         pDC->Draw3dRect(rc, GetSysColor(COLOR_3DHILIGHT),
            GetSysColor(COLOR_3DSHADOW));
      } else {
          CBrush br(clrbk);

          pDC->FillRect(rc,&br);
         
         if ( pDIS->itemState & (ODS_FOCUS|ODS_SELECTED)
            && n == m_nSelCol
            ) {
            if ( bFocus ) {
               COLORREF hl = GetSysColor(COLOR_HIGHLIGHT);
               CBrush bhl(hl);
               pDC->FillRect( rc, &bhl);
               pDC->SetBkColor(hl);
               pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
               if ( pDIS->itemState & ODS_FOCUS ) {
                  pDC->DrawFocusRect(rc);
               }
               bSelected  = TRUE;
            } else if ( GetStyle() & LVS_SHOWSELALWAYS ) {
               CBrush coco(0x00C0C0C0);
               pDC->FillRect( rc, &coco);
            }
         } 
      }

      if ( m_ChkList.m_hImageList && (ci->flags & GRID_TYPEMASK)==GRID_CHECK ) {
         CPoint pt(/*(rc.right+rc.left-11)/2*/ 0,(rc.bottom+rc.top-11)/2);
         pt.x = pt.y;
         if ( pt.x < 0 ) pt.x = 0;
         m_ChkList.Draw(pDC,(*GetCellAddr(nItem,ci))&3,pt,ILD_NORMAL);
         // DrawFrameControl(DFC_BUTTON,DFCS_BUTTONCHECK| ?DFCS_CHECKED:0);
      } else {
         FormatData(nItem,s,ci);
         if ( bSelected ) {
            cTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
         } else {
            if ( (ci->flags & GRID_READONLY) ||
               (ci->flags & GRID_READONLY1) && nItem != 0) 
            {
               cTextColor = GetSysColor(COLOR_GRAYTEXT);
            } else {
               cTextColor = GetSysColor(COLOR_WINDOWTEXT);
            }
         }
         pDC->SetTextColor(cTextColor);
         rc.DeflateRect(2,0);
         pDC->DrawText(s,rc,ci->nDTFormat
            | DT_END_ELLIPSIS 
            | DT_SINGLELINE
            | DT_NOPREFIX );
      }
#undef rci

      rci.left = rci.right;
   }
}

BOOL CGridCtrl2::FormatData(int nrow, CString & s,TColumnInfo *ci)
{ // return FALSE when unknown format or no data
   if ( nrow < 0 ) return FALSE;
   try {
      DWORD f;
      char *p;
      
      f = ci->flags & GRID_EXTYPEMASK;

      if ( f == GRID_INDEX0 ) {
         s.Format(_T("%d"),nrow);
         return TRUE;
      } else if ( f == GRID_INDEX1 ) {
         s.Format(_T("%d"),nrow+1);
         return TRUE;
      } 

      if ( !ci->pData ) {
         s.Empty();
         return FALSE;
      }
      p = GetCellAddr(nrow,ci);

      switch ( f ) {
      case GRID_INT   : s.Format(_T("%d"),  *(int *)p);    break;
      case GRID_DWORD : s.Format(_T("%u"),  *(DWORD *)p);  break;
      case GRID_FLOAT : s.Format(_T("%g"),  *(float *)p ); break;
      case GRID_DOUBLE: s.Format(_T("%g"),  *(double *)p); break;
      case GRID_DOUBLE3:s.Format(_T("%.3f"),*(double *)p); break;
      case GRID_DOUBLE6:s.Format(_T("%f"),  *(double *)p); break;
      case GRID_CHECK : s = (*(BOOL *)p) ? _T("Yes") : _T("No"); break;
      case GRID_DWHEX : s.Format(_T("%08X"), *(DWORD *)p); break;
      case GRID_SZARRAY:s = *(LPCTSTR*)p;                  break;
      case GRID_FLOAT3: s.Format(_T("%.3f"), *(float *)p); break;
      case GRID_FLOAT6: s.Format(_T("%f"),   *(float *)p); break;
      case GRID_FLOAT8: s.Format(_T("%.8f"), *(float *)p); break;
      default:
         return FALSE;
      }
      _asm wait;
   } catch (...) {
      RESETFP();
      s = _T("#error#");
   }
   return TRUE;
}

char * CGridCtrl2::GetCellAddr(int nrow, TColumnInfo *ci)
{
   return (char *)(ci->pData)  + nrow * ci->nDataStep;
}

void CGridCtrl2::ResetAutosizeFlags()
{
   LVCOLUMN lvc;
   lvc.mask = /*LVCF_FMT |*/ LVCF_SUBITEM;
   for(int nColumn = 0; GetColumn(nColumn, &lvc); nColumn++)
   {
      TColumnInfo *c = (TColumnInfo*)lvc.iSubItem;
      ASSERT(c->signature == 0x77);
      c->flags &= ~GRID_AUTOSIZE;
   }
}

void CGridCtrl2::InitSize()
{
   CRect rc;

   GetClientRect(rc);

   LVCOLUMN lvc;
   lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;

   int ncols=0;
   int k = rc.right;
   // считаем автосайзовые колумны!
   int nColumn;
   for( nColumn = 0; GetColumn(nColumn, &lvc); nColumn++)
   {
      TColumnInfo *c = (TColumnInfo*)lvc.iSubItem;
      ASSERT(c->signature == 0x77);
      if ( c->flags & GRID_AUTOSIZE ) {
         ncols++;
      } else {
         k -= lvc.cx;
      }
   }

   if ( ncols ) { // насчитали
      int t = k/ncols;
      if ( t > 0 ) {
         int last = -1;
         for ( nColumn = 0; GetColumn(nColumn, &lvc); nColumn++) {
            TColumnInfo *c = (TColumnInfo*)lvc.iSubItem;
            ASSERT(c->signature == 0x77);
            if ( c->flags & GRID_AUTOSIZE ) {
               SetColumnWidth(nColumn,t);
               k-=t;
               last = nColumn;
            }
         }
         if ( k && (last != -1 ) ) {
            SetColumnWidth(last,GetColumnWidth(last)+k);
         }
      }
   }
}

void CGridCtrl2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   switch ( nChar ) {
#ifdef _DEBUG
   case VK_F5:
      SetSizeToFit(GVSS_HORZ);
      return ;
   case VK_F6:
      SetSizeToFit(GVSS_VERT);
      return ;
   case VK_F7:
      SetSizeToFit(GVSS_BOTH);
      return ;
#endif
   case VK_RETURN:
   case VK_F2:
      EditCurSel();
      return;
   case VK_LEFT: _VK_LEFT:
      if ( m_nSelCol > 1 || (!( m_dwFlags & GVS_FIXCOL ) && m_nSelCol > 0) ) {
         m_nSelCol--;
      } else {
         goto _default;
      }
      break;
   case VK_TAB:
      if ( GetKeyState(VK_SHIFT)&0x8000 ) goto _VK_LEFT;
   case VK_RIGHT:
      {
         if ( m_nSelCol >= GetColumnCount()-1 ) goto _default;
         m_nSelCol++;
      }
      break;
   case VK_INSERT:
      if ( GetKeyState(VK_SHIFT)&0x8000 ) {
         // вставить из буфера обмена
         InsertFromClipboard();
         return;
      }
_default:
   default: 
      CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
      return;
   }
   
   CRect rc;
   int m = GetFocusedItem();
   GetItemRect(m,rc,LVIR_BOUNDS);
   InvalidateRect(rc);
   EnsureVisibleSubItem(m_nSelCol);
}

void CGridCtrl2::EnsureVisibleSubItem(int nSubItem)
{
   CRect rci,rc;
   GetClientRect(rc);
   GetCellRect(0,nSubItem,rci);

   if ( rci.left > rc.left &&  rci.right < rc.right ) return;

   int isize = rci.right-rci.left;
   int rsize = rc.right-rc.left;

   if ( isize > rsize ) { // итем больше
      Scroll(CSize(rci.left,0));
   } else {
      if ( rci.left < 0 ) {
         Scroll(CSize(rci.left,0));
      } else if ( rci.right > rc.right ) {
         Scroll(CSize(rci.right-rc.right,0));
      }
   }
}

void CGridCtrl2::InsertFromClipboard()
{
   int nRows = GetItemCount();
   CPoint pt = GetCurSel();
   int nSelRow = pt.y;
   int nSelCol = pt.x;
   int newRow = nSelRow;
   TColumnInfo *ci = GetColumnInfo(m_nSelCol);

   if ( ci->flags & GRID_READONLY ) return;

   if ( (ci->flags & GRID_READONLY1) && nSelRow != 0 ) return;

   DWORD f = ci->flags;
   UINT nMode;

   switch ( f & GRID_TYPEMASK ) {
   case GRID_INT:
      nMode = 1;
   	break;
   case GRID_DWORD:
   case GRID_DWHEX:
   case GRID_CHECK:
      nMode = 0;
   	break;
   case GRID_FLOAT:
   case GRID_DOUBLE:
      nMode = 2;
   	break;
   default: ;
      return;
   }

   if ( !OpenClipboard() ) return;
   
   HANDLE h = GetClipboardData(CF_TEXT);
   if ( h ) {
      BYTE *p = (BYTE *)GlobalLock(h);
      CMemFile mf(p,lstrlenA((char *)p));
      CString s;
      DWORD dw;
      int n;
      double d;
      TRY {
         CTextArchive ar(&mf,CArchive::load);
         while ( newRow < nRows ) {
            switch ( nMode ) {
            case 0:
               ar >> dw;
               s.Format(_T("%u"),dw);
            	break;
            case 1:
               ar >> n;
               s.Format(_T("%d"),n);
            	break;
            case 2:
               ar >> d;
               s.Format(_T("%g"),d);
               break;
            }
            OnUpdateFromEdit(MAKEWPARAM(newRow,m_nSelCol), LONG((LPCTSTR)s));
            if ( ++newRow >= nRows ) break;
            SetCurSel(nSelCol,newRow);
         }
      } END_TRY;
      GlobalUnlock(h);
   }
   CloseClipboard();
}

BOOL CGridCtrl2::EditCurSel(UINT nChar)
{
   TColumnInfo *ci = GetColumnInfo(m_nSelCol);

   DWORD t = ci->flags & GRID_TYPEMASK;

   if ( t == GRID_INDEX || t == GRID_INDEX1 ) return FALSE;

   if ( ci->flags & GRID_READONLY ) {
      if ( OnReadOnlyEdit() ) return FALSE;
   }

   int nSelRow = GetFocusedItem();
   if ( nSelRow < 0 ) return FALSE;

   CString s;

   if ( (ci->flags & GRID_READONLY1) && nSelRow != 0 ) {
      if ( OnReadOnlyEdit() ) return FALSE;
   }

   if ( t == GRID_CHECK ) {
      if ( nChar && nChar != _T(' ') ) return FALSE;
      BOOL *d = (BOOL *)GetCellAddr(nSelRow,ci);
      *d = !(*d);
      Update(nSelRow);
      return TRUE;
   }

   if ( !FormatData(nSelRow, s, ci) ) return FALSE;
   
   if ( nChar ) s = (TCHAR)nChar;
      
   CRect rc;
   GetCellRect(nSelRow,m_nSelCol,rc);
   rc.right--; rc.bottom--;
   m_Edit.Create(nSelRow,m_nSelCol,rc,this,ci->nDTFormat & 3);
   m_Edit.SetFont(GetFont());
   m_Edit.SetSel(0,-1);
   m_Edit.ReplaceSel(s);
   m_Edit.SetFocus();
   if( !nChar ) m_Edit.SetSel(0,-1);
   
   return TRUE;
}

void CGridCtrl2::MouseHitTest(CPoint point)
{
   LVHITTESTINFO lht;
   lht.pt = point;
   SubItemHitTest(&lht);

   if ( lht.flags & LVHT_ONITEM ) {
      if ( lht.iSubItem > 0 
         || (!( m_dwFlags & GVS_FIXCOL ) && lht.iSubItem == 0) ) {
         m_nSelCol = lht.iSubItem;
         CRect rc;
         GetItemRect(lht.iItem,rc,LVIR_BOUNDS);
         InvalidateRect(rc);
         EnsureVisibleSubItem(lht.iSubItem);
      }
   }
}

void CGridCtrl2::OnLButtonDown(UINT nFlags, CPoint point) 
{
   MouseHitTest(point);
   CListCtrl::OnLButtonDown(nFlags, point);
}

void CGridCtrl2::OnRButtonDown(UINT nFlags, CPoint point) 
{
   MouseHitTest(point);
   CListCtrl::OnRButtonDown(nFlags, point);
}

void CGridCtrl2::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if ( (nFlags & 0x2100) || nChar < 0x20 ) {
      CListCtrl::OnChar(nChar, nRepCnt, nFlags);
   } else {
      EditCurSel(nChar);
   }                 
}

void CGridCtrl2::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   CListCtrl::OnLButtonDblClk(nFlags, point);
   EditCurSel();
}

LONG CGridCtrl2::OnUpdateFromEdit( UINT  wParam, LONG lParam)
{
   CString s((LPCTSTR)lParam );
   
   TColumnInfo *ci = GetColumnInfo(m_nSelCol);

   int ncol = m_nSelCol, 
       nrow = GetFocusedItem();
   if ( nrow < 0 ) return 0;

   ASSERT(MAKEWPARAM(nrow,m_nSelCol) == wParam);

   DWORD f = ci->flags&GRID_TYPEMASK;
   
   if ( f == GRID_INDEX || f == GRID_INDEX1 ) return 0;
   
   if ( !ci->pData ) return 0;
   
   char *p = GetCellAddr(nrow,ci);

   TCHAR **endptr = NULL;
   
   switch ( f ) {
   case GRID_CHECK:  *(BOOL *)p = !!_tcstol(s,0,0); /* для клипборда*/  break;
   case GRID_INT   : *(int *)p   = _tcstol(s,endptr,0);    break;
   case GRID_DWORD : *(DWORD *)p = _tcstoul(s,endptr,0);  break;
   case GRID_DWHEX : *(DWORD *)p = _tcstoul(s,endptr,16); break;
   case GRID_FLOAT : *(float *)p = (float)_tcstod(s,endptr); break;
   case GRID_DOUBLE: *(double *)p = _tcstod(s,endptr); break;
   default: 
      return FALSE;
   }

   if ( m_dwFlags & GVS_SHAREDBUF ) {
      Invalidate();
   } else {
      CRect rc;
      GetCellRect(nrow,ncol,rc);
      InvalidateRect(rc);
   }

   {
      NM_LISTVIEW nl;
      nl.hdr.hwndFrom = m_hWnd;
      nl.hdr.idFrom = GetDlgCtrlID();
      nl.hdr.code = LVN_ITEMCHANGED;
      nl.iItem = nrow;
      nl.iSubItem = m_nSelCol; 
      nl.uNewState = 0;
      nl.uOldState = 0;
      nl.uChanged = LVIF_TEXT;
      GetParent()->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&nl);
   }
   return TRUE;
}

BOOL CGridCtrl2::GetCellRect(int nItem, int nSubItem, CRect &rc)
{
   if ( !GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rc) ) return FALSE;;
   if ( nSubItem == 0 ) {
      rc.right = rc.left+GetColumnWidth(0);
   }
   return TRUE;
}

BOOL CGridCtrl2::ChangeColumn(int idx, int nlen, void * pData, BOOL bRedraw, int datastep)
{
   ASSERT(m_hWnd);
   TColumnInfo *ci = GetColumnInfo(idx);
   ci->pData = pData;
   if ( datastep ) ci->nDataStep = datastep;
   int row = GetCurSel().y;
   SetItemCountEx(nlen,LVSICF_NOSCROLL );
   CPoint tr = GetCurSel();
   if ( tr.y < 0 && row > 0 ) SetCurSel(tr.x,row-1);
   if ( bRedraw ) {
      InitSize();
   }
   return TRUE;
}

BOOL CGridCtrl2::ChangeColumns(BOOL bRedraw, void * )
{  // нулевые все!
   ASSERT(m_hWnd);

   int nCols = GetColumnCount();
   for ( int i=0; i<nCols ;i++ ) {
      TColumnInfo *ci = GetColumnInfo(i);
      ci->pData = NULL;
   }
   SetItemCount(0);
   if ( bRedraw ) Invalidate();
   
   return TRUE;
}

void CGridCtrl2::SetColDataBuf(int ncol, void * buf, BOOL fRepaint)
{  
   TColumnInfo *ci = GetColumnInfo(ncol);
   ci->pData = buf;
   if ( fRepaint ) Invalidate();
}

void CGridCtrl2::SetColumnStyle(int idx, DWORD dwRemove, DWORD dwAdd, BOOL bRedraw)
{
   TColumnInfo *ci = GetColumnInfo(idx);
   ci->flags &= ~dwRemove;
   ci->flags |= dwAdd;
   if ( bRedraw ) Invalidate();
}

void CGridCtrl2::SetColumnName(int idx, LPCTSTR szName, BOOL bRedraw)
{
   TColumnInfo *ci = GetColumnInfo(idx);

   BOOL bModSqrt = FALSE;

   if ( *szName == '~' ) {
      szName++;
      bModSqrt = TRUE;
   }

   CString sName = szName;
           
   if ( !sName.IsEmpty() ) {
      LPTSTR sz = sName.GetBuffer(sName.GetLength());
      TCHAR *cc = _tcschr(sz,'\n');
      if ( cc ) {
         ci->sToolTip = cc+1;
         sName.ReleaseBuffer(cc-sz);
      } else {
         sName.ReleaseBuffer();
         ci->sToolTip = sName;
      }
   }         
   
   LVCOLUMN lvc;
   lvc.mask = LVCF_TEXT;
   lvc.pszText = (LPTSTR)(LPCTSTR)sName;
   SetColumn(idx,&lvc);

   if ( bModSqrt ) ModifySquareRoot(m_hWnd,idx);

   if ( bRedraw ) Invalidate();
}

CString CGridCtrl2::GetColumnName(int idx)
{
   CString sName;
   LVCOLUMN lvc;
   lvc.mask = LVCF_TEXT;
   lvc.pszText = sName.GetBuffer(128);
   lvc.cchTextMax = 128;
   GetColumn(idx,&lvc);
   sName.ReleaseBuffer();
   return sName;
}

CPoint CGridCtrl2::GetCurSel()
{
   CPoint p;
   p.x = m_nSelCol;
   if (m_dwFlags & GVS_FIXCOL) p.x--;
   p.y = GetFocusedItem();
   if ( p.y < 0 ) {
      if ( GetStyle() & LVS_SHOWSELALWAYS ) p.y = 0;
   }
   return p;
}

void CGridCtrl2::SetCurSel(int ncol, int nrow, BOOL bNotify)
{
   m_Edit.DestroyWindow();

   if (m_dwFlags & GVS_FIXCOL) ncol++;

   m_nSelCol = ncol;
   Update(nrow);
   EnsureVisible(nrow,FALSE);
   EnsureVisibleSubItem(ncol);

   m_bNotify = bNotify;
   
   if ( bNotify ) {
      // если нужна нотификация - установим состояние 0, 
      // (т.к. текущее состояние неизвестно), чтобы установка селекции 
      // сработала наверняка и вызвала OnChildNotify
      SetItemState(nrow,0,LVIS_FOCUSED|LVIS_SELECTED);
      // эта стока вызовет OnChildNotify, но парент не получит сообщения 
      // поскольку OldState != 0
   }
   SetItemState(nrow,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
   SetSelectionMark(nrow);

   m_bNotify = TRUE;
}

BOOL CGridCtrl2::SetSizeToFit(DWORD dwFlags)
{
   if ( dwFlags & GVSS_HORZ ) ShowScrollBar( SB_HORZ, FALSE );
   if ( dwFlags & GVSS_VERT ) ShowScrollBar( SB_VERT, FALSE );

   CRect rc;
   GetItemRect(0,rc,LVIR_BOUNDS);
   rc.right -= rc.left;
   rc.bottom -= rc.top;
   {
      CRect rch;
      m_Head.GetWindowRect(rch);
      rc.bottom *= GetItemCount();
      rc.bottom += rch.bottom-rch.top;
   }

   CRect rw;
   GetWindowRect(rw);

   rw.right -= rw.left;
   rw.bottom -= rw.top;

   if ( dwFlags == GVSS_HORZ ) {
      rw.right = rc.right + 2*GetSystemMetrics(SM_CXEDGE);
      if ( rw.bottom < rc.bottom ) {
         rw.right += GetSystemMetrics(SM_CXHSCROLL);
      }
   } else if ( dwFlags == GVSS_VERT ) {
      rw.bottom = rc.bottom + 2*GetSystemMetrics(SM_CYEDGE);
      if ( rw.right < rc.right ) {
         rw.bottom += GetSystemMetrics(SM_CYVSCROLL);
      }
   } else if ( dwFlags == GVSS_BOTH ) {
      rw.right = rc.right + 2*GetSystemMetrics(SM_CXEDGE);
      rw.bottom = rc.bottom + 2*GetSystemMetrics(SM_CYEDGE);
   }
   
   SetWindowPos(NULL,0,0,rw.right,rw.bottom,
      SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER);

   return TRUE;
}

void CGridCtrl2::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	*pResult = 0;
   if ( pDispInfo->item.mask == 1 ) { // when HDN_DIVIDERDBLCLICK
      CString s;
      TColumnInfo *ci = GetColumnInfo(pDispInfo->item.iSubItem);
      if ( FormatData(pDispInfo->item.iItem,s,ci) ) {
         int t = min(s.GetLength(),pDispInfo->item.cchTextMax-1);
         _tcsncpy(pDispInfo->item.pszText,s,t);
         pDispInfo->item.pszText[t] = 0;
      }
   }
   *pResult = 0;
}

BOOL CGridCtrl2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
   m_Edit.DestroyWindow();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CGridCtrl2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   m_Edit.DestroyWindow();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGridCtrl2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   m_Edit.DestroyWindow();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
