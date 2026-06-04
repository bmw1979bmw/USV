// CGraphDispSV.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDispSV.h"
#include <float.h>
#include "math2.h"
#include "resource.h"
#include "InfoPopupWnd.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGraph2dDataSV::CGraph2dDataSV()
{
   nX = nY = 0;
   pX = pY = NULL;
   nYStep = nXStep = 0;
   Xmin = Xmax = Ymin = Ymax = 0;
}

CGraph2dDataSV::~CGraph2dDataSV()
{
}
 
float CGraph2dDataSV::LogDB(float val)
{
   if ( val <=0.0f ) {
      return 0.0f;//-FLT_MAX;
   }
   return (float)(20.0*log10(val/2.0e-5 /*+1.0e-8*/));
}

float CGraph2dDataSV::RangeXmm(float x)
{
    float f = (Xmax-Xmin);
    if ( f == 0 ) return 1;
    return (x-Xmin)/f;
}
 
float CGraph2dDataSV::RangeYmm(float y)
{
    float f = (Ymax-Ymin);
    if ( f == 0 ) return 1;
    return (y-Ymin)/f;
}

CGraph3dDataSV::CGraph3dDataSV()
{
   pZ = NULL;
   Zmin = Zmax = 0;
}

CGraph3dDataSV::~CGraph3dDataSV()
{
}

#ifdef _DEBUG
float CGraph3dDataSV::RangeZmm(float z)
{
   float f = (Zmax-Zmin);
   if ( f == 0 ) return 0;//.5f;
   f = (z-Zmin)/f;
   if ( f > (float)0x3FFFFFF ) f = (float)0x3FFFFFF;
   return f;
}
#endif

void  CGraph3dDataSV::SetMinMaxToShared( float mn, float mx )
{
   Zmin = mn;  //ms_Zmin;
   Zmax = mx;  //ms_Zmax;
}

BOOL CGraph3dDataSV::UpdateMinMax(BOOL bShared)
{
    return TRUE;
}

void CGraph3dDataSV::SetData(int nx, int ny, float *px, float *py, float *pz,
                            int xstep, int ystep,
                            UINT nUpdateMinMax,
                            BOOL bRedraw)
{
    nX = nx;
    nY = ny;
    pX = px;
    pY = py;
    pZ = pz;
    nYStep = ystep;
    nXStep = xstep;
 }

void CGraph3dDataSV::SetZmaxMul(float n, BOOL bRedraw)
{
   if ( Zmax >= FLT_MAX/n ) return;
   Zmax *= n;
   if ( bRedraw ) Redraw();
}

void CGraph3dDataSV::SetZmaxDiv(float n, BOOL bRedraw)
{
   if ( n == 0 ) return;
   Zmax /= n;
   if ( Zmax < Zmin ) Zmax = Zmin;
   if ( bRedraw ) Redraw();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGraphDispSV, CWnd)
   //{{AFX_MSG_MAP(CGraphDispSV)
   ON_WM_PAINT()
   ON_COMMAND(ID_POPUP_LOGDB, OnPopupLogdb)
   ON_COMMAND(ID_POPUP_X20, OnPopupX20)
   ON_COMMAND(ID_POPUP_BAR, OnPopupBar)
   ON_COMMAND(ID_POPUP_LONKMAP, OnPopupLonkMap)
   ON_WM_LBUTTONDBLCLK()
   ON_WM_LBUTTONDOWN()
   ON_WM_CLOSE()
   ON_COMMAND(ID_WHATSIT, OnWhatIsIt)
   ON_WM_ERASEBKGND()
   ON_COMMAND(ID_POPUP_TEST, OnPopupTest)
   ON_COMMAND(ID_POPUP_INFO, OnPopupInfo)
   ON_WM_CONTEXTMENU()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDispSV

CGraphDispSV::CGraphDispSV()
{
   WNDCLASS wc;
   ZeroMemory(&wc,  sizeof(WNDCLASS));
   wc.lpfnWndProc = ::DefWindowProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL,IDC_ARROW); 
   wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = (HBRUSH) CreateSolidBrush(0/*::GetSysColor(COLOR_WINDOW)*/ );//RGB(48,108,209));
   wc.lpszClassName = _T("CGraphDispSV");
   
   VERIFY( AfxRegisterClass(&wc) ); 
   
   //////////////////////////////////////////
   m_fnt.CreatePointFont( 80, _T("MS Sans Serif") );
   idsX = idsY = 0;
   pPalette = NULL;
   m_nScrollType = 0;
   m_hSavParent = NULL;
   m_nHelpId = 0;
   m_pInfo = NULL;

}

TGDPaletteSV CGraphDispSV::palettes[] = {
#include "CGraphDisp.pll"
   /*
   {{1},{ 0, 0, 0},{ 0, 0, 0},{0, 0, 0},{ 8, 0, 0},{ 8, 0, 0},{ 8, 0, 0}, "GrayMap"},
   {{1},{ 5, 0, 0},{ 0, 3, 0},{0, 0, 0},{ 8, 0, 0},{ 8, 0, 0},{ 8, 0, 0}, "BromPortrait"},
   {{1},{ 0, 0, 0},{ 4, 0, 0},{0, 0, 0},{ 8, 0, 0},{ 8, 0, 0},{ 8, 0, 0}, "Steel"},
   {{1},{ 0, 0, 0},{ 0, 0, 0},{4, 0, 0},{ 8, 0, 0},{ 8, 0, 0},{ 8, 0, 0}, "Iron"},
   {{1},{ 0, 0, 0},{ 0, 0, 0},{0, 0, 0},{ 0, 0, 0},{ 8, 0, 0},{ 0, 0, 0}, "Green"},
   {{1},{ 0, 0, 0},{ 0, 0, 0},{0, 0, 0},{ 8, 0, 0},{ 8, 0, 0},{ 0, 0, 0}, "Yellow"},
   {{1},{ 0, 0, 0},{ 0, 0, 0},{0, 0, 0},{ 8, 0, 0},{ 0, 0, 0},{ 0, 0, 0}, "Red"},
   {{1},{ 0, 0, 0},{ 0, 0, 0},{0, 0, 0},{ 0, 0, 0},{ 0, 0, 0},{ 8, 0, 0}, "Blue"},
   {{3},{ 0,20,20},{0, 20,50},{0,50,10},{ 8, 4, 0},{ 8, 4,20},{ 8, 20,0},"Cold June"},
   {{3},{ 0,20,30},{0, 10,30},{0, 30,0},{ 5, 0, 0},{ 5, 0, 7},{ 5, 7, 0}, "Julay"},
   {{2},{ 2,40, 0},{4, 20, 0},{3, 0, 0},{ 4, 3, 0},{ 4, 5, 0},{ 4, 0, 0}, "September"},
   {{3},{ 0,17,30},{0, 20,30},{0,17,25},{5, 1, 2},{ 5, 4, 3},{ 5, 3, 4}, "Helen's"},
   {{3},{ 0, 0,32},{0, 20,30},{0,20,30},{5, 1, 4},{ 5, 3, 4},{ 5, 4, 3}, "Helen's-2"},
   {{3},{ 0, 0,10},{0, 20,30},{0,20,10},{5, 1, 2},{ 5, 3, 5},{ 5, 4, 3}, "Helen's-3"},
   {{2},{ 0, 0, 0},{0, 25, 0},{0, 25,0}, {4, 0, 0},{ 4, 5, 0},{ 4, 4, 0}, "Japan Sea"},
   {{3},{ 2,15,57},{0, 17,30},{0, 25,6}, {6, 1, 1},{ 6, 3, 6},{ 5, 7, 0}, "Saryan"},
   {{2},{ 0,30, 0},{0, 10, 0},{0, 10,0}, {4, 3, 0},{ 4, 5, 0},{ 4, 0, 0}, "Indian Tea"},
   {{2},{ 0,15, 0},{0, 18, 0},{0, 0, 0}, {4, 6, 0},{ 4,  5,0},{ 4, 0, 0}, "a'Lemon"}
   */
};
#define NPALETTES (sizeof(CGraphDispSV::palettes)/sizeof(TGDPaletteSV))

