// FormularANN.cpp : implementation file
//

#include "stdafx.h"
#include "FormularANN.h"
#include "MainFrm.h"
#include "MainDoc.h"
#include "Formularsv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormularANN dialog


CFormularANN::CFormularANN(TFormularANN *psrc,CWnd* pParent /*=NULL*/)
: CDialog(CFormularANN::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFormularANN)
   //}}AFX_DATA_INIT
   p = *psrc;
   p1= psrc;
   mnR = 0;
}


void CFormularANN::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   
//    DDX_Text(pDX, IDC_H_GF, p.H_GF);
   DDX_Text(pDX, IDC_N_K, p.N_K);
   DDX_Check(pDX, IDC_RAZMOTKA, p.Razmotka);
   
   if ( pDX->m_bSaveAndValidate ) 
   {
      DDX_Radio(pDX, IDC_GF1, mnR);
      p.H_GF=20;
      
      switch ( mnR ) 
      {
      case 0:
         p.H_GF = 20;
         break;
      case 1:
         p.H_GF = 250;
         break;
      case 2:
         p.H_GF = 450;
         break;
      default: ;
      }
   } 
   else 
   { 
      mnR=0;

      if( p.H_GF == 20) mnR=0;
      else if( p.H_GF == 250 ) mnR=1;
      else if( p.H_GF == 450 ) mnR=2;
      
      DDX_Radio(pDX, IDC_GF1, mnR);
   }

   //{{AFX_DATA_MAP(CFormularANN)
   // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormularANN, CDialog)
//{{AFX_MSG_MAP(CFormularANN)
ON_BN_CLICKED(IDC_SAVE_FORM, OnSaveForm)
ON_BN_CLICKED(IDC_ADD_FORM, OnAddForm)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormularANN message handlers

void CFormularANN::OnSaveForm() 
{
   // TODO: Add your control notification handler code here
   if(!UpdateData(TRUE)) return;
   CFileDialog dlg( 0,"frm", 0,OFN_HIDEREADONLY,"Ôîðìóëÿð ÐÃÀÁ ÀÍ(*.frm)|*.frm||",this );
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

void CFormularANN::OnAddForm() 
{
   // TODO: Add your control notification handler code here
   CFileDialog dlg( 1,"frm", 0,OFN_HIDEREADONLY,"Ôîðìóëÿð ÐÃÀÁ ÀÍ(*.frm)|*.frm||",this );   
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

void CFormularANN::OnOK() 
{
   // TODO: Add extra validation here
   if (!UpdateData(TRUE)) return;
   
   memcpy(p1,&p,sizeof(TFormularANN));
   
   EndDialog(IDOK);
   
   //	CDialog::OnOK();
}

BOOL CFormularANN::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   //   CMainFrame* pFrame=(CMainFrame*)(AfxGetMainWnd());
   //   CMainDoc* pDoc=(CMainDoc*)(pFrame->GetActiveDocument());
   //   ASSERT_VALID(pDoc);
   
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
