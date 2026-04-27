// CheckDC.h: interface for the CCheckDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKDC_H__B73196A1_C2FF_11D5_B805_0050DAC48A11__INCLUDED_)
#define AFX_CHECKDC_H__B73196A1_C2FF_11D5_B805_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
class CCheckDC  
{
   HDC hdc;
   HPEN hpn;
   HBRUSH hbr;
   HFONT hfn;
//    HBITMAP hbm;
//    HBITMAP tempBM;
public:
	CCheckDC(CDC *pDC)
   {
      hdc = pDC->m_hDC;

      hpn = (HPEN)::SelectObject(hdc,GetStockObject(BLACK_PEN));

      hbr = (HBRUSH)::SelectObject(hdc,GetStockObject(WHITE_BRUSH));
      hfn = (HFONT)::SelectObject(hdc,GetStockObject(ANSI_FIXED_FONT));
//       tempBM = ::CreateCompatibleBitmap ( hdc,1,1 );
//       hbm = (HBITMAP)::SelectObject ( hdc, tempBM );
//       hbm = (HBITMAP)::SelectObject ( hdc, tempBM );
   }
   
	virtual ~CCheckDC()
   {
      
      ASSERT(SelectObject(hdc,hpn) == GetStockObject(BLACK_PEN));
      ASSERT(SelectObject(hdc,hbr) == GetStockObject(WHITE_BRUSH));
      ASSERT(SelectObject(hdc,hfn) == GetStockObject(ANSI_FIXED_FONT));
//       ASSERT(SelectObject(hdc,hbm) == tempBM );
//       
//       DeleteObject(tempBM);
   }
};

#ifdef _AFXDLL

inline CPen::~CPen()
{ 
   if ( m_hObject ) {
      VERIFY(DeleteObject()); 
   }
}

inline CBrush::~CBrush()
{ 
   if ( m_hObject ) {
      VERIFY(DeleteObject()); 
   }
}
inline CBitmap::~CBitmap()
{ 
   if ( m_hObject ) {
      VERIFY(DeleteObject()); 
   }
}

inline CFont::~CFont()
{ 
   if ( m_hObject ) {
      VERIFY(DeleteObject()); 
   }
}

#endif

#else 

class CCheckDC  
{
public:
   CCheckDC(CDC *pDC){}
};

#endif

#endif // !defined(AFX_CHECKDC_H__B73196A1_C2FF_11D5_B805_0050DAC48A11__INCLUDED_)
