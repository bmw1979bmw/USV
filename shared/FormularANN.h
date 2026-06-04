#if !defined(AFX_FORMULARANN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
#define AFX_FORMULARANN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormularANN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormularANN dialog
#include "stru_rgab.h"

class CFormularANN : public CDialog
{
// Construction
public:
	//sv CFormularANN(CWnd* pParent = NULL);   // standard constructor
	CFormularANN(TFormularANN *psrc,CWnd* pParent = NULL);   // standard constructor

   TFormularANN p;
   TFormularANN *p1;
   int mnR;

// Dialog Data
	//{{AFX_DATA(CFormularANN)
	enum { IDD = IDD_FORMULARANN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormularANN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormularANN)
	afx_msg void OnSaveForm();
	afx_msg void OnAddForm();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULARANN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
