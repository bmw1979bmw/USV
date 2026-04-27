// MAcl.h: interface for the CMAcl class.
#pragma once

#define TYPE_TONE  0
#define TYPE_LFM   1
#define TYPE_HFM   2
#define TYPE_NOISE 3

class CMAcl  
{
public:
	BOOL ReducePacks(int nMaxPacks);
	void DeleteContents(void);
	float CalcLength();
	float GetMaxLevel(int *pidx);

#ifdef __TEXTARCHIVE_H__
   void Serialize(CTextArchive &ar);
#endif

   void Serialize(CArchive &ar);
   void Create(int npacks, int nsign);
   CMAcl();
   CMAcl(const CMAcl &Acl);
   void Copy(const CMAcl &Acl);
   CMAcl(int npacks, int nsign);
   /*virtual*/ ~CMAcl();
   void operator=(const CMAcl &Acl) { Copy(Acl); }

   int   nPacks;                // number of packs
   struct TAclPack {
      float Level;   // relative level of pack(0 to 1) уровень
      float Delay;   // задержка пачки, msec
   } *pPacks;

   int   nSignals;              // количество элементарных сигналов в посылке
   struct TAclSignal {
      int   type;      // 0-TON,1-LFM 2-HFM 3-NOISE тип элементарного сигнала
      float length;    // TIMER  lenght,msec длительность элементарного сигнала, мсек.  
      float delay;     // TIMER  lenght,msec задержки элементарных сигналов, мсек.
      float freq;      // Hz   несущие частоты элементарных сигналов, √ц                 средн€€!
      float band;      // Hz   полосы элементарных сигналов, √ц          тон: 1/(length/1000)
      float level;     // level уровни излучени€ ѕа !!!!! - GetRev, fromAcl, OGS::Operate
      int   code;      // кодировка дл€ отладки и разв€зки
   } *pSignals;

};

#define BUR_CODE(id)  ((int)(0x81000000ul|(id)))
#define VIST_CODE(id)  ((int)(0x82000000ul|(id)))
#define IS_VIST_CODE(code) (((DWORD)code&0xFF000000ul)==0x82000000ul)
 
