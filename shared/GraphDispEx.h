#if !defined(AFX_GRAPHDISPEX_H__6E790154_53DC_11D4_9CE6_0060970CEBA8__INCLUDED_)
#define AFX_GRAPHDISPEX_H__6E790154_53DC_11D4_9CE6_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphDispEx.h : header file
//
#include "GraphDisp.h"
#include "structures.h"
#include "TickPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphDispEx window
class CGraphDispEx : public CGraphDisp, public CTickPoints
{
// Construction
public:
	CGraphDispEx();

// Attributes
public:

// Operations
public:
   void AfterDraw(CDC & dc, int width, int height);
	//{{AFX_VIRTUAL(CGraphDispEx)
   virtual void OnDraw(CDC &dc);
	//}}AFX_VIRTUAL

public:
	virtual ~CGraphDispEx();

protected:
	//{{AFX_MSG(CGraphDispEx)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDISPEX_H__6E790154_53DC_11D4_9CE6_0060970CEBA8__INCLUDED_)
