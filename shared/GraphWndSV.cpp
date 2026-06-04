// GraphWndSV.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GraphWndSV.h"
#include <float.h>
#include "InfoPopupWnd.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphWndSV

CGraphWndSV::CGraphWndSV()
{
   WNDCLASS wc;
   ZeroMemory(&wc,  sizeof(WNDCLASS));
   wc.lpfnWndProc = ::DefWindowProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL,IDC_ARROW); 
   wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = (HBRUSH) CreateSolidBrush(0);
   wc.lpszClassName = _T("GraphDispSV");

   VERIFY( AfxRegisterClass(&wc) ); 
   /////////////////////////////////////////////////
   m_pMemDC = NULL;
   m_pBits = NULL;
   m_dwStyle = 0;
   m_pInfo = NULL;
}

CGraphWndSV::~CGraphWndSV()
{
   ASSERT(!m_pMemDC);
}


BEGIN_MESSAGE_MAP(CGraphWndSV, CWnd)
   //{{AFX_MSG_MAP(CGraphWndSV)
   ON_WM_DESTROY()
   ON_WM_PAINT()
   ON_WM_PALETTECHANGED()
   ON_WM_QUERYNEWPALETTE()
   ON_WM_ERASEBKGND()
   ON_WM_LBUTTONDOWN()
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_POPUP_X20, OnPopupX20)
   ON_COMMAND(ID_POPUP_INFO, OnPopupInfo)
   ON_COMMAND(ID_POPUP_LOGDB, OnPopupDB)
   ON_WM_LBUTTONDBLCLK()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphWndSV message handlers

void CGraphWndSV::OnDestroy() 
{
   CWnd::OnDestroy();

   if (m_pMemDC != NULL)
   {
      m_pMemDC->SelectObject(m_pOldBitmap);
      delete m_pMemDC;
      m_pMemDC = NULL;
      m_pOldBitmap = NULL;
   }
   
   // TODO: Add your message handler code here
   if (m_Palette.m_hObject != NULL)
      m_Palette.DeleteObject();

}

void CGraphWndSV::OnDraw(CDC &dc) 
{
   //CPaintDC dc(this); // device context for painting
   if (m_Palette.m_hObject != NULL  && m_Bitmap.m_hObject != NULL) {
      int cx,cy;
      GetBitmapSize(&cx,&cy);

      CPalette *pOldPalette;
      pOldPalette = dc.SelectPalette( &m_Palette, FALSE );
      dc.RealizePalette();
      dc.BitBlt( 0, 0, cx, cy, m_pMemDC, 0, 0, SRCCOPY );
      dc.SelectPalette( pOldPalette, TRUE );

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
         dis.rcItem.bottom = cy;
         dis.rcItem.right = cx;
         dis.itemData = 0; 
         ::SendMessage(::GetParent(m_hWnd),WM_DRAWITEM,dis.CtlID,(LPARAM)&dis);
      }
   }
}

void CGraphWndSV::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
   OnDraw(dc);
}

CPalette * CGraphWndSV::GetPalette()
{
   if (m_Palette.m_hObject != NULL)
      return &m_Palette;
   else
      return NULL;
}

void CGraphWndSV::OnPaletteChanged(CWnd* pFocusWnd) 
{
   if (pFocusWnd == this || IsChild(pFocusWnd))
      return;

   OnQueryNewPalette();
}

BOOL CGraphWndSV::OnQueryNewPalette() 
{
   if (GetPalette() == NULL)
      return FALSE;

   {
      CClientDC dc(this);
      CPalette* pOldPalette = dc.SelectPalette(&m_Palette,
         GetCurrentMessage()->message == WM_PALETTECHANGED);
      UINT nChanged = dc.RealizePalette();
      dc.SelectPalette(pOldPalette, TRUE);

      if (nChanged == 0)  // no change to our mapping
         return FALSE;
   }

   // some changes have been made; invalidate
   Invalidate(FALSE);

   return TRUE;
}

