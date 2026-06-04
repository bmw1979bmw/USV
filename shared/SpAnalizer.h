#if !defined(AFX_SPANALIZER_H__A10DD9B3_03F3_11D5_B8BE_005004552ACE__INCLUDED_)
#define AFX_SPANALIZER_H__A10DD9B3_03F3_11D5_B8BE_005004552ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpAnalizer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpAnalizer window

class CSpAnalizer : public CWnd
{
// Construction
public:
	CSpAnalizer();

// Attributes
public:   
   float max_rev;
   float P1[260];
   float P2[260];
   float P_MAX;
   float F_MIN;
   float F_MAX;
   int    DF;
   int    N_DF;
   int TYPE;

// Operations
public:
       void OnDraw(CDC &dc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpAnalizer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpAnalizer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpAnalizer)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPANALIZER_H__A10DD9B3_03F3_11D5_B8BE_005004552ACE__INCLUDED_)
