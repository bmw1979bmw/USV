// MainView.cpp : implementation of the CMainView class
//

#include "stdafx.h"
#include "USV.h"

#include "MainDoc.h"
#include "MainView.h"
#include "GoodFunc.h"
#include "GakStructs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

BEGIN_MESSAGE_MAP(CMainView, CFormView)
   //{{AFX_MSG_MAP(CMainView)
   ON_COMMAND_RANGE(ID_LEFT,ID_ENTER,OnKeysDown)
   ON_COMMAND(ID_TRANSMITT, OnTransmitt)
   ON_BN_CLICKED(IDC_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_UPR_KURS, OnUprKurs)
	ON_BN_CLICKED(IDC_UPR_SPEED, OnUprSpeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainView construction/destruction

CMainView::CMainView()
   : CFormView(CMainView::IDD)
{
   //{{AFX_DATA_INIT(CMainView)
   //}}AFX_DATA_INIT
   // TODO: add construction code here
   m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), 
      MAKEINTRESOURCE(IDR_MAINVIEW));
  	m_A = 0.0;
	m_A_ALT = 0.0;
	m_ALT = 0.0;
	m_KURS = 0.0;
	m_SPEED = 30.0;
	m_SU_KURS = 1;
	m_SU_SPEED = 0;
	m_ZIRC = 1;
	m_POWER_CMD = 0;

}

CMainView::~CMainView()
{
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMainView)
   //}}AFX_DATA_MAP
   switch ( m_nUprCode ) {
   case 0:{
   TXParams &p = GetDocument()->m_Me;
   DDX_Text(pDX, IDC_EDIT1, p.X);
   DDX_Text(pDX, IDC_EDIT2, p.Y);
   DDX_Text(pDX, IDC_EDIT3, p.Z);
   DDX_Text(pDX, IDC_EDIT4, p.K);
   DDX_Text(pDX, IDC_EDIT5, p.V);
   DDX_Text(pDX, IDC_EDIT6, p.L);
   DDX_Text(pDX, IDC_EDIT7, p.D);
   DDX_Hex(pDX, IDC_EDIT10, p.dwNoises);
   DDX_Text(pDX, IDC_EDIT11, p.dwAcFlags);
   DDX_Text(pDX, IDC_A, m_A);
	DDX_Text(pDX, IDC_KURS, m_KURS);
	DDX_Text(pDX, IDC_ZIRC, m_ZIRC);
	DDX_Text(pDX, IDC_SPEED, m_SPEED);
	DDX_Text(pDX, IDC_SU_SPEED, m_SU_SPEED);
   float fms = p.V*KnMe;
   DDX_Text(pDX, IDC_EDIT8, fms);
          }
   case 2:
   DDX_Text(pDX, IDC_A, m_A);
	DDX_Text(pDX, IDC_KURS, m_KURS);
	DDX_Text(pDX, IDC_ZIRC, m_ZIRC);
   	break;
   case 4:
	DDX_Text(pDX, IDC_SPEED, m_SPEED);
	DDX_Text(pDX, IDC_SU_SPEED, m_SU_SPEED);
   	break;
   default: ;
   }
//   if(pDX->m_bSaveAndValidate){
//   }

//    LRange &r = GetDocument()->m_Range;
// 
//    DDX_Text(pDX, IDC_CPOINTS, r.cPoints);
//    DDX_Text(pDX, IDC_LOW, r.Low);
//   DDX_Text(pDX, IDC_FULL, r.Full);

   m_nUprCode = 0;

}

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

   return CFormView::PreCreateWindow(cs);
}

void CMainView::OnInitialUpdate()
{
   CFormView::OnInitialUpdate();
   GetParentFrame()->RecalcLayout();
   ResizeParentToFit();


}

/////////////////////////////////////////////////////////////////////////////
// CMainView diagnostics

#ifdef _DEBUG
void CMainView::AssertValid() const
{
   CFormView::AssertValid();
}

void CMainView::Dump(CDumpContext& dc) const
{
   CFormView::Dump(dc);
}

