// TacticBase.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TacticBase.h"
#include "CheckDC.h"
#include "RegsKey.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTacticBase

IMPLEMENT_DYNAMIC(CTacticBase, CScrollView)

BEGIN_MESSAGE_MAP(CTacticBase, CScrollView)
//{{AFX_MSG_MAP(CTacticBase)
ON_WM_LBUTTONDOWN()
ON_WM_MBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
ON_COMMAND(ID_NOBLINK, OnNoBlink)
ON_UPDATE_COMMAND_UI(ID_NOBLINK, OnUpdateNoBlink)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_KILLFOCUS()
ON_WM_CHAR()
ON_WM_RBUTTONDOWN()
ON_COMMAND(ID_PROPERTIES, OnProperties)
ON_COMMAND(ID_SHOWGRID, OnShowGrid)
ON_UPDATE_COMMAND_UI(ID_SHOWGRID, OnUpdateShowGrid)
ON_WM_KEYDOWN()
ON_COMMAND(ID_SHOWNOTE, OnShowNote)
ON_UPDATE_COMMAND_UI(ID_SHOWNOTE, OnUpdateShowNote)
ON_COMMAND(ID_BKCOLOR, OnBkcolor)
ON_COMMAND(ID_SHORTNOTE, OnShortnote)
ON_UPDATE_COMMAND_UI(ID_SHORTNOTE, OnUpdateShortnote)
ON_COMMAND(ID_FONT, OnFont)
ON_WM_MOUSEWHEEL()
//}}AFX_MSG_MAP

#ifdef ID_ROTATE_ICONS
ON_COMMAND(ID_ROTATE_ICONS, OnRotateIcons)
ON_UPDATE_COMMAND_UI(ID_ROTATE_ICONS, OnUpdateRotateIcons)
#endif

#ifdef ID_ZOOMIN
ON_COMMAND(ID_ZOOMIN, OnZoomIn)
#endif

#ifdef ID_ZOOMOUT
ON_COMMAND(ID_ZOOMOUT, OnZoomOut)
#endif

#ifdef ID_AUTOSIZE
ON_COMMAND(ID_AUTOSIZE, OnAutoSize)
ON_UPDATE_COMMAND_UI(ID_AUTOSIZE, OnUpdateAutoSize)
#endif

#ifdef ID_KEEP_LIMITS
ON_COMMAND(ID_KEEP_LIMITS, OnKeepLimits)
ON_UPDATE_COMMAND_UI(ID_KEEP_LIMITS, OnUpdateKeepLimits)
#endif

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

CTacticBase::CTacticBase()
{
   m_bWasMouseMove = FALSE;
   CAppKey key("TacticView");
   m_dwFlags = key["Flags"][FLAG_DRAWGRID|FLAG_DRAWNOTE];

   m_bCaptured = FALSE;
   m_bFontChanged = FALSE;
   BKBRUSH = key["BKBRUSH"][DEF_BKBRUSH];

   m_robj.SetRectEmpty();
   m_rsize.SetRectEmpty();

#ifdef DEMO
   m_dwFlags |= FLAG_NOBLINK;
#endif

   TEXTCLR = key["TEXTCLR"];
   InitColors();
}

void CTacticBase::InitColors()
{
   LTBKBRUSH=TryColorAdjustLuma(BKBRUSH, 80, FALSE);  
   DKBKBRUSH=TryColorAdjustLuma(BKBRUSH, -125, FALSE);
   DKCOLOR  =TryColorAdjustLuma(BKBRUSH, -335, FALSE);
}

CTacticBase::~CTacticBase()
{
   CAppKey key("TacticView");
   key["Flags"] = m_dwFlags;

   if ( BKBRUSH != DEF_BKBRUSH ) key["BKBRUSH"] = BKBRUSH;    
   else key["BKBRUSH"].Delete();

   if ( TEXTCLR ) key["TEXTCLR"] = TEXTCLR;
   else key["TEXTCLR"].Delete();

   if ( m_bFontChanged ) {
      key["Font"].Write(&m_LogFont, sizeof(m_LogFont));
   }
}


/////////////////////////////////////////////////////////////////////////////
// CTacticBase drawing
void CTacticBase::CalcCarrierRect(CRect &rc, POINT &carPt)
{
   rc.SetRect(carPt.x-1,
              carPt.y-1,
              carPt.x+ICONCX+2, 
              carPt.y+ICONCY+2);
}

