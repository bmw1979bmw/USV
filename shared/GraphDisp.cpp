// GraphDisp.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDisp.h"
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

CGraph2dData::CGraph2dData()
{
   nX = nY = 0;
   pX = pY = NULL;
   nYStep = nXStep = 0;
   Xmin = Xmax = Ymin = Ymax = 0;
}

CGraph2dData::~CGraph2dData()
{

}

float CGraph2dData::LogDB(float val)
{
   if ( val <=0.0f ) {
      return 0.0f;
   }
   return (float)(20.0*log10(val/2.0e-5));
}

float CGraph3dData::LogDBz(float val)  
{  // по Z - по мощности Па^2
   if ( val <=0.0f ) {
      return 0.0f;
   }
   return (float)(10.0*log10(val/2.0e-5/2.0e-5 ));
}

float CGraph2dData::RangeXmm(float x)
{
   float f = (Xmax-Xmin);
   if ( f == 0 ) return 1;
   return (x-Xmin)/f;
}

float CGraph2dData::RangeYmm(float y)
{
   float f = (Ymax-Ymin);
   if ( f == 0 ) return 1;
   return (y-Ymin)/f;
}


BOOL CGraph2dData::UpdateMinMax()
{
   if ( m_dwStyle & GDS_PAL ) {
      Xmin = 0;
      Xmax = 0;
      Ymin = (float)nX;
      Ymax = 0;
      return TRUE;
   }

   int i;
   float f;
   float *px = pX;
   float *py = pY;

   try {
   if ( !px ) {
      Xmin = Xmax = 0;
   } else {
      Xmin = Xmax = *px;
      for ( i=0; i<nX ;i++ ) {
         f = *px;
         if ( (m_dwStyle & GDS_NOZEROX) && f == 0.0f) continue;
         if ( f < Xmin ) Xmin = f;
         if ( f > Xmax ) Xmax = f;
         DoStepX(px);
      }   
      if ( Xmin == Xmax ) {
         if ( Xmin ) {
            Xmax = 2*Xmin;
         } else {
            Xmax = 1;
         }
      } 
   }
   
   if ( !py ) {
      Ymin = Ymax = 0;
   } else {
      Ymin = Ymax = *py;
      for ( i=0; i<nY ;i++ ) {
         f = *py;
         if ( (m_dwStyle & GDS_NOZEROY) && f == 0.0f) continue;
         if ( f < Ymin ) Ymin = f;
         if ( f > Ymax ) Ymax = f;
         DoStepY(py);
      }
      if ( Ymin == Ymax ) {
         if ( Ymin ) {
            Ymax = 2*Ymin;
         } else {
            Ymax = 1;
         }
      } 
   }

   if ( m_dwStyle & GDS_LOGDBY ) {
      Ymin = LogDB(Ymin);
      Ymax = LogDB(Ymax);
   }

   if ( m_dwStyle & GDS_MINY0 ) {
      if ( Ymin > 0 ) Ymin = 0;
   }
   if ( m_dwStyle & GDS_MINX0 ) {
      if ( Xmin > 0 ) Xmin = 0;
   }
   _asm wait;
   } catch(...) {
      RESETFP();//_clear87();
      return FALSE;
   }
   return TRUE;
}

void CGraph2dData::NormalizeMinMax(float & Amin, float & Amax)
{
   if ( Amin > Amax ) {
      float t = Amax;
      Amax = Amin;
      Amin = t;
   }
}
/////////////////////////////////////////////////////////////////////
float CGraph3dData::ms_Zmax;
float CGraph3dData::ms_Zmin;

CGraph3dData::CGraph3dData()
{
   pZ = NULL;
   Zmin = Zmax = 0;
}

CGraph3dData::~CGraph3dData()
{
   
}

#ifdef _DEBUG
float CGraph3dData::RangeZmm(float z)
{
   float f = (Zmax-Zmin);
   if ( f == 0 ) return 0;
   f = (z-Zmin)/f;
   if ( f > (float)0x3FFFFFF ) f = (float)0x3FFFFFF;
   return f;
}
#endif

void  CGraph3dData::SetMinMaxToShared(float Lsens)
{
   Zmin = ms_Zmin;
   Zmax = ms_Zmax;
   if ( Lsens == 0 ) return;
   Zmax /= Lsens;
   if ( Zmax < Zmin ) Zmax = Zmin;
}

