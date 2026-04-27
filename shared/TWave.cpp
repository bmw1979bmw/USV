// TWave.cpp: implementation of the CTWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TWave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTWave::CTWave()
{
   mg_pSigNB = NULL;
   mt_nV=0;
}

CTWave::~CTWave()
{
   delete mg_pSigNB;
}