CMainDoc* CMainView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMainDoc)));
   return (CMainDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainView message handlers

void CMainView::OnTransmitt() 
{
   if ( !UpdateData(TRUE) ) return ;
   
//   GetDocument()->Transmitt(m_Angle,m_Sector,m_Level,m_Freq,m_Length);
}


//DEL void CMainView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
//DEL {
//DEL    CMainDoc *doc = GetDocument();
//DEL 
//DEL    if ( ! ::IsDlgButtonChecked(m_hWnd,IDC_DRIVE) ) goto _default;
//DEL 
//DEL    switch ( nChar ) {
//DEL    case VK_HOME:
//DEL       doc->m_Me.D += nRepCnt;
//DEL       break;
//DEL    case VK_END:
//DEL       doc->m_Me.D -= nRepCnt;
//DEL       break;
//DEL    case VK_UP:
//DEL       doc->m_Me.V += nRepCnt;
//DEL       break;
//DEL    case VK_DOWN:
//DEL       doc->m_Me.V -= nRepCnt;
//DEL       break;
//DEL    case VK_LEFT:
//DEL       doc->m_Me.K -= nRepCnt;
//DEL       break;
//DEL    case VK_RIGHT:
//DEL       doc->m_Me.K += nRepCnt;
//DEL       break;
//DEL    case VK_NEXT:
//DEL       break;
//DEL    case VK_PRIOR:
//DEL       break;
//DEL    default:
//DEL   _default:
//DEL       CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
//DEL       return ;
//DEL    }
//DEL    UpdateData(FALSE);
//DEL }

//************************************
// Method:    OnUpdate
// FullName:  CMainView::OnUpdate
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: CView * pSender
//************************************
void CMainView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
#ifdef ZERORETURN
return ;
#endif

   if(FLAG_NOUPDATE ||::IsDlgButtonChecked(m_hWnd,IDC_NOUPDATE)) return;

   // TODO: Add your specialized code here and/or call the base class
   UpdateData(FALSE);
}

//DEL void CMainView::OnKeydownCtrlBtn(NMHDR* pNMHDR, LRESULT* pResult) 
//DEL {
//DEL    *pResult = 0;
//DEL    NMLVKEYDOWN* pkd = (NMLVKEYDOWN*)pNMHDR;
//DEL    // TODO: Add your control notification handler code here
//DEL    UINT nChar    = pkd->wVKey;
//DEL    UINT nRepCnt  = pkd->flags; 
//DEL 
//DEL    CMainDoc *doc = GetDocument();
//DEL 
//DEL    if ( ! ::IsDlgButtonChecked(m_hWnd,IDC_DRIVE) ) return;
//DEL 
//DEL    switch ( nChar ) {
//DEL    case VK_HOME:
//DEL       doc->m_Me.D += nRepCnt;
//DEL       break;
//DEL    case VK_END:
//DEL       doc->m_Me.D -= nRepCnt;
//DEL       break;
//DEL    case VK_UP:
//DEL       doc->m_Me.V += nRepCnt;
//DEL       break;
//DEL    case VK_DOWN:
//DEL       doc->m_Me.V -= nRepCnt;
//DEL       break;
//DEL    case VK_LEFT:
//DEL       doc->m_Me.K -= nRepCnt;
//DEL       break;
//DEL    case VK_RIGHT:
//DEL       doc->m_Me.K += nRepCnt;
//DEL       break;
//DEL    case VK_NEXT:
//DEL       break;
//DEL    case VK_PRIOR:
//DEL       break;
//DEL    case VK_RETURN:
//DEL       OnTransmitt();
//DEL       break;
//DEL    default:
//DEL //  _default:
//DEL //    CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
//DEL       return ;
//DEL    }
//DEL    UpdateData(FALSE);
//DEL    
//DEL }

void CMainView::OnBtnOk() 
{
   // TODO: Add your control notification handler code here
   UpdateData(TRUE);   
}

