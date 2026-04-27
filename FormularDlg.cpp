// FormularDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USV.h"
#include "FormularDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormularDlg dialog


CFormularDlg::CFormularDlg(TFormularUSV *psrc,CWnd* pParent /*=NULL*/)
	: CDialog(CFormularDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormularDlg)
	//}}AFX_DATA_INIT
      p = *psrc;
      p1= psrc;
/*
      CString s="mk5.frm";
      CFile file;
      if(file.Open(s,CFile::modeRead)){
      file.Read ( &p,sizeof(p) );
         
      }
      */
}


void CFormularDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormularDlg)
	//}}AFX_DATA_MAP
 DDX_Text(pDX, IDC_KURS_CMD ,   p.     KURS_CMD) ;  //float заданный  курс
 DDX_Text(pDX, IDC_A_CMD,       p.   A_ZACLAD) ;//float курс угол  закладки  рулей
 DDX_Text(pDX, IDC_SU_KURS_CMD, p.       SU_KURS)  ;//int тип управлени€ по курсу
 DDX_Text(pDX, IDC_ZIRC_CMD,    p.      ZIRC);//int направл циркул€ции 1 вправо ,др-влево
 DDX_Text(pDX, IDC_T_CMD_K,     p.     T_CMD_K);
 DDX_Text(pDX, IDC_SPEED_CMD,   p.       SPEED_CMD); ////float скорость
 DDX_Text(pDX, IDC_SU_SPEED_CMD, p.          SU_SPEED); //;//int тип управлени€ по  скорости
 DDX_Text(pDX, IDC_T_CMD_V,      p.     T_CMD_V);
 DDX_Check(pDX, IDC_BUPR, p.bUpr);
 DDX_Check(pDX, IDC_BOTN, p.bOtn);


}


BEGIN_MESSAGE_MAP(CFormularDlg, CDialog)
	//{{AFX_MSG_MAP(CFormularDlg)
	ON_BN_CLICKED(IDC_SAVE_FORM, OnSaveForm)
	ON_BN_CLICKED(IDC_ADD_FORM, OnAddForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormularDlg message handlers

void CFormularDlg::OnOK() 
{
	if (!UpdateData(TRUE)) return;

   memcpy(p1,&p,sizeof(TFormularUSV));

	EndDialog(IDOK);
}

void CFormularDlg::OnSaveForm() 
{
if(!UpdateData(TRUE)) return;
   CFileDialog dlg( 0,"frm", 0,OFN_HIDEREADONLY,"‘ормул€р(*.frm)|*.frm||",this );
   CString s;
    if(dlg.DoModal()==IDOK){
   s = dlg.GetPathName( );
   CFile file;
   if(file.Open(s,CFile::modeCreate|CFile::modeWrite)){
     file.Write ( &p,sizeof(p) );       
     file.Close();            
      }
    }
}

void CFormularDlg::OnAddForm() 
{
   CFileDialog dlg( 1,"frm", 0,OFN_HIDEREADONLY,"‘ормул€р(*.frm)|*.frm||",this );   
   CString s;
   if(dlg.DoModal()==IDOK){
      s = dlg.GetPathName( );
      CFile file;
      if(file.Open(s,CFile::modeRead)){
      file.Read ( &p,sizeof(p) );
      }
   }        
UpdateData(FALSE);
}
