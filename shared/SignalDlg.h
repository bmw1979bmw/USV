#if !defined(AFX_SIGNALDLG_H__83EA5823_D4AB_11D4_B6C2_0050DAC48A11__INCLUDED_)
#define AFX_SIGNALDLG_H__83EA5823_D4AB_11D4_B6C2_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SignalDlg.h : header file
#include "GraphDisp.h"
#include "GraphDispSig.h"
#include "LNSignals.h"
#include "GridCtrl2.h"
#include "SignalDlgRes.h"
/////////////////////////////////////////////////////////////////////////////
// CSignalDlg dialog

class CSignalDlg : public CDialog
{
// Construction
public:
	void SetData(BOOL bGrid = TRUE);
	CSignalDlg(LSSignal *pSignal,LPCTSTR pHead,CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CSignalDlg)
	enum { IDD = IDD_SIGNAL };
   CGraphDispSig m_GDSignal0;
   CGraphDispSig m_GDSignal1;
	CGridCtrl2	m_Grid;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CSignalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSignalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFft();
	afx_msg void OnFft2();
	afx_msg void OnCreate();
	afx_msg void OnItemchangedGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClear();
	afx_msg void OnMirror();
	afx_msg void OnClickGdsignal0(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickGdsignal1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAbstest();
	afx_msg void OnPower();
	afx_msg void OnIdxPower();
	afx_msg void OnAbsToRe();
	afx_msg void OnSaveAsText();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   LSSignal *m_pSignal;
   LSSignal m_Signal;
   LPCTSTR m_pHead;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNALDLG_H__83EA5823_D4AB_11D4_B6C2_0050DAC48A11__INCLUDED_)
