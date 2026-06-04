#if !defined(AFX_RADIODIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
#define AFX_RADIODIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RadioDialog.h : header file
//
#include "GridCtrl.h"
#include "RadioSignal.h"

/////////////////////////////////////////////////////////////////////////////
// CRadioDialog dialog

class CRadioDialog : public CDialog
{
// Construction
public:
   CRadioDialog( CRadioSignal* mRadio, CWnd* pParent = NULL);   // standard constructor

   float      m_S[99];   // сигнал
   float      m_N[99];   // шум
   float      m_P[99];   // помеха
   float      m_H2[99];  // ОСП 
   float      m_OSP;     // пороговое ОСП 


// Dialog Data
	//{{AFX_DATA(CRadioDialog)
	enum { IDD = IDD_RADIODIALOG };
	CGridCtrl2	m_GridOSP;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRadioDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIODIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
