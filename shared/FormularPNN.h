#if !defined(AFX_FORMULARPNN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
#define AFX_FORMULARPNN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormularPNN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormularPNN dialog
#include "stru_rgab.h"

class CFormularPNN : public CDialog
{
// Construction
public:
	//sv CFormularPNN(CWnd* pParent = NULL);   // standard constructor
	CFormularPNN(TFormularPNN *psrc,CWnd* pParent = NULL);   // standard constructor

   TFormularPNN p;
   TFormularPNN *p1;
   int mnR;
   int mnW;

// Dialog Data
	//{{AFX_DATA(CFormularPNN)
	enum { IDD = IDD_FORMULARPNN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormularPNN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormularPNN)
	afx_msg void OnSaveForm();
	afx_msg void OnAddForm();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULARPNN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