BOOL CMainView::PreTranslateMessage(MSG* pMsg) 
{
   // TODO: Add your specialized code here and/or call the base class
//   if (  ::IsDlgButtonChecked(m_hWnd,IDC_DRIVE) ) {
   if (m_hAccelTable) {
      if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) {
         return TRUE;      
      }   
   }
//   }
   return CFormView::PreTranslateMessage(pMsg);
}

void CMainView::OnKeysDown(UINT nID)
{
   CMainDoc *pDoc = GetDocument();

   //
   switch ( nID ) {
   case ID_UP:
  	m_SPEED+=1;
      if(m_SPEED>pDoc->SPEED_MAX) m_SPEED=pDoc->SPEED_MAX;
      pDoc->SPEED_CMD=m_SPEED;


   m_SU_SPEED=pDoc->SU_SPEED =  1;
   
   pDoc->rachet_pow=FALSE;
	pDoc->Comand_V();
      break;
   case ID_DOWN:
  	m_SPEED-=1;
  if(m_SPEED<pDoc->SPEED_MIN) m_SPEED=pDoc->SPEED_MIN;
   pDoc->SPEED_CMD=m_SPEED;

   m_SU_SPEED=pDoc->SU_SPEED =  1;
   
   pDoc->rachet_pow=FALSE;
	pDoc->Comand_V();
      break;
   case ID_LEFT:

   m_A=pDoc->A_ZACLAD = 10;
	m_KURS-=1;
	m_SU_KURS=pDoc->SU_KURS  =  1;
	m_ZIRC=pDoc->ZIRC     =	0;

      if ( m_KURS < 0 ) m_KURS+= 360;

      pDoc->KURS_CMD=m_KURS;
//   pDoc->avto_exit=pDoc->perechet=FALSE;
   pDoc->dv=pDoc->SPEED_CMD-pDoc->m_Me.V;
   pDoc->Comand_K();

      break;
   case ID_RIGHT:
   m_A=pDoc->A_ZACLAD = 10;
	m_KURS+=1;
	m_SU_KURS=pDoc->SU_KURS  =  1;
	m_ZIRC=pDoc->ZIRC     =	1;

      if ( m_KURS >= 360 ) m_KURS-= 360;
      pDoc->KURS_CMD=m_KURS;
//   pDoc->avto_exit=pDoc->perechet=FALSE;
   pDoc->dv=pDoc->SPEED_CMD-pDoc->m_Me.V;
   pDoc->Comand_K();
      break;
   case ID_ENTER:
      OnTransmitt();
      break;
   default:
      return ;
   }
   UpdateData(FALSE);
}

//DEL void CMainView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	
//DEL 	//CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
//DEL    CDC dc;
//DEL    dc.Attach(pDIS->hDC);
//DEL    dc.SelectStockObject(WHITE_PEN);
//DEL    dc.Ellipse(&pDIS->rcItem);
//DEL    CRect rc;
//DEL    rc.CopyRect(&pDIS->rcItem);
//DEL    ///////////////////////////////////////////
//DEL    CPen pnlt(0,0,0x00C0C0C0), pndk(0,0,0x00808080);
//DEL    dc.SelectObject(&pndk);
//DEL    dc.Arc(rc,CPoint(rc.right,rc.top),CPoint(rc.left,rc.bottom));
//DEL    rc.DeflateRect(1,1);
//DEL    dc.SelectObject(&pnlt);
//DEL    dc.Ellipse(rc);
//DEL    ///////////////////////////////////////////
//DEL    CMainDoc *d = GetDocument();
//DEL 
//DEL    CBrush br(0x00622CEF);
//DEL 
//DEL    dc.SelectStockObject(DEFAULT_GUI_FONT);
//DEL    TCHAR buf[32];
//DEL //   wsprintf(buf,_T("%4d  "),d->m_MyWaves.GetCount());
//DEL    dc.SetBkColor(GetSysColor(COLOR_3DFACE));
//DEL    dc.TextOut(0,0,buf,6);
//DEL 
//DEL    dc.SetBkMode(TRANSPARENT);                
//DEL    dc.SetTextAlign(TA_BASELINE|TA_CENTER);
//DEL   // d->m_MyWaves;
//DEL    TXParams &Me = d->m_Me;
//DEL //   CMWave *wo;
//DEL    int r = pDIS->rcItem.right/2;
//DEL 
//DEL    if ( d->m_bBump ) {
//DEL       dc.SelectObject(&br);
//DEL       dc.Ellipse(r/2,r/2,r+r/2,r+r/2);      
//DEL       dc.SelectStockObject(WHITE_BRUSH);
//DEL    }
//DEL 
//DEL //    for( POSITION pos = d->m_MyWaves.GetHeadPosition(); pos; ) {
//DEL //       CMainDoc::TMyWave &w = d->m_MyWaves.GetNext(pos);
//DEL //       float bear = GrRd*(90-w.bearing+Me.K);
//DEL //       long x = (long)(r+(r-15)*cos(bear));
//DEL //       long y = (long)(r-(r-15)*sin(bear));
//DEL // 
//DEL //       if ( w.bAcl ) {
//DEL //          dc.SetTextColor(0xFF);
//DEL //       } else {
//DEL //          dc.SetTextColor(0x00FF0000);
//DEL //       }
//DEL //       dc.TextOut(x,y,w.bAcl?_T("A"):_T("Ш"),1);
//DEL //       dc.MoveTo(r,r);
//DEL //       dc.LineTo(x,y);
//DEL //   }
//DEL 
//DEL    dc.SelectStockObject(BLACK_PEN);
//DEL    dc.Detach();
//DEL }  

