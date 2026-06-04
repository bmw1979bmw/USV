// AclDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AclDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAclDialog dialog
CAclDialog::CAclDialog(float dn_teta,float dn_fi, CSVWave *w,float time,LRange *R,LSSignal *S1,LSSignal *S2, float osp, CWnd* pParent /*=NULL*/)

//CAclDialog::CAclDialog(CMAcl *pAcl, CWnd* pParent /*=NULL*/)
   : CDialog(CAclDialog::IDD, pParent)
{
   m_pAcl=w->pAcl;
   m_SpAnalizer.N_DF=R->cPoints;
   //{{AFX_DATA_INIT(CAclDialog)
   m_nPacks = w->pAcl->nPacks;
   m_nSignals = w->pAcl->nSignals;
   m_TIME=time;
   m_DN_TETA=dn_teta;
   m_DN_FI=dn_fi;
   m_OSP = osp;

   //}}AFX_DATA_INIT
   ZeroMemory(&OSP,sizeof(OSP));
   for ( int i=0; i<m_SpAnalizer.N_DF ;i++ ) {
   m_SpAnalizer.P1[i]=S1->idxpower(i);
   m_SpAnalizer.P2[i]=S2->idxpower(i);
   m_SpAnalizer.max_rev=3;  //  sv max_rev;

   if(m_SpAnalizer.P1[i]>0) 
      OSP[i]=m_SpAnalizer.P2[i]/m_SpAnalizer.P1[i]; 
       else OSP[i]=0;
   }
}


void CAclDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAclDialog)
   DDX_Control(pDX, IDC_GRIDSIGNALS, m_GridSignals);
   DDX_Control(pDX, IDC_GRIDPACKS, m_GridPacks);
   DDX_Control(pDX, IDC_GRIDOSP, m_GridOSP);
   DDX_Control(pDX, IDC_SPANALIZER, m_SpAnalizer);

   DDX_Text(pDX, IDC_NPACKS, m_nPacks);
   DDX_Text(pDX, IDC_NSIGNALS, m_nSignals);
   DDX_Text(pDX, IDC_TIME, m_TIME);
   DDX_Text(pDX, IDC_DN_TETA, m_DN_TETA);
   DDX_Text(pDX, IDC_DN_FI, m_DN_FI);
   DDX_Text(pDX, IDC_OSP, m_OSP);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAclDialog, CDialog)
   //{{AFX_MSG_MAP(CAclDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAclDialog message handlers

BOOL CAclDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
//   m_SpinPacks.SetRange(1,100);  
//   m_SpinPacks.SetPos(m_nPacks);

//   m_SpinSignals.SetRange(1,100);
//   m_SpinSignals.SetPos(m_nSignals);

   m_GridPacks.Init(2,m_nPacks);
   m_GridPacks.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   m_GridPacks.InitColumn(1,_T("Level"),0,GVCF_FLOAT,
      &m_pAcl->pPacks->Level,DT_CENTER,sizeof(CMAcl::TAclPack));
   m_GridPacks.InitColumn(2,_T("Delay,мс"),0,GVCF_FLOAT,
      &m_pAcl->pPacks->Delay,DT_CENTER,sizeof(CMAcl::TAclPack));
   m_GridPacks.InitSize();

   m_GridOSP.Init(3,m_SpAnalizer.N_DF);
   m_GridOSP.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   m_GridOSP.InitColumn(1,_T("Мощность помехи"),0,GVCF_FLOAT,
      &m_SpAnalizer.P1,DT_CENTER,sizeof(float));
   m_GridOSP.InitColumn(2,_T("Мощность сигнала"),0,GVCF_FLOAT,
      &m_SpAnalizer.P2,DT_CENTER,sizeof(float));
   m_GridOSP.InitColumn(3,_T("ОСП"),0,GVCF_FLOAT,
      &OSP,sizeof(float));
   m_GridOSP.InitSize();

    m_GridSignals.Init(6,m_nSignals);
    m_GridSignals.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
    m_GridSignals.InitColumn(1,_T("type"),40,GVCF_INT,
       &m_pAcl->pSignals->type,DT_CENTER,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitColumn(2,_T("length,мс"),0,GVCF_FLOAT,
       &m_pAcl->pSignals->length,DT_RIGHT,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitColumn(3,_T("delay,мс"),0,GVCF_FLOAT,
       &m_pAcl->pSignals->delay,DT_RIGHT,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitColumn(4,_T("freq,Hz"),0,GVCF_FLOAT,
       &m_pAcl->pSignals->freq,DT_RIGHT,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitColumn(5,_T("band,Hz"),0,GVCF_FLOAT,
       &m_pAcl->pSignals->band,DT_RIGHT,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitColumn(6,_T("level,Па"),0,GVCF_FLOAT,
       &m_pAcl->pSignals->level,DT_RIGHT,sizeof(CMAcl::TAclSignal));
    m_GridSignals.InitSize();

   return TRUE;    
}
