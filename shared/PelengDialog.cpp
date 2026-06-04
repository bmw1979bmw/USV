// PelengDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PelengDialog.h"
#include "ODChild.h"
#include "MainDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new  DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPelengDialog dialog
CPelengDialog::CPelengDialog(
                             float t1, float t2, float Dist, float Bearing, float Lev,
                             float porog, float maxporog, float maxosp,
                             /*float *m_MPELENGNB, float *m_MPELENGAC,*/ float *m_MPELENG,
                             /*float *m_MDistNB, float *m_MDistAC, float *m_MDist,*/ /*float Dist, float m_Dmax,*/
                             /*float T1, float T2,*/ CWnd* pParent)
                             : CDialog(CPelengDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPelengDialog)
   //}}AFX_DATA_INIT
   //    ZeroMemory(&mPELENGNB,sizeof(mPELENGNB));
   //    ZeroMemory(&mPELENGAC,sizeof(mPELENGAC));
   ZeroMemory(&mPELENG,sizeof(mPELENG));
   //    ZeroMemory(&mDistNB,sizeof(mDistNB));
   //    ZeroMemory(&mDistAC,sizeof(mDistAC));
   //    ZeroMemory(&mDist,sizeof(mDist));
   
   //    mMaxPel = 0;
   //    mMaxOSP = mPELENG[0];
   
   m_maxPOROG = maxporog;
   m_POROG = porog;
   m_OSP = maxosp;
   
   for ( int i=0; i<30; i++ ) 
   {
      //       mPELENGNB[i] = m_MPELENGNB[i];
      //       mPELENGAC[i] = m_MPELENGAC[i];
      mPELENG[i]   = m_MPELENG[i];
      //       mDistNB[i] = m_MDistNB[i];
      //       mDistAC[i] = m_MDistAC[i];
      //       mDist[i]   = m_MDist[i];
      //       if ( mPELENG[i] > mMaxOSP ) 
      //       {
      //          mMaxPel = i * ( 360.0 / 128.0 );
      //          mMaxOSP = mPELENG[i];
      //       }
   }
   
   mt1 = t1;
   mt2 = t2;
   mBearing = Bearing;
   mDistan = Dist;
   mLev = Lev;
   mPorog = porog;
   mMaxPorog = maxporog;
   mMaxOSP = maxosp;
}


void CPelengDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPelengDialog)
   DDX_Control(pDX, IDC_GRIDPELENG, m_GridPeleng);
   //DDX_Control(pDX, IDC_GRIDDIST, m_GridDist);
   
   DDX_Text(pDX, IDC_TIME1, mt1);
   DDX_Text(pDX, IDC_TIME2, mt2);
   DDX_Text(pDX, IDC_DIST,  mDistan);
   DDX_Text(pDX, IDC_BEARING, mBearing);
   DDX_Text(pDX, IDC_LEV, mLev);
   DDX_Text(pDX, IDC_POROG, mPorog);
   DDX_Text(pDX, IDC_MAXPOROG, mMaxPorog);
   DDX_Text(pDX, IDC_MAXOSP, mMaxOSP);
   
   ::InvalidateRect(::GetDlgItem(m_hWnd,IDC_DRAWP),NULL,FALSE);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPelengDialog, CDialog)
//{{AFX_MSG_MAP(CPelengDialog)
ON_WM_DRAWITEM()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPelengDialog message handlers

BOOL CPelengDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   m_GridPeleng.Init( 1, 30 );
   m_GridPeleng.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   m_GridPeleng.InitColumn(1,_T("Уровень Па*Па"),0,GVCF_FLOAT,
      &mPELENG,DT_CENTER,sizeof(float));
   m_GridPeleng.InitSize();
   
   //     m_GridDist.Init(3,128);
   //     m_GridDist.InitColumn(0,0u,35,GVCF_INDEX1,0,DT_CENTER);
   //     m_GridDist.InitColumn(1,_T("Мощность сигнала"),0,GVCF_FLOAT,
   //        &mDistAC,DT_CENTER,sizeof(float));
   //     m_GridDist.InitColumn(2,_T("Мощность помехи"),0,GVCF_FLOAT,
   //        &mDistNB,DT_CENTER,sizeof(float));
   //     m_GridDist.InitColumn(3,_T("ОСП"),0,GVCF_FLOAT,
   //        &mDist,sizeof(float));
   //     m_GridDist.InitSize();
   
   //    m_PDAnalizerP.Store(mPelengAC, 128, mPmax); 
   //    m_PDAnalizerD.Store(mDist, mDmax); 
   
   HWND w = ::GetDlgItem(m_hWnd,IDC_DRAWP);
   CRect rc;
   ::GetWindowRect(w,rc);
   int t1 = rc.Width();
   int t2 = rc.Height();
   ::SetWindowPos(w,NULL,0,0,t1,t2,SWP_NOACTIVATE|
      SWP_NOCOPYBITS| SWP_NOMOVE| SWP_NOOWNERZORDER|SWP_NOZORDER);
   
   //    HWND w0 = ::GetDlgItem(m_hWnd,IDC_DRAWD);
   //    ::GetWindowRect(w0,rc);
   //    t1 = rc.Width();
   //    t2 = rc.Height();
   //    ::SetWindowPos(w0,NULL,0,0,t1,t2,SWP_NOACTIVATE|
   //       SWP_NOCOPYBITS| SWP_NOMOVE| SWP_NOOWNERZORDER|SWP_NOZORDER);
   
   return TRUE;  
}

void CPelengDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS) 
{
   // TODO: Add your message handler code here and/or call default
   
   CDialog::OnDrawItem(nIDCtl, pDIS);
   //if (!FLAG_NOUPDATE) return;
   if ( ! ( nIDCtl == IDC_DRAWP /*|| nIDCtl == IDC_DRAWD*/ ) ) return;
   
   CMainFrame* pFrame=(CMainFrame*)(AfxGetMainWnd());
   CMainDoc* pDoc=(CMainDoc*)(pFrame->GetActiveDocument());
   
   CDC dc;
   dc.Attach(pDIS->hDC);
   //dc.SelectStockObject(WHITE_PEN);
   //dc.Ellipse(&pDIS->rcItem);
   CRect rc;
   rc.CopyRect(&pDIS->rcItem);
   //CMainDoc *pDoc = GetDocument();
   
   CBrush fillbr(0x00002400);
   dc.SelectObject(&fillbr);
   dc.FillRect(rc, &fillbr);
   
   if ( pDoc->m_Sost != 3 ) return;
   
   //int r = pDIS->rcItem.right/2;
   ///////////////////////////////////////////
   
   //    CRect rc;
   //    GetClientRect(rc); // SetClipRect
   
   float Xmin = FLT_MAX;// MAX_FLOAT;
   float Xmax =-FLT_MAX;// -MAX_FLOAT;
   float Ymin = 0;//FLT_MAX;// MAX_FLOAT;
   float Ymax =-FLT_MAX;// -MAX_FLOAT;
   
   int cx = rc.right;
   int cy = rc.bottom;
   
   switch (nIDCtl)
   {
   case IDC_DRAWP:
      {  // рисунок пеленговой развертки
         rc.DeflateRect( rc.right/10, rc.bottom/10 );
         dc.SetWindowOrg(0,-rc.bottom/20);
         
         double M=0;
         if ( m_POROG > m_maxPOROG ) 
         {
            M = double(rc.bottom) / m_POROG;
         } 
         else 
         {
            M = double( rc.bottom ) / m_maxPOROG;
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
               rc.bottom - int(i*float(rc.bottom)/10.15));
            dc.FillSolidRect(rect,br);
         }
         
         ////////////////////////////////////////////////
         CBrush br(RGB(0, 0, 0)),*oldbr;
         oldbr = dc.SelectObject(&br);
         
         CPoint point[260];
         //   int N=(F_MAX-F_MIN)/DF;
         
         float de = float(rc.right-rc.left) / 30.f; 

         for (  i=0; i<30; i++ ) 
         {
            int h=rc.bottom-int(mPELENG[i]*M);
            if ( h<0 )  h=0;
            
            point[2*i].y   = h;
            point[2*i+1].y = h;
            point[2*i].x   = rc.left + int(float(i)   * de);
            point[2*i+1].x = rc.left + int(float(i+1) * de);
         }
         
         point[60].y = point[61].y = rc.top;
         point[60].x = rc.right;
         point[60].x = rc.left;
         
         dc.Polygon(point, 62 );     // 2 * 30 + 2
         
         CPen pn(PS_SOLID,3,RGB(243, 60, 33)),*olpn;
         olpn = dc.SelectObject(&pn);
         
         
         dc.SelectStockObject(NULL_BRUSH);
         
         for ( i=0; i<30; i++ ) 
         {
            int h=rc.bottom-int(mPELENG[i]*M);
            if ( h<0 )  h=0;
            point[2*i].y   = h;
            point[2*i+1].y = h;
            point[2*i].x   = rc.left + int(float(i) * de);
            point[2*i+1].x = rc.left + int(float(i+1) * de);
         }
         
         point[60].y = point[61].y = rc.bottom;
         point[60].x = rc.right;
         point[61].x = rc.left;
         
         dc.Polygon(point, 62); 
         
         CPen pn2(PS_SOLID,3,RGB(0, 0, 255));
         dc.SelectObject(&pn2);
         dc.SelectStockObject(NULL_BRUSH);
         
         for ( i=0; i<30; i++ ) 
         {
            int h = rc.bottom - int(m_POROG * M);
            if ( h<0 )  h=0;
            
            point[2*i].y   = h;
            point[2*i+1].y = h;
            point[2*i].x   = rc.left + int(float(i) * de);
            point[2*i+1].x = rc.left + int(float(i+1) * de);
         }
         
         point[60].y   = point[61].y = rc.bottom;
         point[60].x   = rc.right;
         point[61].x = rc.left;
         
         dc.Polygon( point, 62 ); 
         
         CFont font,*oldfnt;
         font.CreatePointFont(60, _T("Arial"));
         oldfnt = dc.SelectObject(&font);
         dc.SetTextAlign(TA_LEFT);
         //   dc.SetTextAlign(TA_BASELINE);
         dc.SetBkMode(TRANSPARENT);
         CString s;
         dc.SetTextColor(0x0000D700);
         CSize tz = dc.GetTextExtent(_T("[ ]"));
         
         s.Format("Порог = %.7f",m_POROG);
         dc.TextOut(rc.left, rc.top, s);
         s.Format("Макс.уровень = %.7f",m_maxPOROG);
         dc.TextOut(rc.left, rc.top + int(1.5 * (float)tz.cy), s);
         s.Format("OCП = %.3f",m_OSP);
         dc.TextOut(rc.left, rc.top + 3 * tz.cy, s);
         
         dc.SetTextAlign(TA_BOTTOM);
         dc.TextOut(rc.left - rc.left/2, rc.bottom, "0");
         
         dc.SelectObject(oldbr);
         dc.SelectObject(olpn);
         dc.SelectObject(oldfnt);
         dc.SetWindowOrg(0,0);
         }
         break;
         
      case IDC_DRAWD:
         {
            //              // рисунок дистанционной развертки
            //                int m_N = 128;
            //                // ---
            //                CFont font,*oldfnt;
            //                font.CreatePointFont(70, _T("Arial"));
            //                oldfnt = dc.SelectObject(&font);
            //                CString s;
            //                s.Format( "%.1f", (float)(pDoc->m_DMaxObn) );
            //                CSize sz = dc.GetTextExtent(s);
            //                rc.DeflateRect(0,0,0,sz.cy);
            //                height = rc.bottom;
            //                // нижняя строка
            //                dc.SetBkMode(TRANSPARENT);
            //                dc.SetTextColor(0x0000D700);
            //                
            //                dc.SetTextAlign(TA_TOP);
            //                dc.TextOut(0,rc.bottom,"0");
            //                dc.TextOut(rc.right - sz.cx,rc.bottom,s);
            //                dc.SetTextAlign(TA_CENTER);
            //                s.Format( "%.1f", (float)(pDoc->m_DMaxObn / 2.0) );
            //                dc.TextOut(rc.right/2,rc.bottom,s);
            //                s.Format("%.1f",(float)(pDoc->m_DMaxObn / 4.0) );
            //                dc.TextOut(rc.right/4,rc.bottom,s);
            //                s.Format( "%.1f", (float)(pDoc->m_DMaxObn / 4.0 * 3.0 ) );
            //                dc.TextOut(rc.right/4*3,rc.bottom,s);
            //                
            //                CPen penGreen(PS_SOLID, 1, 0x0000D700);
            //                dc.SelectObject(&penGreen);
            //                float dx = (float)(rc.right / (float)(m_N-1));
            //                int ix;
            //                for ( int i=1; i<m_N; i++ )
            //                {  
            //                   ix = int(dx * i);
            //                   dc.MoveTo(ix,rc.bottom+sz.cy);
            //                   dc.LineTo(ix,rc.bottom+sz.cy-4);
            //                }
            //                // ---
            // 
            //                for ( i=0; i<m_N ;i++ ) {
            //                   if ( pDoc->m_MDist[i] < Ymin ) Ymin = pDoc->m_MDist[i];  
            //                   if ( pDoc->m_MDist[i] > Ymax ) Ymax = pDoc->m_MDist[i];
            //                }   
            //                
            //                dc.SetTextColor(0x0000FFFF);
            //                dc.SetBkMode(TRANSPARENT);
            //                
            //                CPen   yellowpn(0,0,0x0000FFFF);
            //                dc.SelectObject(&yellowpn);
            //                
            //                //float dx=(float)rc.right/127.0;     //nX
            //                
            //                // ОСП
            //                for( ix=0; ix<m_N; ix++) {
            //                   
            //                   int x = int((float)ix*dx);
            //                   int y = rc.bottom - int((pDoc->m_MDist[ix] - Ymin) / (Ymax - Ymin) * height);
            //                   
            //                   if ( ix ) {
            //                      dc.LineTo(x,y);
            //                   } else {
            //                      dc.MoveTo(x,y);
            //                   }
            //                } 
         } // case
         } // swith
         dc.Detach();
}  

