// LDistr.cpp: implementation of the LDistr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDistr.h"
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LDistr::LDistr()
{
   nVolRecords = 0;
   nRecCount = 0;
   m_pLayer = NULL;
}

LDistr::~LDistr()
{
#ifdef _DEBUG
   m_pLayer=NULL;
#endif
}

BOOL LDistr::Create( float Teta0,float dTeta,UINT nTeta, 
                     float Fi0,  float dFi,  UINT nFi,    
                     float T0,   float dT,   UINT nT)
{
   m_Teta0   =   Teta0;
   m_dTeta   =   dTeta;
   m_nTeta   =   nTeta;
   m_Fi0     =   Fi0  ;
   m_dFi     =   dFi  ;
   m_nFi     =   nFi  ;
   m_T0      =   T0   ;
   m_dT      =   dT   ;
   m_nT      =   nT   ;
   
   UINT cRec = nT;
   ASSERT(cRec >= 1);
   nRecCount=0; 

   nVolRecords = nFi * nTeta; // размер слоя
   m_pLayer = NULL;
   LScale scale[3];
   scale[0].Create(cRec, 0, 1);
   scale[1].Create(nFi,   Fi0,   dFi  );
   scale[2].Create(nTeta, Teta0, dTeta);
   if ( !LCube::Create(3,scale,sizeof(float)) ) return FALSE;

   nRecCount = 0;
   m_pLayer = (float *)pData;
   return TRUE;
}

void LDistr::Zero(void)
{  // очистить последний слой
   if ( nVolRecords == 0 ) return;
   float  *to = GetLastLayer();
   memset(to,0,nVolRecords*sizeof(float));
}

void LDistr::Copy(LDistr & src)
{  // копировать в последний слой
   ASSERT( nVolRecords == src.nVolRecords );
   float *to = GetLastLayer();
   float *from = src.GetData();  // сверху исходного!
   memcpy(to,from,nVolRecords*sizeof(float));
}

void LDistr::Add(LDistr & src)
{  // прибавить в последний слой
   ASSERT( nVolRecords == src.nVolRecords );
   float *to = GetLastLayer();
   float *from = src.GetData(); // сверху исходного!
   for(int i=0; i<nVolRecords; i++ ) *to++ += *from++;
}

void LDistr::Fall(LDistr & src,int nFallRow) // same size please!
{  // добавляет строку в верхний слой по горизонтали.   !ПРОВЕРИТЬ!
   float *to = GetData();
   float *from = src.GetData();

   int horz = Get_nTeta();
   ASSERT( horz == src.Get_nTeta());
   
   for(int i=nVolRecords-1; i>=horz; i-- ) to[i] = to[i-horz];
   for (  i=0; i<horz ;i++ ) to[i] = from[i+horz*nFallRow];

}

void LDistr::FallEx(LDistr & src) // same size please!
{  // добавляет максимальные значения!
   float *to = GetData();
   float *from = src.GetData(), *p;

   int horz = Get_nTeta();
   int nfi  = src.Get_nFi();
   ASSERT( horz == src.Get_nTeta());
   
   memmove(&to[horz],to,(nVolRecords-horz)*sizeof(float));

   for ( int i=0; i<horz ;i++ ) {
      float tmax = 0;
      p = from;
      for ( int j=0; j<nfi ;j++ ) {
         float t = *p;
         if ( t > tmax ) tmax = t;
         p+=horz;
      }
      *to++ = tmax;
      from++;
   }

}

void LDistr::Broach(LDistr & src, float Teta) // same size please!
{  // протяжка посредине
   float *to = NULL;
   int midl = src.pScales[2].GetNearest(Teta);
   src.GetVectVert(&to,0,midl);
   MoveMemory(&GetData()[1],GetData(),(nVolRecords-1)*sizeof(float));
   PutVectVert(to,0,0);
   delete to;
}

UINT LDistr::GetVect(float **pData, unsigned dim1, unsigned dim2, unsigned crossPlane)
{
   if (crossPlane==HORIZONTAL) return GetVectHorz(pData,dim1,dim2);
   if (crossPlane==VERTICAL)   return GetVectVert(pData,dim1,dim2);
   if (crossPlane==TIMERECORD) return GetVectTime(pData,dim1,dim2);
   ASSERT(0);
   return 0; 
}

UINT LDistr::PutVect(float *pData, unsigned dim1, unsigned dim2, unsigned crossPlane)
{
   if (crossPlane==HORIZONTAL) return PutVectHorz(pData,dim1,dim2);
   if (crossPlane==VERTICAL)   return PutVectVert(pData,dim1,dim2);
   if (crossPlane==TIMERECORD) return PutVectTime(pData,dim1,dim2);
   ASSERT(0);
   return 0; 
}

UINT LDistr::GetVectHorz(float **pData,unsigned iTime,unsigned iFi)
{
   float *Cube = GetRecord(iTime,iFi,0);
   
   UINT N = Get_nTeta();
   
   if ( ! *pData ) *pData=new float[N];
   float * data = *pData;
   memcpy(data, Cube, N*sizeof(float));

   return N; 
}

UINT LDistr::PutVectHorz(float *pData,unsigned iTime,unsigned iFi)
{
   float *Cube = GetRecord(iTime,iFi,0);
   
   UINT N = Get_nTeta();
   memcpy(Cube, pData, N*sizeof(float));

   return N; 
}