void CGraphWndSV::Init(DWORD dwStyle, int nPal)
{
   m_dwStyle = dwStyle|GDS_COLORMAP;
   m_nPalette = nPal;

   CreatePalette(nPal);

   CreateBitmap();

   {
      CWnd* pActiveWnd = CWnd::GetActiveWindow();
      if (pActiveWnd != NULL)
         pActiveWnd->SendMessage(WM_QUERYNEWPALETTE);
   }
}

TGDPaletteSV CGraphWndSV::palettes[] = {
#include "CGraphDisp.pll"
};
#define NPALETTES (sizeof(CGraphWndSV::palettes)/sizeof(TGDPaletteSV))

void CGraphWndSV::CreatePalette(int nPal)
{
   if (m_Palette.m_hObject != NULL)
      m_Palette.DeleteObject();
   ////////////////////////////////////////////

   TGDPaletteSV *P;
   P = &palettes[nPal];

   LOGPALETTE *pPalette;
   pPalette = (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE)+31*sizeof(PALETTEENTRY)];
                 
   pPalette -> palVersion = 0x300;
   pPalette -> palNumEntries = 32;

   int I0=(int)(32/P->N); 
   ////////////////////////////////////
   int i,j,k;
   for ( i=0,j=0,k=0; i<32 ;i++ ) {
      m_rgbPalette[i].rgbRed   = pPalette->palPalEntry[i].peRed   = P->r[j]+P->dr[j]*k;
      m_rgbPalette[i].rgbGreen = pPalette->palPalEntry[i].peGreen = P->g[j]+P->dg[j]*k;
      m_rgbPalette[i].rgbBlue  = pPalette->palPalEntry[i].peBlue  = P->b[j]+P->db[j]*k;
      m_rgbPalette[i].rgbReserved = 0;
      pPalette->palPalEntry[i].peFlags = 0;//PC_RESERVED; ????
      if ( ++k > I0 ) {
         if ( j+1 > P->N ) {
         } else {
            j++;
            k=0;
         }
      }
   }
   *(DWORD *)&(pPalette->palPalEntry[31])=RGB(255,255,255);
   *(DWORD *)&(m_rgbPalette[31])=RGB(255,255,255);
   ////////////////////////////////////////////
   VERIFY(m_Palette.CreatePalette( pPalette ));
   delete [] (BYTE *)pPalette;
}

#define WIDTHBYTES(bits)  ((((bits) + 31) / 32) * 4)
#define WIDTHBYTES8(bytes) ((bytes+3) & (~3))

void CGraphWndSV::CreateBitmap()
{
   if (m_pMemDC != NULL)
   {
      m_pMemDC->SelectObject(m_pOldBitmap);
      delete m_pMemDC;
      m_pMemDC = NULL;
      m_pOldBitmap = NULL;
   }

   if (m_Bitmap.m_hObject != NULL)
      m_Bitmap.DeleteObject();

   CRect rect;
   CSize size;

   GetClientRect(&rect);
   size = rect.Size();
   LPBITMAPINFO lpbi;

   // Fill in the BITMAPINFOHEADER
   lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD))];
   lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   lpbi->bmiHeader.biWidth = size.cx;
   lpbi->bmiHeader.biHeight = (m_dwStyle & GDS_NOTY) ? size.cy : -size.cy;
   lpbi->bmiHeader.biPlanes = 1;
   lpbi->bmiHeader.biBitCount = 8;
   lpbi->bmiHeader.biCompression = BI_RGB;
   lpbi->bmiHeader.biSizeImage = WIDTHBYTES8((DWORD)size.cx) * size.cy;
   lpbi->bmiHeader.biXPelsPerMeter = 0;
   lpbi->bmiHeader.biYPelsPerMeter = 0;
   lpbi->bmiHeader.biClrUsed = 32;
   lpbi->bmiHeader.biClrImportant = 32;

   // Fill in the color table
   //memset( lpbi->bmiColors, 0, sizeof(RGBQUAD) * 256 );
   memcpy( lpbi->bmiColors, m_rgbPalette, sizeof(RGBQUAD) * 32 );

   CClientDC dc(this);
   //GdiFlush();
   HBITMAP hBitmap = CreateDIBSection( NULL, lpbi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0 );

   ASSERT(hBitmap != NULL);

   delete [] (BYTE *)lpbi;

   m_Bitmap.Attach( hBitmap );

   m_pMemDC = new CDC;

   VERIFY( m_pMemDC->CreateCompatibleDC(&dc) );