BOOL CGraphDispSV::Init(UINT idsx, UINT idsy, DWORD dwStyle,LONG param1,LONG param2)
{
   idsX = idsx;
   idsY = idsy;
   m_dwStyle = dwStyle;

   if ( dwStyle & GDS_COLORMAP ) 
   {
      nPalette = param1;
      CreatePalette(nPalette);
      m_nHelpId = param2;
   } 
   else 
   {
      m_nHelpId = param1;
   }

   return TRUE;
}
/////////////////////////////////////////////////////////////////////
void CGraphDispSV::CreatePalette(int nPal,int nument /*=32*/, TGDPaletteSV *srcP /* =NULL*/)
{
   TGDPaletteSV *P;
   if ( srcP != NULL ) P = srcP;  // Ye Ye may use srcP only
   else                P = &palettes[nPal];

   delete pPalette;
   pPalette = 
      (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE)+(nument-1)*sizeof(PALETTEENTRY)];
                 
   pPalette -> palVersion = 0x300;
   pPalette -> palNumEntries = nument;

   int I0 = (int)(nument/P->N); 

   int i,j,k;
   for ( i=0,j=0,k=0; i<nument ;i++ ) 
   {
      pPalette->palPalEntry[i].peRed   = P->r[j]+P->dr[j]*k;
      pPalette->palPalEntry[i].peGreen = P->g[j]+P->dg[j]*k;
      pPalette->palPalEntry[i].peBlue  = P->b[j]+P->db[j]*k;
      pPalette->palPalEntry[i].peFlags = PC_RESERVED;
      if ( ++k > I0 ) 
      {
         if ( j+1 > P->N ) 
         {
                    
         } 
         else 
         {
            j++;
            k=0;
         }
      }  // if
   }
   if ( !srcP ) 
   {
      *(DWORD *)&(pPalette->palPalEntry[nument-1])=RGB(255,255,255);
   }
} 
///////////////////////////////////////////////////////////////////

