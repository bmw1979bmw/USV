// FormularAN.cpp : implementation file
//

#include "stdafx.h"
#include "FormularAN.h"
#include "MainFrm.h"
#include "MainDoc.h"
#include "Formularsv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormularAN dialog


CFormularAN::CFormularAN(TFormularAN *psrc,CWnd* pParent /*=NULL*/)
: CDialog(CFormularAN::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFormularAN)
   //}}AFX_DATA_INIT
   p = *psrc;
   p1= psrc;
   mnR = 0;
}


void CFormularAN::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   
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
         p.H_GF = 30;
         break;
      case 1:
         p.H_GF = 120;
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

      if( p.H_GF == 30) mnR=0;
      else if( p.H_GF == 120 ) mnR=1;
      else if( p.H_GF == 450 ) mnR=2;
      
      DDX_Radio(pDX, IDC_GF1, mnR);
   }

   //{{AFX_DATA_MAP(CFormularAN)
   // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormularAN, CDialog)
//{{AFX_MSG_MAP(CFormularAN)
ON_BN_CLICKED(IDC_SAVE_FORM, OnSaveForm)
ON_BN_CLICKED(IDC_ADD_FORM, OnAddForm)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormularAN message handlers

void CFormularAN::OnSaveForm() 
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

void CFormularAN::OnAddForm() 
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

void CFormularAN::OnOK() 
{
   // TODO: Add extra validation here
   if (!UpdateData(TRUE)) return;
   
   memcpy(p1,&p,sizeof(TFormularAN));
   
   EndDialog(IDOK);
   
   //	CDialog::OnOK();
}

BOOL CFormularAN::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   //   CMainFrame* pFrame=(CMainFrame*)(AfxGetMainWnd());
   //   CMainDoc* pDoc=(CMainDoc*)(pFrame->GetActiveDocument());
   //   ASSERT_VALID(pDoc);
   
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
