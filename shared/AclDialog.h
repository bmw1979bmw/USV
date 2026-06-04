#if !defined(AFX_ACLDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
#define AFX_ACLDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AclDialog.h : header file
//
#include "GridCtrl.h"
#include "MAcl.h"
#include "LNSignals.h"
#include "LRange.h"
#include "SVWave.h"
#include "SpAnalizer.h"

/////////////////////////////////////////////////////////////////////////////
// CAclDialog dialog

class CAclDialog : public CDialog
{
// Construction
public:
	CMAcl * m_pAcl;
   CAclDialog(float dn_teta,float dn_fi,CSVWave *w,float time,LRange *R,LSSignal *S1,LSSignal *S2,      //float osp,float max_rev,
      float osp, CWnd* pParent = NULL);   // standard constructor

   float m_TIME;
   float m_DN_TETA;
   float m_DN_FI;
   int N_DF;

   float m_OSP;
   float OSP[128];
// Dialog Data
	//{{AFX_DATA(CAclDialog)
	enum { IDD = IDD_ACLDIALOG };
	CGridCtrl2	m_GridSignals;
	CGridCtrl2	m_GridPacks;
	CGridCtrl2	m_GridOSP;
   CSpAnalizer m_SpAnalizer;

//	CSpinButtonCtrl	m_SpinSignals;
//	CSpinButtonCtrl	m_SpinPacks;
	UINT	m_nPacks;
	UINT	m_nSignals;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAclDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAclDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACLDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
