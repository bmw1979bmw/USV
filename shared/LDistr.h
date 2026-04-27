// LDistr.h: interface for the LDistr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDISTR_H__E0F75CC5_307C_11D4_9CC3_0060970CEBA8__INCLUDED_)
#define AFX_LDISTR_H__E0F75CC5_307C_11D4_9CC3_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "LCube.h"
           
#define     HORIZONTAL  0
#define     VERTICAL    1
#define     TIMERECORD  2  
               
class LDistr : /*protected */ public LCube  
{
public:
	BOOL IsFilled(void);
   void  ResetRecCount();
/*
   float * try_operator(int idx) {
      if ( !pData ) return NULL;
      return &((float *)pData)[idx]; 
   }
*/
   /*
   float * operator [](int idx)
   {
      if ( !pData ) return NULL;
      return &((float *)pData)[idx]; 
/*
      ASSERT ( (float *) (LBase::operator[](idx))
      ==
      try_operator(idx) );
      return (float *) (LBase::operator[](idx));
*/
   //}

   float * GetData(){ return (float *)pData; }

   int Get_nTime() const { return m_nT;   /*LCube::GetScaleLen(0);*/ }
   int Get_nFi  () const { return m_nFi;  /*LCube::GetScaleLen(1);*/ }
   int Get_nTeta() const { return m_nTeta;/*LCube::GetScaleLen(2);*/ }

   float *GetScaleTime() const { return LCube::GetScale(0); }
   float *GetScaleFi  () const { return LCube::GetScale(1); }
   float *GetScaleTeta() const { return LCube::GetScale(2); }

private:
   float *GetRecord(UINT iTime, UINT iFi, UINT iTeta)
      { return (float *)LCube::GetRecord(iTime, iFi, iTeta); }
public:
   float *__GetRecord(UINT iTime, UINT iFi, UINT iTeta)  // дл€ отладки
           { return GetRecord(iTime, iFi, iTeta); }
// 
//    float *FindRecord(float Time, float Fi, float Teta) {
//       UINT iTime = (Time-m_T0)/m_dT;
//       if ( iTime < 0 ) iTime = 0;
//       if ( iTime >= m_nT ) iTime = m_nT-1;
//       UINT iFi = (Fi-m_Fi0)/m_dFi;
//       if ( iFi < 0 ) iFi = 0;
//       if ( iFi >= m_nFi ) iFi = m_nFi-1;
//       UINT iTeta = (Teta-m_Teta0)/m_dTeta;
//       if ( iTeta < 0 ) iTeta = 0;
//       if ( iTeta >= m_nTeta ) iTeta = m_nTeta-1;
//       return GetRecord(iTime, iFi, iTeta);
//    }
// 

	void SwapTimeTeta(void);
	void UnSwapTimeTeta(void);
	void SwapTimeTetaKO(void);
	void UnSwapTimeTetaKO(void);
	void operator /=(float val);
	void operator *=(float val);

   float *GetLastLayer()  const { ASSERT(pData); return m_pLayer; }

   void Push(void);

   UINT GetVect(float **pData,unsigned dim1,unsigned dim2,unsigned crossPlane);
   UINT PutVect(float *pData,unsigned dim1,unsigned dim2,unsigned crossPlane);

   UINT GetVectHorz(float **pData,unsigned iTime,unsigned iFi);
   UINT PutVectHorz(float *pData,unsigned iTime,unsigned iFi);
   UINT GetVectVert(float **pData,unsigned iTime,unsigned iTeta);
   UINT PutVectVert(float *pData,unsigned iTime,unsigned iTeta);
   UINT GetVectTime(float **pData,unsigned iFi,unsigned iTeta);
   UINT PutVectTime(float *pData,unsigned iFi,unsigned iTeta);


   void Fall(LDistr & src,int nFallRow);
   void FallEx(LDistr & src);
   void Broach(LDistr & src, float Teta);  // прот€жка
   void Copy(LDistr &src);
   void Add(LDistr &src);
//    void Rand(float Taver /*void*/);
   void Zero(void);
   //unsigned rec_count;  // дл€ предъистории
//   float m_to;
//   float m_dt;
//   float m_df;
//   float m_dfi;
//   float m_dteta;
   //int   m_nFallRow;
   /***************************/
//    float m_f;  // средн€€ частота диапазона

   float m_Teta0;
   float m_dTeta;
   UINT  m_nTeta;
   float m_Fi0  ;
   float m_dFi  ;
   UINT  m_nFi  ;
   float m_T0   ;
   float m_dT   ;
   UINT  m_nT   ;
   /***************************/

   int   nVolRecords; // размер сло€
   int   nRecCount;
   float *m_pLayer;   // адрес тек сло€ (дл€ Push)

   BOOL Create(float Teta0,float dTeta,UINT nTeta, 
               float Fi0,  float dFi,  UINT nFi,    
               float T0=0, float dT=1, UINT nT=1
               );
   LDistr();
   virtual ~LDistr();

};

#endif // !defined(AFX_LDISTR_H__E0F75CC5_307C_11D4_9CC3_0060970CEBA8__INCLUDED_)
