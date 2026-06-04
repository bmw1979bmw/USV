#if !defined(AFX_FORMULARAN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
#define AFX_FORMULARAN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormularAN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormularAN dialog
#include "stru_rgab.h"

class CFormularAN : public CDialog
{
// Construction
public:
	//sv CFormularAN(CWnd* pParent = NULL);   // standard constructor
	CFormularAN(TFormularAN *psrc,CWnd* pParent = NULL);   // standard constructor

   TFormularAN p;
   TFormularAN *p1;
   int mnR;

// Dialog Data
	//{{AFX_DATA(CFormularAN)
	enum { IDD = IDD_FORMULARAN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormularAN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormularAN)
	afx_msg void OnSaveForm();
	afx_msg void OnAddForm();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULARAN_H__88DBA336_9AD7_11D6_AB02_0030482232DB__INCLUDED_)