//    CPalette *pOldPalette = dc.SelectPalette( &m_Palette, FALSE );
   dc.RealizePalette();
   m_pOldBitmap = m_pMemDC->SelectObject( &m_Bitmap );
}

CSize CGraphWndSV::GetBitmapSize()
{
   if (m_Bitmap.m_hObject == NULL)
      return CSize(0,0);

   BITMAP bm;
   m_Bitmap.GetObject( sizeof(BITMAP), &bm );
   return CSize(bm.bmWidth, bm.bmHeight);
}

void CGraphWndSV::GetBitmapSize(int *pcx, int *pcy)
{
   ASSERT (m_Bitmap.m_hObject != NULL);

   BITMAP bm;
   m_Bitmap.GetObject( sizeof(BITMAP), &bm );
   *pcx = bm.bmWidth; 
   *pcy = bm.bmHeight;
}

void CGraphWndSV::Update()
{
   if ( !m_pBits || !pZ || nX < 2 || nY < 2 ) return;
   //if ( Zmin == Zmax ) return;
   int cx,cy;

   GetBitmapSize(&cx, &cy);

   if ( Zmin == Zmax ) 
   {
      ZeroMemory(m_pBits, WIDTHBYTES8((DWORD)cx) * cy);
      return ;
   }

   if ( m_dwStyle & GDS_HORZ ) 
   {
      UpdateHorz(cx,cy);
      return;
   } 
   else
   {
      if ( m_dwStyle & GDS_VERT ) 
      {
         UpdateVert(cx,cy);
         return;
      }
   }

   int x, y;
   BYTE* pRow;

   int kx, ky, nx1, ny1;//, dx, dy;
   bool bChg;

   nx1 = nX-1;
   ny1 = nY-1; 
   kx = -1;
   ky = -1;

   bChg = true;
   int a,b,c,d;
   int ix,iy;
   int begpt;
   int endpt;
   int A,B,C;

   for ( y = 0; y < cy; y++,iy++ ) 
   {
      pRow = (m_pBits + WIDTHBYTES8((DWORD)cx) * y);

      int ty = y*ny1/cy;
      if ( ty != ky ) 
      {
         ky = ty;
         iy = 0;
         bChg = true;
      }
      
      for (x = 0; x < cx; x++,ix++) 
      {
         int tx = x*nx1/cx;
         if ( tx != kx ) 
         {
            kx = tx;
            ix = 0;
            bChg = true;
         }
         if ( bChg ) 
         {
            
            int t = ky*nX+kx;
            if ( m_dwStyle & GDS_LOGDBZ ) 
            {
               a = int(31*RangeZmm(LogDB(pZ[t])));
               t++;
               b = int(31*RangeZmm(LogDB(pZ[t])));
               t+=nX;
               d = int(31*RangeZmm(LogDB(pZ[t])));
               t--;
               c = int(31*RangeZmm(LogDB(pZ[t])));
            } 
            else 
            {
               a = int(31*RangeZmm(pZ[t]));
               t++;
               b = int(31*RangeZmm(pZ[t]));
               t+=nX;
               d = int(31*RangeZmm(pZ[t]));
               t--;
               c = int(31*RangeZmm(pZ[t]));
            }

            begpt = ((2*a+1)*(cy-1)+2*(c-a)*ny1*iy)/(2*(cy-1));
            endpt = ((2*b+1)*(cy-1)+2*(d-b)*ny1*iy)/(2*(cy-1));
            C = (cx-1);
            A = (2*begpt+1)*C;
            B = (endpt-begpt)*nx1;
            B <<=1;//+= B;
            C <<=1;//+= C;
            bChg = false;
         }

         DWORD e = (A+B*ix)/C;
         if ( e > 31 ) e = 31;
         *pRow++ = (BYTE)e;
      }
   }

}