BOOL CGraph3dData::UpdateMinMax(BOOL bShared)
{

   if ( bShared ) {
      /* использовать static Zmin Zmax */
      Zmin = ms_Zmin;
      Zmax = ms_Zmax;
   } else {
      Zmin = 
      Zmax = 0;
   }

   if ( !CGraph2dData::UpdateMinMax() ) return FALSE;
   
   int i,j;
   
   try {
      if ( m_dwStyle & (GDS_COLORMAP|GDS_VECTORS) ) {
         float *p = pZ;
         if ( p ) {
            Zmin = Zmax = *p;
            for ( i=0; i<nY ;i++ ) {
               for ( j=0; j<nX ;j++ ) {
                  if ( *p < Zmin ) Zmin = *p;
                  if ( *p > Zmax ) Zmax = *p;
                  p++;
               }
            }
            if ( m_dwStyle & GDS_LOGDBZ ) {
               Zmin = LogDBz(Zmin);
               Zmax = LogDBz(Zmax);
            }
            if ( m_dwStyle & GDS_ZMAX20 ) {
               try {
                  Zmax /= 20;
               } catch(...) {
                  RESETFP();
                  Zmax = 0;
               }
            }
         }
      }
      
      if ( (m_dwStyle & GDS_MINZ0) && !(m_dwStyle & GDS_LOGDBZ) ) {
         if ( Zmin > 0 ) Zmin = 0;
      }
      
      if ( m_dwStyle & GDS_ZMIN0ALW ) {
         Zmin = 0;
      }
      
      if ( bShared ) {
         ms_Zmin = Zmin;
         ms_Zmax = Zmax;
      } 
      _asm wait;
      
   } catch(...) {
      RESETFP();//_clear87();
      return FALSE;
   }
   return TRUE;
}

void CGraph2dData::SetData(int nx, 
                           float *px, float *py, 
                           int xstep, int ystep, 
                           BOOL bUpdateMinmax, BOOL bRedraw)
{
   nX = nY = nx;
   pX = px;
   pY = py;
   nXStep = xstep;
   nYStep = ystep;

   if ( bUpdateMinmax ) {
      if ( !UpdateMinMax() ) {
         nY = nX = 0;
         pX = pY = NULL;
      }
   }
   if ( bRedraw ) Redraw();
}

void CGraph3dData::SetData(int nx, int ny, float *px, float *py, float *pz,
                           int xstep, int ystep,
                           UINT nUpdateMinMax,
                           BOOL bRedraw)
{
   if ( nX != nx || nY != ny ) nUpdateMinMax |= 1;

   nX = nx;
   nY = ny;
   pX = px;
   pY = py;
   pZ = pz;

   nYStep = ystep;
   nXStep = xstep;

   if ( nUpdateMinMax ) {
      if ( !UpdateMinMax( nUpdateMinMax&2 ) ) { // 2-shared
         nY = nX = 0;
         pX = pY = pZ = NULL;
      }
   }

   if ( bRedraw ) Redraw();
}

void CGraph3dData::SetZmaxMul(float n, BOOL bRedraw)
{
   if ( Zmax >= FLT_MAX/n ) return;
   Zmax *= n;
   if ( bRedraw ) Redraw();
}

void CGraph3dData::SetZmaxDiv(float n, BOOL bRedraw)
{
   if ( n == 0 ) return;
   Zmax /= n;
   if ( Zmax < Zmin ) Zmax = Zmin;
   if ( bRedraw ) Redraw();
}

////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGraphDisp, CWnd)
   //{{AFX_MSG_MAP(CGraphDisp)
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
// CGraphDisp

CGraphDisp::CGraphDisp()
{
   WNDCLASS wc;
   ZeroMemory(&wc,  sizeof(WNDCLASS));
   wc.lpfnWndProc = ::DefWindowProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL,IDC_ARROW); 
   wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = (HBRUSH) CreateSolidBrush(0);
   wc.lpszClassName = _T("GraphDisp");
   
   VERIFY( AfxRegisterClass(&wc) ); 
   
   //////////////////////////////////////////
   m_fnt.CreatePointFont( 80, _T("MS Sans Serif") );
   idsX = idsY = 0;
   pPalette = NULL;
   m_nScrollType = 0;
   m_hSavParent = NULL;
   //m_nFloatLen = 6;
   m_nHelpId = 0;
   m_pInfo = NULL;

}

