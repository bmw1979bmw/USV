// GraphWndEx.cpp : implementation file
//

#include "stdafx.h"
#include "gak.h"
#include "GraphWndExSV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphWndExSV

BEGIN_MESSAGE_MAP(CGraphWndExSV, CGraphWndSV)
	//{{AFX_MSG_MAP(CGraphWndExSV)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CGraphWndExSV::CGraphWndExSV()
{
   //m_fnt.CreatePointFont( 80, _T("MS Sans Serif") );
}

CGraphWndExSV::~CGraphWndExSV()
{
   m_ptList.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphWndExSV message handlers

void CGraphWndExSV::OnDraw(CDC &dc)
{
   CGraphWndSV::OnDraw(dc);
   CRect rc;
   GetClientRect(rc);
   dc.SelectStockObject(DEFAULT_GUI_FONT);
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(0xFFFF);
   //CFont *ofn = dc.SelectObject(&m_fnt);
   AfterDraw(dc,rc.right,rc.bottom);
   //dc.SelectObject(ofn);
   dc.SelectStockObject(WHITE_PEN);
   dc.SelectStockObject(NULL_BRUSH);
}

void CGraphWndExSV::AfterDraw(CDC & dc, int width, int height)
{
   //CPen yellpn(0,0,0x0000FFFF),*opn;
   //CBrush bluebr(0x00FF0000),*obr;
   dc.SelectStockObject(WHITE_PEN);
   dc.SetROP2(R2_XORPEN);
   
   dc.SetTextColor(0x000000FF);
   DrawTicks(dc,width,height,this);
}
