// RadioSignal.h: interface for the RadioSignal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOSIGNAL_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_)
#define AFX_RADIOSIGNAL_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "structures.h"
#include "RWave.h"
#include "sv_stru.h"

class CRadioSignalRA : public TXParams // TXParams объекта от точки рождения волны:
{
public:
	
// 	CRadioSignalRA( TXParams* MR, float m_HAnt, float DN, float H2_DOP, float F0, float hrgab, float hbur );
// 	CRadioSignalRA( TXParams* MR, float HAnt, float DN, float H2_DOP, float hrgab, float hbur );
	CRadioSignalRA();
//    CRadioSignalRA( float DN, float H2_DOP, float hrgab, float hbur );    // !!!
	
	int   AddSignal( CRWave *ro );  // вычисление мощности принятого сигнала
	void  AddPomeha( CRWave *ro );  // добавить мощность помехи от ro
	void Store0( TXParams* MR, TTtxRatoborA* TTX );
	void Store00( float DN, float H2_DOP, TTtxRatoborA* TTX  /*, float hrgab, float hbur*/ );
	
inline float DMax( float h )      //   вычисление максимальной дистанции радиопередачи
{  // вычисление максимальной дистанции радиопередачи
	float dsv = 4.12f * ( (float)sqrt( (float)fabs(m_HRatoborA) ) + (float)sqrt( (float)fabs(h) ) ); // дистанция связи
	return dsv;     //7.f;
}      

inline float GetH2()
{  // вычисление H2 с учетом всех помех
   float sumRadioNP = m_RadioN + m_RadioP;
   if ( sumRadioNP == 0.f ) return 0.f;
   float h2 = m_RadioS / sumRadioNP;
   float h22 = h2 * h2;
	// вероятность ошибочного приема
   float aa = h22 + 10.f;
	float ver = 5.f / aa;
   float bb = - 4.f * h22 / aa;
   float cc = (float)exp( bb );
	ver *= cc;
	// новое Н2 c учетом всего
	h2 =  (float)log( 0.5f / ver );  
	h2 *= 2.f;  
	return h2;
}
  
inline float GetLDop( /*float d*/ )
{  //   возвращает дополнительные потери энергии РВ
	return 3.00020001f;
// 	return LDop;
}

inline float GetL0( float d )
{  //   возвращает основные потери энергии РВ
   float aa = m_LenWave * m_LenWave;
	float res = M_4PI * M_4PI;
	res *= d;
	res *= d;
	res /= aa;
	return  res;
}

inline float GetDN( float DN, int tip, float alfa /*рад*/)
{   // расчет диаграммы направленности в зависимости от типа антенны и угла от горизонта
	float dn = 0;
	switch( tip ){
	case 0:  // БМ
		dn = DN * fcos( alfa );
		if ( dn < 0.1f ) dn = 0.1f;
		break;
	case 6:  // вертолет
		dn = DN;
		break;
	default:  // РГАБ
		dn = DN * fcos( alfa );
		if ( dn < 0.1f ) dn = 0.1f;
		break;
	} // switch
	return dn;
}

	float m_RadioS;       // мощность сигнала
	float m_RadioN;       // мощность шума
	float m_RadioP;       // мощность помехи
	
 	float m_F0;       // рабочая частота
	float m_LenWave;  // длина волны
	float m_H2_Dop;   // пороговое H2
 	float m_Teta;     // коэф передачи мощности антенного тракта
 	float m_Teta2;    // m_Teta * m_Teta  -  коэф передачи мощности антенного тракта

	float m_DN;       // DN приемнника
// 	float m_HAnt;     // высота радиоантенны приемника

	float m_HRatoborA;  // высота радиоантенны 
	float m_HPom;     // высота радиоантенны 

   int   m_Tip;      // тип наиболее сильного РГБ
   int   m_NK;       // N канала РГБ
   int   m_SourceId; // Id

   int   m_RGBTipNK;  // Тип обнаруженного канала РГБ
	int   m_RGBNK;     // N обнаруженного канала РГБ
   int   m_RGBId;     // Id РГБ
	float m_RGBPower;  // Пришедшая мощность на обнаруженном канале РГБ

   int   m_RatoborATipNK;  // Тип обнаруженного канала Ч
	int   m_RatoborANK;     // N обнаруженного канала Ч
   int   m_RatoborAId;     // Id прибора
	float m_RatoborAPower;  // Пришедшая мощность на обнаруженном канале Ч
	int   m_RatoborANPR;     // N  Ч
};

#endif // !defined(AFX_RADIOSIGNAL_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_)
