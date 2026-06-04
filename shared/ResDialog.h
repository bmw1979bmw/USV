#if !defined(AFX_RESDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
#define AFX_RESDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ResDialog.h : header file
//
#include "GridCtrl.h"
#include "MAcl.h"
#include "LNSignals.h"
#include "LRange.h"

#include "SVWave.h"
#include "SpAnalizer.h"

/////////////////////////////////////////////////////////////////////////////
// CResDialog dialog

class CResDialog : public CDialog
{
// Construction
public:
// 	CMAcl * m_pAcl;
   CResDialog( int NN, float *MP, float *MS, float *MOSP, float osp, CWnd* pParent = NULL);   // standard constructor

   int N_DF;

   float m_OSP;
   float OSP[128];
// Dialog Data
	//{{AFX_DATA(CResDialog)
	enum { IDD = IDD_RESDIALOG };
	CGridCtrl2	m_GridOSP;
   CSpAnalizer m_SpAnalizer;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
