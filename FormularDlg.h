#if !defined(AFX_FORMULARDLG_H__159A8B44_1206_11D5_B8C7_005004552ACE__INCLUDED_)
#define AFX_FORMULARDLG_H__159A8B44_1206_11D5_B8C7_005004552ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormularDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormularDlg dialog
#include <MainDoc.h>

class CFormularDlg : public CDialog
{
// Construction
public:
	CFormularDlg(TFormularUSV *psrc,CWnd* pParent = NULL);   // standard constructor

   TFormularUSV p;
   TFormularUSV *p1;


// Dialog Data
	//{{AFX_DATA(CFormularDlg)
	enum { IDD = IDD_FORMULAR };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormularDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormularDlg)
	virtual void OnOK();
	afx_msg void OnSaveForm();
	afx_msg void OnAddForm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULARDLG_H__159A8B44_1206_11D5_B8C7_005004552ACE__INCLUDED_)
