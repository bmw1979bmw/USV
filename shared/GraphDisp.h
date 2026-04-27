#pragma once
// GraphDisp.h : header file
//
#include "InfoPopupWnd.h"

#define GDS_MINY0     0x0001  /* если Y > 0, то минимальный Y = 0 */
#define GDS_MINX0     0x0002 /* если X > 0, то минимальный X = 0 */
#define GDS_MINZ0     0x0004 /* если D > 0, то минимальный D = 0*/ 
#define GDS_NOZEROX   0x0008  /* по шкале X игнорировать нулевые значения */
#define GDS_NOZEROY   0x0010  /* по шкале Y игнорировать нулевые значения */
#define GDS_LOGDBY    0x0020  /* по Y - логарифмическая шкала */
#define GDS_LOGDBZ    0x0040 /* данные в pData - логарифм */
#define GDS_NOTY      0x0080 /* y - вниз */
#define GDS_NOTX      0x0100 /* x - вниз для режима GDS_YX */
#define GDS_ZMAX20    0x0200 /* Dmax /= 20*/

#define GDS_BAR       0x0400 /* отображать в виде дискретов */
#define GDS_YX        0x0800 /* поменять местами x и y & y вниз */
#define GDS_DIAGRAM   0x1000 /* тип - диаграмма */
#define GDS_LONKMAP   GDS_DIAGRAM /* subtype of COLORMAP */
#define GDS_COLORMAP  0x2000 /* тип - smart */
#define GDS_NOBELOWY  0x4000 /* no below Ymax*/
#define GDS_PAL       0x8000 /* палитра */
#define GDS_RAYS      0x10000 /* лучи для лучевой структуры */
#define GDS_RAYS1     0x0100  /* лучи для лучевой структуры */
//#define GDS_RAYS2     0x0200  /* лучи для лучевой структуры */
#define GDS_NAMEFIRST 0x20000
#define GDS_ZNUM      0x40000  /* если pData, *(UINT *)pData[i] - number for drawing */
#define GDS_NOAXES    0x80000  /* без осей */
#define GDS_THRUBMP   0x100000 /* рисовать через битмапку */
#define GDS_VECTORS   0x200000 /* строчки графиков */
#define GDS_TEST      0x400000 /* отладочный параметр */ 
// #define GDS_XMINMAX10 0x800000 /* x выровнять по 10 уровню */
// #define GDS_YMINMAX10 0x1000000 /* y выровнять по 10 уровню */
#define GDS_ZMIN0ALW  0x2000000 /* Zmin == 0 всегда */
#define GDS_AFTERDRAW 0x4000000 /* send WM_DRAWITEM после Paint */
#define GDS_HORZ      0x8000000  /* горизонтальный (водопад) */
#define GDS_VERT      0x10000000 /* вертикальный (водопад) */

struct TGDPalette
{
   int  N;
   int  r[3];
   int  g[3];
   int  b[3];
   int  dr[3];
   int  dg[3];
   int  db[3];
   char Name[20];
};

typedef BOOL BOOL;
typedef UINT UINT;

class CGraph2dData  
{
public:
   virtual void Redraw(void) = 0;
   void SetData(int nx,float * px,float * py,
      int xstep=4,int ystep=4,BOOL bUpdateMinMax = TRUE,BOOL bRedraw=TRUE);
   BOOL UpdateMinMax(void);
   float LogDB(float val);

   CGraph2dData();
   virtual ~CGraph2dData();

   int GetnX() { return nX; }
   int GetnY() { return nY; }

   void  DoStepX( float * &px ) { px = (float *)(((BYTE *)px) + nXStep); }
   void  DoStepY( float * &py ) { py = (float *)(((BYTE *)py) + nYStep); }
   float RangeYmm(float y);
   float RangeXmm(float x);
   void SetStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
   void NormalizeMinMax(float & Amin, float & Amax);
   void  Unbind(){ pX = pY = NULL; }
   /////////////////////////////////////////
   DWORD m_dwStyle;
   int nX, nY;
   float *pX, Xmin, Xmax;
   int nXStep;
   float *pY, Ymin, Ymax;
   int nYStep;
};


