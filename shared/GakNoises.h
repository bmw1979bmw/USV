// GakNoises.h: interface for the CGakNoises class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAKNOISES_H__31CBD0A3_3780_11D4_9CCB_0060970CEBA8__INCLUDED_)
#define AFX_GAKNOISES_H__31CBD0A3_3780_11D4_9CCB_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "LCatBase.h"
#include "MAcl.h"
#include "LNSignals.h"

class CGakNoises  
{
public:
// old BOOL Load(LPCTSTR szRegion, LPCTSTR szCarAlias, LPCTSTR /*unused*/,
//       LPCTSTR szRevAlias=_T(""),BOOL bLoadSea=TRUE);

   BOOL Load( 
      LPCTSTR szCarAlias, 
      LPCTSTR szRegion,
      BOOL    bCar = 1,
      BOOL    bSea = 1,
      BOOL    bRev = 1,
      LPCTSTR szRevSuffix = NULL);
   // пуста€ строка - использовать szCarAlias
   // NULL - не загружать
   
   LBase seanoise;
//   int   seaindex;
protected:
   BOOL  LoadSeaNoise(LPCTSTR szRegion);
public:
   // ѕа^2/√ц
   float GetSeaNoise2(BOOL bWinter,float depth,float vangle,float freq);
   float *GetSeaNoiseAngPtr(float depth,float freq,BOOL bNonEqualNotify=TRUE);
   float SeaNoiseCalc(BOOL bWinter, float freq);
   BOOL  NewSeaNoise(int *pseaindex,float depth);

   LBase carnoise;
protected:
   BOOL  LoadCarNoise(LPCTSTR szAlias);
public:
   // ѕа^2/√ц
   float  GetCarNoise2(float depth,float speed,float vert_angle,float hor_angle,float freq,BOOL *pbFreqErr=NULL);
   BOOL  IsCarNoiseNoAngle();
   BOOL  NewCarNoise(int *pcarindex, float depth, float speed, float freq);

   LBase revnoise;
protected:
   BOOL  LoadRevNoise(LPCTSTR szRegion,LPCTSTR szRevAlias);
public:

   // дл€ формировани€ спектра == излученному
   float GetRevNoise41(   // ѕа^2  - !!! 
      LSSignal &sig, // куда всЄ запихнуть (добавить) - вначале чистите сами
      float depth,   // глубина
      float vangle,  // верт. угол
      float bearing, // пеленг прихода реверберации  !!! 41
      float curtime/*ms*/,
      float Vms,  // мо€ скорость м/с по оси излучени€
      BOOL bRealAddOnly // - в реальную часть sig добавить
      );

   // без спектра - всЄ, что попадает в полосу - сумма
   float GetRevNoise51(   // ѕа^2/√ц - !!! - дл€ суммировани€ помех в гаке
      float low,    // приЄм - нижн€€
      float full,   // приЄм - полоса
      float depth,  // глубина
      float vangle, // верт. угол
      float bearing, // пеленг прихода реверберации
                    // если гориз угол не попадает в сектор излучени€ - 
                    // этот сигнал не суммируетс€
      float curtime/*ms*/,
      float Vms  // мо€ скорость м/с по оси излучени€
      );

   BOOL  ExistRevNoise();

   void AddSignForRev(
       float time/*ms*/, // time  врем€ излучени€ сигнала    мс 
       const CMAcl *pAcl,
       float bearing,  // пеленг излучени€ волны, ∞
       float sector    // сектор излучени€, ∞  0-не использовать
      );

	void ResetRevSigns(); // удалить всех

protected:
public:
   CGakNoises();
   virtual ~CGakNoises();

   // излученные сигналы хран€тс€ в списке, пока у них не вышло врем€ реверберации
   struct TRevSign {
      float time;   // врем€ излучени€ сигнала    мс
      float bearing;  // пеленг излучени€ волны, ∞
      float sector; // сектор излучени€, ∞      
      CMAcl acl;
   };

   CList<TRevSign, TRevSign &> m_SignList;

   float m_REVKoeff;
};

extern CGakNoises g_Noises;

#endif // !defined(AFX_GAKNOISES_H__31CBD0A3_3780_11D4_9CCB_0060970CEBA8__INCLUDED_)
