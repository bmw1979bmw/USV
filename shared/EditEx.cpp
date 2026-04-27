// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "doclib.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEx

CEditEx::CEditEx()
: 
m_pFRDlg(NULL), 
m_bDownFind(TRUE)
{
}

CEditEx::~CEditEx()
{
}


extern /*static*/ UINT WM_FINDREPLACE;// = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
   ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

void CEditEx::OnEditFind() 
{
   if ( m_pFRDlg && m_pFRDlg->IsTerminating() ) {
      m_pFRDlg = NULL;
   }
   
   if ( !m_pFRDlg ) {
      m_pFRDlg = new CFindReplaceDialog();  // Must be created on the heap
      m_pFRDlg->Create( TRUE, m_sFind, "", 
         m_bDownFind
         ? (FR_DOWN|FR_NOMATCHCASE|FR_NOWHOLEWORD)
         : (FR_NOMATCHCASE|FR_NOWHOLEWORD), 
         this ); 
   } else {
      m_pFRDlg->SetFocus();
   }
}

void CEditEx::OnUpdateEditFind(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(1);
}

LRESULT StdFindReplace(CEdit &a, LPARAM lParam, CString &sFind, BOOL &bDown);

LRESULT CEditEx::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
   return StdFindReplace(*this,lParam,m_sFind,m_bDownFind);
}