TGDPalette CGraphDisp::palettes[] = {
#include "CGraphDisp.pll"
};
#define NPALETTES (sizeof(CGraphDisp::palettes)/sizeof(TGDPalette))

BOOL CGraphDisp::Init(UINT idsx, UINT idsy, DWORD dwStyle,LONG param1,LONG param2)
{
   idsX = idsx;
   idsY = idsy;
   m_dwStyle = dwStyle;

   if ( dwStyle & GDS_COLORMAP ) {
      nPalette = param1;
      CreatePalette(nPalette);
      m_nHelpId = param2;
   } else {
      m_nHelpId = param1;
   }

   return TRUE;
}

/////////////////////////////////////////////////////////////////////
void CGraphDisp::CreatePalette(int nPal,int nument /*=32*/, TGDPalette *srcP /* =NULL*/)
{
   TGDPalette *P;
   if ( srcP != NULL ) P = srcP;  // Ye Ye may use srcP only
   else                P = &palettes[nPal];

   delete pPalette;
   pPalette = 
      (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE)+(nument-1)*sizeof(PALETTEENTRY)];
                 
   pPalette -> palVersion = 0x300;
   pPalette -> palNumEntries = nument;

   int I0=(int)(nument/P->N); 

   ////////////////////////////////////
   int i,j,k;
   for ( i=0,j=0,k=0; i<nument ;i++ ) {
      pPalette->palPalEntry[i].peRed   = P->r[j]+P->dr[j]*k;
      pPalette->palPalEntry[i].peGreen = P->g[j]+P->dg[j]*k;
      pPalette->palPalEntry[i].peBlue  = P->b[j]+P->db[j]*k;
      pPalette->palPalEntry[i].peFlags = PC_RESERVED;
      if ( ++k > I0 ) {
         if ( j+1 > P->N ) {
                    
         } else {
            j++;
            k=0;
         }
      }
   }
   if ( !srcP ) {
      *(DWORD *)&(pPalette->palPalEntry[nument-1])=RGB(255,255,255);
   }
} 

