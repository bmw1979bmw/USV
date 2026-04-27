// GraphDispEx.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GraphDispEx.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDispEx

CGraphDispEx::CGraphDispEx()
{
}

CGraphDispEx::~CGraphDispEx()
{
}

void CGraphDispEx::AfterDraw(CDC & dc, int width, int height)
{
   DrawTicks(dc,width,height,this);
}

void CGraphDispEx::OnDraw(CDC & dc)
{
   if ( !pZ ) return;

   CRect rc;
   GetClientRect(rc);
   int cx = rc.right;
   int cy = rc.bottom;

   if ( cx < 150 || cy < 50 ) return;
  
   int nDCSav = dc.SaveDC();
   
   dc.SelectObject(&m_fnt);
   dc.SetTextColor(0x0000FFFF);
   dc.SetBkMode(TRANSPARENT);

   CBrush fillbr(0x00C80000);
   CPen   yellowpn(0,0,0x0000FFFF);
   dc.SelectObject(&fillbr);
   dc.SelectObject(&yellowpn);

   
   if ( m_dwStyle & GDS_COLORMAP ) {
      if ( m_dwStyle & GDS_BAR ) { 
         DrawColorMapBar(dc,cx,cy);
      } else {
         DrawColorMap(dc,cx,cy);
      } 
      AfterDraw(dc,cx,cy);
   } else if ( m_dwStyle & GDS_VECTORS ) {
      if ( m_dwStyle & GDS_DIAGRAM ) {
         DrawVectorsTest(dc,cx,cy);
      } else {
         DrawVectors(dc,cx,cy);
      }
      AfterDraw(dc,cx,cy );
   }
   
   dc.RestoreDC(nDCSav);
}

BEGIN_MESSAGE_MAP(CGraphDispEx, CGraphDisp)
	//{{AFX_MSG_MAP(CGraphDispEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDispEx message handlers

void CGraphDispEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if ( nFlags  == (MK_LBUTTON|MK_SHIFT) ) { // VERIFYING
      CGraphDisp::OnLButtonDblClk(MK_LBUTTON, point);
      return ;
   }
   StdMouseNotifyParent(m_hWnd,NM_DBLCLK,point.x,point.y);
}

void CGraphDispEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
   StdMouseNotifyParent(m_hWnd,NM_CLICK,point.x,point.y);
}

