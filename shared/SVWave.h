// SVWave.h: interface for the CSVWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_)
#define AFX_SVWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MWave.h"
#include "LNSignals.h"


class CSVWave : public CMWave  
{
public:
   LSSignal *sv_pSigNB;
   LSSignal *sv_pSigNBP;
   float    sv_Dist;       // по вертикали
//    float    sv_VBearing;   // по вертикали
   float    sv_Bearing; // по горизонтали
   float    sv_Time; // время
   float    sv_Level; // xz
   int      sv_FiltrMax;

	CSVWave();
	virtual ~CSVWave();

};

typedef CTypedPtrList<CPtrList, CSVWave *> CSVWaveList;

#endif // !defined(AFX_SVWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_)
