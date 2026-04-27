// USV.h : main header file for the USV application
//

#if !defined(AFX_USV_H__366DCE95_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
#define AFX_USV_H__366DCE95_1140_11D5_B73F_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSVApp:
// See USV.cpp for the implementation of this class
//
// 
// class CUSVApp : public CWinApp
#include "StandApp.h"

class CUSVApp : public CStandApp //CWinApp

{
public:
   CUSVApp();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CUSVApp)
   public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL

// Implementation
   //{{AFX_MSG(CUSVApp)
   afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#include "structures.h"
#include "WorldClient.h"
#include "CmdLine.h"
#include "TXParamsDlg.h"
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USV_H__366DCE95_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
