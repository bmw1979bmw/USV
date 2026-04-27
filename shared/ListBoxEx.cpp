// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
   //{{AFX_MSG_MAP(CListBoxEx)
   ON_WM_KEYDOWN()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

int CListBoxEx::AddString(UINT id)
{
   CString s;
   if ( s.LoadString(id) ) {
      return AddString((LPCTSTR)s);
   }
   return LB_ERR;
}

CListBoxEx & CListBoxEx::operator <<(UINT id)
{
   if ( AddString(id) == LB_ERR ) {
      AddString(_T("?"));
   }
   return *this;
}


void CListBoxEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if ( nChar == 46 && nFlags == 339 ) {
      GetParent()->SendMessage(WM_COMMAND,IDC_REMBUTTON);
   }
   /*
   CString s;
   s.Format(_T("%u %u %u"),nChar, nRepCnt, nFlags );
   AfxMessageBox(s);
   */
   CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CListBoxEx::DeleteAllStrings(void)
{ 
   while(DeleteString(0) != LB_ERR);
}
