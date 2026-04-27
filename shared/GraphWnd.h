#pragma once

// GraphWnd.h : header file
//
#include "GraphDisp.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphWnd window
#define id_POPUP_INFO  400
#define id_POPUP_X20   401
#define id_POPUP_LOGDB 402
#define id_POPUP_BAR   403

class CGraphWnd : public CWnd, public CGraph3dData
{
// Construction
public:
   CGraphWnd();
   CSize GetBitmapSize();
   void GetBitmapSize(int *pcx, int *pcy);

   //{{AFX_VIRTUAL(CGraphWnd)
   protected:
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void OnDraw(CDC &dc);
   //}}AFX_VIRTUAL

// Implementation
public:
	void UpdateVert(int cx, int cy);
	void UpdateHorz(int cx, int cy);
   void UpdateBar(int cx, int cy);
   virtual void Redraw(void);
   CPalette * GetPalette(void);
   void Init(DWORD dwStyle, int nPal = 4);
   virtual ~CGraphWnd();
   // Generated message map functions
protected:
   void Update(void);
   void CreateBitmap();
   void CreatePalette(int nPal);
   //{{AFX_MSG(CGraphWnd)
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
   afx_msg void OnPopupBar();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   ////////////////////////////////////
   CPalette m_Palette;
   int      m_nPalette;
   CDC* m_pMemDC;
   CBitmap m_Bitmap;
   CBitmap *m_pOldBitmap;
   BYTE* m_pBits;

   static TGDPalette palettes[];
   RGBQUAD m_rgbPalette[32];
   CInfoPopupWnd *m_pInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

