// LRange.cpp: implementation of the LRange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LRange.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int LRange::CalcIndex(float freq)
{  // индекс для частоты
   int idx = int( (freq - Low)*(cPoints-1)/Full );
   if ( idx < 0 ) {
      idx = 0;
      TRACE(_T("%s(%d): LRange: WARNING! Subrange bounds under!\n"),__FILE__,__LINE__);
   } else
   if ( idx >= cPoints ) {
      idx = cPoints-1;
      TRACE(_T("%s(%d): LRange: WARNING! Subrange bounds over!\n"),__FILE__,__LINE__);
   }
   return idx;
}