void CTacticBase::OnInitialUpdate()
{
   m_dwFlags |= FLAG_AUTOSIZE;
   UpdateAll();
   if ( IsWindowVisible() ) Invalidate();
}

void CTacticBase::OnDraw(CDC* pDC)
{
   CFont *o = pDC->SelectObject(&m_fnt);

   pDC->SetBkMode(TRANSPARENT);
   pDC->SetTextColor(TEXTCLR);

   m_LtBkPen.DeleteObject();
   m_LtBkPen.CreatePen(0, 0, LTBKBRUSH);

   if ( m_dwFlags & FLAG_DRAWGRID ) DrawGrid(pDC);

   CPen redpen(0, 0, 0xc0);

   CPen *opn = pDC->SelectObject(&redpen);

   DrawAllObjects(pDC);

   pDC->SetViewportOrg(0, 0);

   pDC->SelectStockObject(WHITE_BRUSH);
   pDC->SelectObject(opn);
   pDC->SelectObject(o);
}

/////////////////////////////////////////////////////////////////////////////
// CTacticBase message handlers

BOOL CTacticBase::PreCreateWindow(CREATESTRUCT& cs) 
{
   WNDCLASS wc;
   memset(&wc, 0, sizeof(WNDCLASS));
   wc.lpfnWndProc = ::DefWindowProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL, IDC_ARROW); 
   wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = CreateSolidBrush( BKBRUSH );
   wc.lpszClassName = _T("TacticView");
   if ( !AfxRegisterClass(&wc) ) return FALSE;
   cs.lpszClass = wc.lpszClassName;
   return CScrollView::PreCreateWindow(cs);
}

void CTacticBase::DPtoRP(LPPOINT lpPoint, LPPOINT lpScroll, int nCount)
{
   do {
      lpPoint->x -= m_objorg.cx;
      lpPoint->y -= m_objorg.cy;

      if ( lpScroll != NULL ) {     
         lpPoint->x += lpScroll->x;
         lpPoint->y += lpScroll->y;
      }

      lpPoint->y = m_rsize.bottom-(LONG)(lpPoint->y*m_scale);
      lpPoint->x = m_rsize.left + (LONG)(lpPoint->x*m_scale);

      ++lpPoint;
   } while ( --nCount );
}

void CTacticBase::RPtoDP(LPPOINT lpPoint, LPPOINT lpScroll, int nCount)
{
   // если вызывать RPtoDP(pt, NULL), то результат используется для
   // CDC, подготовленного с OnPrepareDC
   // а если вызвать RPtoDP(pt, &GetDeviceScrollPosition()), то 
   // результат можно использовать для сравнения с координатами мышки, и т.д.

   do {
      lpPoint->x = (LONG)((lpPoint->x-m_rsize.left)/m_scale);

      lpPoint->y = (LONG)((m_rsize.bottom-lpPoint->y)/m_scale);
         
      if ( lpScroll != NULL ) {     
         lpPoint->x -= lpScroll->x;
         lpPoint->y -= lpScroll->y;
      }

      lpPoint->x += m_objorg.cx;
      lpPoint->y += m_objorg.cy;

      ++lpPoint;
   } while ( --nCount );
}

void CTacticBase::MakeScaleToFit()
{
   //  make scale to fit m_rsize
   CRect rc;
   GetClientRect(&rc);
   
   rc.right  -= m_sizenote.cx;
   rc.bottom -= m_sizenote.cy;
   
   CSize sc = rc.Size();
   CSize sr = m_rsize.Size();
   
   float  k  = (float)sr.cx / sc.cx,
      ky = (float)sr.cy / sc.cy;
   if ( ky > k ) k = ky;
   m_scale = k;
}

void CTacticBase::UpdateScrollSizes()
{
   m_rsize.right  += (LONG)(m_sizenote.cx*m_scale);
   m_rsize.bottom += (LONG)(m_sizenote.cy*m_scale);

   CSize sz = m_rsize.Size();

   sz.cx = (LONG)(sz.cx/m_scale);
   sz.cy = (LONG)(sz.cy/m_scale);

   if ( sz.cx < 1 ) sz.cx = 1;
   if ( sz.cy < 1 ) sz.cy = 1;

   m_objorg.cx = 25;
   m_objorg.cy = -m_sizenote.cy*2/3;
   
   SetScrollSizes(MM_TEXT, sz);
}

