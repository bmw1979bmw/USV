#if !defined(AFX_GRAPHDISPSIG_H__62AE0DF4_7F02_11D4_9D11_0060970CEBA8__INCLUDED_)
#define AFX_GRAPHDISPSIG_H__62AE0DF4_7F02_11D4_9D11_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphDispSig.h : header file
//
#include "Complex.h"
#include "GraphDisp.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphDispSig window

class CGraphDispSig : public CGraphDisp
{
// Construction
public:
	CGraphDispSig();

	//{{AFX_VIRTUAL(CGraphDispSig)
	virtual void OnDraw(CDC &dc);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetData(int nLen, Complex *pdata, BOOL bRedraw=TRUE);
	virtual ~CGraphDispSig();
public:
   int m_xclk;

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphDispSig)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDISPSIG_H__62AE0DF4_7F02_11D4_9D11_0060970CEBA8__INCLUDED_)
