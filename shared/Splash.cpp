// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h"  // e. g. stdafx.h
#include "resource.h"  // e.g. resource.h

#include "Splash.h"  // e.g. splash.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class


CSplashWnd::CSplashWnd()
{
}

CSplashWnd::CSplashWnd(BOOL bEnable)
{
   Create(bEnable);
}

CSplashWnd::~CSplashWnd()
{
   DestroyWindow();
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CSplashWnd::Create(BOOL bEnable)
{
   if ( !bEnable ) return TRUE;

	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	if( CreateEx(0,
   		AfxRegisterWndClass(0, 
            AfxGetApp()->LoadStandardCursor(IDC_ARROW),0,
            AfxGetApp()->LoadIcon(IDR_MAINFRAME)),
		NULL, WS_POPUP|WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, NULL, NULL)
      ) {
     	CenterWindow();
      ShowWindow(SW_SHOW);
      UpdateWindow();
      return TRUE;
   } else {
      return FALSE;
   }
}

void CSplashWnd::Show()
{
   if ( m_hWnd ) ShowWindow(SW_SHOW);
}

void CSplashWnd::Hide()
{
   if ( m_hWnd ) ShowWindow(SW_HIDE);
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);
   

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);

   if ( m_szMessage ) {
      dc.SetBkMode(TRANSPARENT);
      CFont *ofnt = (CFont *)dc.SelectStockObject(DEFAULT_GUI_FONT);
      
      dc.DrawText(m_szMessage,CRect(bm.bmWidth/10,bm.bmHeight*2/4,bm.bmWidth/2,bm.bmHeight*3/4),DT_LEFT);
         
      dc.SelectObject(ofnt);
   }
}


void CSplashWnd::Message(LPCTSTR szMessage)
{
   if ( m_hWnd ) {
      m_szMessage = szMessage;
      InvalidateRect(NULL);
      UpdateWindow();
   }
}
