// SpAnalizer.cpp : implementation file
//

#include "stdafx.h"
//#include "UDAR.h"
#include "SpAnalizer.h"
#include "math2.h"
#include "FuncSv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpAnalizer

CSpAnalizer::CSpAnalizer()
{
   WNDCLASS wc;
   ZeroMemory(&wc,  sizeof(WNDCLASS));
   wc.lpfnWndProc = ::DefWindowProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
   wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = (HBRUSH) CreateSolidBrush(0x00008200 );//);
   wc.lpszClassName = _T("SpAnalizer");
   wc.hIcon = NULL;
   
   VERIFY( AfxRegisterClass(&wc) );
   
   //   m_fnt.CreatePointFont( 80, _T("MS Sans Serif") );
   
   
   //ZeroMemory(P1, N_DF*sizeof(double));
   //ZeroMemory(P2, N_DF*sizeof(double));
   P_MAX=100;
   
}

CSpAnalizer::~CSpAnalizer()
{
}


BEGIN_MESSAGE_MAP(CSpAnalizer, CWnd)
//{{AFX_MSG_MAP(CSpAnalizer)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_LBUTTONDBLCLK()
ON_WM_RBUTTONDBLCLK()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpAnalizer message handlers

void CSpAnalizer::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
   
   CRect rc;
   GetClientRect(rc);
   
   //dc.SetWindowOrg(-rc.right/2,-rc.bottom/2);
   
   CBitmap bm,*obm;
   CDC DC;
   DC.CreateCompatibleDC(&dc);
   bm.CreateCompatibleBitmap(&dc,rc.right,rc.bottom);
   obm = DC.SelectObject(&bm);
   
   ///////////////////                                  TBS_ENABLESELRANGE
   DWORD dwStyle = GetStyle();
   
   // TODO: Add your message handler code here
   OnDraw(DC);
   // Do not call CSliderCtrl::OnPaint() for painting messages
   
   dc.BitBlt(0,0,rc.right,rc.bottom,&DC,0,0,SRCCOPY);
   DC.SelectObject(obm);
   
   
   // TODO: Add your message handler code here
   
   // Do not call CWnd::OnPaint() for painting messages
}

BOOL CSpAnalizer::OnEraseBkgnd(CDC* pDC) 
{
   // TODO: Add your message handler code here and/or call default
   
   return  TRUE/*CWnd::OnEraseBkgnd(pDC)*/;
}


void CSpAnalizer::OnDraw(CDC &dc)
{
   CRect rc;
   GetClientRect(rc);

   
   rc.DeflateRect(rc.right/10 ,rc.bottom/10);
   double M=double(rc.bottom)/P_MAX;
   int i;
   i=0;
   for ( i=0; i<10 ;i++ ) 
   {
      int R=i*45;
      if(R>=250) R=250;
      int G=i*25;
      DWORD br =(RGB(R,250-G,0));
      CRect rect(
         rc.left,
         rc.top,
         rc.right,
         rc.bottom-i*rc.bottom/10);
      dc.FillSolidRect(rect,br);
   }
   ////////////////////////////////////////////////
   CBrush br(RGB(0, 0, 0)),*oldbr;
   oldbr = dc.SelectObject(&br);

   CPoint point[260];
//   int N=(F_MAX-F_MIN)/DF;

   float de = float( rc.right - rc.left ) / float(N_DF);
   for ( i=0; i<N_DF; i++ ) 
   {
      int h = rc.bottom - int( P2[i] * M );
      if ( h<0 )  h=0;
      point[2*i].y   = h;
      point[2*i+1].y = h;
      point[2*i].x   = rc.left + int(float(i) * de);
      point[2*i+1].x = rc.left + int(float(i+1) * de);
   }


   point[2*N_DF].y   = point[2*N_DF+1].y = rc.top;
   point[2*N_DF].x   = rc.right;
   point[2*N_DF+1].x = rc.left;
   
   dc.Polygon(point,2*N_DF+2); 

   CPen pn(PS_SOLID,3,RGB(243, 60, 33)), *oldpn;
   oldpn = dc.SelectObject(&pn);


  dc.SelectStockObject(NULL_BRUSH);

   for ( i=0; i<N_DF;i++ ) 
   {
      int h = rc.bottom - int(P1[i]*M);
      if ( h < 0 )  h=0;
      point[2*i].y   = h;
      point[2*i+1].y = h;
      point[2*i].x   = rc.left + int(float(i) * de);
      point[2*i+1].x = rc.left + int(float(i+1) * de);
   }

   point[2*N_DF].y   = point[2*N_DF+1].y = rc.bottom;
   point[2*N_DF].x   = rc.right;
   point[2*N_DF+1].x = rc.left;
   
   dc.Polygon(point,2*N_DF+2); 
   
   CPen pn2(PS_SOLID,3,RGB(0, 0, 255));
   dc.SelectObject(&pn2);

   dc.SelectStockObject(NULL_BRUSH);

  for ( i=0; i<N_DF;i++ ) 
  {
      int h = rc.bottom - int(max_rev * M);
      if ( h<0 ) h = 0;

      point[2*i].y   = h;
      point[2*i+1].y = h;
      point[2*i].x   = rc.left + int(float(i) * de);
      point[2*i+1].x = rc.left + int(float(i+1) * de);
   }

   point[2*N_DF].y = point[2*N_DF+1].y = rc.bottom;
   point[2*N_DF].x = rc.right;
   point[2*N_DF+1].x = rc.left;
   
   dc.Polygon(point,2*N_DF+2); 

  /////////////////////////////////////////
   CPen penOrang(PS_SOLID, 1, RGB(250, 150, 50));
   dc.SelectObject(&penOrang);
   
   dc.MoveTo(rc.right,rc.bottom);
   dc.LineTo(rc.left-rc.left/2,rc.bottom);
   
   dc.MoveTo(rc.right,rc.top);
   dc.LineTo(rc.left-rc.left/2,rc.top);
   //////////////////////////////////////////////  
   
   CFont font,*oldfnt;
   font.CreatePointFont(50, _T("Arial"));
   oldfnt = dc.SelectObject(&font);
   dc.SetTextAlign(TA_CENTER);
   //   dc.SetTextAlign(TA_BASELINE);
   dc.SetBkMode(TRANSPARENT);
   CString s;
   dc.SetTextColor(0x0000D700);
   
   s.Format("%.7f",P_MAX);
   dc.TextOut(rc.left-rc.left/2,rc.top,s);
   
   dc.SetTextAlign(TA_BOTTOM);
   dc.TextOut(rc.left-rc.left/2,rc.bottom,"0");
   
   dc.SelectObject(oldbr);
   //dc.SelectObject(oldpn);
   //dc.SelectObject(oldfnt);
}


void CSpAnalizer::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // TODO: Add your message handler code here and/or call default
   
   //	CWnd::OnLButtonDblClk(nFlags, point);
   if(P_MAX>0.0000001) P_MAX/=10;
   InvalidateRect(0);
}

void CSpAnalizer::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   // TODO: Add your message handler code here and/or call default
   if(P_MAX<10000) P_MAX*=10;
   InvalidateRect(0);
   
   //	CWnd::OnRButtonDblClk(nFlags, point);
}
