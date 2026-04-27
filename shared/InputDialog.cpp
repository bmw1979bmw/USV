// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog

CInputDialog::CInputDialog(LPCTSTR szName, int *n, CWnd* pParent)
   : CDialog(CInputDialog::IDD, pParent)
{
   dwRes = (DWORD *)n;
   dwInput = *n;
   cType = 'I';
   Name = szName;
}

CInputDialog::CInputDialog(LPCTSTR szName, DWORD *dw, CWnd* pParent)
   : CDialog(CInputDialog::IDD, pParent)
{
   dwRes = dw;
   dwInput = *dw;
   cType = 'D';
   Name = szName;
}

CInputDialog::CInputDialog(LPCTSTR szName, float *fl, CWnd* pParent)
   : CDialog(CInputDialog::IDD, pParent)
{
   fRes = fl;
   fInput = *fl;
   cType = 'F';
   Name = szName;
}

CInputDialog::CInputDialog(LPCTSTR szName, CString *s, int _nStrlen, CWnd* pParent)
   : CDialog(CInputDialog::IDD, pParent)
{
   sRes = s;
   sInput = *s;
   cType = 'S';
   nStrLen = _nStrlen;

   Name = szName;
}

CInputDialog::CInputDialog(LPCTSTR szName, TCHAR *s, int _nStrlen, CWnd* pParent)
   : CDialog(CInputDialog::IDD, pParent)
{
   tRes = s;  // TCHAR *
   sInput = (TCHAR const *)s;
   cType = 'T';
   nStrLen = _nStrlen;

   Name = szName;
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
   switch(cType){
   case 'S':
   case 'T':
      DDX_Text(pDX, IDC_INPUTEDIT, sInput);
      DDV_MaxChars(pDX, sInput, nStrLen);
      break;
   case 'F':
      DDX_Text(pDX, IDC_INPUTEDIT, fInput);
      break;
   case 'D':
      DDX_Text(pDX, IDC_INPUTEDIT, dwInput);
      break;
   case 'I':
      DDX_Text(pDX, IDC_INPUTEDIT, (int &)dwInput);
      break;
   default:
      ASSERT(0);
   }
   //{{AFX_DATA_MAP(CInputDialog)
   DDX_Text(pDX, IDC_INPUTNAME, Name);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
   //{{AFX_MSG_MAP(CInputDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDialog message handlers

INT_PTR CInputDialog::DoModal() 
{
   INT_PTR ret = CDialog::DoModal();
   if ( ret == IDOK ) {
      switch(cType){
      case 'S':
         *sRes = sInput;
         break;
      case 'T':
         CopyMemory(tRes, (LPCTSTR)sInput,
                    sizeof(TCHAR)*(nStrLen+1));
         break;
      case 'F':
         *fRes = fInput;
         break;
      case 'D':
      case 'I':
         *dwRes = dwInput;
         break;
      default:       ;
      } 
   }
   return ret;
}