CGraphDispSV::~CGraphDispSV()
{
   delete pPalette;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDispSV message handlers
void CGraphDispSV::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
   try {
   if ( m_dwStyle & GDS_TEST ) 
   {
      DrawColorTest(dc);
   } 
   else
   {
      if ( m_dwStyle & GDS_THRUBMP ) 
      {
         CRect rc;
         GetClientRect(rc);
         CBitmap bm,*obm;
         CDC DC;
         DC.CreateCompatibleDC(&dc);
         bm.CreateCompatibleBitmap(&dc,rc.right,rc.bottom); // mb *2
         obm = DC.SelectObject(&bm);
         // Fill
         DC.FillRect(rc,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
         ///////////////////
         OnDraw(DC);
         ///////////////////
         dc.BitBlt(0,0,rc.right,rc.bottom,&DC,0,0,SRCCOPY);
         DC.SelectObject(obm);
      } 
      else 
      {
         // standard paint routine
         OnDraw(dc);
      }
   }  // else
   }  //try
   catch (...) 
   {
      RESETFP();
      dc.TextOut(0,0,_T("#error#"));
   }
}

void CGraphDispSV::AfterDraw(CDC &dc, int width, int height)
{
   if ( m_dwStyle & GDS_AFTERDRAW ) 
   {
      DRAWITEMSTRUCT dis;
      dis.CtlType = 0;  
      dis.CtlID   = ::GetDlgCtrlID(m_hWnd);
      dis.itemID  = 0;
      dis.itemAction = ODA_DRAWENTIRE;
      dis.itemState = 0;
      dis.hwndItem = m_hWnd;
      dis.hDC = dc.m_hDC;
      dis.rcItem.left = 
         dis.rcItem.top = 0;
      dis.rcItem.bottom = height;
      dis.rcItem.right = width;
      dis.itemData = 0; 
      ::SendMessage(::GetParent(m_hWnd),WM_DRAWITEM,dis.CtlID,(LPARAM)&dis);
   }
}

int CGraphDispSV::DrawScales(CDC &dc)  // рисует надписи и формирует m_Rg
{  // return string height
   CRect rc;
   GetClientRect(rc);

   CString s, sx0, sx1, sy0, sy1;
   CSize   zx0, zx1, zy0, zy1;
   
   /* формируем надписи */
   sx0.Format(_T("%g"),Xmin);
   sx1.Format(_T("%g"),Xmax);
   sy0.Format(_T("%g"),Ymin);
   sy1.Format(_T("%g"),Ymax);

   s.LoadString(idsX);
   s = s.SpanExcluding(_T("\n"));

   //if ( m_dwStyle & (GDS_YX|GDS_NOTY) ) {
   if ( m_dwStyle & GDS_NOTX ) 
   {
      if ( m_dwStyle & GDS_NAMEFIRST ) 
      {
         sx0 = s + _T(' ') + sx0;
      } 
      else 
      {
         sx0 += _T(' ')/*;  sx0 +=*/+ s;
      }
   } 
   else 
   {
      if ( m_dwStyle & GDS_NAMEFIRST ) 
      {
         sx1 = s + _T(' ') + sx1;
      } 
      else 
      {
         sx1 += _T(' ');  sx1 += s;
      }
   }

   zx0 = dc.GetTextExtent(sx0);
   zx1 = dc.GetTextExtent(sx1);
   s.LoadString(idsY);
   s = s.SpanExcluding(_T("\n"));
   zy0 = dc.GetTextExtent(sy0);
   zy1 = dc.GetTextExtent(sy1);
   
   if ( m_dwStyle & GDS_DIAGRAM ) sx1 += _T(" (равномерный шаг!)");
   if ( m_dwStyle & GDS_YX ) 
   {
      m_Rg.top = zx1.cy;
      m_Rg.bottom = rc.bottom-zy0.cy;
      m_Rg.right = rc.right-8;
      
      //if ( m_dwStyle & GDS_BAR ) rg.right -= 3;
      if ( m_dwStyle & GDS_NOTX ) 
      {
         m_Rg.left = zx1.cx;
         dc.TextOut( 0, 0, sx0);
         dc.TextOut( 0, rc.bottom - zy0.cy - zx0.cy ,sx1);
      } 
      else 
      {
         m_Rg.left = zx0.cx;
         dc.TextOut( 0, 0, sx1);
         dc.TextOut( 0, rc.bottom - zy0.cy - zx0.cy ,sx0);
      }
      dc.TextOut( zx0.cx, rc.bottom-zy0.cy, sy0);
      dc.TextOut( rc.right-zy1.cx-1, rc.bottom-zy0.cy,sy1);
   } 
   else 
   {
      m_Rg.top = zy1.cy;
      m_Rg.left = zy0.cx;
      m_Rg.bottom = rc.bottom-zx0.cy;
      m_Rg.right = rc.right-8;
      if ( m_dwStyle & GDS_BAR ) m_Rg.right -= 3;
      dc.TextOut( 0, 0, sy1);
      dc.TextOut( 0, rc.bottom - zx0.cy - zy0.cy ,sy0);
      dc.TextOut( zy0.cx, rc.bottom-zx0.cy, sx0);
      dc.TextOut( rc.right-zx1.cx-1, rc.bottom-zx0.cy,sx1);
   }

   return zx0.cy;
}

void CGraphDispSV::OnDraw(CDC &dc) 
{
   if ( !pX || !pY || ((m_dwStyle & (GDS_COLORMAP|GDS_VECTORS)) && !pZ) ) 
   {
      return;
   }

   CRect rc;
   GetClientRect(rc); // SetClipRect
   int nDCSav = dc.SaveDC();
   dc.SelectObject(&m_fnt);
   dc.SetTextColor(0x0000FFFF);
   dc.SetBkMode(TRANSPARENT);
   
   int zcy;
   if ( m_dwStyle & GDS_PAL  ) 
   {
      GetClientRect(m_Rg);
   } 
   else 
   {
      zcy = DrawScales( dc );
   }
   
   ////////////////////////////////////////////////////////////////////   
   int cx, cy;
   {
      CSize zg = m_Rg.Size();
      cx = zg.cx-10;      /** was -1, -10 for letters at right side **/
      cy = zg.cy-1;
   }
   
   long  center = cy;
   
   if ( !(m_dwStyle & GDS_NOBELOWY) ) 
   {
      center -= (long)( cy*RangeYmm(0) );
   }    // +/- ????????????
   
   /****** рисуем оси **********************/
   CBrush fillbr(0x00C80000);
   CPen   yellowpn(0,0,0x0000FFFF);
   if( ! (m_dwStyle & GDS_NOAXES) ) 
   {
      CPen   orange(0,0,0x000093FF);
      dc.SelectObject(&orange);
      dc.MoveTo(m_Rg.left,m_Rg.top);
      dc.LineTo(m_Rg.left,m_Rg.bottom);
      if ( !(m_dwStyle & GDS_YX) ) 
      {
         dc.MoveTo(m_Rg.left,m_Rg.top+center+1);
         dc.LineTo(m_Rg.right,m_Rg.top+center+1);
      }  
   }
   dc.SelectObject(&fillbr);
   dc.SelectObject(&yellowpn);
   /****************************************/
   dc.SetViewportOrg(m_Rg.left+1,m_Rg.top+1);
   
   float *py = pY;
   float *px = pX;
   
   CPoint pt[4];
   if ( m_dwStyle & GDS_YX ) 
   {
      pt[0].x = 0; 
      pt[0].y = 0;
      pt[3] = pt[1] = pt[0]; 
   } 
   else 
   {
      pt[0].x = (long)(0+cx*RangeXmm(*px));
      pt[0].y = center;
      pt[3] = pt[1] = pt[0]; 
   }
   //============================================================
   if ( m_dwStyle & GDS_VECTORS ) {
      DrawVectors(dc,cx,cy);
   } else if ( m_dwStyle & GDS_RAYS ) {
      cx -= 15;
      DrawRays(dc,cx,cy);
   } else  if ( m_dwStyle & GDS_PAL ) {
      DrawPalette(dc,cx,cy);
   } else if ( m_dwStyle & GDS_COLORMAP ) {
      //dc.SetViewportOrg(rg.left+1,rg.top);
      if ( m_dwStyle & GDS_BAR ) { 
         DrawColorMapBar(dc,cx,cy);
      } else /*if ( m_dwStyle & GDS_LONKMAP )*/ { // old, quick
         DrawColorMap(dc,cx,cy);
      }/* else {
         DrawColorMapY(dc,cx,cy);
      }*/
   } else {
      //============================================================//
      for ( int i=0; i<nX ;i++ ) {
         if ( (m_dwStyle & GDS_NOZEROX) && *px == 0.0f) continue;
         float fx = *px;
         
         float fy = *py;
         if ( (m_dwStyle & GDS_NOZEROY) && fy == 0.0f) continue;
         
         if ( m_dwStyle & GDS_LOGDBY ) fy = LogDB(fy);
         //==============================================
         if ( m_dwStyle & GDS_DIAGRAM ) {
            if ( m_dwStyle & GDS_YX ) {
               int dy = cy/nX;
               CRect r;
               r.left = 0;
               r.top = (long)(dy * (nX-1) * RangeXmm(fx));
               r.right  = (long)(fy*cx/Ymax)+2;
               r.bottom = r.top+dy;//(cy/nX);
               dc.Rectangle(r);
            } else {
//               ASSERT(0);
            }
         } else if ( m_dwStyle & GDS_BAR ) {
            pt[1].y = 
               pt[2].y = cy 
               - (long)( cy * RangeYmm(fy) );
            pt[0].x = pt[1].x = (long)(cx*RangeXmm(fx));//(long)(fx*cx/Xmax);
            pt[2].x = pt[3].x = pt[0].x+3;
            dc.Polygon(pt,4);
            if ( m_dwStyle & GDS_ZNUM && pZ ) {
               TCHAR buf[16];
               wsprintf(buf,_T("%d"), *(UINT *)&pZ[i]);
               dc.TextOut( pt[2].x+1, pt[2].y-zcy/2, buf);
            }
            
         } else {
            if ( m_dwStyle & GDS_YX ) {
               pt[2].y = pt[3].y = (long)(cy*RangeXmm(fx));
               pt[2].x = (long)(cx * RangeYmm(fy));
               if ( m_dwStyle & GDS_NOTY ) {
                  CPoint pt2[4];
                  for ( int k=0; k<4 ;k++ ) {
                     pt2[k].x = pt[k].x;
                     pt2[k].y = cy-pt[k].y;
                  }
                  dc.Polygon(pt2,4);
               } else {
                  dc.Polygon(pt,4);
               }
               pt[0] = pt[3];
               pt[1] = pt[2];
               
            } else {
               pt[2].x = pt[3].x = (long)(cx*RangeXmm(fx));//(fx*cx/Xmax);
               pt[2].y = cy - (long)(cy * RangeYmm(fy) );
               dc.Polygon(pt,4);
               pt[0] = pt[3];
               pt[1] = pt[2];
            }
         }
         //==============================================
         DoStepY(py);// = (float *)( (LPBYTE)py + nYDataStep );
         DoStepX(px);// = (float *)( (LPBYTE)px + nXDataStep );
      }
   }
   AfterDraw(dc,cx,cy);
   dc.RestoreDC(nDCSav);
   // Do not call CWnd::OnPaint() for painting messages
}



void CGraphDispSV::DrawPalette(CDC & dc, int width, int height, int style)
{
   CPalette pal,*opal;
   CreatePalette(-1,nX,(TGDPaletteSV *)pX);

   pal.CreatePalette(pPalette);
   opal = dc.SelectPalette(&pal,0);
   dc.TextOut(0,0,_T("Palette"));

   for ( int i=0; i<nX ;i++ ) {
      CBrush brush(PALETTEINDEX(i))/*,*oldbrush*/;
      dc.FillRect(CRect(0,height*i/nX,width,height*(i+1)/nX),&brush);
   } 
   dc.SelectPalette(opal,0);
}

#pragma warning(disable:4244)
void CGraphDispSV::DrawColorMapBar(CDC & dc, int width, int height, int style)
{
   if( Zmax == Zmin ) return;
   ASSERT ( m_dwStyle & GDS_COLORMAP );
   ASSERT (pPalette);

   if ( !pZ ) return;
   //--------------------------
   CPalette pal,*opal;

   pal.CreatePalette(pPalette);
   opal = dc.SelectPalette(&pal,0);
   dc.RealizePalette();
   //---------------------------------
   int nColors=32; 
   
   float dx=(float)width/(nX);
   float dy=(float)height/(nY);  
   
   float *p = pZ;
   
   {
   for(int iy=0;iy<nY;iy++) { 
      for(int ix=0;ix<nX;ix++, p++) {
         typedef float FLOAT;
         FLOAT a=p[0];
         
         if ( m_dwStyle & GDS_LOGDBZ ) {
            a = LogDB(a);
         }
         
         int nColor = int(32*RangeZmm(a));
         
         if( nColor >=  nColors ) nColor=nColors-1; 
         if( nColor < 0 ) nColor=0;       
         
         CBrush brush(PALETTEINDEX(nColor));
         //CPen   pen(0,0,/*0x0000FFFF*/PALETTEINDEX(nColor));
         
         //dc.SelectObject(&brush); 
         //dc.SelectObject(&pen);
         
         FLOAT x = ix*dx;
         FLOAT y = iy*dy;
         
         if ( m_dwStyle & GDS_NOTY ) {
            y = height - dy - y;
         }                 
         dc.FillRect(CRect(x,y,x+dx,y+dy),&brush);
         //dc.FillSolidRect(x,y,x+dx,y+dy,PALETTEINDEX(nColor));
         //dc.Rectangle/*Ellipse*/(x,y,x+dx,y+dy);
         
         ////////////////////////////////////////////   
      } //for(ix
   }  //for(iy  
   }
   dc.SelectPalette(opal,0);
}
#pragma warning(default:4244)

void CGraphDispSV::DrawColorTest(CDC & dc)
{
   if( Zmax == Zmin ) return;
   CRect rc;
   GetClientRect(rc);
   int width = rc.right; 
   int height = rc.bottom;

   if ( width < 100 || height <50 ) return;

#if(1)

   width = (width+3)/4*4;

   CDC md;
   HBITMAP dib;
   CPalette pal/*,*opal*/;
   int i;

   BITMAPINFO *bmi = (BITMAPINFO *)new BYTE[
      sizeof(BITMAPINFO)+//-
      //4
      sizeof(RGBQUAD)*
      256//*sizeof(short)
      ];
   bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi->bmiHeader.biWidth = width;
   bmi->bmiHeader.biHeight = -height;
   bmi->bmiHeader.biPlanes  = 1;
   bmi->bmiHeader.biBitCount = 8;
   bmi->bmiHeader.biCompression = BI_RGB;
   bmi->bmiHeader.biSizeImage    =  0;//width * 4 * height;
   bmi->bmiHeader.biXPelsPerMeter = 0;
   bmi->bmiHeader.biYPelsPerMeter  = 0;
   bmi->bmiHeader.biClrUsed        =  0;
   bmi->bmiHeader.biClrImportant   =  0;
   //*(DWORD *)(bmi->bmiColors) = NULL;
  
   for ( i=0; i<32 ;i++ ) {
      /*
      ((short *)&(bmi->bmiColors))[i] = i;//PALETTEINDEX(i);
         */
      bmi->bmiColors[i].rgbBlue = pPalette->palPalEntry[i].peBlue;
      bmi->bmiColors[i].rgbRed = pPalette->palPalEntry[i].peRed;
      bmi->bmiColors[i].rgbGreen = pPalette->palPalEntry[i].peGreen;
   }
                                 
   BYTE /*COLORREF*/ *pbits/*,*p*/;

   dib = CreateDIBSection(0/*dc.m_hDC*/,
      bmi,
      DIB_RGB_COLORS,
      (void **)&pbits,
      0,0
      );
   VERIFY(dib);
   
   float dx=(float)width/(nX-1);
   float dy=(float)height/(nY-1);  
   
//    float fx = 0;
   {
      BYTE *pv = pbits;
      float ay=0;
      float ax=0;
      int iy=0;
      int ix=0;
      float *py,*px;
      int tx,ty;
      for ( int y=0; y<height ;y++,ty++ ) 
      {
         if ( y >= ay ) 
         {
            ay += dy;
            py = &pZ[iy*nX];
            iy++;
            ty=0;
         }
         ix=0;
         ax=0;
         for ( int x=0; x<width ;x++,tx++ ) 
         {
            if ( x >= ax ) 
            {
               ax += dx;
               px = &py[ix];
               ix++;
               tx=0;
            } 
            FLOAT a=px[0];
            FLOAT b=px[1];
            FLOAT c=px[nX];
#pragma warning(disable:4244)
            int ia = 31*RangeZmm(a);
            int ib = 31*RangeZmm(b);
            int ic = 31*RangeZmm(c);
            *pv = ia+ty*(ic-ia)/dy+tx*(ib-ia)/dx;
            pv++;
#pragma warning(default:4244)
         }
      }
   }
   //////////////////////////////////////////////////////////////////////

SetDIBitsToDevice( dc, 
0,0,width, 
height, 
0, 
0, 
0, 
height, 
pbits, 
bmi, 
DIB_RGB_COLORS
); 
 
GdiFlush();

   DeleteObject(dib);
   delete [] (BYTE *)bmi;
#endif
}


#pragma warning(disable:4244)
//////////////////////////////////////////////////////////////////
void CGraphDispSV::DrawVectors(CDC & dc, int width, int height, int style)
{

   if ( !pZ ) return;
   if ( Zmax == Zmin) return;
   if ( nX <= 1 ) return;

   float dx=(float)width/(nX-1);
   float dy=(float)(height-1)/(nY);  
   
      float *p = pZ;

      if (!( m_dwStyle & GDS_PAL )) {
         CPen pn(0,0,0x00006F6F), *opn;
         opn = dc.SelectObject(&pn);
         for(int iy=nY-1;iy>=0;iy--) { 
            dc.MoveTo(width,(nY-1-iy)*dy+dy);
            dc.LineTo(0,(nY-1-iy)*dy+dy);
         }
         dc.SelectObject(opn);
      }

      for(int iy=nY-1;iy>=0;iy--) { 

         dc.MoveTo(0,(nY-1-iy)*dy+dy);

         if ( m_dwStyle & GDS_BAR ) {
            dc.BeginPath();
         }
         
         for(int ix=0;ix<nX;ix++/*, p++*/) {
typedef float FLOAT;
            FLOAT a;
            if ( m_dwStyle & GDS_YX ) {
               a=p[ix*nY+iy];
            } else {
               a=p[iy*nX+ix];
            }

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDB(a);
            }

            float nColor = dy*RangeZmm(a);

            if ( !(m_dwStyle & GDS_BAR) ) {
               if ( nColor > dy ) nColor = dy; // обрезать верхние значения
            }
            
            int ny = iy;
            if ( m_dwStyle & GDS_NOTY ) {
               ny = nY-1-iy;
            }    
            
            FLOAT x = ix*dx;
            FLOAT y = ny*dy  +dy-nColor;

            //if ( ix ) {
               dc.LineTo(x,y);
            //} else {
            //   dc.MoveTo(x,y);
            //}
         ////////////////////////////////////////////   
         } //for(ix

         dc.LineTo((nX-1)*dx,(nY-1-iy)*dy+dy);

         if ( m_dwStyle & GDS_BAR ) {
            dc.EndPath();
            dc.StrokeAndFillPath();
         }
      }  //for(iy  
}
////////////////////////////////////////////////////////
void CGraphDispSV::DrawVectorsTest(CDC & dc, int width, int height, int style)
{

   if ( !pZ ) return;
   if ( Zmax == Zmin) return;
   if ( nX < 1 ) return;

   float dx=(float)width/(nX);
   float dy=(float)(height-1)/(nY);  
   
      float *p = pZ;
 
      for(int iy=nY-1;iy>=0;iy--) { 

         dc.MoveTo(0,(nY-1-iy)*dy+dy);

         if ( m_dwStyle & GDS_BAR ) {
            dc.BeginPath();
         }
         
         for(int ix=0;ix<nX;ix++/*, p++*/) {
typedef float FLOAT;
            FLOAT a;
            if ( m_dwStyle & GDS_YX ) {
               a=p[ix*nY+iy];
            } else {
               a=p[iy*nX+ix];
            }

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDB(a);
            }

            float nColor = dy*RangeZmm(a);

            if ( !(m_dwStyle & GDS_BAR) ) {
               if ( nColor > dy ) nColor = dy; // обрезать верхние значения
            }
            
            int ny = iy;
            if ( m_dwStyle & GDS_NOTY ) {
               ny = nY-1-iy;
            }    
            
            FLOAT x = ix*dx;
            FLOAT y = ny*dy  +dy-nColor;

            //if ( ix ) {
            dc.MoveTo(x,ny*dy+dy);
            dc.LineTo(x,y);
            dc.LineTo(x+dx,y);
            dc.LineTo(x+dx,ny*dy+dy);
            //} else {
            //   dc.MoveTo(x,y);
            //}
         ////////////////////////////////////////////   
         } //for(ix


         if ( m_dwStyle & GDS_BAR ) {
            dc.LineTo((nX-1)*dx,(nY-1-iy)*dy+dy);
            dc.EndPath();
            dc.StrokeAndFillPath();
         }
      }  //for(iy  
}
////////////////////////////////////////////////////////
void CGraphDispSV::DrawColorMap(CDC & dc, int width, int height, int style)
{
   ASSERT ( m_dwStyle & GDS_COLORMAP );
   ASSERT (pPalette);

   if ( !pZ ) return;
   
   const int nColors=32;
   float inter=(Zmax-Zmin)/(nColors+1);
   //--------------------------
   if ( inter == 0 ) return;

   CPalette pal,*opal;

   pal.CreatePalette(pPalette);
   opal = dc.SelectPalette(&pal,0);
   dc.RealizePalette();
   //---------------------------------
    
   
   float dx=(float)width/(nX-1);
   float dy=(float)height/(nY-1);  
   
   ///////////////////////////////////////////////////////////
   CBrush *oldbrush = (CBrush *)dc.SelectStockObject(NULL_BRUSH);
   CPen   *oldpen   = (CPen *)dc.SelectStockObject(NULL_PEN);

   POINT node[5];   

   for( float dd=Zmin; dd<Zmax; dd+=inter) {
      int nColor = nColors*RangeZmm(dd);
      if( nColor >=  nColors ) nColor=nColors-1; 
      if( nColor < 0 ) nColor=0; 

      CBrush brush(PALETTEINDEX(nColor))/*,*oldbrush*/;
      CPen   pen(0,0,/*0x0080FF80*/PALETTEINDEX(nColor))/*,*oldpen*/;

      /*oldbrush =*/ dc.SelectObject(&brush); 
      /*oldpen =*/ dc.SelectObject(&pen);

      float *p = pZ;
      for(int j=0;j<nY-1;j++,p++) { 

         for(int i=0;i<nX-1;i++, p++) {
            float a=p[0];
            float b=p[1];
            float c=p[nX];
            float d=p[1+nX];

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDB(a);
               b = LogDB(b);
               c = LogDB(c);
               d = LogDB(d);
            }
            
               
            float Min=min(min(a,b),min(c,d));  
            
            if( dd <=Min /* && style==COLORMAP*/) {   
               CRect r;
               r.left = i*dx;
               r.top  = j*dy;
               r.right = (i+1)*dx;
               r.bottom = (j+1)*dy;
               if ( m_dwStyle & GDS_NOTY ) {
                  r.top  = height - r.top;
                  r.bottom = height - r.bottom;
               }
               dc.Rectangle(r);       
               continue;
            } 

            float Max=max(max(a,b),max(c,d)); 
            if ( ! ( dd > Min && dd < Max ) ) continue;

            bool top, bottom, left, right;
            int  sum; 
            top    = ( dd > min(a,c) && dd < max(a,c) );
            bottom = ( dd > min(b,d) && dd < max(b,d) );
            left   = ( dd > min(a,b) && dd < max(a,b) );
            right  = ( dd > min(c,d) && dd < max(c,d) );
            sum=top+bottom+left+right;
            
            float jtop=0;
            float jbott=0;
            float ileft=0;
            float iright=0;
            int n=0;

            switch(sum) {
            case 2: 
               if ( top ) {
                  jtop=j + (dd-a)/(c-a);
                  if ( bottom ) {
                     jbott=j + (dd - b)/(d-b);
                     n=4;
                     node[0].x= node[1].x= i*dx;
                     node[3].x= node[2].x= (i+1)*dx;
                     if ( c > a || d > b ) {   
                        node[0].y= 
                        node[3].y= (j+1)*dy;
                     } else {
                     //if( c <= a || d <= b ) {     
                        node[0].y= 
                        node[3].y= j*dy;
                     }         
                     node[1].y= jtop*dy;
                     node[2].y= jbott*dy;   
                  } else {
                     if(left) {
                        ileft=i+(dd-a)/(b-a);
                        node[0].x=
                        node[1].x=i*dx;
                        node[2].y=j*dy;       
                        if( a > d ) {     
                           n=3;
                           node[0].y=node[2].y;
                        } else {
                        //if( a < d ) {
                           n=5;
                           node[0].y=node[4].y=(j+1)*dy;
                           node[3].x=node[4].x=(i+1)*dx;
                           node[3].y=node[2].y;
                        }       
                        node[1].y=jtop*dy;
                        node[2].x=ileft*dx;
                     } else {
                        iright=i+(dd-c)/(d-c);
                        node[0].x=
                        node[1].x=i*dx;
                        node[2].y=(j+1)*dy;   
                        if ( c > b ) { 
                           n=3;
                           node[0].y=(j+1)*dy;
                        } else {
                        //if( c < b ) { 
                           n=5;
                           node[0].y=node[4].y=j*dy;
                           node[3].x=node[4].x=(i+1)*dx;
                           node[3].y=(j+1)*dy;  
                        }  
                        node[1].y=jtop*dy;
                        node[2].x=iright*dx;      //4
                     }
                  }
               } else {
                  if(bottom) {
                     jbott=j+(dd - b)/(d-b);
                     if ( left ) {
                        ileft=i+(dd - a)/(b-a);
                        node[0].x=node[1].x=(i+1)*dx;
                        node[1].y=jbott*dy;           //2
                        node[2].x=ileft*dx;
                        node[2].y=j*dy;       
                        
                        if(b>c) {
                           n=3;
                           node[0].y=node[2].y;
                        } else {
                        //if(b<c) {
                           n=5;
                           node[0].y=node[4].y=(j+1)*dy;
                           node[3].x=node[4].x=i*dx;
                           node[3].y=node[2].y; //
                        }      
                     } else {
                        iright=i+(dd-c)/(d-c);
                        node[0].y=node[1].y=(j+1)*dy;            //3
                        node[1].x=iright*dx;
                        node[2].x=(i+1)*dx;
                        node[2].y=jbott*dy;
                        
                        if(a<d) {
                           n=3;
                           node[0].x=node[2].x;
                        } else {
                        //if(a>d) {
                           n=5;   //
                           node[0].x=node[4].x=i*dx;
                           node[3].x=node[2].x;
                           node[3].y=node[4].y=j*dy; 
                        }              
                     }
                  } else {
                     ileft=i+(dd-a)/(b-a);
                     iright=i+(dd-c)/(d-c);
                     node[1].x=ileft*dx;
                     node[2].x=iright*dx;
                     node[0].y=node[1].y=j*dy;
                     node[2].y=node[3].y=(j+1)*dy;
                     n=4;
                     if(b>a||d>c) {
                        node[0].x=
                        node[3].x=(i+1)*dx;
                     } else {
                     //if(b<=a||d<=c) { 
                        node[0].x=
                        node[3].x=i*dx;
                     }               
                  }
               }
               //dc.Polygon(node,n);
               
               break;
               
            case 4: 
               jtop=j+(dd-a)/(c-a);
               jbott=j+(dd-b)/(d-b);
               ileft=i+(dd-a)/(b-a);
               iright=i+(dd-c)/(d-c);
               
               n=4;
               node[0].x=(dx*ileft);
               node[0].y=(dy*j);
               node[1].x=(dx*i);
               node[1].y=(dy*jtop);
               node[2].x=(dx*(i+1));
               node[2].y=(dy*jbott);
               node[3].x=(dx*iright);
               node[3].y=(dy*(j+1));
               
               //dc.Polygon(node,n);
               
               break;
            default:
               continue;
            }  
            if ( n ) {            
               if ( m_dwStyle & GDS_NOTY ) {
                  for ( int idx=0; idx<n ;idx++ ) {
                     node[idx].y = height - node[idx].y;
                  }
               }                    
               dc.Polygon(node,n);
            }

         } //for(i
      }  //for(j  
   }  //for(dd
   dc.SelectObject(oldbrush); 
   dc.SelectObject(oldpen);
   dc.SelectPalette(opal,0);
}
////////////////////////////////////////////////////////
#if(0)
void CGraphDispSV::DrawColorMapY0(CDC & dc, int width, int height, int style)
{
   ASSERT ( m_dwStyle & GDS_COLORMAP );
   ASSERT (pPalette);

   if ( !pData ) {
      dc.TextOut(0,0,_T("NO DATA"));
      return;
   }
   //--------------------------

   CPalette pal,*opal;

   pal.CreatePalette(pPalette);
   opal = dc.SelectPalette(&pal,0);
   dc.RealizePalette();
   //---------------------------------
   int nColors=32; 
   
   float inter=(Dmax-Dmin)/(nColors+1);           
   
   float dx=(float)width/(nX-1);
   float dy=(float)height/(nY-1);  
   
   ///////////////////////////////////////////////////////////
   
   CPoint node[6];
   for( float dd=Dmin; dd<Dmax; dd+=inter) {
      float *p = pData;
 
      int nColor = (dd-Dmin)*nColors/(Dmax-Dmin);
      
      if( nColor >=  nColors ) nColor=nColors-1; 
      if( nColor < 0 ) nColor=0;       
      
   ///////////////////////////////////////////////////////
      CBrush brush(PALETTEINDEX(nColor)),*oldbrush;
      CPen   pen(0,0,/*0x0000FFFF*/PALETTEINDEX(nColor)),*oldpen;

      oldbrush = dc.SelectObject(&brush); 
      oldpen = dc.SelectObject(&pen);
      
      for(int iy=0;iy<nY-1;iy++,p++) { 
         for(int ix=0;ix<nX-1;ix++, p++) {
typedef float FLOAT;
            FLOAT a=p[0];
            FLOAT b=p[1];
            FLOAT c=p[nX];
            FLOAT d=p[1+nX];

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDB(a);
               b = LogDB(b);
               c = LogDB(c);
               d = LogDB(d);
            }
            
            FLOAT e = (a+b+c+d)/4.0f;
            //FLOAT Max=max(max(a,b),max(c,d));     
            //FLOAT Min=min(min(a,b),min(c,d));  
            
            FLOAT x = ix*dx;
            FLOAT y = iy*dy;
            FLOAT d0;
#define DD0(a,b) ((dd-a)/(b-a))
#define ADD(i,j) {node[k].x=i;node[k].y=j;k++;}
#define ADD_COOR_a ADD(x,y)
#define ADD_COOR_b ADD(x+dx,y)
#define ADD_COOR_c ADD(x,y+dy)
#define ADD_COOR_d ADD(x+dx,y+dy)
#define ADD_COOR_e ADD(x+(dx/2),y+(dy/2))
#define ADD_COOR_dab ADD(x+(d0*dx),y)
#define ADD_COOR_dbe ADD(x+dx-(dx*d0/2),y+(dy*d0/2))
#define ADD_COOR_dea ADD(x+(dx/2)-(dx*d0/2),y+(dy/2)-(dy*d0/2))
#define ADD_COOR_dac ADD(x,y+(dy*d0))
#define ADD_COOR_dce ADD(x+(dx*d0/2),y+dy-(dy*d0/2))
#define ADD_COOR_ded ADD(x+(dx/2)+(dx*d0/2),y+(dy/2)+(dy*d0/2))
#define ADD_COOR_ddc ADD(x+dx-(dx*d0),y+dy)
#define ADD_COOR_ddb ADD(x+dx,y+dy-(dy*d0))

#define OTR(i,j) if ( dd <= max(i,j) ) { \
                  FLOAT t = min(i,j);\
                  if ( dd <= t ) {    \
                  /*if ( dd > t-inter ) */{\
                     ADD_COOR_##i ;     \
                     ADD_COOR_##j ;     \
                  }\
               } else {                 \
                  d0 = DD0(i,j);        \
                  if ( i < j ) {        \
                     ADD_COOR_d##i##j ;    \
                     ADD_COOR_##j ;       \
                  } else {              \
                     ADD_COOR_##i ;       \
                     ADD_COOR_d##i##j ;    \
                  }                     \
               }                        \
            }
#define TRI(i,j,k) {OTR(i,j); OTR(j,k); OTR(k,i);}

       int k;
       // abe
            k=0;
            TRI(a,b,e);
            if ( k>2 ) dc.Polygon(node,k);
       // eac++++++++++++++++++++++++++++++++++++++++++++
            k=0;
            TRI(e,a,c);
            if ( k>2 ) dc.Polygon(node,k);
       // ced++++++++++++++++++++++++++++++++++++++++++++
            k=0;
            TRI(c,e,d);
            if ( k>2 ) dc.Polygon(node,k);
       // bed++++++++++++++++++++++++++++++++++++++++++++
            k=0;
            TRI(b,e,d);
            if ( k>2 ) dc.Polygon(node,k);
         ////////////////////////////////////////////   
         } //for(ix
      }  //for(iy  
      dc.SelectObject(oldbrush); 
      dc.SelectObject(oldpen);
   }  //for(cont 
   dc.SelectPalette(opal,0);
}
#endif
////////////////////////////////////////////////////////