class CGraph3dData : public CGraph2dData
{
public:
   void SetZmaxMul(float n, BOOL bRedraw = TRUE);
   void SetZmaxDiv(float n, BOOL bRedraw = TRUE);
   void  SetMinMaxToShared(float Lsens);
   BOOL UpdateMinMax(BOOL bShared=0);
   float RangeZmm(float z);
   float LogDBz(float val);

   CGraph3dData();
   virtual ~CGraph3dData();

   void SetData(int nx,float * px,float * py,
      int xstep=4,int ystep=4,UINT nUpdateMinMax = 1,BOOL bRedraw=TRUE)
   {
         CGraph2dData::SetData(nx, 
                           px, py, 
                           xstep, ystep, 
                           nUpdateMinMax, bRedraw);
   }

   void SetData(int nx, int ny, float *px, float *py, float *pz,
                int xstep = 4, int ystep = 4,
                UINT nUpdateMinMax = 1,
                BOOL bRedraw = TRUE );
   void  Unbind(){ pX = pY = pZ = NULL; }

   //////////////////////////////////////////////////////////////
   float *pZ, Zmin, Zmax;
   static float ms_Zmin, ms_Zmax;
};


#ifndef _DEBUG
inline float CGraph3dData::RangeZmm(float z)
{
   float f = (Zmax-Zmin);
   if ( f == 0 ) return 0;
   return (z-Zmin)/f;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDisp window

#define NMAXR 30

struct TRayGraph {
   int   nRay;
   float val;
   float width; // 0...1
   TRayGraph(){nRay = -1; }
};
struct NMGRCLICK : public tagNMMOUSE {
    CSize   sz;
};
typedef  NMGRCLICK NMGRMOUSE;
                            
class CGraphDisp : public CWnd, public CGraph3dData
{
// Construction
public:
   CGraphDisp();

// Attributes
public:
// Operations
public:

// Overrides
   //{{AFX_VIRTUAL(CGraphDisp)
   protected:
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void OnDraw(CDC &dc);
   virtual void AfterDraw(CDC &dc, int width, int height);
   //}}AFX_VIRTUAL

// Implementation
public:
	int DrawScales(CDC &dc);
   virtual void Redraw(void);
   void DrawColorMapBar(CDC & dc, int width, int height, int style=0);
   static TGDPalette palettes[];
   BOOL Init(UINT idsx, UINT idsy, DWORD dwStyle,LONG param1=0,LONG param2 = 0);
   void CreatePalette(int nPal,int nument=32, TGDPalette *srcP = NULL);
   virtual ~CGraphDisp();
   // Generated message map functions
protected:
   void RedrawWindow(void);
   
   void DrawVectors(CDC &dc, int width, int height,int style=0);
   void DrawVectorsTest(CDC &dc, int width, int height,int style=0);
   void DrawPalette(CDC &dc, int width, int height,int style=0);
   void DrawColorMap(CDC &dc, int width, int height,int style=0);
   void DrawRays(CDC &dc, int width, int height,int style=0);
   void DrawColorTest(CDC &dc);
   //{{AFX_MSG(CGraphDisp)
   afx_msg void OnPaint();
   afx_msg void OnPopupLogdb();
   afx_msg void OnPopupX20();
   afx_msg void OnPopupBar();
   afx_msg void OnPopupLonkMap();
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnClose();
   afx_msg void OnWhatIsIt();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnPopupTest();
   afx_msg void OnPopupInfo();
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
protected:
   int m_nScrollType;
   void ChangePalette(int d, int pos=0);
   LOGPALETTE * pPalette;
   int nPalette;
   UINT idsX;
   UINT idsY;
   CFont m_fnt;
   HWND  m_hSavParent;
   RECT  m_SavRect;
   UINT  m_SavID;
   UINT  m_nHelpId;
   CRect m_Rg;
   CInfoPopupWnd *m_pInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

