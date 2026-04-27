// TXParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TXParamsDlg.h"
#include "structures.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTXParamsDlg dialog

CTXParamsDlg::CTXParamsDlg(TXParams *psrc, CWnd* pParent /*=NULL*/, BOOL bAutoOK)
   : CDialog(CTXParamsDlg::IDD, pParent), m_br(SI_COLOR)
{
   m_bAutoOK = bAutoOK;
   bExtend = TRUE;
   //{{AFX_DATA_INIT(CTXParamsDlg)
   //}}AFX_DATA_INIT
   p = *psrc;
   m_pSrc = psrc;
   bReadOnly = FALSE;
   
}

CTXParamsDlg::CTXParamsDlg(TMParams *psrc, CWnd* pParent /*=NULL*/, BOOL bAutoOK)
   : CDialog(CTXParamsDlg::IDD, pParent), m_br(SI_COLOR)
{
   m_bAutoOK = bAutoOK;
   bExtend = FALSE;
   //{ {AFX_DATA_INIT(CTXParamsDlg)
   //} }AFX_DATA_INIT
   *(TMParams *)&p = *psrc;
   m_pSrc = (TXParams *)psrc;
   bReadOnly = FALSE;
}

void CTXParamsDlg::DoDataExchange(CDataExchange* pDX)
{
//    CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTXParamsDlg)
	//}}AFX_DATA_MAP
   if ( !ms_bSI ) {
      DDX_Text(pDX, 100, p.X);
      DDX_Text(pDX, 101, p.Y);
      DDX_Text(pDX, 104, p.V);
   } else {
      float X;
      float Y;
      float V;
      if ( !pDX->m_bSaveAndValidate ) {
         X = p.X*1000;
         Y = p.Y*1000;
         V = p.V*KnMe;
      }
      DDX_Text(pDX, 100, X);
      DDX_Text(pDX, 101, Y);
      DDX_Text(pDX, 104, V);
      if ( pDX->m_bSaveAndValidate ) {
         p.X = X/1000;
         p.Y = Y/1000;
         p.V = V*MeKn;
      }
   }
   DDX_Text(pDX, 102, p.Z);
   DDX_Text(pDX, 103, p.K);
   DDX_Text(pDX, 105, p.L);
   DDX_Text(pDX, 106, p.D);
   if ( bExtend ) {
      DDX_Hex(pDX, 107, p.dwNoises);
      DDX_Hex(pDX, 108, p.dwAcFlags);
   }               
}

BEGIN_MESSAGE_MAP(CTXParamsDlg, CDialog)
   //{{AFX_MSG_MAP(CTXParamsDlg)
	ON_BN_CLICKED(300, OnSIClicked)
   ON_MESSAGE(WM_CTLCOLOREDIT, OnCtlColorEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTXParamsDlg message handlers

UINT CTXParamsDlg::ms_fID = 0;     // focused item
BOOL CTXParamsDlg::ms_bSI = FALSE; // система си

BOOL CTXParamsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   if ( !bExtend ) {
      ::EnableWindow(::GetDlgItem(m_hWnd, 107), FALSE);
      ::EnableWindow(::GetDlgItem(m_hWnd, 108), FALSE);
      ::EnableWindow(::GetDlgItem(m_hWnd, 207), FALSE);
      ::EnableWindow(::GetDlgItem(m_hWnd, 208), FALSE);
   }

   if ( bReadOnly ) ::EnableWindow(::GetDlgItem(m_hWnd, IDOK), FALSE);
   
   if ( ms_fID ) {
      HWND hWnd = ::GetDlgItem(m_hWnd, ms_fID);
      ::SetFocus(hWnd);
      TCHAR buf[5];
      if ( ::GetClassName(hWnd, buf, 5) == 4 ) 
      if ( _tcsicmp(buf, "EDIT") == 0 ) 
         ::SendMessage(hWnd, EM_SETSEL, 0, -1);
   }
   UpdateForSi();

   return FALSE;
}

void CTXParamsDlg::OnOK() 
{
   ms_fID = ::GetDlgCtrlID(::GetFocus());
   if (!UpdateData(TRUE)) return;
   if ( m_bAutoOK ) memcpy(m_pSrc, &p, bExtend?sizeof(TXParams):sizeof(TMParams));
   EndDialog(IDOK);
}

void CTXParamsDlg::OnSIClicked() 
{
   if ( !UpdateData(TRUE) ) return;
   ms_bSI = IsDlgButtonChecked(300);
   UpdateForSi();
   UpdateData(FALSE);
   Invalidate();
}

void CTXParamsDlg::UpdateForSi()
{
   CheckDlgButton(300, ms_bSI);
   if ( ms_bSI ) {
      SetDlgItemText(200, "м, X");
      SetDlgItemText(201, "м, Y");
      SetDlgItemText(204, "скорость,м/с V");
   } else {
      SetDlgItemText(200, "км, X");
      SetDlgItemText(201, "км, Y");
      SetDlgItemText(204, "скорость,уз V");
   }
}

HBRUSH CTXParamsDlg::OnCtlColorEdit( HDC wParam, HWND lParam )
{
   if ( ms_bSI ) {
   UINT nID = ::GetDlgCtrlID((HWND)lParam);
   switch ( nID ) {
   case 100:
   case 101:
   case 104:
      ::SetBkColor(wParam, SI_COLOR);
      return (HBRUSH)m_br;
   }
   }
   return (HBRUSH)Default();
}

