// DetectorDlg.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "Detector.h"

#include "DetectorDlg.h"
#include "ODChild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetectorDlg dialog

void Circle(CDC *dc,int x,int y,int r);

CDetectorDlg::CDetectorDlg(CMainDoc *pDoc,LSSignal *S,LRange *R,float max_rev,float pow_s,int nom_rev,CWnd* pParent /*=NULL*/)
	: CDialog(CDetectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDetectorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//    m_Detector.N_DF=R->cPoints;
//    m_Detector.max_rev=max_rev;
    P_MAX=0;
    PPR=max_rev;
     m_pow_s=pow_s;
     m_S=S;
     m_pDocument=pDoc;
    for ( int i=0; i<m_pDocument->m_Range.cPoints ;i++ ) 
    {
    
       if(abs(i-nom_rev)<=1) P[i]=0; 
       else
       P[i]=S->idxpower(i);
       if ( P_MAX<P[i] ) 
       {P_MAX=P[i];} 

    }
   RegisterODChild(0);

}


void CDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
 //     DDX_Control(pDX, IDC_DETECTOR, m_Detector);
 //     DDX_Control(pDX, IDC_DN     , m_DNDisp);

      //{{AFX_DATA_MAP(CDetectorDlg)
   DDX_Control(pDX, IDC_GRIDOSP, m_GridOSP);

		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDetectorDlg, CDialog)
	//{{AFX_MSG_MAP(CDetectorDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetectorDlg message handlers

void CDetectorDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS) 
{
   CWnd::OnDrawItem(nIDCtl, pDIS);
   if(pDIS->CtlID != IDC_DETECTOR)return;
// 
//    case IDC_DETECTOR:

   // TODO: Add your message handler code here and/or call default
   CDC dc;
   dc.Attach(pDIS->hDC);
   CRect rc;
   rc.CopyRect(&pDIS->rcItem);
//   dc.Ellipse(&pDIS->rcItem);
///////////////////////////////////////////////////////
   
//    switch(pDIS->CtlID){
// 
//    case IDC_DETECTOR:
//       {

   
   rc.DeflateRect(rc.right/10 ,rc.bottom/10);
   dc.SetWindowOrg(0,-rc.bottom/20);

   double M=0;
   if ( PPR>P_MAX ) {
   M=double(rc.bottom)/PPR;
   } 
   else 
   {
      M=double(rc.bottom)/P_MAX;
   }
     dc.SelectStockObject(NULL_PEN);

   for ( int i=0; i<10 ;i++ ) {
       int R=i*45;
       if(R>=250) R=250;
       int G=i*25;
       DWORD br =(RGB(R,250-G,0));
       CRect rect(
          rc.left,
          rc.top,
          rc.right,
          rc.bottom-int (i*float(rc.bottom)/10.15));
       dc.FillSolidRect(rect,br);
    }

    ////////////////////////////////////////////////
   CBrush br(RGB(0, 0, 0)),*oldbr;
   oldbr = dc.SelectObject(&br);

   CPoint point[260];
//   int N=(F_MAX-F_MIN)/DF;

   for (  i=0; i<m_pDocument->m_Range.cPoints;i++ ) {

      int h=rc.bottom-int(P[i]*M);
      if ( h<0 )  h=0;

      point[2*i].y   = h;
      point[2*i+1].y = h;
      point[2*i].x   = rc.left + int(float(i) * float(rc.right-rc.left) / float(m_pDocument->m_Range.cPoints));
      point[2*i+1].x = rc.left + int(float(i+1) * float(rc.right-rc.left) / float(m_pDocument->m_Range.cPoints));
   }


   point[2*m_pDocument->m_Range.cPoints].y=point[2*m_pDocument->m_Range.cPoints+1].y=rc.top;
   point[2*m_pDocument->m_Range.cPoints].x=rc.right;
   point[2*m_pDocument->m_Range.cPoints+1].x=rc.left;
   
   dc.Polygon(point,2*m_pDocument->m_Range.cPoints+2); 

   CPen pn(PS_SOLID,3,RGB(243, 60, 33)),*olpn;
   olpn = dc.SelectObject(&pn);


  dc.SelectStockObject(NULL_BRUSH);
 
    for ( i=0; i<m_pDocument->m_Range.cPoints;i++ ) {
 
       int h=rc.bottom-int(P[i]*M);
       if ( h<0 )  h=0;
 
 
      point[2*i].y=
       point[2*i+1].y=h;
       point[2*i].x=rc.left+i*double(rc.right-rc.left)/double(m_pDocument->m_Range.cPoints);
       point[2*i+1].x=rc.left+(i+1)*double(rc.right-rc.left)/double(m_pDocument->m_Range.cPoints);
    }
 
    point[2*m_pDocument->m_Range.cPoints].y=point[2*m_pDocument->m_Range.cPoints+1].y=rc.bottom;
    point[2*m_pDocument->m_Range.cPoints].x=rc.right;
    point[2*m_pDocument->m_Range.cPoints+1].x=rc.left;
    
    dc.Polygon(point,2*m_pDocument->m_Range.cPoints+2); 


    CPen pn2(PS_SOLID,3,RGB(0, 0, 255));
  dc.SelectObject(&pn2);


  dc.SelectStockObject(NULL_BRUSH);

  for ( i=0; i<m_pDocument->m_Range.cPoints;i++ ) {

      int h=rc.bottom-int(PPR*M);
      if ( h<0 )  h=0;


      point[2*i].y=
      point[2*i+1].y=h;
      point[2*i].x=rc.left+i*double(rc.right-rc.left)/double(m_pDocument->m_Range.cPoints);
      point[2*i+1].x=rc.left+(i+1)*double(rc.right-rc.left)/double(m_pDocument->m_Range.cPoints);
   }

   point[2*m_pDocument->m_Range.cPoints].y=point[2*m_pDocument->m_Range.cPoints+1].y=rc.bottom;
   point[2*m_pDocument->m_Range.cPoints].x=rc.right;
   point[2*m_pDocument->m_Range.cPoints+1].x=rc.left;
   
   dc.Polygon(point,2*m_pDocument->m_Range.cPoints+2); 

   
   CFont font,*oldfnt;
   font.CreatePointFont(60, _T("Arial"));
   oldfnt = dc.SelectObject(&font);
   dc.SetTextAlign(TA_LEFT);
   //   dc.SetTextAlign(TA_BASELINE);
   dc.SetBkMode(TRANSPARENT);
   CString s;
   dc.SetTextColor(0x0000D700);
   CSize tz = dc.GetTextExtent(_T("[ ]"));

   s.Format("œœ–=%.7f",PPR);
   dc.TextOut(rc.left, rc.top, s);
   s.Format("Pmax=%.7f",P_MAX);
   dc.TextOut(rc.left, rc.top + int(1.5f * (float)tz.cy),s);
   s.Format("OCœ=%.3f", m_pow_s);
   dc.TextOut(rc.left, rc.top + 3 * tz.cy, s);
   
   dc.SetTextAlign(TA_BOTTOM);
   dc.TextOut(rc.left - rc.left / 2, rc.bottom, "0");
   
   dc.SelectObject(oldbr);
   dc.SelectObject(olpn);
   dc.SelectObject(oldfnt);
   dc.SetWindowOrg(0,0);

   
//       }
//    break;


//   }
//////////////////////////////////////////////

   dc.Detach();
}


BOOL CDetectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

   m_GridOSP.Init(2,m_pDocument->m_Range.cPoints);
   m_GridOSP.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   m_GridOSP.InitColumn(1,_T("”Ó‚ÂÌ¸ œ‡*œ‡"),0,GVCF_FLOAT,
      &P,DT_CENTER,sizeof(float));
   m_GridOSP.InitSize();







	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
