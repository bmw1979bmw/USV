#if !defined(AFX_GRAPHWNDSV_H__00B12E56_A8AA_11D4_B650_0050DAC48A11__INCLUDED_)
#define AFX_GRAPHWNDSV_H__00B12E56_A8AA_11D4_B650_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphWnd.h : header file
//
#include "GraphDispSV.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphWndSV window

class CGraphWndSV : public CWnd,  public CGraph3dDataSV
{
// Construction
public:
   CGraphWndSV();
// Attributes
public:
//    CPoint m_clkPt;
   CSize GetBitmapSize();
   void GetBitmapSize(int *pcx, int *pcy);
// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGraphWndSV)
   protected:
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void OnDraw(CDC &dc);
   //}}AFX_VIRTUAL

// Implementation
public:
	void UpdateVert(int cx, int cy);
	void UpdateHorz(int cx, int cy);
   virtual void Redraw(void);
   CPalette * GetPalette(void);
   void Init(DWORD dwStyle, int nPal = 4);
   virtual ~CGraphWndSV();
   // Generated message map functions
protected:
   void Update(void);
   void CreateBitmap();
   void CreatePalette(int nPal);
   //{{AFX_MSG(CGraphWndSV)
   afx_msg void OnDestroy();
   afx_msg void OnPaint();
   afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
   afx_msg BOOL OnQueryNewPalette();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnPopupX20();
   afx_msg void OnPopupInfo();
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnPopupDB();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   ////////////////////////////////////
   CPalette m_Palette;
   int      m_nPalette;
   CDC* m_pMemDC;
   CBitmap m_Bitmap;
   CBitmap *m_pOldBitmap;
   BYTE* m_pBits;

   static TGDPaletteSV palettes[];
   RGBQUAD m_rgbPalette[32];
   CInfoPopupWnd *m_pInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHWNDSV_H__00B12E56_A8AA_11D4_B650_0050DAC48A11__INCLUDED_)