void CTacticBase::UpdateAll()
{
   if ( m_dwFlags & FLAG_KEEP_LIMITS ) {
      m_bUpdateRObjFirst = FALSE;
   } else {
      m_bUpdateRObjFirst = TRUE;
      m_robj.SetRectEmpty();
   }

   TXParams *mm = UpdateRSize();

   m_rsize = m_robj;

   // округляем до километра
   Round1000rsize();

   if ( m_dwFlags & FLAG_AUTOSIZE ) MakeScaleToFit();

   UpdateScrollSizes();

   // делаем селектированный видимым (можно не надо)
   if ( mm ) {
      MakeRPVisible(*mm);
   }
}

void CTacticBase::OnePointSize(const CPoint &pt)
{  // служебная - рассчитать одну точку в rect m_robj

   if ( m_bUpdateRObjFirst ) {
      m_bUpdateRObjFirst = FALSE;
      m_robj.left   = 
      m_robj.right  = pt.x;
      m_robj.top    =
      m_robj.bottom = pt.y;
      return;
   }

   if ( pt.x < m_robj.left  )  m_robj.left  = pt.x;
   if ( pt.x > m_robj.right )  m_robj.right = pt.x;
   if ( pt.y < m_robj.top    ) m_robj.top   = pt.y;
   if ( pt.y > m_robj.bottom ) m_robj.bottom= pt.y;
}


void CTacticBase::DrawGrid(CDC *pDC)
{
   if ( m_scale < 1 || m_scale > 125 ) return;

   CPen pn(0, 0, DKBKBRUSH), pn0(0, 0, DKCOLOR);

   CRect rc;
   GetClientRect(rc);
   CPoint vp = GetDeviceScrollPosition();
   rc.left += vp.x;
   rc.right += vp.x;
   rc.top += vp.y;
   rc.bottom += vp.y;

   CPoint p;
   CPoint pt;
   TCHAR buf[32];

   int cy = pDC->GetTextExtent(_T("9"), 1).cy;

   pDC->SetTextColor(DKCOLOR);
   
   p = m_rsize.TopLeft();
   p.x /= 1000; 
   p.y /= 1000; 

   pt = m_rsize.BottomRight();
   pt.x /= 1000;
   pt.y /= 1000;

#define _OLD_DRAWGRID   
#ifdef _OLD_DRAWGRID   
   POINT qt;
   qt.x = pt.x*1000;
   qt.y = pt.y*1000;
   RPtoDP(&qt);
#endif

   POINT q;

   DPtoRP((LPPOINT)(LPRECT)rc, NULL, 2);

   rc.left /= 1000;
   rc.right /= 1000;
   rc.top /= 1000;
   rc.bottom /= 1000;

#ifndef _OLD_DRAWGRID
   // при скролле исчезают верхние части сетки!
   POINT qt;
   qt.x = min(pt.x, rc.right)*1000;
   qt.y = min(pt.y, rc.top)*1000;
   RPtoDP(&qt);
#endif

   int i;
   for ( i = max(p.x, rc.left); i <= min(pt.x, rc.right); i++ ) {
      q.x = i*1000;
      q.y = p.y*1000;
      RPtoDP(&q);

      if ( i == 0 )  pDC->SelectObject(&pn0);
      else           pDC->SelectObject(&pn);
      pDC->MoveTo(q.x, q.y);
      pDC->LineTo(q.x, qt.y);

      ++q.x;
      ++q.y;
      pDC->SelectObject(&m_LtBkPen);

      pDC->MoveTo(q.x, q.y);
      pDC->LineTo(q.x, qt.y);

      wsprintf(buf, _T("%d"), i);

      pDC->TextOut(q.x, q.y, buf);
   }

   pDC->SetTextAlign(TA_RIGHT);
   for ( i = max(p.y, rc.bottom); i <= min(pt.y, rc.top); i++ ) {
      q.x = p.x*1000;
      q.y = i*1000;
      RPtoDP(&q);

      if ( i == 0 ) pDC->SelectObject(&pn0);
      else          pDC->SelectObject(&pn);
      pDC->MoveTo(q.x, q.y);
      pDC->LineTo(qt.x, q.y);

      ++q.x;
      ++q.y;
      pDC->SelectObject(&m_LtBkPen);

      pDC->MoveTo(q.x, q.y);
      pDC->LineTo(qt.x, q.y);

      wsprintf(buf, _T("%d"), i);
      pDC->TextOut(q.x-5, q.y-cy/2, buf);
   }

   pDC->SetTextAlign(TA_LEFT);
   pDC->SetTextColor(0);
   pDC->SelectStockObject(BLACK_PEN);
}

