// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
   //{{AFX_MSG_MAP(CListCtrlEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers
int CListCtrlEx::InsertColumn(int idx, LPCTSTR str, int size, int fmt)
{
   LV_COLUMN lvc;
   
   lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT;
   lvc.cx = size;
   lvc.fmt = fmt;
   lvc.pszText = (LPTSTR)str;
   
   return CListCtrl::InsertColumn(idx, &lvc);
}

int CListCtrlEx::InsertColumn(int idx, UINT idstr, int size, int fmt)
{  
   CString s;
   s.LoadString(idstr);
   return InsertColumn(idx, s, size, fmt);
}
////////////////////////////////////////////////////////////////
int CListCtrlEx::InsertItem(int idx, int img, UINT idstr)
{  
   CString s;
   
   s.LoadString(idstr);

   return InsertItem( idx,  img, s );
}

int CListCtrlEx::InsertItem2(int idx, LPCTSTR str)
{
   if ( SetItemText(idx, 0, str) ) return idx;
   return CListCtrl::InsertItem(idx, str);
}

int CListCtrlEx::InsertItem2Int(int idx, LPCTSTR str, int data)
{
   if ( !SetItemText(idx, 0, str) ) 
      if ( (idx = CListCtrl::InsertItem(idx, str)) < 0 ) return FALSE;
   
   return SetItemInt(idx, 1, data);
}

int CListCtrlEx::InsertItem2Float(int idx, LPCTSTR str, float data, TCHAR *fmt)
{
   if ( !SetItemText(idx, 0, str) ) 
      if ( (idx = CListCtrl::InsertItem(idx, str)) < 0 ) return FALSE;
   return SetItemFloat(idx, 1, data, fmt);
}

int CListCtrlEx::InsertItem2Text(int idx, LPCTSTR str, LPCTSTR data)
{
   if ( !SetItemText(idx, 0, str) ) 
      if ( (idx = CListCtrl::InsertItem(idx, str)) < 0 ) return FALSE;
   return SetItemText(idx, 1, data);
}

int CListCtrlEx::InsertItem2Dwhex(int idx, LPCTSTR str, DWORD data)
{
   if ( !SetItemText(idx, 0, str) ) 
      if ( (idx = CListCtrl::InsertItem(idx, str)) < 0 ) return FALSE;
   return SetItemDword(idx, 1, data);
}

int CListCtrlEx::InsertItem(int idx, int img, LPCTSTR str)
{  
   LV_ITEM lvi;

   lvi.mask = LVIF_TEXT | LVIF_IMAGE;
   lvi.pszText = (LPTSTR)str;
   
   lvi.iItem = idx;
   lvi.iSubItem = 0;
   lvi.iImage = img;

   return CListCtrl::InsertItem(&lvi);
}
////////////////////////////////////////////////////////////////   

BOOL CListCtrlEx::SetItemInt(int item, int subitem, int value)
{
   CString s;
   s.Format(_T("%d"), value);
   return SetItemText( item, subitem, s);
}

BOOL CListCtrlEx::SetItemQuotedInt(int item, int subitem, int value)
{   // 'ABC'
   if ( value ) {
      char bb[7];
      // 'XABC'_
      char *d = bb+4;
      bb[0] = '\'';
      bb[5] = '\'';
      bb[6] = 0;
      DWORD r = (DWORD)value;
      for ( ; d != bb; d-- ) {
         *d = (BYTE)(r&0xFF);
         if ( !*d ) { // нулевой байт
            if ( !r ) *d = '\''; // еще что-то есть - ошибка
            break;
         } else {
            if ( *d <= 0x20 ) { // непечатные - ошибка
               *d = 0;
               break;
            }
         }
         r >>= 8; 
      }
      if ( *d ) return SetItemText ( item, subitem, d );
   }    
   return SetItemInt  ( item, subitem, value );
}

BOOL CListCtrlEx::SetItemFloat(int item, int subitem, float value, TCHAR *fmt)
{
   CString s;
   s.Format(fmt, value);
   return SetItemText( item, subitem, s);
}

BOOL CListCtrlEx::SetItemDword(int item, int subitem, DWORD value, TCHAR *fmt)
{
   CString s;
   s.Format(fmt, value);
   return SetItemText( item, subitem, s);
}

BOOL CListCtrlEx::SetItemText(int item, int subitem, UINT idstr)
{
   CString s;
   s.LoadString(idstr);
   return SetItemText( item, subitem, s);
}

////////////////////////////////////////////////////////////////
// CListCtrl

int CListCtrlEx::GetCurSel()
{        
   return GetNextItem(-1, LVIS_SELECTED);
}
////////////////////////////////////////////////////////////////
BOOL CListCtrlEx::SetCurSel(int nItem)
{  // подсветить только один элемент, остальные снять
   POSITION pos = GetFirstSelectedItemPosition();
   while (pos) {
      int iItem = GetNextSelectedItem(pos);
      SetItemState(iItem, 0, LVIS_SELECTED|LVIS_FOCUSED);
   }
   if ( nItem < 0 ) return TRUE;
   return SetItemState( nItem, 
                        LVIS_SELECTED|LVIS_FOCUSED,
                        LVIS_SELECTED|LVIS_FOCUSED);
}

