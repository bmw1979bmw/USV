// TWave.h: interface for the CTWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_)
#define AFX_TWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MWave.h"
#include "LNSignals.h"

class CTWave : public CMWave  
{
public:
   LSSignal *mg_pSigNB;
   float    mt_Fi;   // по вертикали
   float    mt_Teta; // по горизонтали
   float    mt_Time; // врем€
   float    mt_Level; // xz
   int      mt_nFiltrMax;
   int      mt_nV; //номер выЅорки
  
//   int      mt_nNum1; // xz
//   int      mt_nNum2; // xz

	CTWave();
	virtual ~CTWave();

};

typedef CTypedPtrList<CPtrList, CTWave *> CTWaveList;

#endif // !defined(AFX_TWAVE_H__053FC893_823A_11D8_BB5F_005004552ACE__INCLUDED_)