void CGraphDispSV::OnContextMenu(CWnd*, CPoint point)
{
   if ( m_dwStyle & GDS_COLORMAP  ) {
      CMenu menu;
      VERIFY(menu.LoadMenu(IDR_POPUP_GRAPH_DISP));

      CMenu* pPopup = menu.GetSubMenu(0);
      ASSERT(pPopup != NULL);

      CMenu *mpal = pPopup->GetSubMenu(0);
      ASSERT(mpal != NULL);
      mpal->RemoveMenu(0,MF_BYPOSITION);

      for ( int i=0; i<NPALETTES ;i++ ) {
         mpal->AppendMenu(MF_STRING,500+i,palettes[i].Name);
         if ( i == nPalette ) {
            mpal->CheckMenuItem(nPalette,MF_BYPOSITION|MF_CHECKED);
         }
      }
      
      CWnd* pWndPopupOwner = this;
      if ( m_nScrollType ) {
         pPopup->CheckMenuItem(9+m_nScrollType,MF_BYPOSITION|MF_CHECKED);
      }

      if (m_dwStyle & GDS_LOGDBZ ) pPopup->CheckMenuItem(ID_POPUP_LOGDB,MF_BYCOMMAND|MF_CHECKED);
      if (m_dwStyle & GDS_ZMAX20 ) pPopup->CheckMenuItem(ID_POPUP_X20,MF_BYCOMMAND|MF_CHECKED);
      if (m_dwStyle & GDS_BAR    ) pPopup->CheckMenuItem(ID_POPUP_BAR,MF_BYCOMMAND|MF_CHECKED);
      else /*if (m_dwStyle & GDS_LONKMAP)*/ pPopup->CheckMenuItem(ID_POPUP_LONKMAP,MF_BYCOMMAND|MF_CHECKED);
      if (m_dwStyle & GDS_TEST   ) pPopup->CheckMenuItem(ID_POPUP_TEST,MF_BYCOMMAND|MF_CHECKED);
      
      if ( m_nHelpId ) {
         pPopup->AppendMenu(MF_SEPARATOR);
         pPopup->AppendMenu(MF_STRING,ID_WHATSIT,_T("&Что это такое?"));
      }
      //while (pWndPopupOwner->GetStyle() & WS_CHILD)
      // pWndPopupOwner = pWndPopupOwner->GetParent();
      
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
         pWndPopupOwner);
   } else { //*********************************************************************
      CMenu menu;
      VERIFY(menu.LoadMenu(IDR_POPUP_GRAPH_DISP1));
      CMenu* pPopup = menu.GetSubMenu(0);

      if (m_dwStyle & GDS_VECTORS ) {
         pPopup->AppendMenu(MF_STRING,ID_POPUP_BAR,_T("Закраска"));
         if (m_dwStyle & GDS_BAR)pPopup->CheckMenuItem(ID_POPUP_BAR,MF_BYCOMMAND|MF_CHECKED);
      }

      if ( m_nHelpId ) {
         pPopup->AppendMenu(MF_SEPARATOR);
         pPopup->AppendMenu(MF_STRING,ID_WHATSIT,_T("&Что это такое?"));
      }
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
   }
   
}
///////////////////////////////////////////////////////////
void CGraphDispSV::OnWhatIsIt() 
{
   WinHelp(0x10000+m_nHelpId, HELP_CONTEXTPOPUP);  
}
void CGraphDispSV::DrawRays(CDC &dc, int cx, int cy,int style)
{
   TRayGraphSV *ps = (TRayGraphSV *)pZ;
   float *px = pX;
   CPen *opn,pn(0,1,0xFF00);
   CBrush *obr,br(0xFF00);
   opn = dc.SelectObject(&pn);
   obr = dc.SelectObject(&br);
   for ( int i=0; i<nX ;i++ ) { // по дистанции
      int x = cx*RangeXmm(*px);
      for ( int j=0; j<NMAXR ;j++ ) {
         TRayGraphSV *p = &ps[i*NMAXR+j];
         if ( p->nRay == -2 ) continue;
         if ( p->nRay < 0 ) break;
         int y = cy - cy*RangeYmm(p->val);
         DWORD dwColor = (RGB(232,320/*16*/,120/*32*/)*p->nRay)&0xFFFFFF|0x808080;

         if ( i < nX-1 ) { // для непоследних
            float *pX2 = px;
            DoStepX(pX2);
            for ( int k=0; k<NMAXR ;k++ ) {
               TRayGraphSV *p2 = &ps[(i+1)*NMAXR+k];
               if ( p->nRay == p2->nRay ) {
                  dc.SelectObject(opn);
                  pn.DeleteObject();
                  pn.CreatePen(0,5*p2->width, dwColor/*(p->nRay*0xFF/(NMAXR-1))<<8*/);
                  dc.SelectObject(&pn);
                  dc.MoveTo(x,y);
                  dc.LineTo(cx*RangeXmm(*pX2),cy-cy*RangeYmm(p2->val));
                  goto _next1;//break;
               }
            }
         }
         // тут не найден
         TCHAR buf[16];
         if ( m_dwStyle & GDS_RAYS1 ) {
            wsprintf(buf,_T("%d-%d"),p->nRay>>16,p->nRay&0xFFFF);
         } else {
            wsprintf(buf,_T("%d"),p->nRay);
         }                
         dc.SetTextColor(dwColor);
         dc.TextOut(x+2,y-14,buf);
        
         _next1:;
      }
      DoStepX(px);
   }
   dc.SelectObject(obr);
   dc.SelectObject(opn);
}



