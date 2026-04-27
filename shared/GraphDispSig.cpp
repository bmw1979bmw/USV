// GraphDispSig.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDispSig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDispSig

CGraphDispSig::CGraphDispSig()
{
}

CGraphDispSig::~CGraphDispSig()
{
}

BEGIN_MESSAGE_MAP(CGraphDispSig, CGraphDisp)
	//{{AFX_MSG_MAP(CGraphDispSig)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphDispSig message handlers

void CGraphDispSig::OnDraw(CDC & dc)
{
   if ( !pZ ) return;
   if ( nX <= 1 ) return;

   CRect rc;
   GetClientRect(rc);
   int cx = rc.right;
   int cy = rc.bottom;
   int height = rc.bottom/3;

   if ( cx < 150 || cy < 50 ) return;

   
   int nDCSav = dc.SaveDC();
   
   dc.SetTextColor(0x0000FFFF);
   dc.SetBkMode(TRANSPARENT);

   CBrush fillbr(0x00C80000);
   CPen   yellowpn(0,0,0x0000FFFF);
   CPen   axepn(0,0,0x00C0C0C0);
   dc.SelectObject(&fillbr);
   dc.SelectObject(&axepn);

   //AXES
   int t = height - int(RangeXmm(0)*height);
   dc.MoveTo(0,t);  dc.LineTo(cx,t);
   t += height;
   dc.MoveTo(0,t);  dc.LineTo(cx,t);

   dc.SelectObject(&yellowpn);
   
   float dx=(float)rc.right/(nX-1);
   
   Complex *p = (Complex *)pZ;
   
   int ix;
   for( ix=0;ix<nX;ix++,p++) {
      
      int x = int((float)ix*dx);
      int y = height - int(RangeXmm(p->re)*height);
      
      if ( ix ) {
         dc.LineTo(x,y);
      } else {
         dc.MoveTo(x,y);
      }
   } 
   p = (Complex *)pZ;
   
   for( ix=0;ix<nX;ix++,p++) {
      
      int x = int((float)ix*dx);
      int y = height+height - int(RangeXmm(p->im)*height);
      
      if ( ix ) {
         dc.LineTo(x,y);
      } else {
         dc.MoveTo(x,y);
      }
   } 

   // now summ
   p = (Complex *)pZ;
   
   for( ix=0;ix<nX;ix++,p++) {
                                    
      int x = int((float)ix*dx);
      int y = rc.bottom - int(RangeZmm(p->abs())*height);
      
      if ( ix ) {
         dc.LineTo(x,y);
      } else {
         dc.MoveTo(x,y);
      }
   } 

   dc.RestoreDC(nDCSav);
}

BOOL CGraphDispSig::SetData(int nLen, Complex * pdata, BOOL bRedraw)
{
   if ( !nLen ) return FALSE;

   nX = nLen;
   pZ = (float *)pdata;

   Xmin = FLT_MAX;
   Xmax =-FLT_MAX;
   Zmin = 0;
   Zmax =-FLT_MAX;

   Complex *p = pdata;

   for ( int i=0; i<nX ;i++ ) {
      if ( p->re < Xmin ) Xmin = p->re;
      if ( p->re > Xmax ) Xmax = p->re;
      if ( p->im < Xmin ) Xmin = p->im; // re & im - same scale
      if ( p->im > Xmax ) Xmax = p->im;
      float t = p->abs();
      if ( t > Zmax ) Zmax = t;

      p++;
   }   

   if ( qfabs(Xmin) > Xmax ) {
      Xmax = -Xmin;
   } else {
      Xmin = -Xmax;
   }

   if ( fabs(Xmax-Xmin) <= fabs(Xmax/1e+5f) ) {
      Xmin = -1;
      Xmax = 1;
   }

   if ( !Zmax ) {
      Zmax = 1;
   }

   if ( bRedraw )  Redraw();
   
   return TRUE;
}

void CGraphDispSig::OnPaint() 
{
	CPaintDC dc(this);
   OnDraw(dc);	
}

void CGraphDispSig::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CWnd *pParent = GetParent();
   if ( pParent == NULL ) return;

   CRect rc;
   GetClientRect(rc);
   m_xclk = point.x*nX/rc.right;

   NMHDR hdr;
   hdr.hwndFrom = m_hWnd;
   hdr.idFrom = GetDlgCtrlID();
   hdr.code = NM_CLICK;

   pParent->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&hdr);
}

void CGraphDispSig::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   CWnd *pParent = GetParent();
   if ( pParent == NULL ) return;

   CRect rc;
   GetClientRect(rc);
   m_xclk = point.x*nX/rc.right;

   NMHDR hdr;
   hdr.hwndFrom = m_hWnd;
   hdr.idFrom = GetDlgCtrlID();
   hdr.code = NM_DBLCLK;

   pParent->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&hdr);
}

