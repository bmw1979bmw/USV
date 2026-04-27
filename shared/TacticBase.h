#pragma once

// TacticBase.h : header file

#include "structures.h"

/////////////////////////////////////////////////////////////////////////////
// CTacticBase view
#define FLAG_AUTOSIZE     1
#define FLAG_NOBLINK      2
#define FLAG_DRAWGRID     8
#define FLAG_DRAWNOTE     16
#define FLAG_DRAWEXISTS   64
#define FLAG_SHORTNOTE    128
#define FLAG_ROTATE_ICONS 256    /* build 445 */
#define FLAG_KEEP_LIMITS  512    /* 446 */

//monitor  #define FLAG_NODRAWPATT 1024
//monitor  #define FLAG_DRAWACF    2048

#define FLAG_SHOW_REDBLUE  4096    /* 447 */

class CTacticBase : public CScrollView
{
public:
protected:
   CBitmap m_bmp;
   DWORD   m_dwFlags;
   float   m_scale;

   CPoint  m_vptCaptured;
   BOOL    m_bCaptured;
   BOOL    m_bWasMouseMove;

   CRect   m_robj;  // мин/макс значения координат объектов
   BOOL    m_bUpdateRObjFirst; // признак первого объекта для UpdateRSize
   CRect   m_rsize; // реальные координаты отображения - по ним работают преобразования

   CFont   m_fnt;
public:
   struct CSizeNote:public CSize{int textcy;} m_sizenote;//размер подписей к объекту
   CSize   m_objorg;  // origin for objects
protected:
   BOOL    m_bFontChanged;
   LOGFONT m_LogFont;
   CPen    m_LtBkPen;

protected:
	CTacticBase();           // protected constructor used by dynamic creation
 	DECLARE_DYNAMIC(CTacticBase)

// Attributes
protected:
//   CMainDoc* GetDocument_() { return (CMainDoc*)m_pDocument; }

public:
   enum { ICONCX = 34, ICONCY = 16};

   DWORD GetFlags() const { return m_dwFlags; }

protected:

#ifndef DEF_BKBRUSH
#  ifdef DEMO
#define DEF_BKBRUSH  0x00D1D29B
#  else
#define DEF_BKBRUSH  0x00E2C9B4
#  endif
#endif

#ifndef DEF_SELTEXT
#define DEF_SELTEXT  0x00DCFFFF
#endif

#ifndef DEF_BLENDCOLOR
#define DEF_BLENDCOLOR  0x0099CCFF
#endif


//public:
   COLORREF BKBRUSH;  // 0xDECB7B
   COLORREF LTBKBRUSH;//=0x00E6D79B,
   COLORREF DKBKBRUSH;//=0x00D0B548,
   COLORREF DKCOLOR;  //=0x008A7624;
   COLORREF TEXTCLR;
   
protected:

   virtual BOOL IsDontDraw() = 0;

// Operations
protected:
	void Round1000rsize(void);
   void CalcCarrierRect(CRect &rc, POINT &carPt);
   BOOL IsOnSpace(CPoint &pt);// const;
   void DrawGrid(CDC *pDC);
   void CreateDevBitmap();
public:
   void DrawCourse(CDC *pDC,CPoint p, TMParams *m, BOOL bAndDepth=TRUE);
   void RPtoDP(LPPOINT lpPoint, LPPOINT lpScroll=NULL, int nCount=1);
   void DPtoRP(LPPOINT lpPoint, LPPOINT lpScroll, int nCount=1);
   CPoint GetDeviceScrollPosition(void) const;
protected:
   void MakeRPVisible(CPoint pt);
   void ScrollToEx(CPoint pt);
	void SetScrollSizes(int nMapMode, SIZE sizeTotal,
				const SIZE& sizePage = sizeDefault,
				const SIZE& sizeLine = sizeDefault);
   void MakeScaleToFit();
   void OnePointSize(const CPoint &pt);// обновить в прямоугольнике с макс. координатами объектов
   void UpdateScrollSizes();
   void UpdateAll();
	void InitSizeNote();
   void InitColors();

public:
   static BOOL distance_lt(POINT &pt1, POINT &pt2, int delta)
   {  // дистанция между двумя точками меньше указанной
      int dx = pt1.x - pt2.x;
      int dy = pt1.y - pt2.y;
      return dx*dx + dy*dy < delta*delta;
   }

protected:

   virtual void ExtraKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}
   virtual void DrawAllObjects(CDC *pDC)=0;

   virtual TXParams *UpdateRSize()=0; // обновить прямоугольник с макс. координатами объектов

   virtual void EditSelObjectParams() = 0;
   virtual void SelectNextObject(BOOL bShift) = 0;


	//{{AFX_VIRTUAL(CTacticBase)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTacticBase();

	// Generated message map functions
	//{{AFX_MSG(CTacticBase)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAutoSize();
	afx_msg void OnUpdateAutoSize(CCmdUI* pCmdUI);
   afx_msg void OnKeepLimits();
   afx_msg void OnUpdateKeepLimits(CCmdUI* pCmdUI);
	afx_msg void OnNoBlink();
	afx_msg void OnUpdateNoBlink(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnShowGrid();
	afx_msg void OnUpdateShowGrid(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowNote();
	afx_msg void OnUpdateShowNote(CCmdUI* pCmdUI);
	afx_msg void OnBkcolor();
	afx_msg void OnShortnote();
	afx_msg void OnUpdateShortnote(CCmdUI* pCmdUI);
	afx_msg void OnFont();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRotateIcons();
	afx_msg void OnUpdateRotateIcons(CCmdUI* pCmdUI);
	//}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

