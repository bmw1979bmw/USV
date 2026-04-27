#pragma once

// TXParamsDlg.h : header file
/////////////////////////////////////////////////////////////////////////////
// CTXParamsDlg dialog

#include "structures.h"

class CTXParamsDlg : public CDialog
{
   static UINT ms_fID; // focused item
   static BOOL ms_bSI; // система си
   enum { SI_COLOR = 0x00FFF3DD };
   CBrush   m_br;
   BOOL     m_bAutoOK; // скопировать обратно по кнопке ОК, иначе делать вручную

   void UpdateForSi();
public:
   explicit CTXParamsDlg(TXParams *psrc, CWnd* pParent = NULL,BOOL bAutoOK=TRUE); 
   explicit CTXParamsDlg(TMParams *psrc, CWnd* pParent = NULL,BOOL bAutoOK=TRUE);

   //{{AFX_DATA(CTXParamsDlg)
	enum { IDD = IDD_TXPARAMS };
	//}}AFX_DATA
   TXParams p;
   TXParams *m_pSrc;
protected:
   BOOL bExtend;
   BOOL bReadOnly;

   //{{AFX_VIRTUAL(CTXParamsDlg)
	protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CTXParamsDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
	afx_msg void OnSIClicked();
   afx_msg HBRUSH OnCtlColorEdit( HDC wParam, HWND lParam );
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