void CMainView::OnUprKurs() 
{
   m_nUprCode = 2;// TODO: Add your control notification handler code here
     if (!UpdateData(TRUE) ) return;

   CMainDoc *pDoc = GetDocument();
   if(fabs(m_A)>35)  {
   m_A=35;
   CString s;
   s.Format("Исполняемое значение угла закладки %.0f",m_A);
   AfxMessageBox(s);
   }
   if(m_A<0)  {m_A=fabs(m_A);
   CString s;
   s.Format("Исполняемое значение угла закладки %.0f",m_A);
   AfxMessageBox(s);
   }
   pDoc->A_ZACLAD = 	m_A;

   if(m_KURS<0) { 
   while(m_KURS<0)  m_KURS+=360;
   CString s;
   s.Format("Исполняемое значение курса %.0f",m_KURS);
   AfxMessageBox(s);
   }
   if(m_KURS>=360) {   while(m_KURS>=360) m_KURS-=360;
   CString s;
   s.Format("Исполняемое значение курса %.0f",m_KURS);
   AfxMessageBox(s);
   }

                if(!(m_SU_KURS==1 || m_SU_KURS==2 || m_SU_KURS==3))
               {
                   m_SU_KURS=1;
              AfxMessageBox("Способ управления 1");
               }

   pDoc->KURS_CMD     =	m_KURS;
	pDoc->SU_KURS  =  m_SU_KURS;
	pDoc->ZIRC     =	m_ZIRC;
   pDoc->dv=pDoc->SPEED_CMD-pDoc->m_Me.V;

   pDoc->Comand_K();


	
}


void CMainView::OnUprSpeed() 
{
   m_nUprCode = 4;
	// TODO: Add your control notification handler code here
  if (!UpdateData(TRUE) ) return;

   CMainDoc *pDoc = GetDocument();
   if(fabs(m_SPEED)>pDoc->SPEED_MAX)  {
   m_SPEED=pDoc->SPEED_MAX;
   CString s;
   s.Format("Исполняемое значение скорости %.0f",m_SPEED);
   AfxMessageBox(s);
   }
   if(m_SPEED<pDoc->SPEED_MIN)  {
   m_SPEED=pDoc->SPEED_MIN;
   CString s;
   s.Format("Исполняемое значение скорости %.0f",m_SPEED);
   AfxMessageBox(s);
   }

	pDoc->SPEED_CMD    =	m_SPEED;
	pDoc->SU_SPEED =  m_SU_SPEED;
   
   pDoc->rachet_pow=FALSE;
	pDoc->Comand_V();
}
