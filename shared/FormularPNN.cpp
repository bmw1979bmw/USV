// FormularPNN.cpp : implementation file
//

#include "stdafx.h"
#include "FormularPNN.h"
#include "MainFrm.h"
#include "MainDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormularPNN dialog


CFormularPNN::CFormularPNN(TFormularPNN *psrc,CWnd* pParent /*=NULL*/)
: CDialog(CFormularPNN::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFormularPNN)
   //}}AFX_DATA_INIT
   p = *psrc;
   p1= psrc;
   mnR = 0;
   mnW = 0;
}


void CFormularPNN::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   
//    DDX_Text(pDX, IDC_T_WORK, p.T_Work);
//    DDX_Text(pDX, IDC_H_GF, p.H_GF);

   DDX_Text(pDX, IDC_N_K, p.N_K);
   DDX_Check(pDX, IDC_RAZMOTKA, p.Razmotka);

   if ( pDX->m_bSaveAndValidate ) 
   {
      DDX_Radio(pDX, IDC_GF1, mnR);
      p.H_GF=30;
      
      switch ( mnR ) 
      {
      case 0:
         p.H_GF = 20;
         break;
      case 1:
         p.H_GF = 300;
         break;
      default: ;
      }

      DDX_Radio(pDX, IDC_WORK1, mnW);
      p.T_Work=1;
      
      switch ( mnW ) 
      {
      case 0:
         p.T_Work = 1;
         break;
      case 1:
         p.T_Work = 4;
         break;
      case 2:
         p.T_Work = 8;
         break;
      default: ;
      }
   } 
   else 
   { 
      mnR = 0;
      if( p.H_GF == 20) mnR = 0;
      else if( p.H_GF == 300 ) mnR = 1;
      DDX_Radio(pDX, IDC_GF1, mnR);

      mnW = 0;
      if( p.T_Work == 1 ) mnW = 0;
      else if( p.T_Work == 4 ) mnW = 1;
      else if( p.T_Work == 8 ) mnW = 2;
      DDX_Radio(pDX, IDC_WORK1, mnW);
   }
   
   //{{AFX_DATA_MAP(CFormularPNN)
   // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormularPNN, CDialog)
//{{AFX_MSG_MAP(CFormularPNN)
ON_BN_CLICKED(IDC_SAVE_FORM, OnSaveForm)
ON_BN_CLICKED(IDC_ADD_FORM, OnAddForm)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormularPNN message handlers

void CFormularPNN::OnSaveForm() 
{
   // TODO: Add your control notification handler code here
   if(!UpdateData(TRUE)) return;
   CFileDialog dlg( 0,"frm", 0,OFN_HIDEREADONLY,"Ôîðìóëÿð ÐÃÀÁ ÏÍÍ(*.frm)|*.frm||",this );
   CString s;
   if(dlg.DoModal()==IDOK){
      s = dlg.GetPathName( );
      CStandFile file;
      if(file.Open(s,CStandFile::modeCreate|CStandFile::modeWrite)){
         file.Write ( &p,sizeof(p) );       
         file.Close();            
      }
   }
}

void CFormularPNN::OnAddForm() 
{
   // TODO: Add your control notification handler code here
   CFileDialog dlg( 1,"frm", 0,OFN_HIDEREADONLY,"Ôîðìóëÿð ÐÃÀÁ ÏÍÍ(*.frm)|*.frm||",this );   
   CString s;
   if(dlg.DoModal()==IDOK){
      s = dlg.GetPathName( );
      CStandFile file;
      if(file.Open(s,CStandFile::modeRead)){
      file.Read ( &p,sizeof(p) );
      }
   }        
   UpdateData(FALSE);
}

void CFormularPNN::OnOK() 
{
   // TODO: Add extra validation here
   if (!UpdateData(TRUE)) return;
   
   memcpy(p1,&p,sizeof(TFormularPNN));
   
   EndDialog(IDOK);
   
   //	CDialog::OnOK();
}

BOOL CFormularPNN::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   //   CMainFrame* pFrame=(CMainFrame*)(AfxGetMainWnd());
   //   CMainDoc* pDoc=(CMainDoc*)(pFrame->GetActiveDocument());
   //   ASSERT_VALID(pDoc);
   
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