void CGraphWndSV::UpdateHorz(int cx, int cy)
{
   int x, y;
   BYTE* pRow;

   int kx, nx1;
   bool bChg;

   nx1 = nX-1;

   kx = -1;

   bChg = true;

   int a,b;//,c,d;

   int ix;

   int begpt;
   int endpt;
   int A,B,C;

   if ( cy > nY ) cy = nY;

   for ( y = 0; y < cy; y++ ) 
   {
      pRow = (m_pBits + WIDTHBYTES8((DWORD)cx) * y);
      bChg = true;
      
      for (x = 0; x < cx; x++,ix++) 
      {
         int tx = x*nx1/cx;
         if ( tx != kx ) 
         {
            kx = tx;
            ix = 0;
            bChg = true;
         }
         if ( bChg ) 
         {
            int t = y*nX+kx;
            if ( m_dwStyle & GDS_LOGDBZ ) 
            {
               a = int(31*RangeZmm(LogDB(pZ[t])));
               t++;
               b = int(31*RangeZmm(LogDB(pZ[t])));
            } else 
            {
               a = int(31*RangeZmm(pZ[t]));
               t++;
               b = int(31*RangeZmm(pZ[t]));
            }

            begpt = (2*a+1)/2;
            endpt = (2*b+1)/2;
            C = (cx-1);
            A = (2*begpt+1)*C;
            B = (endpt-begpt)*nx1;
            B <<=1;//+= B;
            C <<=1;//+= C;
            bChg = false;
         }

         DWORD e = (A+B*ix)/C;
         if ( e > 31 ) e = 31;
         *pRow++ = (BYTE)e;
      }
   }

}

void CGraphWndSV::UpdateVert(int cx, int cy)
{
   int x, y;
   BYTE* pRow;

   int ky, ny1;//, dx, dy;
   ny1 = nY-1; 
   ky = -1;

   int a,/*b,*/c/*,d*/;
   int iy;
   int begpt;
   int A;

   for ( y = 0; y < cy; y++,iy++ ) 
   {
      pRow = (m_pBits + WIDTHBYTES8((DWORD)cx) * y);
      int ty = y*ny1/cy;
      if ( ty != ky ) 
      {
         ky = ty;
         iy = 0;
      }
      
      for (x = 0; x < cx; x++ ) 
      {
         {
            int t = ky*nX+x;
            if ( m_dwStyle & GDS_LOGDBZ ) 
            {
               a = int(31*RangeZmm(LogDB(pZ[t])));
               t+=nX;
               c = int(31*RangeZmm(LogDB(pZ[t])));
            } else 
            {
               a = int(31*RangeZmm(pZ[t]));
               t+=nX;
               c = int(31*RangeZmm(pZ[t]));
            }

            begpt = ((2*a+1)*(cy-1)+2*(c-a)*ny1*iy)/(2*(cy-1));
            A = (2*begpt+1);//*C;
         }

         DWORD e = A/2;
         if ( e > 31 ) e = 31;
         *pRow++ = (BYTE)e;
      }
   }
}

BOOL CGraphWndSV::OnEraseBkgnd(CDC* pDC) 
{
   return TRUE;
}

//DEL void CGraphWndSV::SetData(int nx, int ny, float *pdata, BOOL bRedraw,BOOL bUpdateMinMax)
//DEL {
//DEL    nX = nx;
//DEL    nY = ny;
//DEL    pData = pdata;
//DEL 
//DEL    if ( bUpdateMinMax ) if ( !UpdateMinMax() ) { // were errors
//DEL       nY = nX = 0;
//DEL       pData = NULL;
//DEL    }
//DEL 
//DEL    Update();
//DEL 
//DEL    if ( bRedraw ) {
//DEL       RedrawWindow();
//DEL    }
//DEL 
//DEL }