#if 0  //{{AFX
void CGraphDispSV::DrawRays(CDC &dc)
{
   CRect rc;
   GetClientRect(rc);
   int r = rc.bottom/2;
   POINT pts[4] = 
   {
         0, r,
         10,0,
         0,10,
      rc.right,r
   };

 //  r = min(r,rc.right/2);
   
   CPen pn(0,0,0x0000FF00),*opn;//, rpn(0,0,255); 
   opn = dc.SelectObject(&pn);

   dc.SelectStockObject(NULL_BRUSH);
   dc.Rectangle(rc.right*2/5,-1,rc.right*3/5,rc.bottom+1);
   dc.DrawText(_T("Поведение\r\nнеизвестно"), rc, DT_CENTER);

   dc.TextOut(0,0,_T("Источник"));
   dc.SetTextAlign(TA_RIGHT);
   dc.TextOut(rc.right,0,_T("Приемник"));

   float *px = pXData;
   float *py = pYData;
   for ( int i=0; i<nX ;i++ ) {
      double a = (*px)*GrRd;  // out theta
      pts[1].x = r * fcos(a);
      pts[1].y = r - r * fsin(a);
      
      a = *py*GrRd;  // in theta
      pts[2].x = rc.right - r * fcos(a);
      pts[2].y = r - r * fsin(a);
      
      dc.PolyBezier(pts,4);

      py = (float *)( (LPBYTE)py + nYDataStep );
      px = (float *)( (LPBYTE)px + nXDataStep );

      //dc.SelectObject(&rpn);
   }
   



   dc.SelectObject(opn);
}