void  CTacticBase::DrawCourse(CDC *pDC, CPoint p, TMParams *m, BOOL bAndDepth /*=TRUE*/ )
{
   if ( bAndDepth ) {
      if ( m->Z > 0 ) {
         CPen *opn;
         opn = pDC->SelectObject(&m_LtBkPen);

         // !pDC->MoveTo не всегда срабатывает
         ::MoveToEx(pDC->m_hDC, p.x, p.y+ICONCY, NULL);// pDC->MoveTo(p.x, p.y);
         int t;
         pDC->LineTo( p.x, t = p.y+ICONCY+int(m->Z/5) );
         ::MoveToEx(pDC->m_hDC, p.x-1, t, NULL);// pDC->MoveTo(p.x, p.y);
         pDC->LineTo( p.x+2, t);
         pDC->SelectObject(opn);
      }
   }

   float V = m->V*2;
   ::MoveToEx(pDC->m_hDC, p.x, p.y, NULL);// pDC->MoveTo(p.x, p.y);
   float K = GrRd*m->K; 
   CPoint p0(p.x+int(V*sin(K)), p.y-int(V*cos(K)));
   pDC->LineTo(p0);
   K -= GrRd*20;
   pDC->LineTo( p0.x-int(10.*sin(K)), p0.y+int(10.*cos(K)) );
   ::MoveToEx(pDC->m_hDC, p0.x, p0.y, NULL);// pDC->MoveTo(p0);
   K += GrRd*40;
   pDC->LineTo( p0.x-int(10.*sin(K)), p0.y+int(10.*cos(K)) );
}

void CTacticBase::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
   CPoint ptVpOrg = -GetDeviceScrollPosition();
   ::SetViewportOrgEx(pDC->m_hDC, ptVpOrg.x, ptVpOrg.y, NULL);
}

void CTacticBase::InitSizeNote()
{
   CClientDC dc(this);

   CFont *o = dc.SelectObject(&m_fnt);

   CSize sz(14, 13);
#ifdef _DEBUG
   if ( !
#endif
      GetTextExtentPoint32(dc.m_hDC, "Hg", 2, &sz) 
#ifdef _DEBUG      
      ) {
#endif
      ;
#ifdef _DEBUG
      FormatMessageBox(GetLastError(), "!GetTextExtent InitSizeNote", 0);
   }
#endif
   m_sizenote.textcy = sz.cy;
   m_sizenote.cx = ICONCX+(8*sz.cx) + 5;
   m_sizenote.cy = ICONCY+ICONCY+sz.cy;

   dc.SelectObject(o);
}


CPoint CTacticBase::GetDeviceScrollPosition() const
{
   CPoint pt(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
   ASSERT(pt.x >= 0 && pt.y >= 0);

   {
      CRect rect;
      GetClientRect(&rect);

      // if client area is larger than total device size,
      // the scroll positions are overridden to place origin such that
      // output is centered in the window
      // GetDeviceScrollPosition() must reflect this

      if (m_totalDev.cx < rect.right)
         pt.x = -((rect.right - m_totalDev.cx) / 2);
      if (m_totalDev.cy < rect.bottom)
         pt.y = -((rect.bottom - m_totalDev.cy) / 2);
   }

   return pt;
}

void CTacticBase::MakeRPVisible(CPoint pt)
{
   CRect rc;
   GetClientRect(rc);

   RPtoDP(&pt);
   CPoint t = -GetDeviceScrollPosition();
   t.x += pt.x;
   t.y += pt.y;
   if ( !rc.PtInRect(t) ) {
      pt.y += m_objorg.cy;
      ScrollToPosition(pt);
   }
}

void CTacticBase::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   if ( IsDontDraw() ) return;
   if ( !IsWindowVisible() ) return;

   UpdateAll();
   Invalidate();
   UpdateWindow();
}