//DEL BOOL CGraphWndSV::UpdateMinMax()
//DEL {
//DEL    int i,j;
//DEL //   float f;
//DEL  
//DEL    Zmin = FLT_MAX;
//DEL    Zmax =-FLT_MAX;
//DEL 
//DEL    try {
//DEL 
//DEL       float *p = pData;
//DEL       if ( p ) {
//DEL          for ( i=0; i<nY ;i++ ) {
//DEL             for ( j=0; j<nX ;j++ ) {
//DEL                if ( *p < Zmin ) Zmin = *p;
//DEL                if ( *p > Zmax ) Zmax = *p;
//DEL                p++;
//DEL             }
//DEL          }
//DEL          /*
//DEL          if ( m_dwStyle & GDS_LOGDBZ ) {
//DEL             Zmin = LogDB(Zmin);
//DEL             Zmax = LogDB(Zmax);
//DEL          }
//DEL          */
//DEL          
//DEL          if ( m_dwStyle & GWS_Zmax20 ) {
//DEL             Zmax /= 20;
//DEL             Zmin /= 20;
//DEL          }
//DEL          if ( m_dwStyle & GWS_MIND0 ) {
//DEL             if ( Zmin > 0 ) Zmin = 0;
//DEL          }
//DEL       }
//DEL     /*
//DEL       if ( m_dwStyle & GDS_LOGDBY ) {
//DEL          //if ( Ymin == 0.0f || Ymax == 0.0f ) return FALSE;
//DEL          Ymin = LogDB(Ymin);
//DEL          Ymax = LogDB(Ymax);
//DEL       }
//DEL 
//DEL       if ( m_dwStyle &  GDS_XMINMAX10 ) { // x выровн€ть по 10 уровню
//DEL          long t = (long)Xmin;
//DEL          t = (t/10)*10;
//DEL          Xmin = (float)t;
//DEL          t = (long)Xmax;
//DEL          t = ((t+9)/10)*10;
//DEL          Xmax = (float)t;
//DEL       }
//DEL 
//DEL       if ( m_dwStyle &  GDS_YMINMAX10 ) { // Y выровн€ть по 10 уровню
//DEL          long t = (long)Ymin;
//DEL          t = (t/10)*10;
//DEL          Ymin = (float)t;
//DEL          t = (long)Ymax;
//DEL          t = ((t+9)/10)*10;
//DEL          Ymax = (float)t;
//DEL       }
//DEL    
//DEL       if ( m_dwStyle & GDS_MINY0 ) {
//DEL          if ( Ymin > 0 ) Ymin = 0;
//DEL       }
//DEL       if ( m_dwStyle & GDS_MINX0 ) {
//DEL          if ( Xmin > 0 ) Xmin = 0;
//DEL       } 
//DEL       */
//DEL       _asm wait;
//DEL 
//DEL    } catch(...) {
//DEL       _clear87();
//DEL       return FALSE;
//DEL    }
//DEL    if ( Zmin == Zmax ) return FALSE;
//DEL 
//DEL    return TRUE;
//DEL }

//DEL float CGraphWndSV::RangeDmm(float d)
//DEL {
//DEL    return (d-Zmin)/(Zmax-Zmin);
//DEL }
/*
// TODO: Add your message handler code here and/or call default
++m_nPalette %= NPALETTES;
CreatePalette(m_nPalette);
CreateBitmap();
Update();
//   InvalidateRect(0);
//   UpdateWindow();
//RedrawWindow();
Invalidate();
//CWnd::OnLButtonDown(nFlags, point);
*/

void CGraphWndSV::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CWnd *pParent = GetParent();
   if ( pParent == NULL ) return;

//    m_clkPt = point;
//    NMHDR hdr;
//    hdr.hwndFrom = m_hWnd;
//    hdr.idFrom = GetDlgCtrlID();
//    hdr.code = NM_CLICK;
// 
//    pParent->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&hdr);

   StdMouseNotifyParent(m_hWnd, NM_CLICK, point.x, point.y);

   if ( nFlags & MK_CONTROL ) {
      ++m_nPalette %= NPALETTES;
      CreatePalette(m_nPalette);
      CreateBitmap();
      Update();
      Redraw();
   }
   //CWnd::OnLButtonDown(nFlags, point);

}

