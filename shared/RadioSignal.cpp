// RadioSignal.cpp: implementation of the RadioSignal  classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RadioSignal.h"
#include "Math.h"
#include "Math2.h"
#include "FuncSv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRadioSignal::CRadioSignal()
{
   // 3 !!!
	m_F0 = 155.f;     //( 136.f + 174.f ) / 2.0;
	m_LenWave = 1.9355f;   // 300.f / m_F0;  // длина волны
	m_Teta = 0.9f;            // коэф передачи мощности антенного тракта
   m_Teta2 = 0.81f;
	
	m_RadioS = 0.f;
	m_RadioN = 0.f;
	m_RadioP = 0.f;
	m_Pom = 0;
   m_H2_Dop = 0;
	m_Tip = 0;
	m_DN = 0;
	m_HAnt = 0;
	m_HRgab = 0;
	m_HBur = 0;
}

void CRadioSignal::Store00( int Tip, float DN, float H2_DOP, float hrgab, float hbur )
{
	m_H2_Dop = H2_DOP;   // пороговое H2
	m_Tip  = Tip;
	m_DN   = DN;
   m_HRgab = hrgab;    // высота радиоантенны 
	m_HBur  = hbur;     // высота радиоантенны 
}

void CRadioSignal::Store0( TXParams* MR )
{
	X = MR->X;
	Y = MR->Y;
	Z = MR->Z;
	m_HAnt = MR->Z;

	m_RadioS = 0.f;
	m_RadioN = 0.f;
	m_RadioP = 0.f;
	m_Pom = 0;
}

// float RadioSignal::TSum()
// {  //   возвращает полную эквивалентную температуру шумов всей приемной системы
//    float T0 = 290.f;  // абсолютная Т среды
// 
//    // эквивалентная температура шумов антенны
//    float ta = 0.f;
//    // космическое радиоизлучение
//    float t_k = 2500;
//    ta += t_k;
//    // прием излучения атмосферы
//    float t_a = T0 - 32;
//    ta += t_a;
//    // прием излучения земной поверхности
//    float r_z = 0.35;
//    float t_z = T0 * ( 1 - r_z * r_z );
//    ta += t_z;
//    // отражение атмосферного излучения от морской поверхности
//    float t_az = t_z * r_z * r_z;
//    ta += t_az;
//    // омические потери энергии в элементах антенны
//    float t_h = 0;
//    ta += t_h;
// 
//    // эквивалентная температура шумов приемника
//    float tpr = 190.f;
// 
//    float teta = 0.9f;   // коэф. передачи волнового тракта
//    
//    float tsum = 0.f;
//    tsum = ta + T0 * ( 1 - teta ) / teta + tpr / teta;
//    return tsum;
// }

// float CRadioSignal::DMax( float h /*волны*/ )
// {  // вычисление максимальной дистанции радиопередачи
// 	float dsv = 4.12f * ( (float)sqrt( (float)fabs(m_HAnt) ) + (float)sqrt( (float)fabs(h) ) ); // дистанция связи
// 	return dsv;     //120000;
// }      

int CRadioSignal::AddSignal( CRWave *ro, int Pom )
{  // вычисление мощности принятого сигнала, возвращает 1 - сигнал, надо записывать  P D
   int sig_ok = 0;

   // высота антенны

   // --- км
	float x = ro->X - X;
	float y = ro->Y - Y;
	float h = ( m_HRgab - (float)fabs(Z) ) * 0.001f;     // / 1000.f; км
   float xx = x * x;
   float yy = y * y;
   float xy = xx + yy;
	float dist2 = (float)(sqrt( xy ));  // км
	float dist3 = (float)(sqrt( xy + h*h ));  // км
// 	float dist2 = Distance2( ro->X, ro->Y, X, Y );  // км
// 	float dist3 = Distance3( ro->X, ro->Y, m_HRgab, X, Y, (float)fabs(Z) );  // км

   float beta = (float)acos( dist2 / dist3 );  // угол к горизонту рад
	float dn1 = GetDN( ro->DN, ro->Tip, beta );
	float dn2 = GetDN( m_DN, m_Tip, beta );
   float poteri = GetLDop();
   float umn = ro->Power * m_Teta2 / poteri;
	float mm_RadioS = umn * dn1 * dn2 / GetL0( dist3 * 1000.f );
	//float mm_RadioS = ro->Power * dn1 * dn2 * m_Teta2 / GetL0( dist3 * 1000.f ) / GetLDop();
	
   if ( mm_RadioS > m_RadioS )    
   {  // замена сигнала большим
      sig_ok = 1;
      m_RadioP += m_RadioS;
      m_RadioS = mm_RadioS;

      // вычисление мощности шума для максимальной дистанции связи
      float dst;   // 120000 км
      if ( Pom == 0 ) dst = DMax( m_HRgab );   // 120000 км
      else dst = DMax( m_HBur );   // 120000 км
      m_RadioN = umn * ro->DN * m_DN / GetL0( dst * 1000.f );
//      m_RadioN = ro->Power * ro->DN * m_DN * m_Teta2 / GetL0( dst * 1000.f ) / GetLDop();
   }
   else
   {  // сигнал есть - добавить помеху
      m_RadioP += mm_RadioS;
   }
   return sig_ok;
}

void CRadioSignal::AddPomeha( CRWave *ro )
{  // добавить мощность помехи от ro
   // --- км
	float x = ro->X - X;
	float y = ro->Y - Y;
	float h = ( m_HBur - (float)fabs(Z) ) * 0.001f;     // / 1000.f; км
   float xx = x * x;
   float yy = y * y;
   float xy = xx + yy;
	float dist2 = (float)(sqrt( xy ));  // км
	float dist3 = (float)(sqrt( xy + h*h ));  // км
// 	float dist2= Distance2( ro->X, ro->Y, X, Y );
// 	float dist3= Distance3( ro->X, ro->Y, m_HBur, X, Y, (float)fabs(Z) );
	float beta = (float)acos( (float) dist2 / dist3 );  // угол к горизонту рад
	float dn1 = GetDN( ro->DN, ro->Tip, beta );
	float dn2 = GetDN( m_DN, m_Tip, beta );
	m_RadioP += ro->Power * dn1 * dn2 * m_Teta2 / GetL0( dist3 * 1000.f ) / GetLDop();
}

// void CRadioSignal::AddPomehaB( CRWave *ro )
// {  // добавить мощность помехи от ro
// 	float dist2= Distance2( ro->X, ro->Y, X, Y );
// 	float dist3= Distance3( ro->X, ro->Y, m_HBur, X, Y, (float)fabs(Z) );
// 	float beta = (float)acos( (float) dist2 / dist3 );  // угол к горизонту рад
// 	float dn1 = GetDN( ro->DN, ro->Tip, beta );
// 	float dn2 = GetDN( m_DN, m_Tip, beta );
// 	m_RadioPB += ro->Power * dn1 * dn2 * m_Teta * m_Teta / GetL0( dist3 * 1000.f ) / GetLDop();
// //	m_RadioN = ro->Power * ro->DN * m_DN * m_Teta * m_Teta / GetL0( dist3 * 1000.f ) / GetLDop();
// 
//    // вычисление мощности шума для максимальной дистанции связи
// 	float dst = DMax( m_HRgab );   // 120000 км
// 	float mm_RadioN = ro->Power * ro->DN * m_DN * m_Teta * m_Teta / GetL0( dst * 1000.f ) / GetLDop();
//    if ( mm_RadioN > m_RadioN )    // ?????????????
//    {
//       m_RadioN = mm_RadioN;
//    }
// 	m_Pom = 1;
// }