UINT LDistr::GetVectVert(float **pData,unsigned iTime,unsigned iTeta)
{
   float *Cube = GetRecord(iTime,0,iTeta);
   
   UINT N = Get_nFi(); 
   UINT nTeta = Get_nTeta();
   
   if ( ! *pData ) *pData=new float[N];
   float * data = *pData;

   for (unsigned i=0;i<N;i++) {
      *data++ = Cube[nTeta*i];
   }
   
   return N;
}

UINT LDistr::PutVectVert(float *pData,unsigned iTime,unsigned iTeta)
{
   float *Cube = GetRecord(iTime,0,iTeta);
   
   UINT N = Get_nFi(); 
   UINT nTeta = Get_nTeta();

   for (unsigned i=0;i<N;i++) {
      Cube[nTeta*i] = *pData++;
   }
   
   return N;
}

UINT LDistr::GetVectTime(float **pData,unsigned iFi,unsigned iTeta)
{
   float *Cube = GetRecord(0,iFi,iTeta);
   
   UINT N=Get_nTime(); 
   
   if ( ! *pData ) *pData=new float[N];
   float * data = *pData;
   
   for (unsigned i=0;i<N;i++) {
      *data++ = Cube[nVolRecords*i];
   }
   
   return N; 
}

UINT LDistr::PutVectTime(float *pData,unsigned iFi,unsigned iTeta)
{
   float *Cube = GetRecord(0,iFi,iTeta);
   
   UINT N=Get_nTime(); 
   
   for (unsigned i=0;i<N;i++) {
      Cube[nVolRecords*i] = *pData++;
   }
   
   return N; 
}

void LDistr::Push()
{  // сдвинуть слой вниз на один

   nRecCount++;
   if ( nRecCount >= Get_nTime() ) {
      nRecCount = Get_nTime()-1;

      // now shift box!
      float  *to=GetData();
      MoveMemory(to,&to[nVolRecords],(nRecords-nVolRecords)*sizeof(float));
   } else {
      ASSERT ( m_pLayer );
      ASSERT ( m_pLayer + nVolRecords == &m_pLayer[nVolRecords] );
      m_pLayer += nVolRecords;
   }
}

void  LDistr::ResetRecCount()
{
   nRecCount = 0;
   m_pLayer = (float *)pData;
}

BOOL LDistr::IsFilled()
{
   return nRecCount == Get_nTime()-1;
}

void LDistr::operator /=(float val)
{
   float *p = GetData();
   for ( UINT i=0; i<nRecords ;i++ ) {
      *p++ /= val;
   }
}

void LDistr::operator *=(float val)
{
   float *p = GetData();
   for ( UINT i=0; i<nRecords ;i++ ) {
      *p++ *= val;
   }
}

void LDistr::SwapTimeTeta()  // время ставится в тету для детекта
{  // swap Time & Teta for detector - только для nTeta == 1

   ASSERT(m_nTeta == 1);
   ASSERT(nDataLen);

   float * p = (float *)LCube::pData;
   float *pp = new float[nDataLen/4];

   int k=0;
   for ( UINT j=0; j<m_nFi ;j++ ) {
      for ( UINT i=0; i<m_nT ;i++ ) {
         pp[k] = p[i*m_nFi+j];
         ++k;
      }
   }

   memcpy(p,pp,nDataLen); 
   delete [] pp;
}

void LDistr::UnSwapTimeTeta()
{  // swap Time & Teta for detector - только для nTeta == 1

   ASSERT(m_nTeta == 1);
   ASSERT(nDataLen);

   float * p = (float *)LCube::pData;
   float *pp = new float[nDataLen/4];

   for ( UINT j=0; j<m_nFi ;j++ ) {
      for ( UINT i=0; i<m_nT ;i++ ) {
         pp[i*m_nFi+j] = *p;
         ++p;
      }
   }

   memcpy(LCube::pData,pp,nDataLen);
   delete [] pp;
}

void LDistr::SwapTimeTetaKO()  // время ставится в тету для детекта
{  // swap Time & Teta for detector - только для nFi == 1

   ASSERT(m_nFi == 1);
   ASSERT(nDataLen);

   float * p = (float *)LCube::pData;
   float *pp = new float[nDataLen/4];

   int k=0;
   for ( UINT j=0; j<m_nTeta ;j++ ) {
      for ( UINT i=0; i<m_nT ;i++ ) {
         pp[k] = p[i*m_nTeta+j];
         ++k;
      }
   }

   memcpy(p,pp,nDataLen);
   delete [] pp;
}

void LDistr::UnSwapTimeTetaKO()
{  // swap Time & Teta for detector - только для nTeta == 1

   ASSERT(m_nFi == 1);
   ASSERT(nDataLen);

   float * p = (float *)LCube::pData;
   float *pp = new float[nDataLen/4];

   for ( UINT j=0; j<m_nTeta ;j++ ) {
      for ( UINT i=0; i<m_nT ;i++ ) {
         pp[i*m_nTeta+j] = *p;
         ++p;
      }
   }

   memcpy(LCube::pData,pp,nDataLen);
   delete [] pp;
}
