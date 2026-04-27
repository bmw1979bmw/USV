// MWave.cpp: implementation of the CMWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MWave.h"
#include "structures.h"
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMWave::CMWave()
{
   waveType = wave_NONE;
   nReflect = 0;
   InitTime = 0;
   nObjLength = 0;
   nClass = 0;
   pAcl = NULL;
   m_dwDeleteMask = 0;
   m_RadVel = 0;
   Sector = 0;

   m_nPattLen = 0;
   m_pPatt = NULL;

   m_Dist = 0;
   m_Bearing = 0;
   nPtNum = 0;     // номер принимающей точки

   NoisesList = NULL;
   NoisesListLen = 0;

   nRays = 0;
   pRaysData = NULL;
   nRaysDataSize = 0;
}

void CMWave::CreatePattern(int nlen)
{
   delete m_pPatt;
   m_pPatt = new TFPoint[m_nPattLen = nlen];
   ZeroMemory(m_pPatt,nlen * sizeof(TFPoint));
}

CMWave::~CMWave()
{
   ASSERT( waveType != wave_ACL || pAcl );

   delete [] NoisesList;
   delete pAcl;
   delete m_pPatt;
   delete pRaysData;
}

void CMWave::AddNoise(int idx,const TPatt2d * pWbn,const TPatt2d * pNbn)
{  // добавляет шум любой из 32 операций в список несомых шумов
   ASSERT(idx < NoisesListLen);
   CopyWbn(&NoisesList[idx].pWbn, pWbn);
   CopyNbn(&NoisesList[idx].pNbn, pNbn);
   waveType = wave_NOISE;
}


DWORD CMWave::CalcRaysDataSize() {
   if ( TypeACL() ) {
      ASSERT(pAcl);
      return nRays*RAYACL_SIZE;
   } else {
      return nRays*RAYNOI_SIZE;
   }
}

void CMWave::AllocRaysData(int nrays)
{  // 0 - for one ray
   ASSERT(pRaysData==NULL);
   nRays = nrays;
   if ( nRays ) {
      nRaysDataSize = CalcRaysDataSize();
   } else { // для прямого луча
      nRaysDataSize = sizeof(TWRayOne);
   }
   pRaysData = new char[nRaysDataSize];
}

void CMWave::Serialize(CArchive & ar)
{
   DWORD t;
   if ( ar.IsStoring() ) {
      ar.Write(static_cast<TMParams*>(this),sizeof(TMParams));
      ar << waveType
         << nSourceId
         << InitTime
         // от кого пришла
         << nObjLength
         << nClass

         << m_Dist
         << m_Bearing
         << m_RadVel   // радиальная скорость сближения объектов, м/с  
         << nPtNum     // номер принимающей точки
         << (DWORD)0;         // reserved
      switch(waveType){
      case wave_NOISE: 
         ar << NoisesListLen;
         if ( NoisesListLen ) ar.Write(NoisesList,NoisesListLen*sizeof(TWNoise));
         break;
      case wave_ACL:
         ar << Kw         // угол излучения, градусы GNR     
            << Sector     // сектор излучения, градусы     
            << nReflect;  // число отражений   при излучении = 0||2
         ASSERT(pAcl); 
         pAcl->Serialize(ar);
         ar << m_nPattLen;
         if ( m_nPattLen ) ar.Write(m_pPatt, m_nPattLen*sizeof(TFPoint));
         break;
      default:  // wave_NONE:
         ASSERT(0);
         break;
      }
      ar << nRays
         << nRaysDataSize;
      // при создании волны эти переменные равны нулю

      if ( nRaysDataSize ) ar.Write(pRaysData,nRaysDataSize);

   } else {  ////////////////////////////////////////////////////////////
      ar.Read(static_cast<TMParams*>(this), sizeof(TMParams));
      ar >> (int &)waveType
         >> nSourceId
         >> InitTime
         >> nObjLength
         >> nClass

         >> m_Dist
         >> m_Bearing
         >> m_RadVel   // радиальная скорость сближения объектов, м/с  
         >> nPtNum     // номер принимающей точки
         >> t;         // reserved

      switch(waveType){
      case wave_NOISE: 
         ar >> NoisesListLen;
         if ( NoisesListLen ) {
            AllocNoisesList(NoisesListLen);
            ar.Read(NoisesList,NoisesListLen*sizeof(TWNoise));
         }
         break;
      case wave_ACL:
         ar >> Kw
            >> Sector     // сектор излучения, градусы     
            >> nReflect;  // число отражений   при излучении = 0||2
         ASSERT(pAcl == NULL);
         pAcl = new CMAcl;
         pAcl->Serialize(ar);

         ar >> m_nPattLen;
         if ( m_nPattLen ) {
            ASSERT(m_pPatt == NULL);
            m_pPatt = new TFPoint[m_nPattLen];
            ar.Read(m_pPatt, m_nPattLen*sizeof(TFPoint));
         }
         break;
      default:  // wave_NONE:
         ASSERT(0);
         break;
      }
      ar >> nRays
         >> nRaysDataSize;

      if ( nRaysDataSize ) {
         ASSERT(pRaysData==NULL);
         pRaysData = new char[nRaysDataSize];
         ar.Read(pRaysData,nRaysDataSize);
      }
   }
}

void CMWave::AllocNoisesList(int nLen)
{
   ASSERT(nLen);
   ASSERT(NoisesList==NULL);
   NoisesList = new TWNoise[NoisesListLen=nLen];
   
}

//EOF