#if !defined(AFX_PELENGDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
#define AFX_PELENGDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PelengDialog.h : header file
//
#include "GridCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPelengDialog dialog
    ;;
class CPelengDialog : public CDialog
{
// Construction
public:
   static int t;
    CPelengDialog(
       float t1, float t2, float Dist, float Bearing, float Lev,
       float porog, float maxporog, float maxosp,
       /*float *m_MPELENGNB, float *m_MPELENGAC,*/ float *m_MPELENG, /*float Bearing, float m_Pmax,*/ 
       /*float *m_MDistNB, float *m_MDistAC, float *m_MDist,*/ /*float Dist, float m_Dmax,*/
                  /*float T1, float T2,*/ CWnd* pParent = NULL);   // standard constructor

      float mPELENG[30];
//       float mDistNB[128];
//       float mDistAC[128];
//       float mDist[128];
//       float mDmax;
//       float mDistan;
// 

     float m_maxPOROG;
     float m_POROG;
     float m_OSP;

     float mt1;
     float mt2;
     float mBearing;
     float mDistan;
     float mLev;
     float mPorog;
     float mMaxPorog;
     float mMaxOSP;

// Dialog Data
	//{{AFX_DATA(CPelengDialog)
	enum { IDD = IDD_PELENGDIALOG };
	CGridCtrl2	m_GridPeleng;

   //}}AFX_DATA
// 	CGridCtrl	m_GridSignals;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPelengDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPelengDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PELENGDIALOG_H__E3647763_6846_11D4_9CF9_0060970CEBA8__INCLUDED_)