#endif  //}}AFX

#include "RGB2HLS.h"


void CGraphDispSV::ChangePalette(int d,int pos)
{
   if ( !m_nScrollType ) return ;
   union {
      LONG L;
      BYTE B[4];
   }h;
   for ( int i=1; i<31 ;i++ ) { // white & black leave
      h.L = 
      RGBtoHLS( 0xFFFFFF & (*(DWORD *)&pPalette->palPalEntry[i]));
      int t;
      if ( d ) {
         t = h.B[m_nScrollType-1];
         t += d;
         if ( t > 240 ) t=240;
         if ( t < 0 ) t = 0;
      } else {
         t = pos;
      }
      h.B[m_nScrollType-1] = (BYTE)t;
      
      (*(DWORD *)&pPalette->palPalEntry[i]) = HLStoRGB(h.L);
   }
   Redraw();
}


void CGraphDispSV::OnPopupLogdb() 
{
   m_dwStyle ^= GDS_LOGDBZ;
   UpdateMinMax(0);
   Redraw();
}

void CGraphDispSV::OnPopupX20() 
{
   m_dwStyle ^= GDS_ZMAX20;
   UpdateMinMax(0);
   Redraw();
}


void CGraphDispSV::OnPopupBar() 
{
   m_dwStyle ^= GDS_BAR;
   //UpdateMinMax();
   Redraw();
}

