// EditViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "EditViewEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditViewEx

IMPLEMENT_DYNCREATE(CEditViewEx, CEditView)

CEditViewEx::CEditViewEx() 
: 
m_hFRDlg(NULL), 
m_bDownFind(TRUE)
{
}

CEditViewEx::~CEditViewEx()
{
}

/*static*/ UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CEditViewEx, CEditView)
	//{{AFX_MSG_MAP(CEditViewEx)
   ON_COMMAND(ID_EDIT_FIND, OnEditFind)
   ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
   ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditViewEx message handlers

void CEditViewEx::OnEditFind() 
{
   if ( !IsWindow(m_hFRDlg) ) {
      CFindReplaceDialog *pFRDlg = new CFindReplaceDialog();  // Must be created on the heap
      pFRDlg->Create( TRUE, m_sFind, "", 
         m_bDownFind
         ? (FR_DOWN|FR_NOMATCHCASE|FR_NOWHOLEWORD|FR_HIDEMATCHCASE|FR_HIDEWHOLEWORD)
         : (FR_NOMATCHCASE|FR_NOWHOLEWORD|FR_HIDEMATCHCASE|FR_HIDEWHOLEWORD), 
         this ); 
      m_hFRDlg = pFRDlg->m_hWnd;
   } else {
      ::ShowWindow(m_hFRDlg, SW_SHOW);
      ::SetFocus(m_hFRDlg);
   }
}

void CEditViewEx::OnUpdateEditFind(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(1);
}

LRESULT StdFindReplace(CEdit &a, LPARAM lParam, CString &sFind, BOOL &bDown)
{
   CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier(lParam);
   
   if ( !pDlg || pDlg->IsTerminating() ) return 0;
   
   int nStartChar;
   int nEndChar;
   a.GetSel(nStartChar, nEndChar);
   int nlen = a.GetWindowTextLength();
   
   if ( nEndChar > nlen ) return 0;
   
   CString sText;
   
   a.GetWindowText(sText);
   sText.MakeUpper();
   CString sFindWhat = pDlg->GetFindString();
   sFind = sFindWhat;
   sFindWhat.MakeUpper();
   
   int tlast = -1;
   if ( pDlg->SearchDown() ) {
      bDown = TRUE;
      if ( nEndChar < 0 ) nEndChar = 0;
      tlast = sText.Find(sFindWhat, nEndChar);
   } else {
      bDown = FALSE;
      while (1) {
         int t = sText.Find(sFindWhat, tlast+1);
         if ( t < 0 ) break;
         if ( t >= nStartChar ) break;
         tlast = t;
      }
   }
   if ( tlast >= 0 ) {
      nStartChar = tlast;
      nEndChar = nStartChar+sFindWhat.GetLength();
      a.SetSel(nStartChar, nEndChar, FALSE);
   }
   
   return 0;
}


LRESULT CEditViewEx::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
   return StdFindReplace(GetEditCtrl(), lParam, m_sFind, m_bDownFind);
}