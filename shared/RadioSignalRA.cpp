// RadioSignal.cpp: implementation of the RadioSignal  classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RadioSignalRA.h"
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

// CRadioSignalRA::CRadioSignalRA(  TXParams* MR, float HAnt, float DN, float H2_DOP, float F0, float hrgab, float hbur )
// {
// //	ASSERT(F0);
//    //1
// // 	m_F0 = F0;       -------------------------------
// 	m_LenWave = 300.f / F0;  // длина волны
// 	m_H2_Dop = H2_DOP;       // пороговое H2
//  	m_Teta = 0.9f;            // коэф передачи мощности антенного тракта
//    m_Teta2 = 0.81f;
// 	X = MR->X;
// 	Y = MR->Y;
// 	Z = MR->Z;
// 	m_DN = DN;
// 	
// 	m_RadioS = 0.f;
// 	m_RadioN = 0.f;
// 	m_RadioP = 0.f;
// 
// 	m_HRatoborA = hrgab;    // высота радиоантенны 
// 	m_HPom  = hbur;     // высота радиоантенны 
// 
// }

// CRadioSignalRA::CRadioSignalRA( TXParams* MR, float HAnt, float DN, float H2_DOP, float hrgab, float hbur )
// {
//    //2    ------------------------
// 	float F0 = 155.f;    //( 136.f + 174.f ) / 2.0;
// 	m_LenWave = 300.f / F0;  // длина волны
// 	m_H2_Dop = H2_DOP;   // пороговое H2
//  	m_Teta = 0.9f;            // коэф передачи мощности антенного тракта
//    m_Teta2 = 0.81f;
// 	X = MR->X;
// 	Y = MR->Y;
// 	Z = MR->Z;
// 	m_DN = DN;
// 	
// 	m_RadioS = 0.f;
// 	m_RadioN = 0.f;
// 	m_RadioP = 0.f;
// 
// 	m_HRatoborA = hrgab;    // высота радиоантенны 
// 	m_HPom  = hbur;     // высота радиоантенны 
// }

CRadioSignalRA::CRadioSignalRA()
{
   // 3 !!!  +++++++++++++++++++++
	m_F0 = 155.f;     //( 136.f + 174.f ) / 2.0;
	m_LenWave = 1.9355f;   // 300.f / m_F0;  // длина волны
 	m_Teta = 0.9f;            // коэф передачи мощности антенного тракта
   m_Teta2 = 0.81f;
	
	m_RadioS = 0.f;
	m_RadioN = 0.f;
	m_RadioP = 0.f;
}

void CRadioSignalRA::Store00( float DN, float H2_DOP, TTtxRatoborA* TTX /*, float hrgab, float hbur*/ )
{
   m_HRatoborA = TTX -> HAnt;
	m_H2_Dop = H2_DOP;   // пороговое H2
	m_DN   = DN;
}

void CRadioSignalRA::Store0( TXParams* MR, TTtxRatoborA* TTX )
{
	X = MR->X;      // м
	Y = MR->Y;
	Z = MR->Z;

   m_HPom = 0;

   m_Tip = 0;      // тип наиболее сильного РГБ
   m_NK = 0;       // N канала РГБ
   m_SourceId = 0;  // Id

	m_RGBPower = 0; // Пришедшая мощность на обнаруженном канале РГБ
   m_RGBTipNK = 0;  // Тип обнаруженного канала РГБ
	m_RGBNK = 0;     // N обнаруженного канала РГБ
   m_RGBId = 0;     // Id РГБ

   m_RatoborATipNK = 0;  // Тип обнаруженного канала Ч
	m_RatoborANK = 0;     // N обнаруженного канала Ч
   m_RatoborAId = 0;     // Id прибора
	m_RatoborAPower = 0;  // Пришедшая мощность на обнаруженном канале Ч

	m_RadioS = 0.f;
	m_RadioN = 0.f;
	m_RadioP = 0.f;
}


