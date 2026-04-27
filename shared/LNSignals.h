// LNSignals.h: interface for the LSSignal LMSignal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LNSIGNALS_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_)
#define AFX_LNSIGNALS_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "structures.h"
#include "Complex.h"
#include "GakStructs.h"
#include "LCatBase.h"
#include "LRange.h"
#include "MWave.h"


class LSSignal  
{
   LSSignal(const LSSignal &src);  // не нужно присваивать сигналы 
                                   // или возвращать их на стеке
protected:
//    void SetRev(float F0, float Timp, float power); юзай GetRevNoise4
   void SpreadPowerAdd(float power, int L=1);
   void add(LSSignal * what,Complex *weight,int K);
   void add(LSSignal * what,int L,Complex *weight,int K);
   void conj();
   void FFT(int L);
   void FFT2(Complex *s,int L/*1 - переход в частотную область- прямое БПФ */);
   void XFFT(int L);
   void xtransfere(int from,int to);
   void copy(int from,int to);
   UINT impulse(int type,float length,float delay,float level,float currfreq,float band,BOOL bAnalitic,BOOL bTransfere);
   // максимизирует спектр по всему входящему ACLю 
   UINT fromACLmax(CMAcl *pAcl);
   // реальная часть спектра только
   UINT fromACLmaxR(CMAcl *pAcl, 
      BOOL bTranslate=TRUE,
      CMWave::TWRayAcl *o=NULL);

   // для ШП с временем осреднения
//DEL   UINT fromACLaver(CMAcl *pAcl, float Delay,float Taver);

   UINT corrACLaver(float *dd, float Delay,float Taver);
//DEL UINT fromACLold(CMAcl *pAcl, float Delay);
//    UINT fromACL(CMAcl *pAcl, float Delay); //quick version
   Complex summ(int L);

   void mul2(LSSignal &A, LSSignal &B,int L);
   void mul2CR(LSSignal &A, LSSignal &B,int L); // умнож. A.complex на B.re
   void mulpart(LSSignal &with,int startPos,int nPoints,int L);
   void mul(LSSignal &with,int L);
   void mulR(LSSignal &with,int L);  // умнож. this.complex на with.re

//    void fromNBN(TPatt2d *pNBN);
//    void fromWBN(TPatt2d *pWBN);
//    void fromNBNadd(TPatt2d *pNBN);  // добавляет к существующим значениям
//    void fromWBNadd(TPatt2d *pWBN);  // добавляет к существующим значениям
//DEL void fromNBNold(TPatt2d *pNBN, float Taver/*только в старом rand*/);
//DEL void fromWBNold(TPatt2d *pWBN, float Taver/*только в старом rand*/);
   void add(Complex what,int POS,int L);
   void add(float re, float im,int POS,int L);


//////////////////////////////////////////////////////////////////////////
//    Complex *lock(int N){ ASSERT(N>=0 && N<=1); return pData[N]/*+pos*/; }

   BOOL allocate();
   
//////////////////////////////////////////////////////////////////////////
public:
   int    n; //количество точек в сигнале
   float full; //частота дискретизации сигнала
   float low;//нижняя частота сигнала  

   Complex *pData[2];
   Complex * & Time;  // pData[0]
   Complex * & Freq;  // pData[1] 
   /////////////////////////////////////////////
   LSSignal();
   virtual ~LSSignal();
   explicit LSSignal(LSSignal *base);
   explicit LSSignal(LRange &range);

   BOOL create(const LRange &range);    
   void create(LSSignal *from);
   BOOL IsCreated(void) { return !!pData[0]; }
   void clear();
   void copy(LSSignal * from,int L);
   void clear(int L);
   void free();
   void add(LSSignal * what,int L);
   void pow_add(LSSignal * what);
   float power(int L);
   float idxpower(int idx, int L=1);
   void transfere(int from,int to);
   void copy(LSSignal * from);
   void shift(float tay);
   void mulconj(LSSignal * with,int startPos,int nPoints,int L);
   Complex subsumm(int L, int nIdxLo, int nIdxHi);
   float subsummR(int L, int nIdxLo, int nIdxHi);
   void conj(int L);
   Complex corr(LSSignal  &with,int L);
   float corr_max(LSSignal  &with,int L,BOOL bMulSig,int &maxidx);
   float corr_idx_copy(LSSignal  &with,int L,int maxidx);
   float corr_idx_inplace(LSSignal  &with,int L,int maxidx);

