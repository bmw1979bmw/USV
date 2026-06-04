// MgMoment.cpp: implementation of the CMgMoment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MgWave.h"
#include "stru_rgab.h"
#include "FuncSv.h"
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMgMoment::CMgMoment()
{
    InitTime = 0;
    nClass   = 0;
    nSourceId  = 0;
    
    M = 0;     // магнитный момент
    MXY.X = 0;
    MXY.Y = 0;
    MXY.K = 0;
    MXY.V = 0;
    MXY.Z = 0;
    MXY.D = 0;
    MXY.L = 0;
}

CMgMoment::~CMgMoment()
{
}


void CMgMoment::Serialize(CArchive & ar)
{
}

float CMgMoment::GetMoment( TXParams* LA )
{   // возвращает величину момента или -1
	float kf = 0.05f;     // наилучшие условия приема
	float L = Distance3( LA->X, LA->Y, LA->Z, MXY.X, MXY.Y, MXY.Z );  // расстояние текущее
	float R = (float)pow( M * 100000.f / 3.f / kf, 1.f / 3.f ) * 0.001f;    // дальность приема 
	float ret;
	if ( R < L ) ret = -1;      
	else ret = M;
	
	// вероятность приема
	int vr = rand()%10;
	if ( vr >= 8 ) ret = -1;
	return ret;
}

float CMgMoment::GetLenta( TXParams* LA )
{   // возвращает ширину полосы приема 
	float kf = 0.05f;     // наилучшие условия приема
	float R = (float)pow( M * 100000.f / 3.f / kf, 1.f / 3.f ) * 0.001f;    // дальность приема 
	float ret;
	ret = 2.f * (float)sqrt( (float)fabs( R * R - ( LA->Z - MXY.Z ) * ( LA->Z - MXY.Z ) * 0.000001f ));
	return ret;
}






