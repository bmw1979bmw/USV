#if !defined(AFX_DETECTORDLG_H__6E2765D5_2C22_11D6_B9CD_005004552ACE__INCLUDED_)
#define AFX_DETECTORDLG_H__6E2765D5_2C22_11D6_B9CD_005004552ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetectorDlg.h : header file
//
#include "LNSignals.h"
#include "LRange.h"
//#include "SignalDlg.h"
#include "GridCtrl.h"
#include "MainDoc.h"

//#include "Detector.h"
//#include "DNDisp.h"

/////////////////////////////////////////////////////////////////////////////
// CDetectorDlg dialog

class CDetectorDlg : public CDialog
{
// Construction
public:
	CDetectorDlg(CMainDoc *pDoc,LSSignal *S,LRange *R,float max_rev,float pow_s,int nom_rev,CWnd* pParent =NULL);   // standard constructor

   CMainDoc * m_pDocument;
   LSSignal * m_S;
   float P_MAX;
   float P[256];
   float m_pow_s;
   float PPR;
// Dialog Data
	//{{AFX_DATA(CDetectorDlg)
	enum { IDD = IDD_DETECT_DLG };
	CGridCtrl	m_GridOSP;
		// NOTE: the ClassWizard will add data members here
//   CDetector m_Detector;
//   CDNDisp   m_DNDisp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDetectorDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETECTORDLG_H__6E2765D5_2C22_11D6_B9CD_005004552ACE__INCLUDED_)
