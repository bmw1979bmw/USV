// InfoPopupWnd.cpp : implementation file
//

#include "stdafx.h"
#include "InfoPopupWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoPopupWnd

CInfoPopupWnd::CInfoPopupWnd(CWnd *pParent, CPoint pt, LPCTSTR szMessage, CInfoPopupWnd **ppBackPtr)
{
   m_ppBackPtr = ppBackPtr;
   pParent->ClientToScreen(&pt);
   CreateEx(0   ,
      AfxRegisterWndClass(
         CS_HREDRAW | CS_VREDRAW,
         ::LoadCursor(NULL,IDC_ARROW),
         ::GetSysColorBrush(COLOR_INFOBK)),
      szMessage,
      WS_BORDER|WS_POPUP,
      pt.x,pt.y,0,0,
      pParent->m_hWnd,
      0);

   UpdateText();
}

void CInfoPopupWnd::UpdateText()
{
   CString szMessage;
   GetWindowText(szMessage);
   CClientDC dc(this);
   dc.SelectStockObject(DEFAULT_GUI_FONT);
   CRect rc(0,0,1280,10);
   dc.DrawText(szMessage,&rc,DT_CALCRECT|DT_NOPREFIX|DT_EXPANDTABS|DT_WORDBREAK);
   SetWindowPos(0,0,0,
      rc.right+6,rc.bottom+4+2,
      SWP_NOMOVE       |
      SWP_NOZORDER     |
      SWP_SHOWWINDOW   |
      SWP_NOOWNERZORDER);
}

void CInfoPopupWnd::Move(CPoint pt)
{
   SetWindowPos(0,pt.x,pt.y,
      0,0,
      SWP_NOSIZE       |
      SWP_NOZORDER     |
      SWP_SHOWWINDOW   |
      SWP_NOOWNERZORDER);
}

CInfoPopupWnd::~CInfoPopupWnd()
{
}

BEGIN_MESSAGE_MAP(CInfoPopupWnd, CWnd)
   //{{AFX_MSG_MAP(CInfoPopupWnd)
   ON_WM_KILLFOCUS()
   ON_MESSAGE(WM_SETTEXT,OnSetText)
   ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPopupWnd message handlers

void CInfoPopupWnd::OnKillFocus(CWnd* pNewWnd) 
{
   CWnd::OnKillFocus(pNewWnd);
   DestroyWindow();
}

void CInfoPopupWnd::PostNcDestroy() 
{
   CWnd::PostNcDestroy();
   if ( m_ppBackPtr ) *m_ppBackPtr = NULL;
   delete this;
}

void CInfoPopupWnd::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
   
   CString s;
   GetWindowText(s);

   dc.SelectStockObject(DEFAULT_GUI_FONT);
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
   CRect rc;
   GetClientRect(rc);
   rc.OffsetRect(2,2);
   
   dc.DrawText(s,&rc,DT_NOPREFIX|DT_EXPANDTABS|DT_WORDBREAK);
}

void CInfoPopupWnd::OnSetText(WPARAM wParam, LPARAM lParam)
{
   Default();
   UpdateText();
}