void CTacticBase::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CScrollView::OnLButtonDown(nFlags, point);
   SetCapture();
   m_bWasMouseMove = FALSE;
   m_bCaptured = TRUE;
   m_vptCaptured = point;
}

void CTacticBase::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if ( m_bCaptured ) {
      m_bCaptured = FALSE;
      ReleaseCapture();
   }
   CScrollView::OnLButtonUp(nFlags, point);
}

void CTacticBase::OnMouseMove(UINT nFlags, CPoint point) 
{
   if ( m_bCaptured ) {
      if ( ::GetCapture() != m_hWnd ) {
         m_bCaptured = FALSE;
         return;
      }

      CPoint p(m_vptCaptured.x - point.x, m_vptCaptured.y - point.y);

      SCROLLBARINFO sbi;
      sbi.cbSize = sizeof sbi;
      if ( p.x ) {
         GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbi);
         if ( sbi.rgstate[0] == STATE_SYSTEM_INVISIBLE ) p.x = 0;
      }
      if ( p.y ) {
         GetScrollBarInfo(m_hWnd, OBJID_VSCROLL, &sbi);
         if ( sbi.rgstate[0] == STATE_SYSTEM_INVISIBLE ) p.y = 0;
      }

      if ( !p.x && !p.y ) return ;

      CPoint sp(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
      p += sp;

      m_bWasMouseMove = TRUE;

      ScrollToPosition(p);
      m_vptCaptured = point;
      CPoint sp1(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
      if ( sp != sp1 ) {
         Invalidate();
      }
   }
}

void CTacticBase::OnMButtonDown(UINT nFlags, CPoint point) 
{
   if ( m_bCaptured ) {
      m_bCaptured = FALSE;
      ReleaseCapture();
   }
   CScrollView::OnMButtonDown(nFlags, point);
   m_dwFlags |= FLAG_AUTOSIZE;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if ( IsOnSpace(point) ) return;
   OnProperties();
}

void CTacticBase::OnAutoSize() 
{
   m_dwFlags ^= FLAG_AUTOSIZE;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateAutoSize(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_AUTOSIZE));
}

void CTacticBase::OnKeepLimits() 
{
   m_dwFlags ^= FLAG_KEEP_LIMITS;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateKeepLimits(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_KEEP_LIMITS));
}

void CTacticBase::OnNoBlink() 
{
   m_dwFlags ^= FLAG_NOBLINK;
   CreateDevBitmap();
}

void CTacticBase::OnUpdateNoBlink(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_NOBLINK));
}

BOOL CTacticBase::OnEraseBkgnd(CDC* pDC) 
{
   if ((m_dwFlags & FLAG_NOBLINK) && !IsDontDraw() ) return TRUE;
   return CScrollView::OnEraseBkgnd(pDC);
}

void CTacticBase::OnPaint() 
{
   if ( !IsDontDraw() ) GdiFlush();

   CPaintDC dc(this); // device context for painting

   if ( IsDontDraw() ) return;
   
   int sdc =   dc.SaveDC();

   if ( m_dwFlags & FLAG_NOBLINK ) {
      CRect rc;
      GetClientRect(rc);
      CBitmap bm, *obm;
      CDC DC;
      DC.CreateCompatibleDC(&dc);
      
      if ( DC.m_hDC == 0 ) return;

      obm = DC.SelectObject(&m_bmp);
      // Fill
      CBrush br(BKBRUSH);
      DC.FillRect(rc, &br);
      ///////////////////

      OnPrepareDC(&DC);
      OnDraw(&DC);
      DC.SetViewportOrg(0, 0);
      ///////////////////
      dc.BitBlt(0, 0, rc.right, rc.bottom, &DC, 0, 0, SRCCOPY);
      DC.SelectObject(obm);
   } else {
      // standard paint routine
      OnPrepareDC(&dc);
      OnDraw(&dc);
   }
   dc.RestoreDC(sdc);         
}

int CTacticBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CScrollView::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   CAppKey key(_T("TacticView"));
   if ( !key["Font"].Read(&m_LogFont, sizeof(m_LogFont)) ||
      !m_fnt.CreateFontIndirect(&m_LogFont)
   )
   {
      m_fnt.CreatePointFont(80, _T("MS Sans Serif"));
      m_fnt.GetLogFont(&m_LogFont);
   }
   InitSizeNote();

   CreateDevBitmap();
   
   return 0;
}

BOOL CTacticBase::IsOnSpace(CPoint & pt)// const
{
   CDC *dc = CWnd::GetDC();
   COLORREF c  = dc->GetPixel(pt);

   ReleaseDC(dc);

   return c == BKBRUSH || c == LTBKBRUSH || c == DKBKBRUSH;
}

void CTacticBase::CreateDevBitmap()
{
   if ( (m_dwFlags & FLAG_NOBLINK) && !m_bmp.m_hObject ) {
      CWnd *w = GetDesktopWindow();
      CRect rc;
      w->GetClientRect(rc);
      CClientDC dc(w);
      m_bmp.CreateCompatibleBitmap(&dc, rc.right, rc.bottom);
   }
}

void CTacticBase::OnKillFocus(CWnd* pNewWnd) 
{
   CScrollView::OnKillFocus(pNewWnd);
   if ( m_bCaptured ) {
      ReleaseCapture();
      m_bCaptured = FALSE;
   }
}

void CTacticBase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if ( nChar == VK_RETURN ) OnProperties();
}

void CTacticBase::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if ( IsOnSpace(point) ) return ;

   ClientToScreen(&point);
   CMenu menu;
   menu.CreatePopupMenu();
   CMenu* pPopup = &menu;
   pPopup->AppendMenu(MF_STRING, ID_PROPERTIES, _T("Свойства"));
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
      this);
}

void CTacticBase::OnProperties() 
{
   EditSelObjectParams();
}

void CTacticBase::OnZoomIn() 
{
   m_dwFlags &= ~FLAG_AUTOSIZE;
   if ( m_scale > 0.01f ) m_scale *= 0.9f;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnZoomOut() 
{   // мельче
   m_dwFlags &= ~FLAG_AUTOSIZE;
   m_scale *= 1.111f;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnShowGrid() 
{
   m_dwFlags ^= FLAG_DRAWGRID;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateShowGrid(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_DRAWGRID));
}

void CTacticBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   BOOL bShift = GetKeyState(VK_SHIFT)&0x8000;

   switch ( nChar ) {
   case 'R':
      m_dwFlags ^= FLAG_SHOW_REDBLUE;
      OnUpdate(0, 0, 0);
      break;
   case VK_ADD:
      OnZoomIn();
      break;
   case VK_SUBTRACT:
      OnZoomOut();
      break;
   case VK_MULTIPLY:
      OnAutoSize();
      break;
   case VK_TAB:
      SelectNextObject(bShift);
      break;
   default:
      ExtraKeyDown( nChar, nRepCnt, nFlags);
   }
   CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTacticBase::OnShowNote() 
{
   m_dwFlags &= ~FLAG_SHORTNOTE;
   m_dwFlags ^= FLAG_DRAWNOTE;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateShowNote(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_DRAWNOTE));
}

void CTacticBase::ScrollToEx(CPoint pt)
{
   RPtoDP(&pt);
   pt.y += m_objorg.cy;
   ScrollToPosition(pt);
}

void CTacticBase::SetScrollSizes(int nMapMode, SIZE sizeTotal,
   const SIZE& sizePage, const SIZE& sizeLine)
{
   m_nMapMode = MM_TEXT;
   ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);

   m_totalLog = sizeTotal;

   m_totalDev = m_totalLog;
   m_pageDev = sizePage;
   m_lineDev = sizeLine;
   if (m_totalDev.cy < 0)
      m_totalDev.cy = -m_totalDev.cy;
   if (m_pageDev.cy < 0)
      m_pageDev.cy = -m_pageDev.cy;
   if (m_lineDev.cy < 0)
      m_lineDev.cy = -m_lineDev.cy;

   // now adjust device specific sizes
   ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
   if (m_pageDev.cx == 0)
      m_pageDev.cx = m_totalDev.cx / 10;
   if (m_pageDev.cy == 0)
      m_pageDev.cy = m_totalDev.cy / 10;
   if (m_lineDev.cx == 0)
      m_lineDev.cx = m_pageDev.cx / 10;
   if (m_lineDev.cy == 0)
      m_lineDev.cy = m_pageDev.cy / 10;

   if (m_hWnd != NULL) {
      // window has been created, invalidate now
      UpdateBars();
   }
}