// BOOL CListCtrlEx::SetCurFocus(int nItem) 
// {  // установить фокус на указанный элемент, остальные снять / Proj9
//    POSITION pos = GetFirstSelectedItemPosition();
//    while (pos) {
//       int iItem = GetNextSelectedItem(pos);
//       SetItemState(iItem, 0, LVIS_FOCUSED);
//    }
//    if ( nItem < 0 ) return TRUE;
//    return SetItemState( nItem, 
//       LVIS_SELECTED|LVIS_FOCUSED,
//       LVIS_SELECTED|LVIS_FOCUSED);
// }

void CListCtrlEx::SelectAll()
{
   SetRedraw(FALSE);
   int nI = GetItemCount();
   for ( int i=0; i<nI ;i++ ) {
      SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
   }
   SetRedraw(TRUE);
}

////////////////////////////////////////////////////////////////

BOOL CListCtrlEx::SaveColumnSizes(UINT nSection)
{
   CString s;
   if ( !s.LoadString(nSection) ) return FALSE;
   return SaveColumnSizes(s);
}   

BOOL CListCtrlEx::SaveColumnSizes(LPCTSTR szSection)
{

   int k = GetColumnCount();
   if ( !k ) return TRUE;

   int *siz = new int[k];

   for (int i=0; i<k ;i++ ) {
      siz[i] = GetColumnWidth( i );
   }

   k = AfxGetApp()->WriteProfileBinary(szSection,
      _T("Columns"), (LPBYTE)siz, k*sizeof(int));

   delete [] siz;
   return k;
}

BOOL CListCtrlEx::SaveColumnOrder(LPCTSTR szSection)
{
   int k = GetColumnCount();
   if ( !k ) return TRUE;

   int *siz = new int[k];

   GetColumnOrderArray(siz, k);
   
   k = AfxGetApp()->WriteProfileBinary(szSection,
      _T("Order"), (LPBYTE)siz, k*sizeof(int));

   delete [] siz;
   return k;
}

BOOL CListCtrlEx::LoadColumnSizes(UINT nSection)
{
   CString s;
   if ( !s.LoadString(nSection) ) return FALSE;
   return LoadColumnSizes(s);
}

BOOL CListCtrlEx::LoadColumnSizes(LPCTSTR szSection)
{
   BOOL ret;
   int *siz;
   int  num;
   ret = AfxGetApp()->GetProfileBinary(szSection,
      _T("Columns"), (BYTE **)&siz, (UINT *)&num);
   if ( ret ) {
      for (int i=0; i<num/(int)sizeof(int) ;i++ ) {
         SetColumnWidth(i, siz[i]);
      }
   }
   delete siz;
   return ret;
}

BOOL CListCtrlEx::LoadColumnOrder(LPCTSTR szSection)
{
   BOOL ret;
   int *siz;
   int  num;
   ret = AfxGetApp()->GetProfileBinary(szSection,
      _T("Order"), (BYTE **)&siz, (UINT *)&num);
   if ( ret ) {
      SetColumnOrderArray(num/sizeof(int), siz);
   }
   delete siz;
   return ret;
}

int CListCtrlEx::GetColumnCount()
{
   CHeaderCtrl* ph = GetHeaderCtrl();
   if ( !ph ) return 0;
   return ph->GetItemCount();
}

void CListCtrlEx::DeleteAllColumns(void)
{
   int n = GetColumnCount();
   while ( n-- ) DeleteColumn(0);
}

void CListCtrlEx::ReduceItems(int nItems)
{
   int nc = GetItemCount();
   if ( nc <= nItems ) return;
   while ( nc > nItems ) {
      DeleteItem(--nc);
   }
}

DWORD CListCtrlEx::GetItemSpacing()
{
   return ListView_GetItemSpacing(m_hWnd, TRUE);
}

int CListCtrlEx::GetItemInt(int item, int subitem)
{
   CString S = GetItemText(item, subitem);
   return _tcstol(S, NULL, 0);
}

float CListCtrlEx::GetItemFloat(int item, int subitem)
{
   CString S = GetItemText(item, subitem);
   return (float)_tcstod(S, NULL);
}

DWORD CListCtrlEx::GetItemDword(int item, int subitem)
{
   CString S = GetItemText(item, subitem);
   return _tcstoul(S, NULL, 0);
}

DWORD CListCtrlEx::GetItemDwHex(int item, int subitem)
{
   CString S = GetItemText(item, subitem);
   return _tcstoul(S, NULL, 16);
}

void CListCtrlEx::GetSelectedArray(CDWordArray *a)
{
   if ( !a ) return;
   int iItem = -1;
   while( (iItem = GetNextItem(iItem, LVNI_SELECTED)) >= 0 ) {
      a->Add((DWORD)iItem);
   }
}

void CListCtrlEx::SetSelectedArray(CDWordArray *a)
{
   SetRedraw(FALSE);
   int nI = GetItemCount();
   for ( int i=0; i<nI ;i++ ) {
      SetItemState(i, 0, LVIS_FOCUSED|LVIS_SELECTED);
   }
   if ( a ) {
      int n = a->GetSize();
      for ( int i=0; i<n ;i++ ) {
         int iItem = a->GetAt(i);
         if ( iItem >= nI ) continue;
         SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
      }
   }
   SetRedraw(TRUE);
}