CGraphDisp::~CGraphDisp()
{
   delete pPalette;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDisp message handlers
void CGraphDisp::OnPaint() 
{
   CPaintDC dc(this);

   try {
   if ( m_dwStyle & GDS_TEST ) {
      DrawColorTest(dc);
   } else  if ( m_dwStyle & GDS_THRUBMP ) {
      CRect rc;
      GetClientRect(rc);
      CBitmap bm,*obm;
      CDC DC;
      DC.CreateCompatibleDC(&dc);
      bm.CreateCompatibleBitmap(&dc,rc.right,rc.bottom);
      obm = DC.SelectObject(&bm);
      // Fill
      DC.FillRect(rc,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
      ///////////////////
      OnDraw(DC);
      ///////////////////
      dc.BitBlt(0,0,rc.right,rc.bottom,&DC,0,0,SRCCOPY);
      DC.SelectObject(obm);
   } else {
      // standard paint routine
      OnDraw(dc);
   }
   } catch (...) {
      RESETFP();
      dc.TextOut(0,0,_T("#error#"));
   }
}

void CGraphDisp::AfterDraw(CDC &dc, int width, int height)
{
   if ( m_dwStyle & GDS_AFTERDRAW ) {
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

int CGraphDisp::DrawScales(CDC &dc)  // рисует надписи и формирует m_Rg
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


   if ( m_dwStyle & GDS_NOTX ) {
      
      if ( m_dwStyle & GDS_NAMEFIRST ) {
         sx0 = s + _T(' ') + sx0;
      } else {
         sx0 += _T(' ') + s;
      }
   } else {
      if ( m_dwStyle & GDS_NAMEFIRST ) {
         sx1 = s + _T(' ') + sx1;
      } else {
         sx1 += _T(' ');  sx1 += s;
      }
      
   }


   zx0 = dc.GetTextExtent(sx0);
   zx1 = dc.GetTextExtent(sx1);
   
   s.LoadString(idsY);
   s = s.SpanExcluding(_T("\n"));
   sy1 += _T(" "); sy1 += s;

   zy0 = dc.GetTextExtent(sy0);
   zy1 = dc.GetTextExtent(sy1);
   
   if ( m_dwStyle & GDS_DIAGRAM ) sx1 += _T(" (равномерный шаг!)");
   
   if ( m_dwStyle & GDS_YX ) {
      m_Rg.top = zx1.cy;
      m_Rg.bottom = rc.bottom-zy0.cy;
      m_Rg.right = rc.right-8;
      
      if ( m_dwStyle & GDS_NOTX ) {
         m_Rg.left = zx1.cx;
         dc.TextOut( 0, 0, sx0);
         dc.TextOut( 0, rc.bottom - zy0.cy - zx0.cy ,sx1);
      } else {
         m_Rg.left = zx0.cx;
         dc.TextOut( 0, 0, sx1);
         dc.TextOut( 0, rc.bottom - zy0.cy - zx0.cy ,sx0);
      }
      dc.TextOut( zx0.cx, rc.bottom-zy0.cy, sy0);
      dc.TextOut( rc.right-zy1.cx-1, rc.bottom-zy0.cy,sy1);
   } else {
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

void CGraphDisp::OnDraw(CDC &dc) 
{
   if ( !pX || !pY || ((m_dwStyle & (GDS_COLORMAP|GDS_VECTORS)) && !pZ) ) {
      return;
   }

   CRect rc;
   GetClientRect(rc); 
   
   int nDCSav = dc.SaveDC();
   
   dc.SelectObject(&m_fnt);
   dc.SetTextColor(0x0000FFFF);
   dc.SetBkMode(TRANSPARENT);
   
   int zcy;
   if ( m_dwStyle & GDS_PAL  ) {
      GetClientRect(m_Rg);
   } else {
      zcy = DrawScales( dc );
   }
   
   ////////////////////////////////////////////////////////////////////   
   int cx, cy;
   {
      CSize zg = m_Rg.Size();
      cx = zg.cx-10;
      cy = zg.cy-1;
   }
   
   long  center = cy;
   
   if ( !(m_dwStyle & GDS_NOBELOWY) ) {
      center -= (long)( cy*RangeYmm(0) );
   } 
   
   /****** рисуем оси **********************/
   CBrush fillbr(0x00C80000);
   CPen   yellowpn(0,0,0x0000FFFF);
   if( ! (m_dwStyle & GDS_NOAXES) ) {
      CPen   orange(0,0,0x000093FF);
      
      dc.SelectObject(&orange);
      dc.MoveTo(m_Rg.left,m_Rg.top);
      dc.LineTo(m_Rg.left,m_Rg.bottom);
      if ( !(m_dwStyle & GDS_YX) ) {
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
   
   
   if ( m_dwStyle & GDS_YX ) {
      pt[0].x = 0; 
      pt[0].y = 0;
      pt[3] = pt[1] = pt[0]; 
   } else {
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
      if ( m_dwStyle & GDS_BAR ) { 
         DrawColorMapBar(dc,cx,cy);
      } else  { 
         DrawColorMap(dc,cx,cy);
      }
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
               ASSERT(0);
            }
         } else if ( m_dwStyle & GDS_BAR ) {
            pt[1].y = 
               pt[2].y = cy 
               - (long)( cy * RangeYmm(fy) );
            pt[0].x = pt[1].x = (long)(cx*RangeXmm(fx));
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
               pt[2].x = pt[3].x = (long)(cx*RangeXmm(fx));
               pt[2].y = cy - (long)(cy * RangeYmm(fy) );
               dc.Polygon(pt,4);
               pt[0] = pt[3];
               pt[1] = pt[2];
            }
         }
         //==============================================
         DoStepY(py);
         DoStepX(px);
      }
   }
   AfterDraw(dc,cx,cy);
   
   dc.RestoreDC(nDCSav);
}

void CGraphDisp::DrawPalette(CDC & dc, int width, int height, int style)
{
   CPalette pal,*opal;
   CreatePalette(-1,nX,(TGDPalette *)pX);

   pal.CreatePalette(pPalette);

   opal = dc.SelectPalette(&pal,0);
   dc.TextOut(0,0,_T("Palette"));

   width+=10;

   for ( int i=0; i<nX ;i++ ) {
      CBrush brush(PALETTEINDEX(i));
      dc.FillRect(CRect(0,height*i/nX,width,height*(i+1)/nX),&brush);
   } 
   dc.SelectPalette(opal,0);
}

#pragma warning(disable:4244)
void CGraphDisp::DrawColorMapBar(CDC & dc, int width, int height, int style)
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
   
   for(int iy=0;iy<nY;iy++) { 
      for(int ix=0;ix<nX;ix++, p++) {
         typedef float FLOAT;
         FLOAT a=p[0];
         
         if ( m_dwStyle & GDS_LOGDBZ ) {
            a = LogDBz(a);
         }
         
         int nColor = int(32*RangeZmm(a));
         
         if( nColor >=  nColors ) nColor=nColors-1; 
         if( nColor < 0 ) nColor=0;       
         
         CBrush brush(PALETTEINDEX(nColor));
         
         FLOAT x = ix*dx;
         FLOAT y = iy*dy;
         
         if ( m_dwStyle & GDS_NOTY ) {
            y = height - dy - y;
         }                 
         dc.FillRect(CRect(x,y,x+dx,y+dy),&brush);
      } //for(ix
   }  //for(iy  
   
   dc.SelectPalette(opal,0);
}
#pragma warning(default:4244)

void CGraphDisp::DrawColorTest(CDC & dc)
{
   if( Zmax == Zmin ) return;
   CRect rc;
   GetClientRect(rc);
   int width = rc.right; 
   int height = rc.bottom;

   if ( width < 100 || height <50 ) return;

   width = (width+3)/4*4;

   CDC md;
   HBITMAP dib;
   CPalette pal;
   int i;

   BITMAPINFO *bmi = (BITMAPINFO *)new BYTE[
      sizeof(BITMAPINFO)+
      sizeof(RGBQUAD)*
      256
      ];
   bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi->bmiHeader.biWidth = width;
   bmi->bmiHeader.biHeight = -height;
   bmi->bmiHeader.biPlanes  = 1;
   bmi->bmiHeader.biBitCount = 8;
   bmi->bmiHeader.biCompression = BI_RGB;
   bmi->bmiHeader.biSizeImage    =  0;
   bmi->bmiHeader.biXPelsPerMeter = 0;
   bmi->bmiHeader.biYPelsPerMeter  = 0;
   bmi->bmiHeader.biClrUsed        =  0;
   bmi->bmiHeader.biClrImportant   =  0;
  
   for ( i=0; i<32 ;i++ ) {
      bmi->bmiColors[i].rgbBlue = pPalette->palPalEntry[i].peBlue;
      bmi->bmiColors[i].rgbRed = pPalette->palPalEntry[i].peRed;
      bmi->bmiColors[i].rgbGreen = pPalette->palPalEntry[i].peGreen;
   }
                                 
   BYTE *pbits;

   dib = CreateDIBSection(0,
      bmi,
      DIB_RGB_COLORS,
      (void **)&pbits,
      0,0
      );
   VERIFY(dib);
   
   float dx=(float)width/(nX-1);
   float dy=(float)height/(nY-1);  
   
   {
      BYTE *pv = pbits;
      float ay=0;
      float ax=0;
      int iy=0;
      int ix=0;
      float *py,*px;
      int tx,ty;
      for ( int y=0; y<height ;y++,ty++ ) {
         if ( y >= ay ) {
            ay += dy;
            py = &pZ[iy*nX];
            iy++;
            ty=0;
         }
         ix=0;
         ax=0;
         for ( int x=0; x<width ;x++,tx++ ) {
            if ( x >= ax ) {
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
   delete (BYTE *)bmi;
}


#pragma warning(disable:4244)
//////////////////////////////////////////////////////////////////
void CGraphDisp::DrawVectors(CDC & dc, int width, int height, int style)
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
      
      for(int ix=0;ix<nX;ix++) {
         typedef float FLOAT;
         FLOAT a;
         if ( m_dwStyle & GDS_YX ) {
            a=p[ix*nY+iy];
         } else {
            a=p[iy*nX+ix];
         }
         
         if ( m_dwStyle & GDS_LOGDBZ ) {
            a = LogDBz(a);
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
         
         dc.LineTo(x,y);
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
void CGraphDisp::DrawVectorsTest(CDC & dc, int width, int height, int style)
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
      
      for(int ix=0;ix<nX;ix++) {
         typedef float FLOAT;
         FLOAT a;
         if ( m_dwStyle & GDS_YX ) {
            a=p[ix*nY+iy];
         } else {
            a=p[iy*nX+ix];
         }
         
         if ( m_dwStyle & GDS_LOGDBZ ) {
            a = LogDBz(a);
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
         
         dc.MoveTo(x,ny*dy+dy);
         dc.LineTo(x,y);
         dc.LineTo(x+dx,y);
         dc.LineTo(x+dx,ny*dy+dy);
      } //for(ix
      
      if ( m_dwStyle & GDS_BAR ) {
         dc.LineTo((nX-1)*dx,(nY-1-iy)*dy+dy);
         dc.EndPath();
         dc.StrokeAndFillPath();
      }
   }  //for(iy  
}
////////////////////////////////////////////////////////
void CGraphDisp::DrawColorMap(CDC & dc, int width, int height, int style)
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

      CBrush brush(PALETTEINDEX(nColor));
      CPen   pen(0,0,PALETTEINDEX(nColor));

      dc.SelectObject(&brush); 
      dc.SelectObject(&pen);

      float *p = pZ;
      for(int j=0;j<nY-1;j++,p++) { 

         for(int i=0;i<nX-1;i++, p++) {
            float a=p[0];
            float b=p[1];
            float c=p[nX];
            float d=p[1+nX];

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDBz(a);
               b = LogDBz(b);
               c = LogDBz(c);
               d = LogDBz(d);
            }
            
               
            float Min=min(min(a,b),min(c,d));  
            
            if( dd <=Min ) {   
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
                        node[0].x=
                        node[3].x=i*dx;
                     }               
                  }
               }
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
void CGraphDisp::DrawColorMapY0(CDC & dc, int width, int height, int style)
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
               a = LogDBz(a);
               b = LogDBz(b);
               c = LogDBz(c);
               d = LogDBz(d);
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


void CGraphDisp::OnContextMenu(CWnd*, CPoint point)
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
      else pPopup->CheckMenuItem(ID_POPUP_LONKMAP,MF_BYCOMMAND|MF_CHECKED);
      if (m_dwStyle & GDS_TEST   ) pPopup->CheckMenuItem(ID_POPUP_TEST,MF_BYCOMMAND|MF_CHECKED);
      
      if ( m_nHelpId ) {
         pPopup->AppendMenu(MF_SEPARATOR);
         pPopup->AppendMenu(MF_STRING,ID_WHATSIT,_T("&Что это такое?"));
      }
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
void CGraphDisp::OnWhatIsIt() 
{
   if ( !m_nHelpId ) return;
   //WinHelp(0x10000+m_nHelpId, HELP_CONTEXTPOPUP);  

   CPoint pt;
   GetCursorPos(&pt);
   ScreenToClient(&pt);
   CString s;
   s.LoadString(m_nHelpId);
   m_pInfo = new CInfoPopupWnd(this,pt,s,&m_pInfo);

}

void CGraphDisp::DrawRays(CDC &dc, int cx, int cy,int style)
{
   TRayGraph *ps = (TRayGraph *)pZ;
   float *px = pX;
   CPen *opn,pn(0,1,0xFF00);
   CBrush *obr,br(0xFF00);
   opn = dc.SelectObject(&pn);
   obr = dc.SelectObject(&br);
   for ( int i=0; i<nX ;i++ ) { // по дистанции
      int x = cx*RangeXmm(*px);
      for ( int j=0; j<NMAXR ;j++ ) {
         TRayGraph *p = &ps[i*NMAXR+j];
         if ( p->nRay == -2 ) continue;
         if ( p->nRay < 0 ) break;
         int y = cy - cy*RangeYmm(p->val);
         DWORD dwColor = (RGB(232,320,120)*p->nRay)&0xFFFFFF|0x808080;

         if ( i < nX-1 ) { // для непоследних
            float *pX2 = px;
            DoStepX(pX2);
            for ( int k=0; k<NMAXR ;k++ ) {
               TRayGraph *p2 = &ps[(i+1)*NMAXR+k];
               if ( p->nRay == p2->nRay ) {
                  dc.SelectObject(opn);
                  pn.DeleteObject();
                  pn.CreatePen(0,5*p2->width, dwColor);
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

#include "RGB2HLS.h"

void CGraphDisp::ChangePalette(int d,int pos)
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

void CGraphDisp::OnPopupLogdb() 
{
   m_dwStyle ^= GDS_LOGDBZ;
   UpdateMinMax(0);
   Redraw();
}

void CGraphDisp::OnPopupX20() 
{
   m_dwStyle ^= GDS_ZMAX20;
   UpdateMinMax(0);
   Redraw();
}


void CGraphDisp::OnPopupBar() 
{
   m_dwStyle ^= GDS_BAR;
   Redraw();
}

void CGraphDisp::OnPopupLonkMap() 
{
   m_dwStyle ^= GDS_BAR;
   Redraw();
}
void CGraphDisp::OnPopupTest() 
{
   m_dwStyle ^= GDS_TEST;
   Redraw();
}

void CGraphDisp::OnPopupInfo() 
{
   CPoint pt;
   GetCursorPos(&pt);
   ScreenToClient(&pt);
   CString s;
   s.Format(_T(
      "nX=%d,\tXmin=%g,  \tXmax=%g\n"
      "nY=%d,\tYmin=%g,  \tYmax=%g\n"
      "Zmin=%g,\tZmax=%g"
      ),
       nX, Xmin, Xmax,
       nY, Ymin, Ymax,
      Zmin, Zmax);
   m_pInfo = new CInfoPopupWnd(this,pt,s,&m_pInfo);
}


void CGraphDisp::OnLButtonDblClk(UINT nFlags, CPoint point) 
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
      if ( m_hSavParent != NULL ) {
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

void CGraphDisp::OnClose()
{
   if ( m_hSavParent != NULL ) {
      ModifyStyle(WS_MAXIMIZE,0,0);
      MoveWindow(&m_SavRect);
      ::SetParent(m_hWnd,m_hSavParent);
      ModifyStyle(CGD_STYLE,WS_CHILD,SWP_FRAMECHANGED);
      ::SetWindowLong(m_hWnd, GWL_ID, m_SavID);
      m_hSavParent = NULL;
   }
#undef CGD_STYLE
}

void CGraphDisp::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if ( m_dwStyle & GDS_PAL ) {
      CClientDC dc(this);
      COLORREF c = dc.GetPixel(point);
      AfxMessageBox(0,_T("RGB(%d,%d,%d)"),GetRValue(c),GetGValue(c),GetBValue(c));
   }

   if ( ! m_Rg.PtInRect(point) ) return;
   
   NMGRCLICK nm;
   nm.hdr.hwndFrom = m_hWnd;
   nm.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
   nm.hdr.code = NM_CLICK;
   nm.pt.x = point.x-m_Rg.left;
   nm.pt.y = m_Rg.bottom-point.y;
   nm.sz = m_Rg.Size();

   if ( m_dwStyle & GDS_RAYS ) {
      nm.sz.cx -= 15+10;
   }
   ::SendMessage(::GetParent(m_hWnd),WM_NOTIFY,nm.hdr.idFrom,(LPARAM)&nm);
}

BOOL CGraphDisp::OnCommand(WPARAM wParam, LPARAM lParam) 
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

void CGraphDisp::RedrawWindow()
{
   CWnd::RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
}

BOOL CGraphDisp::OnEraseBkgnd(CDC* pDC) 
{
   if ( m_dwStyle & GDS_THRUBMP ) return TRUE;
   return CWnd::OnEraseBkgnd(pDC);
}

#if(0)
void CGraphDisp::DrawColorMapY(CDC & dc, int width, int height, int style)
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
      CPen   pen(0,0,PALETTEINDEX(nColor)),*oldpen;

      oldbrush = dc.SelectObject(&brush); 
      oldpen = dc.SelectObject(&pen);
      
      for(int j=0;j<nY-1;j++,p++) { 

         for(int i=0;i<nX-1;i++, p++) {
            float a=p[0];
            float b=p[1];
            float c=p[nX];
            float d=p[1+nX];

            if ( m_dwStyle & GDS_LOGDBZ ) {
               a = LogDBz(a);
               b = LogDBz(b);
               c = LogDBz(c);
               d = LogDBz(d);
            }
            
            float Max=max(max(a,b),max(c,d));     
            float Min=min(min(a,b),min(c,d));  
            float rt; 

            if ( ! ( dd > Min && dd < Max ) ) continue;
            
            if( dd <= Min ) rt = 0.5;
            else {
               rt = (dd-Min)/(Max-Min);
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

void CGraphDisp::Redraw()
{
   RedrawWindow();

   if ( m_pInfo ) {
      CString s;
      s.Format(_T(
         "nX=%d,\tXmin=%g,  \tXmax=%g\n"
         "nY=%d,\tYmin=%g,  \tYmax=%g\n"
         "Zmin=%g,\tZmax=%g"
         ),
          nX, Xmin, Xmax,
          nY, Ymin, Ymax,
         Zmin, Zmax);
      m_pInfo->SetWindowText(s);
   }
}
