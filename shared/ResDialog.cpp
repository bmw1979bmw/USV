// ResDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ResDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResDialog dialog
CResDialog::CResDialog( int NN, float *MP, float *MS, float *MOSP, float osp, CWnd* pParent /*=NULL*/)
   : CDialog(CResDialog::IDD, pParent)
{
   m_SpAnalizer.N_DF = NN;
   //{{AFX_DATA_INIT(CResDialog)
   m_OSP = osp;

   //}}AFX_DATA_INIT
   ZeroMemory(&OSP,sizeof(OSP));
   for ( int i=0; i<m_SpAnalizer.N_DF ;i++ ) 
   {
      m_SpAnalizer.P1[i] = MP[i];
      m_SpAnalizer.P2[i] = MS[i];
      OSP[i] = MOSP[i]; 
   }
}


void CResDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CResDialog)
   DDX_Control(pDX, IDC_GRIDOSP, m_GridOSP);
   DDX_Control(pDX, IDC_SPANALIZER, m_SpAnalizer);
   DDX_Text(pDX, IDC_OSP, m_OSP);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResDialog, CDialog)
   //{{AFX_MSG_MAP(CResDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResDialog message handlers

BOOL CResDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_GridOSP.Init(3,m_SpAnalizer.N_DF);
   m_GridOSP.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   m_GridOSP.InitColumn(1,_T("Мощность помехи"),0,GVCF_FLOAT,
      &m_SpAnalizer.P1,DT_CENTER,sizeof(float));
   m_GridOSP.InitColumn(2,_T("Мощность шумов"),0,GVCF_FLOAT,
      &m_SpAnalizer.P2,DT_CENTER,sizeof(float));
   m_GridOSP.InitColumn(3,_T("ОСП"),0,GVCF_FLOAT,
      &OSP,sizeof(float));
   m_GridOSP.InitSize();

   return TRUE;    
}
