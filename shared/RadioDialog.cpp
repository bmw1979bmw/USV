// RadioDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RadioDialog.h"
#include "RadioSignal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioDialog dialog
CRadioDialog::CRadioDialog(  CRadioSignal* mRadio, CWnd* pParent /*=NULL*/)
   : CDialog(CRadioDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CRadioDialog)

   //}}AFX_DATA_INIT
//   ZeroMemory(&OSP,sizeof(OSP));
   m_OSP = mRadio[1].m_H2_Dop;   // пороговое ОСП

   m_S[0]  = 0;
   m_N[0]  = 0;
   m_P[0]  = 0;
   m_H2[0] = 0;
   for ( int i=1; i<100; i++ ) 
   {
      m_S[i]  = mRadio[i].m_RadioS;
      m_N[i]  = mRadio[i].m_RadioN;
      m_P[i]  = mRadio[i].m_RadioP;
      m_H2[i] = mRadio[i].GetH2();
   }
}


void CRadioDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CRadioDialog)
   DDX_Control(pDX, IDC_GRIDOSP, m_GridOSP);
   DDX_Text(pDX, IDC_OSP, m_OSP);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRadioDialog, CDialog)
   //{{AFX_MSG_MAP(CRadioDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioDialog message handlers

BOOL CRadioDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_GridOSP.Init(4,99);
   m_GridOSP.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);

   m_GridOSP.InitColumn(1,_T("Мощность сигнала"),0,GVCF_FLOAT, &m_S[1],DT_CENTER,sizeof(float));
   
   m_GridOSP.InitColumn(2,_T("Мощность шума"),0,GVCF_FLOAT, &m_N[1],DT_CENTER,sizeof(float));
   
   m_GridOSP.InitColumn(3,_T("Мощность помехи"),0,GVCF_FLOAT, &m_P[1],DT_CENTER,sizeof(float));
   
   m_GridOSP.InitColumn(4,_T("ОСП"),0,GVCF_FLOAT, &m_H2[1],sizeof(float));
   m_GridOSP.InitSize();

   return TRUE;    
}
