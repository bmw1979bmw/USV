// MgMoment.h: interface for the CMgMoment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGMOMENT_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_)
#define AFX_MGMOMENT_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stru_rgab.h"

class CMgMoment     //: public TMParams // TMParams объекта от точки рождения волны:
{
	int operator =(const CMgMoment &); // no implementation
public:
	
public:
	void Serialize(CArchive &ar);
	CMgMoment();
	virtual ~CMgMoment();
	
	float GetMoment( TXParams* LA );   // возвращает величину момента или -1
	float GetLenta( TXParams* LA );    // возвращает ширину полосы приема 
	
	TXParams MXY;        // координаты 
	UINT     nSourceId;  // породил/отразил волну
	UINT     nClass;     // класс источника
	UINT     InitTime;   // время рождения волны, мс
	
	float    M;          // магнитный момент
};


typedef CTypedPtrList<CPtrList, CMgMoment *> CMgMomentList;


#endif // !defined(AFX_MGMOMENT_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_)