   __declspec(deprecated)
   float corr_idx(LSSignal  &with,int L,int maxidx)
   { return corr_idx_inplace(with,L,maxidx); }

   float subpower(int L,int nIdxLo, int nIdxHi);
   float subpowerR(int L,int nIdxLo, int nIdxHi);
   float summR(int L);
   void SpreadPower(float power, int L=1);
   void SpreadPower_re_idx(float power, int L=1);
   void mul(float t,int L);

   void add1hn(const LSSignal * what, float XH); // 11.11.2020 08:22
   float mul1conjpow(const LSSignal * with);    // 11.11.2020 08:33

   //////////////////////////////////////////////////////////
   float fromWaveNoi(    // возвращает мощность - Па^2
      CMWave *w,         // из волны  TypeNOISE
//       UINT Time,         // текущее время
      BOOL bClear=TRUE,  // очистить перед суммированием шумов
      BOOL bWbn=TRUE,    // делать из Wbn
      BOOL bNbn=TRUE,    // делать из Nbn
      BOOL bIdxPow=FALSE,// Freq[i].re = Па^2 для i=0..n/2 (половина)
      BOOL bPow=FALSE,   // Freq[i].re = Па^2 для всех (симметрично)
      BOOL bRandom=FALSE,// делать шум как случайный процесс (Gauss)
                         // иначе только в реальную часть спектра
      BOOL bUseRay=TRUE, // использовать лучевку (множить на аномалию)
      int nray=0         // номер луча
      );

   BOOL fromAcl(   // возвращает TRUE если он кончился!!!
      CMAcl *pAcl,
      float Delay, //ms
      BOOL bTranslate/*=TRUE*/,
      CMWave::TWRayAcl *o/*=NULL*/);

   BOOL fromWaveAcl(  // TRUE - можно удалять
      CMWave *w,      // из волны  TypeACL
      UINT Time,      // текущее время
      BOOL bClear = TRUE,  // очистить вначале
      BOOL bTranslate = TRUE, // БПФ сделать
      BOOL bRandom=FALSE,// для типа3 делать шум как случайный процесс (Gauss)
                        // иначе как тон
      BOOL bUseRay=TRUE,// использовать лучевку
      int nray=0   // номер луча
      );

   // максимизирует спектр по всему входящему ACLю 
   // в реальную часть спектра
   BOOL fromWaveAclMaxR(  // TRUE (всегда!) - можно удалять
      CMWave *w,      // из волны  TypeACL
      BOOL bRandom=FALSE,// для типа3 делать шум как случайный процесс (Gauss)
                        // иначе как тон
      BOOL bUseRay=TRUE,// использовать лучевку
      int nray=0   // номер луча
      );

   // присуммировать в сигнал максимальные мощности всего акля в реальную часть спектра
   // вернуть число шагов (длину акля)
   UINT fromWaveAclPowR( 
      CMWave *w,      // из волны  TypeACL
      BOOL bUseRay=TRUE,// использовать лучевку
      int nray=0   // номер луча
      );
   
};





class LMSignal 
{
public:
   void Copy(LMSignal & src,int L);
   LSSignal & operator[](int idx);
   LMSignal();
   virtual ~LMSignal();
   LMSignal(LRange &range, int M);
   void Create(LRange &range, int M);
   void Clear(int L); // чистит время(0) / спектр(1) у всех
   void Clear(); // чистит и время и спектр у всех
/*
   void focusing(M0Rays * prays, LScale *f, BOOL bActive = FALSE);
            // BOOL bActive - для гидролокации TRUE 
            // (туда и обратно - аномалия в квадрате)

*/

   int m; // количество сигналов
   LSSignal *pSign;
};

#endif // !defined(AFX_LNSIGNALS_H__A05DF5D3_8227_11D4_9D13_0060970CEBA8__INCLUDED_)
