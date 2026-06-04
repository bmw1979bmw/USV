// FuncSv.h: interface for the FuncSv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCSV_H__4F8A4426_66C8_11D5_B79F_0050DAC48A11__INCLUDED_)
#define AFX_FUNCSV_H__4F8A4426_66C8_11D5_B79F_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DebugFlags.h"
#include "Math2.h"

class FuncSv  
{
public:
	FuncSv();
};

// inline int svint( float x ) 
// {  // float -> int
// 	x -= 0.5f;
//    int i(0);
//    _asm
//    {
//        fld x;
// 	   fistp i;
//    }
//    return i;
// }

inline int svint( float x ) 
{  // float -> int
//	x -= 0.5f;
	float y(0.5f);
   int i(0);
   _asm
   {
       fld x;
	   fsub y
	   fistp i;
   }
   return i;
}

inline float Distance2(float X1, float Y1, float X2, float Y2 )
{  // км
	float x = X1 - X2 ;
	float y = Y1 - Y2 ;
	return (float)(sqrt( x*x + y*y ));
}

inline float Distance3(float X1, float Y1, float H1, float X2, float Y2, float H2 )
{
	float x = X1 - X2 ;
	float y = Y1 - Y2 ;
	float h = ( H1 - H2 ) * 0.001f;     // / 1000.f;
	return (float)(sqrt( x*x + y*y + h*h ));
}


//определение пеленга
inline float  PELENG( float X1, float Y1, float X2, float Y2 )
{
    float dx = X2 - X1;
    float dy = Y2 - Y1;
    float pel = bearing2(dx,dy) * RdGr;
    return pel;
}

// --- определение обратного пеленга ( в градусах 0-360 )
inline float  Peleng2(float  pel)
{
	if (  pel >= 0.f   && pel < 180.f )  return pel + 180.f;
	if (  pel == 180.f ) return 0;
	if (  pel > 180.f  && pel <= 360.f ) return pel - 180.f;
	return 0.f;
}


// пересчет пеленга в пределах 0-360
inline float Peleng0_360( float a )
{
ASSERT( a < 5000.0 );
	if ( qlt0(a) ) a += 360.f;
	else if ( a >= 360.f ) a -= 360.f;
   //if ( qlt0(a) || a >= 360.f ) a = Peleng0_360( a );
   return a;  
}

// угол между двумя пеленгами
inline float UgolPP( float p1, float p2 )  
{
	float pp2 = p2 + 360;
	float ug1 = Peleng0_360( pp2 - p1 );
	float ug2 = 360 - ug1;
	if ( ug1 < ug2 ) ug2 = ug1;
	return ug2;  
}

// inline float fround3( float x ) 
// {  // округление до 3-х знаков
// 	x -= 0.0005f;
// 	x *= 1000.f;
// 	x = ceil(x);
// 	return x * 0.001f;       //     / 1000.f;
// }

inline float fround3( float x ) 
{  // округление до 3-х знаков
	float z(1000.f), q(0.001f);
//   int i(0);
   _asm
   {
      fld  x;
	   fmul z;
	   frndint;
	   fmul q;
	   fstp x;
   }
	
	return x;
}

inline void Mem_Set(void *dest1 /*массив*/, /*UINT data чем забить,*/ int count1 /*размерность*/)
{  // заполнение памяти по 32 бита.соunt - размер памяти в 4-х байтовых словах
	_asm
	{
      push edi
      push ecx
      push eax
      mov edi,dest1
		//lea edi,dest
		mov ecx,count1
		mov eax,0      
		rep stosd
      pop eax
      pop ecx
      pop edi
	}

//           _asm
//           {
//             // push edi
//                lea edi,cccc
//                mov ecx,1000
//                mov eax,0
//                rep stosd
//               // pop edi
//           }
}






inline float NewZnachenie( float zn, float delta ) 
{  // вычисление приближенного пеленга
//	return zn;
    float dF = 0.222222222222f * delta;      //   2/9
    int   sl = rand()%10;
    float zn2 = Peleng0_360( zn - delta + sl * dF );
    return zn2;
}




inline float NewZnachenieP( float pp ) 
{      // ---
      int  ipel(-1);   // номер пеленга
      if ( pp >= 0.f ) ipel = (int)(pp / 12.f);   // номер пеленга
      int  ipel1(30);
      int  ipel2(0);
      if ( ipel > 0 )  ipel1 = ipel - 1;
      if ( ipel < 29 ) ipel2 = ipel + 1;
      
      int   ipp, vr1, vr2;
      float vr;
      float newpel(0.f);
      
      while (TRUE) 
      {
         vr1 = rand()%10;
         vr2 = rand()%10;
         vr = (float)( vr1 * 10 + vr2 ) / 100.f;  // 0-99
         ipp = (int)( vr * 30.f );
         if ( ipp != ipel && ipp != ipel1 && ipp != ipel2 ) break;
      }
         newpel = (ipp + 0.5f) * 12.f;
         newpel = NewZnachenie( newpel, 3 );
   return newpel;
}


inline float NewZnachenieD( float zn, float delta ) 
{  // вычисление приближенной дист
//	return zn;
    int   sl = rand()%10;
    float zn2(0);
    if ( zn < 1.f ) 
    {
       zn2 = 3.f + sl / 10.f;
    } 
    else 
    {
       zn2 = sl / 10.f;
    }
    return zn2;
}



BOOL  Distance2qlt( float x1, float y1, float x2, float y2, float dist); //km
float VERT_ANG_KOORD(float  X1,float  Y1,float  H1,
                     float  X2,float  Y2,float  H2,float  diff); //определение вертикального  курсового  угла
float KURS_ANG_KOORD(float  X1,float  Y1,float  X2,float  Y2,float  K2); //определение курсового угла
float CalcOSPI( float signA, float signS, float porog );   // вычисление ОСП для 1 числа
float fround5( float x );  // округление до 5-х знаков
float PelengSum( float a, float b );  // суммирование 2-х пеленгов
int   MinCirk( float oldK, float newK ); // расчет кратчайшей циркуляции 1-право  2-лево

float GetX(float X0, float D, float K);  // новая координата Х на расстоянии D по курсу K
float GetY(float Y0, float D, float K);  // новая координата Y на расстоянии D по курсу K

#endif // !defined(AFX_FUNCSV_H__4F8A4426_66C8_11D5_B79F_0050DAC48A11__INCLUDED_)
