// LRange.h: interface for the LRange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LRANGE_H__97D46C61_E22A_11D4_B6E5_0050DAC48A11__INCLUDED_)
#define AFX_LRANGE_H__97D46C61_E22A_11D4_B6E5_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GakStructs.h"

class LRange
{
public:
   int   cPoints;
   float Low;        
   float Full;
   float Overlap;  // перекрытие выборки акля (множитель к шагу выборки акля)  
   float Middle(){ return Low+Full/2; }
   float Mid(){ return Low+Full/2; }
   float dF() { return Full/(cPoints-1); }  // дельта по частоте
   float High()  { return Low+Full; }
   float operator[](int i) { return Low+i*Full/(cPoints-1); } 

	int  CalcIndex(float freq);
};

#endif // !defined(AFX_LRANGE_H__97D46C61_E22A_11D4_B6E5_0050DAC48A11__INCLUDED_)
