#if !defined(AFX_FORMULARPN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
#define AFX_FORMULARPN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormularPN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormularPN dialog
#include "stru_rgab.h"

class CFormularPN : public CDialog
{
// Construction
public:
	//sv CFormularPN(CWnd* pParent = NULL);   // standard constructor
	CFormularPN(TFormularPN *psrc,CWnd* pParent = NULL);   // standard constructor

   TFormularPN p;
   TFormularPN *p1;
   int mnR;
   int mnW;

// Dialog Data
	//{{AFX_DATA(CFormularPN)
	enum { IDD = IDD_FORMULARPN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormularPN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormularPN)
	afx_msg void OnSaveForm();
	afx_msg void OnAddForm();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULARPN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
