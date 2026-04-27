// SignalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GoodFunc.h"
#include "SignalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSignalDlg dialog

CSignalDlg::CSignalDlg(LSSignal *pSignal,LPCTSTR pHead,CWnd* pParent /*=NULL*/)
   : CDialog(CSignalDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSignalDlg)
   //}}AFX_DATA_INIT
   m_pSignal = pSignal;
   m_pHead = pHead;
   DoModal();
}

void CSignalDlg::DoDataExchange(CDataExchange* pDX)
{
   //{{AFX_DATA_MAP(CSignalDlg)
   DDX_Control(pDX, IDCSD_GDSIGNALT0, m_GDSignal0);
   DDX_Control(pDX, IDCSD_GDSIGNALF0, m_GDSignal1);
   DDX_Control(pDX, IDCSD_GRID, m_Grid);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSignalDlg, CDialog)
   //{{AFX_MSG_MAP(CSignalDlg)
   ON_BN_CLICKED(IDCSD_FFT, OnFft)
   ON_BN_CLICKED(IDCSD_FFT2, OnFft2)
   ON_BN_CLICKED(IDCSD_CREATE, OnCreate)
   ON_NOTIFY(LVN_ITEMCHANGED, IDCSD_GRID, OnItemchangedGrid)
   ON_BN_CLICKED(IDCSD_CLEAR, OnClear)
   ON_NOTIFY(NM_CLICK, IDCSD_GDSIGNALT0, OnClickGdsignal0)
   ON_NOTIFY(NM_CLICK, IDCSD_GDSIGNALF0, OnClickGdsignal1)
   ON_BN_CLICKED(IDCSD_ABSTEST, OnAbstest)
   ON_BN_CLICKED(IDCSD_POWER, OnPower)
   ON_BN_CLICKED(IDCSD_IDXPOWER, OnIdxPower)
   ON_BN_CLICKED(IDCSD_ABSTORE, OnAbsToRe)
	ON_BN_CLICKED(IDCSD_SAVEASTEXT, OnSaveAsText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSignalDlg message handlers

BOOL CSignalDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_GDSignal0.Init(0,0,0);
   m_GDSignal1.Init(0,0,0);
   if ( m_pHead ) {
      SetWindowText(m_pHead);
   }

   m_Signal.create(m_pSignal);
   m_Grid.Init(4,m_pSignal->n);
   m_Grid.InitColumn(0,0u,35,GVCF_INDEX0,0,DT_CENTER);

   m_Grid.InitColumn(1,_T("Time.re"),0,GVCF_FLOAT,&m_Signal.Time->re,DT_CENTER,sizeof(Complex));
   m_Grid.InitColumn(2,_T("Time.im"),0,GVCF_FLOAT,&m_Signal.Time->im,DT_CENTER,sizeof(Complex));
   m_Grid.InitColumn(3,_T("Freq.re"),0,GVCF_FLOAT,&m_Signal.Freq->re,DT_CENTER,sizeof(Complex));
   m_Grid.InitColumn(4,_T("Freq.im"),0,GVCF_FLOAT,&m_Signal.Freq->im,DT_CENTER,sizeof(Complex));
   m_Grid.InitSize();

   OnCreate();

   SetForegroundWindow();
   BringWindowToTop();

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CSignalDlg::OnFft() 
{
   m_Signal.transfere(1,0);
   SetData();
}

void CSignalDlg::OnFft2() 
{
   m_Signal.transfere(0,1);
   SetData();
}

void CSignalDlg::OnCreate() 
{
   m_Signal.copy(m_pSignal);
   SetData();
   OnPower();
}
void CSignalDlg::OnClear() 
{
   m_Signal.clear();
   SetData();
}

void CSignalDlg::SetData(BOOL bGrid)
{
   m_GDSignal0.SetData(m_Signal.n, m_Signal.Time);
   m_GDSignal1.SetData(m_Signal.n, m_Signal.Freq);
   if ( bGrid ) m_Grid.Invalidate();
}

void CSignalDlg::OnItemchangedGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW &nm = *(NM_LISTVIEW*)pNMHDR;
   if ( nm.uChanged == LVIF_TEXT ) {
      SetData(FALSE);
   }
   *pResult = 0;
}