void CTacticBase::OnBkcolor() 
{
   if ( PRESSED_SHIFT ) {
      BKBRUSH = DEF_BKBRUSH;
   } else {
      CColorDialog dlg(BKBRUSH, CC_ANYCOLOR|CC_FULLOPEN, this);
      if ( dlg.DoModal() != IDOK ) return;
      BKBRUSH = dlg.GetColor();
   }
   DeleteObject((HGDIOBJ)SetClassLong(m_hWnd, GCL_HBRBACKGROUND,
      (LONG)CreateSolidBrush(BKBRUSH)));
   
   InitColors();
   InvalidateRect(NULL);
}

void CTacticBase::OnShortnote() 
{
   m_dwFlags &= ~FLAG_DRAWNOTE;
   m_dwFlags ^= FLAG_SHORTNOTE;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateShortnote(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_SHORTNOTE));
}

void CTacticBase::Round1000rsize()
{
#define _OLD_ROUND
#ifdef _OLD_ROUND
   // округляем до километра
   CPoint &p = m_rsize.TopLeft();
   if ( p.x <= 0 ) p.x-=1000;  p.x = (p.x/1000)*1000; 
   if ( p.y <= 0 ) p.y-=1000;  p.y = (p.y/1000)*1000;
   
   CPoint &pt = m_rsize.BottomRight();
   if ( pt.x >= 0 ) pt.x+=1000; pt.x = (pt.x/1000)*1000;
   if ( pt.y >= 0 ) pt.y+=1000; pt.y = (pt.y/1000)*1000;
#else
   // округляем до километра
   CPoint &p = m_rsize.TopLeft();
   p.x = ((p.x-999)/1000)*1000; 
   p.y = ((p.y-999)/1000)*1000; 
   
   CPoint &pt = m_rsize.BottomRight();
   pt.x = ((pt.x+999)/1000)*1000; 
   pt.y = ((pt.y+999)/1000)*1000; 
#endif
}

void CTacticBase::OnFont() 
{
   if ( GetAsyncKeyState(VK_SHIFT) & 0x8000 ) {
      m_fnt.DeleteObject();
      m_fnt.CreatePointFont(60, _T("MS Sans Serif"));
      m_fnt.GetLogFont(&m_LogFont);
      m_bFontChanged = TRUE;
      InitSizeNote();
      TEXTCLR = 0;
      OnUpdate(0, 0, 0);
      return;
   }

   m_LogFont.lfCharSet = 204; // ин рашен плиз
   CFontDialog dlg(&m_LogFont, CF_BOTH|CF_EFFECTS);
   dlg.m_cf.rgbColors = TEXTCLR;
   
   if ( dlg.DoModal() == IDOK ) {
      m_fnt.DeleteObject();
      memcpy(&m_LogFont, &dlg.m_lf, sizeof(m_LogFont));
      if ( !m_fnt.CreateFontIndirect(&m_LogFont) ) {
         m_fnt.CreatePointFont(60, _T("MS Sans Serif"));
         m_fnt.GetLogFont(&m_LogFont);
      }
      TEXTCLR = dlg.m_cf.rgbColors;
      m_bFontChanged = TRUE;
      InitSizeNote();
      OnUpdate(0, 0, 0);
   }
}

BOOL CTacticBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
   if ( nFlags & (MK_CONTROL|MK_RBUTTON) ) {
      m_dwFlags &= ~FLAG_AUTOSIZE;
      if ( zDelta < 0 ) {
         OnZoomOut();
      } else {
         OnZoomIn();
      }
      ScrollToPosition(pt);
      OnUpdate(0, 0, 0);
      return 0;
   }
   return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


void CTacticBase::OnRotateIcons() 
{
   m_dwFlags ^= FLAG_ROTATE_ICONS;
   OnUpdate(0, 0, 0);
}

void CTacticBase::OnUpdateRotateIcons(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(!!(m_dwFlags & FLAG_ROTATE_ICONS));
}
