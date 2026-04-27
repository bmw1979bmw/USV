// MWave.h: interface for the CMWave class.
#pragma once

#include "MAcl.h"
#include "structures.h"

class CMWave : public TMParams // TMParams объекта от точки рождения волны:
{
   void operator =(const CMWave &); // no implementation
   CMWave(const CMWave &w);    // не передавать волны на стеке - дурной тон
public:
   CMWave();
   virtual ~CMWave();
   void Serialize(CArchive &ar);

//////////////////////////////////////////////////////////////////
// общие данные. Сериализуются
   enum { wave_NONE, wave_NOISE, wave_ACL } waveType;
   DWORD   nSourceId;  // породил/отразил волну
   UINT    InitTime;   // время рождения волны, мс

   UINT    nObjLength; // длина объекта из свойств его системы
   UINT    nClass;     // класс источника

   float   m_Dist;     // дистанция до источника, м
   float   m_Bearing;  // пеленг на источник, град от севера направо
   float   m_RadVel;   // радиальная скорость сближения объектов, м/с
   UINT    nPtNum;     // номер принимающей точки, заполняется монитором

   BOOL  TypeACL() { return waveType == wave_ACL; }
   BOOL  TypeNOISE() { return waveType == wave_NOISE; }

   ////////////////// NOISE //////////////////////////////
   struct TWNoise {
      TPatt2d pWbn[1];  // [1] - для совместимости со старыми указателями
      TPatt2d pNbn[1]; 
   };
   int NoisesListLen;
   TWNoise *NoisesList;
   //------------------------
	void AllocNoisesList(int nLen);
   BOOL NoNoises(void) const { return !NoisesListLen; }
   void AddNoise(int idx,const TPatt2d *pWbn,const TPatt2d *pNbn);

   ////////////////// ACL ////////////////////////////////
   UINT    nReflect;  // число отражений: 1-отраженная, 2-неотражаемая
                      // задать 2, чтобы не отражалась

   float   Kw;  // угол излучения, град от севера направо GNR (курс хода волны)
   float   Sector; // сектор излучения, градусы  0 == 360 (круговая - 0)

   CMAcl *pAcl;

   // диаграмма излучения /опциональная/ ! по горизонтали
   int m_nPattLen;
   TFPoint *m_pPatt;   
   // x - модуль угла, град; y - коэффициент к Па 0..1 при угле меньше x
	void CreatePattern(int nlen);
   ///////////////////////////////////////////////////////
   // лучёвка
   int nRays; // число лучей - если 0 - значит отключена лучевка или френель
              // тогда pRaysData->TWRayOne иначе по типу волны TWRayAcl/TWRayNoi
   DWORD  nRaysDataSize;
   char *pRaysData;
   // для прямого луча nRays == 0
   struct TWRayOne {
      float EnterTime; // время входа на объект, мс
      float TetaIn;    // угол входа ВП, град
   };
   // для других
   // ACL
#pragma warning (disable : 4200)
   struct TWRayAcl : TWRayOne{
      float L[/*nSignals*/];
   };
   // NOI
   struct TWRayNoi : TWRayOne{
      float L[/*NoisesListLen*/][2][44];  // WBN+NBN
   };
#pragma warning (default : 4200)
   DWORD CalcRaysDataSize();
   void AllocRaysData(int nrays);
//    void FreeRaysData();
#define RAYNOI_SIZE (4+4+4*88*NoisesListLen)
#define RAYACL_SIZE (4+4+4*pAcl->nSignals)
   int GetRaySize() { return TypeACL()?RAYACL_SIZE:RAYNOI_SIZE; }

   TWRayOne *GetRayOne() { return (TWRayOne*)pRaysData; }
   TWRayNoi *GetRayNoi(int nray) {  ASSERT(TypeNOISE());
      ASSERT((!nray && !nRays) // можно взять нулевой луч если нет лучей
         || RAYNOI_SIZE*nray <= int(nRaysDataSize-RAYNOI_SIZE)); // иначе ошибка
      return (TWRayNoi*)&pRaysData[RAYNOI_SIZE*nray]; }
   TWRayAcl *GetRayAcl(int nray) { ASSERT(TypeACL());
      ASSERT(pAcl);
      ASSERT((!nray && !nRays) // можно взять нулевой луч если нет лучей
         || RAYACL_SIZE*nray <= int(nRaysDataSize-RAYACL_SIZE));
      return (TWRayAcl*)&pRaysData[RAYACL_SIZE*nray];}
   TWRayOne *GetRayOne(int nray) { return TypeACL()
      ?((TWRayOne *)GetRayAcl(nray))
      :(GetRayNoi(nray)); }

//////////////////////////////////////////////////////////////////
public:
   DWORD m_dwDeleteMask;  // маска для удаления разными трактами
   BOOL MarkedToDelete(DWORD dwBit=1) { return !!(m_dwDeleteMask&dwBit); }
   BOOL MarkedToDeleteAll(DWORD dwBit) 
      { return (m_dwDeleteMask&dwBit) == dwBit; }
   void MarkToDelete(DWORD dwBit=1) { m_dwDeleteMask|=dwBit; }
};
///////////////////////////////////////////////////////////////////////

typedef CTypedPtrList<CPtrList, CMWave *> CMWaveList;
#define for_each_noi(l,m) for( int ii_##m = 0; ii_##m < l##Len && (m=&l[ii_##m])!=NULL; ii_##m++ ) 

///////////////////////////////////////////////////////////////////////
// пример наследования:
// class CTWave : public CMWave { // для  торпеды
// public:
//    float    mt_Fi;   // по вертикали
//    float    mt_Teta; // по горизонтали
//    float    mt_Time; // время
//    float    mt_Level; // xz
//    int      mt_nFiltrMax;
//    int      mt_nNum1; // xz
//    int      mt_nNum2; // xz
// };
// 
// typedef CTypedPtrList<CPtrList, CTWave *> CTWaveList;
///////////////////////////////////////////////////////////////////////


