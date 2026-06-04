#if !defined(AFX_GRAPHWNDEXSV_H__4D81A437_A994_11D4_B654_0050DAC48A11__INCLUDED_)
#define AFX_GRAPHWNDEXSV_H__4D81A437_A994_11D4_B654_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphWndEx.h : header file
//
#include "GraphWndSV.h"
/////////////////////////////////////////////////////////////////////////////
// CGraphWndSV window
#include "TickPointsSV.h"

class CGraphWndExSV : public CGraphWndSV, public CTickPointsSV
{
// Construction
public:
	CGraphWndExSV();

// Attributes
public:

// Operations
public:
   void AfterDraw(CDC & dc, int width, int height);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphWndExSV)
   virtual void OnDraw(CDC &dc);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphWndExSV();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphWndExSV)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//    struct TTickPoint
//    {
//       float ahor;
//       float aver;
//       int   type;
//       float val;
//    };
//    typedef CList<TTickPoint, TTickPoint &> CTickList;
//    
//    CTickList m_ptList;
//   CFont m_fnt;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHWNDEXSV_H__4D81A437_A994_11D4_B654_0050DAC48A11__INCLUDED_)
