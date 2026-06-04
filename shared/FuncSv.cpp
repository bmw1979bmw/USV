// FuncSv.cpp: implementation of the FuncSv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FuncSv.h"
#include "RegsKey.h"
#include "math2.h"

#include "DebugFlags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FuncSv::FuncSv()
{
}


BOOL Distance2qlt( float x1, float y1, float x2, float y2, float dist) //km
{  // истина, если расстояние  меньше заданного
	dist*=dist;
	float x = x1 - x2;
	x*=x;
	if ( x > dist ) return FALSE;
	float y = y1 - y2;
	y*=y;
	if ( y > dist ) return FALSE;
	return ((x+y) <= dist);
}


///////////////////////определение вертикального  курсового  угла////////////////////////////////////////////////
//отрицательный против   часовой стрелки
float  VERT_ANG_KOORD(float  X1,float  Y1,float  H1,float  X2,float  Y2,float  H2,float  diff)
{
	float d=Distance2( X1, Y1, X2, Y2);
	float dz = H1-H2;
	return (float)(atan2(dz,d)*RdGr-diff);
}

////////////////определение курсового угла//////////////////////////////////////////////////
float  KURS_ANG_KOORD(float  X1,float  Y1,float  X2,float  Y2,float  K2)
{
	float  d_k=PELENG(X1, Y1, X2, Y2)-K2;
	if(d_k>0 && d_k<=180)
		return(d_k);
	if(d_k>0 && d_k>180)
		return(d_k-360);
	if(d_k<0 && d_k>=-180)
		return(d_k);
	if(d_k<0 && d_k<-180)
		return(d_k+360); else return 0.;
}
// -------------------------------

// --- вычисление ОСП для одного числа
float CalcOSPI( float signA, float signS, float porog )
{
	float pomecha, signal;
	if ( signS < porog ) { pomecha = porog; } 
	else { pomecha =  signS; }
	
	if ( signA < porog ) { signal = 0; } 
	else { signal =  signA; }
	
	return (float) (signal / pomecha);
}

// --- суммирование 2-х пеленгов ( в градусах 0-360 )
float PelengSum( float a, float b )
{  
	a += b;
	if ( qlt0(a) ) a += 360.f;
	else if ( a >= 360.f ) a -= 360.f;
	return a;  
}             

//float fround3( float x ) 
//{  // округление до 3-х знаков
//	x -= 0.0005f;
//	x *= 1000.f;
//	x = (float)ceil(x);
//	return x * 0.001f;            //      / 1000.f;
//}

float fround5( float x ) 
{  // округление до 5-х знаков
	x -= (float)0.000005;
	x = (float)(x * 100000);
	x = (float)ceil(x);
	return (float)(x / 100000);
}

int MinCirk( float oldK, float newK )                
{ 	// расчет кратчайшей циркуляции 1-право  2-лево 0-нет циркуляции
	double d_kl = newK - oldK; 

	
	int crk;
	if ( d_kl>=0 ) 
	{
		if ( d_kl<=180 ) crk = 1;    // правая
		else  crk = 2;    // левая
	} 
	else 
	{
		if ( fabs(d_kl) <= 180 ) crk = 2;   // левая
		else crk = 1;    // правая
	}
	return crk;
}

float GetX(float X0, float D, float K)
{  // новая координата Х на расстоянии D по курсу K
	float X = X0 + D * fsin( GrRd * K );
   return X;
}

float GetY(float Y0, float D, float K)
{  // новая координата Y на расстоянии D по курсу K
	float Y = Y0 + D * fcos( GrRd * K );
   return Y;
}

