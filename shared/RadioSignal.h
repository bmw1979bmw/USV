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

class CRadioSignal : public TXParams // TXParams объекта от точки рождения волны:
{
public:
	
public:
	
	CRadioSignal();
	
	int   AddSignal( CRWave *ro, int Pom );  // вычисление мощности принятого сигнала
	void  AddPomeha( CRWave *ro );  // добавить мощность помехи от ro
	void Store0( TXParams* MR );
	void Store00( int Tip, float DN, float H2_DOP, float hrgab, float hbur );
	
inline float DMax( float h )      //   вычисление максимальной дистанции радиопередачи
{  // вычисление максимальной дистанции радиопередачи
	float dsv = 4.12f * ( (float)sqrt( (float)fabs(m_HAnt) ) + (float)sqrt( (float)fabs(h) ) ); // дистанция связи
	return dsv;     //120000;
}      

inline float GetH2()
{  // вычисление H2 с учетом всех помех
   float sumRadioNP = m_RadioN + m_RadioP;
   if ( sumRadioNP == 0.f ) return 0.f;
   float h2 = m_RadioS / sumRadioNP;

//    if ( m_Pom == 1 ) 
//    { // есть структурная помеха
// 	   h2 = m_RadioPB / ( sumRadioNP + m_RadioS );
//    } 
//    else 
//    { // обычная помеха
// 	   h2 = m_RadioS / sumRadioNP;
//    }

// 	float kf2 = 2.f;   // наилучшая связь k2
//    float kf22 = 4.f;    //kf2 * kf2;
//    float h22 = h2 * h2;
//    float kf22_2 = 8.f;    //2.f * kf22;
// 	// вероятность ошибочного приема
// 	float ver = ( kf22 + 1.f ) / ( h22 + kf22_2 + 2.f );
// 	ver *= (float)exp( - kf22 * h22 / ( h22 + kf22_2 + 2.f ) );
// 	// новое Н2 c учетом всего
// 	h2 = 2.f * (float)log( 0.5f / ver );  

	//float kf2 = 2.f;   // наилучшая связь k2
   //float kf22 = 4.f;    //kf2 * kf2;
   float h22 = h2 * h2;
   //float kf22_2 = 8.f;    //2.f * kf22;
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
// 	float LDop = 0.f;
// 	// ослабление РВ в тропосфере и ионосфере
// 	LDop += 0.0002f;
// 	// ослабление РВ в дожде и тумане
// 	LDop += 0.00000001f;
// 	// рефракция РВ
// 	LDop += 0.f;
// 	// поляризация РВ
// 	LDop += 3.f;
// 	// тепловое излучение и шумы поглощения
// 	//    float kf_bolz = 1.38e-23;   // пост. Больцмана
// 	//    LDop += kf_bolz * TSum() * 1.3f * ( 136.f + 174.f ) * 0.5f;
// 	return LDop;
}

inline float GetL0( float d )
{  //   возвращает основные потери энергии РВ
//	float res = M_4PI * M_4PI * d * d / m_LenWave / m_LenWave;
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
	default:  // РГАБ   53 62 101 125
		dn = DN * fcos( alfa );
		if ( dn < 0.1f ) dn = 0.1f;
		break;
	} // switch
	return dn;
}

	//    ~RadioSignal();
	//    float RadioSignal::GetL0( float d );  //   возвращает основные потери энергии РВ
	
	float m_RadioS;       // мощность сигнала
	float m_RadioN;       // мощность шума
	float m_RadioP;       // мощность помехи
// 	float m_RadioPB;      // мощность структ. помехи
	
	float m_F0;       // рабочая частота
	float m_LenWave;  // длина волны
	float m_H2_Dop;   // пороговое H2
	float m_Teta;     // коэф передачи мощности антенного тракта
	float m_Teta2;    // m_Teta * m_Teta  -  коэф передачи мощности антенного тракта

	int   m_Tip;      // тип приемника
	float m_DN;       // DN приемнника
	float m_HAnt;     // высота радиоантенны приемника

	float m_HRgab;    // высота радиоантенны 
	float m_HBur;     // высота радиоантенны 

	int   m_Pom;      // наличие структ. помехи
   
};

#endif // !defined(AFX_RADIOSIGNAL_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_)