void CGraphDispSV::OnPopupLonkMap() 
{
   m_dwStyle ^= GDS_BAR;//GDS_LONKMAP;
   //UpdateMinMax();
   Redraw();
}
void CGraphDispSV::OnPopupTest() 
{
   m_dwStyle ^= GDS_TEST;
   Redraw();
}

void CGraphDispSV::OnPopupInfo() 
{
//    CPoint pt;
//    GetCursorPos(&pt);
//    ScreenToClient(&pt);
//    CString s;
//    s.Format(_T(
//       "nX=%d,\tXmin=%g,  \tXmax=%g\n"
//       "nY=%d,\tYmin=%g,  \tYmax=%g\n"
//       "Zmin=%g,\tZmax=%g"
//       ),
//        nX, Xmin, Xmax,
//        nY, Ymin, Ymax,
//       Zmin, Zmax);
//    m_pInfo = new CInfoPopupWnd(this,pt,s,&m_pInfo);
}


void CGraphDispSV::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if ( nFlags  == (MK_LBUTTON|MK_CONTROL) ) {
      CColorDialog dlg(0,CC_ANYCOLOR|CC_FULLOPEN,this);
      if ( dlg.DoModal() != IDOK ) return;
      DeleteObject((HGDIOBJ)SetClassLong(m_hWnd,GCL_HBRBACKGROUND,
         (LONG)CreateSolidBrush(dlg.GetColor())));
      InvalidateRect(NULL);
      return;
   }

   if ( (nFlags & (MK_LBUTTON |MK_SHIFT)) == (MK_LBUTTON|MK_SHIFT) ) {
#define CGD_STYLE  (WS_CAPTION|WS_THICKFRAME|WS_POPUP|WS_MAXIMIZEBOX|WS_SYSMENU)
      if ( m_hSavParent != NULL /*GetStyle()&WS_CAPTION*/ ) {
         ModifyStyle(WS_MAXIMIZE,0,0);
         MoveWindow(&m_SavRect);
         ::SetParent(m_hWnd,m_hSavParent);
         ModifyStyle(CGD_STYLE,WS_CHILD,SWP_FRAMECHANGED);
         ::SetWindowLong(m_hWnd, GWL_ID, m_SavID);
         m_hSavParent = NULL;
      } else {
         m_hSavParent = ::GetParent(m_hWnd);
         if ( m_hSavParent ) {
            m_SavID = ::GetDlgCtrlID(m_hWnd);
            ModifyStyle(WS_CHILD,CGD_STYLE,SWP_FRAMECHANGED);
            ::SetParent(m_hWnd, NULL);
            GetWindowRect(&m_SavRect);
         }
      }
   }
}