int CRadioSignalRA::AddSignal( CRWave *ro )
{  // вычисление мощности принятого сигнала, возвращает 1 - сигнал, надо записывать  P D
   int sig_ok = 0;

   // высота антенны     км
	float x = ro->X - X/*/1000.f*/;
	float y = ro->Y - Y/*/1000.f*/;
	float h = ( m_HRatoborA - ro->HAnt ) * 0.001f;     // / 1000.f; км
   float xx = x * x;
   float yy = y * y;
   float xy = xx + yy;
	float dist2 = (float)(sqrt( xy ));  // км
	float dist3 = (float)(sqrt( xy + h*h ));  // км
// 	float dist2 = Distance2( ro->X, ro->Y, X, Y );  // км
// 	float dist3 = Distance3( ro->X, ro->Y, m_HRgab, X, Y, (float)fabs(Z) );  // км

   float beta = (float)acos( dist2 / dist3 );  // угол к горизонту рад
	float dn1 = GetDN( ro->DN, ro->Tip, beta );
	float dn2 = GetDN( m_DN, 0 /*m_Tip*/, beta );
   float umn = ro->Power * m_Teta2 / GetLDop();
	float mm_RadioS = umn * dn1 * dn2 / GetL0( dist3 * 1000.f );
	//float mm_RadioS = ro->Power * dn1 * dn2 * m_Teta2 / GetL0( dist3 * 1000.f ) / GetLDop();
	
   if ( mm_RadioS > m_RadioS )    
   {  // замена сигнала большим
      sig_ok = 1;
      m_RadioP += m_RadioS;
      m_RadioS = mm_RadioS;

      // вычисление мощности шума для максимальной дистанции связи
      float dst;   // 7 км
      dst = DMax( m_HRatoborA );   // 6 км
      m_RadioN = umn * ro->DN * m_DN / GetL0( dst * 1000.f );

      m_Tip = ro->Tip;      // тип наиболее сильного РГБ
      m_NK = ro->N_K;       // N канала РГБ
      m_SourceId = ro->nSourceId; // Id   2222224567

      m_RGBPower = ro->RGBPower;  // Пришедшая мощность на обнаруженном канале РГБ
      m_RGBTipNK = ro->RGBTipNK;  // Тип обнаруженного канала РГБ
	   m_RGBNK    = ro->RGBNK;     // N обнаруженного канала РГБ
      m_RGBId    = ro->RGBId;     // Id РГБ

      m_RatoborATipNK = ro->CherryTipNK;     // Id прибора                               
	   m_RatoborANK    = ro->CherryNK;     // N обнаруженного канала Ч                 
      m_RatoborAId    = ro->CherryId;  // Тип обнаруженного канала Ч                  
	   m_RatoborAPower = ro->CherryPower;  // Пришедшая мощность на обнаруженном канале Ч 
	   m_RatoborANPR   = ro->CherryNPR;     // N  Ч                 
//      m_RadioN = ro->Power * ro->DN * m_DN * m_Teta2 / GetL0( dst * 1000.f ) / GetLDop();
   }
   else
   {  // сигнал есть - добавить помеху
      m_RadioP += mm_RadioS;
   }
   return sig_ok;
}

void CRadioSignalRA::AddPomeha( CRWave *ro )
{  // добавить мощность помехи от ro
   // --- км
	float x = ro->X - X/*/1000.f*/;   //км
	float y = ro->Y - Y/*/1000.f*/;   //км
	float h = ( ro->HAnt - m_HRatoborA ) * 0.001f;     // / 1000.f; км
   float xx = x * x;
   float yy = y * y;
   float xy = xx + yy;
	float dist2 = (float)(sqrt( xy ));  // км
	float dist3 = (float)(sqrt( xy + h*h ));  // км
// 	float dist2= Distance2( ro->X, ro->Y, X, Y );
// 	float dist3= Distance3( ro->X, ro->Y, m_HBur, X, Y, (float)fabs(Z) );
	float beta = (float)acos( (float) dist2 / dist3 );  // угол к горизонту рад
	float dn1 = GetDN( ro->DN, ro->Tip, beta );
	float dn2 = GetDN( m_DN, 0 /*m_Tip*/, beta );
	float m_RadioPnew = ro->Power * dn1 * dn2 * m_Teta2 / GetL0( dist3 * 1000.f ) / GetLDop();

   if ( m_RGBPower <= m_RadioPnew ) 
   {
      m_Tip = ro->Tip;      // тип наиболее сильного РГБ
      m_NK = ro->N_K;       // N канала РГБ
      m_SourceId = ro->nSourceId;  // Id

      m_RGBPower = m_RadioPnew; // Пришедшая мощность на обнаруженном канале РГБ
      m_RGBTipNK = ro->Tip;  // Тип обнаруженного канала Ч
	   m_RGBNK    = ro->N_K;     // N обнаруженного канала Ч
      m_RGBId    = ro->nSourceId;     // Id прибора

      m_RatoborATipNK = -1;  // Тип обнаруженного канала Ч
	   m_RatoborANK    = -1;     // N обнаруженного канала Ч
      m_RatoborAId    = -1;     // Id прибора
	   m_RatoborAPower = -1;  // Пришедшая мощность на обнаруженном канале Ч
   
      m_RadioP = m_RadioPnew;
   }

//	m_RadioP += m_RadioPnew;
}



