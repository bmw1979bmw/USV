// SVWave.cpp: implementation of the CSVWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVWave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSVWave::CSVWave()
{
   sv_pSigNB = NULL;
   sv_pSigNBP = NULL;
   sv_Dist = 0.f;       // по вертикали
   sv_Bearing = 0.f;    // по горизонтали
   sv_Time = 0.f;       // время
   sv_Level = 0.f;      // xz
   sv_FiltrMax = 0;
}

CSVWave::~CSVWave()
{
   delete sv_pSigNB;
   delete sv_pSigNBP;
}