void CGraphDispSV::OnClose()
{
   // TODO: Add your message handler code here and/or call default
   if ( m_hSavParent != NULL/*GetStyle()&WS_CAPTION*/ ) {
      ModifyStyle(WS_MAXIMIZE,0,0);
      MoveWindow(&m_SavRect);
      ::SetParent(m_hWnd,m_hSavParent);
      ModifyStyle(CGD_STYLE,WS_CHILD,SWP_FRAMECHANGED);
      ::SetWindowLong(m_hWnd, GWL_ID, m_SavID);
      m_hSavParent = NULL;//::GetParent(m_hWnd);
   } else {
   //  CWnd::OnClose();
   }
#undef CGD_STYLE
}

void CGraphDispSV::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if ( m_dwStyle & GDS_PAL ) {
      CClientDC dc(this);
      COLORREF c = dc.GetPixel(point);
//       CString s;
//       s.Format(_T("RGB(%d,%d,%d)"),GetRValue(c),GetGValue(c),GetBValue(c)),
//       AfxMessageBox(s);
      AfxMessageBox(0,_T("RGB(%d,%d,%d)"),GetRValue(c),GetGValue(c),GetBValue(c));
   }

   //CWnd::OnLButtonDown(nFlags, point);
   if ( ! m_Rg.PtInRect(point) ) return;
   
   NMGRCLICKSV nm;
   //NMHDR nm;
   nm.hdr.hwndFrom = m_hWnd;
   nm.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
   nm.hdr.code = NM_CLICK;
   nm.pt.x = point.x-m_Rg.left;//-1;
   nm.pt.y = m_Rg.bottom-point.y;//-1;
   nm.sz = m_Rg.Size();

   if ( m_dwStyle & GDS_RAYS ) {
      nm.sz.cx -= 15+10;
   }
   
//    HWND w = ::GetParent(m_hWnd);
   ::SendMessage(::GetParent(m_hWnd),WM_NOTIFY,nm.hdr.idFrom,(LPARAM)&nm);

   //CWnd::OnLButtonDown(nFlags, point);

}

BOOL CGraphDispSV::OnCommand(WPARAM wParam, LPARAM lParam) 
{
   if ( LOWORD(wParam) >= 500 && LOWORD(wParam) < 500+NPALETTES ) {
      nPalette = LOWORD(wParam)-500;
      nPalette %= NPALETTES;
      CreatePalette(nPalette);
      m_nScrollType = 0;
      SetScrollRange(SB_VERT,0,0);
      Redraw();
      return TRUE;
   } else {
      return CWnd::OnCommand(wParam, lParam);
   }
}

void CGraphDispSV::RedrawWindow()
{
   //InvalidateRect(NULL);
   CWnd::RedrawWindow(NULL,NULL,RDW_ERASE|/*RDW_INTERNALPAINT|*/RDW_INVALIDATE);
   //UpdateWindow();
}

BOOL CGraphDispSV::OnEraseBkgnd(CDC* pDC) 
{
   if ( m_dwStyle & GDS_THRUBMP ) return TRUE;
   return CWnd::OnEraseBkgnd(pDC);
}

#if(1)
#else
void CGraphDispSV::DrawColorMapY(CDC & dc, int width, int height, int style)
{
   ASSERT ( m_dwStyle & GDS_COLORMAP );
   ASSERT (pPalette);

   if ( !pData ) {
      dc.TextOut(0,0,_T("NO DATA"));
      return;
   }
   //--------------------------

   CPalette pal,*opal;

   pal.CreatePalette(pPalette);
   opal = dc.SelectPalette(&pal,0);
   dc.RealizePalette();
   //---------------------------------
   int nColors=32; 
   
   float inter=(Dmax-Dmin)/(nColors+1);           
   
   float dx=(float)width/(nX-1);
   float dy=(float)height/(nY-1);  
   
   ///////////////////////////////////////////////////////////
   
   CPoint node[5];   
   
   for( float dd=Dmin; dd<Dmax; dd+=inter) {
      float *p = pData;
 
      int nColor = (dd-Dmin)*nColors/(Dmax-Dmin);
      
      if( nColor >=  nColors ) nColor=nColors-1; 
      if( nColor < 0 ) nColor=0;       
      
      CBrush brush(PALETTEINDEX(nColor)),*oldbrush;
      CPen   pen(0,0,/*0x0080FF80*/PALETTEINDEX(nColor)),*oldpen;

      oldbrush = dc.SelectObject(&brush); 
      oldpen = dc.SelectObject(&pen);
      
      for(int j=0;j<nY-1;j++,p++) { 

         for(int i=0;i<nX-1;i++, p++) {
            float a=p[0];
            float b=p[1];
            float c=p[nX];
            float d=p[1+nX];

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDB(a);
               b = LogDB(b);
               c = LogDB(c);
               d = LogDB(d);
            }
            
            float Max=max(max(a,b),max(c,d));     
            float Min=min(min(a,b),min(c,d));  
            
            
            float rt; 

            if ( ! ( dd > Min && dd < Max ) ) continue;
            
            if( dd <= Min ) rt = 0.5;
            else {
               rt = (dd-Min)/(Max-Min);//RangeDmm(dd);
            }


            {
               CRect r;
               r.left = i*dx-dx*rt;
               r.top  = j*dy-dy*rt;
               r.right =  i*dx+dx*rt;
               r.bottom = j*dy+dy*rt;
               if ( m_dwStyle & GDS_NOTY ) {
                  r.top  = height - r.top;
                  r.bottom = height - r.bottom;
               }
               dc.Ellipse(r);       
            } 
            
         } //for(i
      }  //for(j  
      dc.SelectObject(oldbrush); 
      dc.SelectObject(oldpen);
   }  //for(cont 
   dc.SelectPalette(opal,0);
}

#endif

void CGraphDispSV::Redraw()
{
   RedrawWindow();

//www    if ( m_pInfo ) {
//       CString s;
//       s.Format(_T(
//          "nX=%d,\tXmin=%g,  \tXmax=%g\n"
//          "nY=%d,\tYmin=%g,  \tYmax=%g\n"
//          "Zmin=%g,\tZmax=%g"
//          ),
//           nX, Xmin, Xmax,
//           nY, Ymin, Ymax,
//          Zmin, Zmax);
// 
//       m_pInfo->SetWindowText(s);
//    }
}