void CSignalDlg::OnMirror() 
{
   Complex *s = m_Signal.Freq;
   int n = m_Signal.n;
   for ( int i=1; i<n/2 ;i++ ) {
      s[n-i] = s[i];
      s[n-i].im = -s[n-i].im;
   }
   SetData();
}

void CSignalDlg::OnClickGdsignal0(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;
   CString s;
   s.Format(_T("%d"),m_GDSignal0.m_xclk);
   SetDlgItemText(IDCSD_SIGNV0,s);
   m_Grid.SetCurSel(0,m_GDSignal0.m_xclk,0);
}

void CSignalDlg::OnClickGdsignal1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;
   CString s;
   s.Format(_T("%d"),m_GDSignal1.m_xclk);
   SetDlgItemText(IDCSD_SIGNV1,s);
   m_Grid.SetCurSel(2,m_GDSignal1.m_xclk,0);
}

void CSignalDlg::OnAbstest() 
{
   AfxMessageBox("Пытаемся возвести в квадрат");

   Complex *p = m_Signal.Freq;

   int n = m_Signal.n;

   for ( int i=0; i<n ;i++ ) {
      p[i].re = p[i].pow();
      p[i].im = 0;
   }
   SetData();
}

void CSignalDlg::OnPower() 
{
   CString s;
   s.Format(_T("%g"),m_Signal.power(0));
   SetDlgItemText(IDCSD_SIGNV0,s);

   s.Format(_T("%g"),m_Signal.power(1));
   SetDlgItemText(IDCSD_SIGNV1,s);
}

void CSignalDlg::OnIdxPower() 
{
   Complex *p = m_Signal.Freq;

   int n = m_Signal.n;
   int i;
   for ( i=0; i<n/2 ;i++ ) {
      p[i].re = m_Signal.idxpower(i);
      p[i].im = 0;
   }
   for ( ; i<n ;i++ ) p[i].re = p[i].im = 0;
   
   SetData();
}

void CSignalDlg::OnAbsToRe() 
{
   Complex *s = m_Signal.Time;

   int n = m_Signal.n;
   for ( int i=0; i<n ;i++ ) {
      s[i].re = s[i].abs();
      s[i].im = 0;
   }
   SetData();
}

void CSignalDlg::OnSaveAsText() 
{                    
   static int t=0;
   TCHAR name[16];
   wsprintf(name,_T("Signal%03d"),t);
   t++;
   CFileDialog dlg(FALSE,_T("txt"),name,
      OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_DONTADDTORECENT,
      _T("Текстовые файлы (*.txt)|*.txt||"),AfxGetMainWnd());
   if ( dlg.DoModal() == IDOK ) {
      CString s = dlg.GetPathName( );
      //Base.DoRayProtocol(s);
      CFile file;
      if ( file.Open(s,CFile::modeCreate|CFile::modeWrite)) {
         CTextArchive ar(&file,CArchive::store);
         GetWindowText(s);
         ar << "Signal\r\n";
         ar << (LPCSTR)s << "\r\n";
         ar << m_Signal.n << ";точек\r\n";
         ar << "No\tTime.re\tTime.im\tFreq.re\tFreq.im\r\n";
         for ( int i=0; i<m_Signal.n ;i++ ) {
            ar << i << ":\t"
               << m_Signal.Time[i].re << "\t"
               << m_Signal.Time[i].im << "\t"
               << m_Signal.Freq[i].re << "\t"
               << m_Signal.Freq[i].im
               << "\r\n";
         }
         
      } else {
         AfxMessageBox(0,_T("Unable to open '%s'"),(LPCTSTR)s);
      }
   }  
}