void CGraphWndSV::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//    CWnd *pParent = GetParent();
//    if ( pParent == NULL ) return;

//    m_clkPt = point;
//    NMHDR hdr;
//    hdr.hwndFrom = m_hWnd;
//    hdr.idFrom = GetDlgCtrlID();
//    hdr.code = NM_DBLCLK;
// 
//    pParent->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&hdr);

   StdMouseNotifyParent(m_hWnd, NM_DBLCLK, point.x, point.y);
}

void CGraphWndSV::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   CMenu mpal;
   VERIFY(mpal.CreatePopupMenu());
   for ( int i=0; i<NPALETTES ;i++ ) {
      mpal.AppendMenu(MF_STRING,500+i,palettes[i].Name);
      if ( i == m_nPalette ) {
         mpal.CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
      }
   }

   CMenu menu;
   VERIFY(menu.LoadMenu(IDR_POPUP_GRAPH_WND));
   CMenu* pPopup = menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);
   if (m_dwStyle & GDS_ZMAX20 ) pPopup->CheckMenuItem(ID_POPUP_X20,MF_BYCOMMAND|MF_CHECKED);
   if (m_dwStyle & GDS_LOGDBZ ) pPopup->CheckMenuItem(ID_POPUP_LOGDB,MF_BYCOMMAND|MF_CHECKED);
   pPopup->AppendMenu(MF_STRING|MF_POPUP,(UINT)(HMENU)mpal,_T("ѕалитра"));

   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,this);

}





BOOL CGraphWndSV::OnCommand(WPARAM wParam, LPARAM lParam) 
{

   if ( LOWORD(wParam) >= 500 && LOWORD(wParam) < 500+NPALETTES ) {
      m_nPalette = LOWORD(wParam)-500;
      m_nPalette %= NPALETTES;
      CreatePalette(m_nPalette);
      CreateBitmap();
      Redraw();
      return TRUE;
   } else {
      return CWnd::OnCommand(wParam, lParam);
   }
   
}

void CGraphWndSV::OnPopupX20() 
{
   m_dwStyle ^= GDS_ZMAX20;
   UpdateMinMax(0);
   Redraw();
}

void CGraphWndSV::OnPopupDB() 
{
   m_dwStyle ^= GDS_LOGDBZ;
   UpdateMinMax(0);
   //Update();
   //Invalidate();////RedrawWindow();
   Redraw();
}
/*
void CGraphWndSV::OnPopupZoomin() 
{
   Zmax /= 2;
   if ( Zmax <= 0 ) Zmax = 1;
   //UpdateMinMax();
   Redraw();
}

void CGraphWndSV::OnPopupZoomout() 
{
   Zmax *= 2;
   //UpdateMinMax();
   Redraw();
}
*/

void CGraphWndSV::OnPopupInfo() 
{
   CPoint pt;
   GetCursorPos(&pt);
   ScreenToClient(&pt);
   CString s;
   s.Format(_T(
      "nX=%d,\t"
      "nY=%d\n"
      "Zmin=%g,\tZmax=%g"
      ),
       nX,
       nY,
      Zmin, Zmax);
   m_pInfo = new CInfoPopupWnd(this,pt,s,&m_pInfo);
}


void CGraphWndSV::Redraw()
{
// gg  ASSERT(IsWindowVisible());
   Update();
   CString s;
   
     if ( m_pInfo ) {
        s.Format(_T(
           "nX=%d,\t"
           "nY=%d\n"
           "Zmin=%g,\tZmax=%g"
           ),
           nX,
           nY,
           Zmin, Zmax);
        m_pInfo->SetWindowText(s);
     }

   Invalidate();
//    static t=0;
//    CClientDC dc(this);
//    dc.SelectStockObject(DEFAULT_GUI_FONT);
//    
//    dc.TextOut(0,t*20,s);
//    t^=1;
}



