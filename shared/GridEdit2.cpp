// GridEdit2.cpp : implementation file
//

#include "stdafx.h"
#include "GridEdit2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridEdit2
// AFX_STATIC UINT _afxMsgMouseWheel;

BEGIN_MESSAGE_MAP(CGridEdit2, CEdit)
	//{{AFX_MSG_MAP(CGridEdit2)
   ON_WM_KEYDOWN()
   ON_WM_GETDLGCODE()
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGridEdit2::Create( int nItem, int nSubItem, 
                        const RECT& rect, CWnd* pParentWnd, 
                        DWORD dwStyle, UINT nID ) 
{
   if ( !CEdit::Create( dwStyle |
      WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL
      , rect, pParentWnd, nID ) ) 
      return FALSE;
   
   SetMargins(2, 2);

   m_wParam = MAKEWPARAM(nItem, nSubItem);

   SetForegroundWindow();
   SetCapture();

   bCancel = FALSE;
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridEdit2 message handlers

void CGridEdit2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   switch ( nChar ) {
   case VK_UP:
   case VK_DOWN:
   case VK_TAB:
   case VK_NEXT:
   case VK_PRIOR:
   case VK_RETURN: 
      {
         HWND hParent = GetParent()->GetSafeHwnd(); // this ì.á. destroy â SendUpdateFromEdit
         bCancel = TRUE;
         SendUpdateFromEdit();
         if ( nChar == VK_RETURN ) nChar = VK_DOWN;  // nOnReturnKey
         ::SendMessage(hParent, WM_KEYDOWN, nChar,
            MAKELPARAM(nRepCnt, nFlags));
      }
   case VK_ESCAPE:
      bCancel = TRUE;
      DestroyWindow();
      break;
   default: 
      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
   }
}

UINT CGridEdit2::OnGetDlgCode() 
{
   return CEdit::OnGetDlgCode()|DLGC_WANTALLKEYS;
}

void CGridEdit2::SendUpdateFromEdit()
{
   CString s;
   GetWindowText(s);
   GetParent()->SendMessage(WMY_UPDATEFROMEDIT, m_wParam, (LONG)(LPCTSTR)s);
}

void CGridEdit2::OnDestroy() 
{
   if ( !bCancel )  SendUpdateFromEdit();
   CEdit::OnDestroy();
   ReleaseCapture();
}

BOOL CGridEdit2::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN &&
      (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) ) {
      OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) ;
      return TRUE;
   }

   return CEdit::PreTranslateMessage(pMsg);
}

LRESULT CGridEdit2::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   if ( message >= WM_LBUTTONDOWN && message <= WM_MBUTTONDBLCLK ) {
      POINT pt = { LOWORD(lParam), HIWORD(lParam) };
      CRect rc;
      GetClientRect(rc);
      if ( !rc.PtInRect(pt) ) {

         ClientToScreen(&pt);

         int t = message - WM_LBUTTONDOWN;
         static const DWORD dwFlags[9] = 
         {
            MOUSEEVENTF_LEFTDOWN,
               MOUSEEVENTF_LEFTUP,
               MOUSEEVENTF_LEFTDOWN,
               MOUSEEVENTF_RIGHTDOWN,
               MOUSEEVENTF_RIGHTUP,
               MOUSEEVENTF_RIGHTDOWN,
               MOUSEEVENTF_MIDDLEDOWN,
               MOUSEEVENTF_MIDDLEUP,
               MOUSEEVENTF_MIDDLEDOWN
         };         

         DestroyWindow();
         if ( t < 9 ) {
            mouse_event(dwFlags[t]|MOUSEEVENTF_ABSOLUTE,
               pt.x, pt.y, 0, 0);
         }

      } else {
         LRESULT res = CEdit::DefWindowProc(message, wParam, lParam);
         if ( ::GetCapture() != m_hWnd) {
            SetCapture();
         } else {
         }
         return res;
      }
   } else if ( message == WM_PASTE ) {
      if ( !OpenClipboard() ) return 0;
      
      HANDLE h = GetClipboardData(CF_TEXT);
      if ( h ) {
         BYTE *buf = (BYTE *)GlobalLock(h);
         
         int len = lstrlen((char*)buf);
         BYTE *pb = new BYTE[len+1];
         memcpy(pb, buf, len+1);
         BYTE *p = pb;
         while ( *p ) { if ( *p < 0x20 ) *p = 0x20; p++; }
         ReplaceSel((char*)pb, 1);
         delete [] pb;

         GlobalUnlock(h);
      }
      CloseClipboard();
      return 0;
   } else if ( message == WM_KILLFOCUS ) {
      DestroyWindow();
   }
	
	return CEdit::DefWindowProc(message, wParam, lParam);
}


