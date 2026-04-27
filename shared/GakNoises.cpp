// GakNoises.cpp: implementation of the CGakNoises class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GakNoises.h"
#include "GakStructs.h"
#include "GoodFunc.h"
#include "GakDebugFlags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGakNoises g_Noises;
//DWORD g_dwDebugFlags;

CGakNoises::CGakNoises() :
   m_REVKoeff(0)
{
}

CGakNoises::~CGakNoises()
{
   m_SignList.RemoveAll();
}

void CGakNoises::ResetRevSigns()
{
   m_SignList.RemoveAll();
}

BOOL CGakNoises::NewCarNoise(int *pcarindex, float depth, float speed, float freq)
{
   int index = carnoise.FindSubCubeIdx(3,depth,speed,freq);
   BOOL ret = ( *pcarindex != index );
   *pcarindex = index;
   return ret;
}

float CGakNoises::GetCarNoise2(float depth, float speed, float vert_angle, float hor_angle, float freq,BOOL *pbFreqErr)
{  // Па^2/Гц
   float deltas[5];
   float *plevel;
   float level=0;
   int idx;
   idx = carnoise.FindRecordIdxEx(deltas,depth,speed,freq,vert_angle,hor_angle); 
   if ( ffabs(deltas[2]) > 5 ) {
      int idx1000 = carnoise.FindRecordIdxEx(deltas,depth,speed,1000.f,vert_angle,hor_angle);
      if ( ffabs(deltas[2]) > 1) {
         if ( pbFreqErr ) *pbFreqErr = TRUE;
         else {
	    TRACE("Неверные частоты в CAR файле!\r\n");
         }
	 goto _1;
      } else {
         plevel = (float *)carnoise[idx1000];
         level = plevel? ( *plevel / (freq/1000) ):0;
      }
   } else {
_1:
      plevel = (float *)carnoise[idx];
      level = plevel?*plevel:0;
   }
   return level*level; 
}

BOOL CGakNoises::IsCarNoiseNoAngle()
{  // отсутствует распределение по углу
   if ( carnoise.nScales < 5 ) return FALSE;
   return carnoise.pScales[3].nLen == 0 && carnoise.pScales[4].nLen == 0;
}


BOOL CGakNoises::NewSeaNoise(int *pseaindex,float depth)
{
   int index = seanoise.FindSubCubeIdx(1,depth);
   BOOL ret = ( *pseaindex != index );
   *pseaindex = index;
   return ret;
}

float CGakNoises::SeaNoiseCalc(BOOL bWinter, float freq)
{  // Па/v~Гц
   float wind = bWinter ? 7.4f : 5.2f;
   return (float)(0.018*pow(wind,1.5f)*pow(freq,-0.84f));
}

float CGakNoises::GetSeaNoise2(BOOL bWinter, float depth, float vangle, float freq)
{  // Па^2/Гц
   float level = SeaNoiseCalc(bWinter, freq);
   level *= level; // Па^2
   if ( FLAG_SEANOISE1 ) {
      //level = 1;
   } else {
      level *= *(float *)seanoise.FindRecord(depth,freq,vangle);
   }
   return level;
}

float * CGakNoises::GetSeaNoiseAngPtr(float depth,float freq,BOOL bNonEqualNotify)
{
   float deltas[2];
   int idx = seanoise.FindSubCubeIdxEx(deltas,2,depth,freq);
   if ( bNonEqualNotify ) {
      if ( fabs(deltas[1]) > 100 ) {
         AfxMessageBox(MB_ICONASTERISK,"нет Sea для частоты %g! (>100Гц разница)",freq);
      }
   }
   return (float *)seanoise[idx];
}

BOOL CGakNoises::LoadCarNoise(LPCTSTR szAlias)
{
   TCHAR carname[MAX_PATH];
   wsprintf(carname,_T("%s.car"),szAlias);
   if ( ! carnoise.LoadFromFileIgnore(carname,szAlias)) return FALSE;
   carnoise.CheckAscendScales();
   return TRUE;
}

BOOL CGakNoises::LoadSeaNoise(LPCTSTR szRegion)
{
   if ( ! seanoise.LoadFromFileIgnore(
      CString(szRegion) + ".sea"
      ,_T("SEA"))) return FALSE;
   seanoise.CheckAscendScales();
   return TRUE;
}

BOOL CGakNoises::LoadRevNoise(LPCTSTR szRegion,LPCTSTR szRevAlias)
{
   CString s;
   if ( szRevAlias && *szRevAlias ) {
      s = szRevAlias;
      s += " ";
      s += szRegion;
   } else {
      szRevAlias = NULL;
      s = szRegion;
   }
   s += ".rev";
   if ( !revnoise.LoadFromFileIgnore(s, szRevAlias) ) return FALSE;
   revnoise.CheckAscendScales();
   return TRUE;
}

float CGakNoises::GetRevNoise41( // Па^2
   LSSignal &sig, // куда всё запихнуть (добавить) - вначале чистите сами
   float depth,  // глубина                                    
   float vangle, // верт. угол                                 
   float bearing, // пеленг прихода реверберации  !!! 41
   float curtime/*ms*/,
   float Vms,  // моя скорость м/с по оси излучения
   BOOL bRealAddOnly // - в реальную часть sig добавить
   )
{  // Па^2/Гц
   ASSERT(revnoise.IsCreated());

   if ( m_SignList.GetCount() == 0 ) return 0;

   LSSignal s1(&sig);

   POSITION pos,pos0;
   for ( pos = m_SignList.GetHeadPosition(); pos ; ) {
      pos0 = pos; // for delete
      const TRevSign &rs = m_SignList.GetNext(pos);// pos changed here
      ASSERT(rs.time <= curtime);

      float time_s = (curtime-rs.time)/1000.f;  // с

      if ( time_s > revnoise.pScales[4].GetLast()  ) { // время последнее
         m_SignList.RemoveAt(pos0); // давим
         continue;
      }

      if ( rs.sector != 0 && qfabs(bearing-rs.bearing) > rs.sector ) continue;

      CMAcl acl(rs.acl);
      for ( int i=0; i<acl.nSignals ;i++ ) {
         // Sic!
         float *pw = (float *)revnoise.FindRecord(
                           depth,     //0
                           acl.pSignals[i].freq,   //1
                           acl.pSignals[i].length/1000,//2 s
                           vangle,    //3 
                           time_s);   //4 
         float w = pw? (*pw):0;

         acl.pSignals[i].freq *= (1+ Vms/750.f);
         acl.pSignals[i].level *= fsqrt(w);
      }

      s1.fromAcl(&acl,0,1,NULL);
      if ( bRealAddOnly ) {
         sig.pow_add(&s1); // - в реальную часть sig добавить
         // можно заоптимизировать 
      } else {
         sig.add(&s1,1);
      }
      s1.clear(0);
   }
   return sig.power(1);
}

float CGakNoises::GetRevNoise51( // Па^2/Гц
                               float low,
                               float full,
                               float depth,
                               float vangle,
                               float bearing,
                               float curtime/*ms*/,
                               float Vms  // моя скорость м/с по оси излучения
                               )
{  // Па^2/Гц
   ASSERT(revnoise.IsCreated());

   if ( m_SignList.GetCount() == 0 ) return 0;

   float sum = 0; // Па^2

   POSITION pos,pos0;
   for ( pos = m_SignList.GetHeadPosition(); pos ; ) {
      pos0 = pos; // for delete
      const TRevSign &rs = m_SignList.GetNext(pos);// pos changed here

      if ( rs.time > curtime ) continue; // in future

      float time_s = (curtime-rs.time)/1000.f;  // с

      if ( time_s > revnoise.pScales[4].GetLast()  ) { // время последнее
         m_SignList.RemoveAt(pos0); // давим
         continue;
      }

      if ( rs.sector != 0 && qfabs(bearing-rs.bearing) > rs.sector ) continue;

      for ( int i=0; i<rs.acl.nSignals ;i++ ) {
         //
         float *pw = (float *)revnoise.FindRecord(
                           depth,     //0
                           rs.acl.pSignals[i].freq,   //1
                           rs.acl.pSignals[i].length/1000,//2 s
                           vangle,    //3 
                           time_s);   //4 
         float w = pw? (*pw):0;

         float af = rs.acl.pSignals[i].freq * (1+ Vms/750.f);

         if ( af >= low && af <= low+full ) {
            for ( int j=0; j<rs.acl.nPacks ;j++ ) {
               float t = rs.acl.pSignals[i].level
                       * rs.acl.pPacks[j].Level;
               sum += t*t*w 
                  *
                  ( m_REVKoeff ?  m_REVKoeff : 1 );
                  // ((FLAG_ZTEST)?1:0.00122f); // for test only
//#pragma chMSG("FLAG_ZTEST rev * 0.00122")
            }
         }         
      }
   }
   return sum /full;
}

BOOL CGakNoises::ExistRevNoise()
{
   if ( !revnoise.IsCreated() ) return 0;
   return m_SignList.GetCount() != 0;
}

BOOL CGakNoises::Load( 
          LPCTSTR szCarAlias, 
          LPCTSTR szRegion,
          BOOL    bCar /*= 1*/,
          BOOL    bSea /*= 1*/,
          BOOL    bRev /*= 1*/,
          LPCTSTR szRevAlias /*= NULL*/ )
// Load(LPCTSTR szRegion, LPCTSTR szCarAlias, LPCTSTR /*unused*/,
//                       LPCTSTR szRevAlias,BOOL bLoadSea)
{  
#ifdef _DEBUG
   if ( bSea || bRev ) ASSERT(szRegion && *szRegion);
#endif

   BOOL bRes = 1;
   if ( bCar ) bRes &= LoadCarNoise(szCarAlias);
   if ( bSea ) bRes &= LoadSeaNoise(szRegion);
   if ( bRev ) {
      if ( !szRevAlias ) szRevAlias = szCarAlias;
      bRes &= LoadRevNoise(szRegion, szRevAlias);
   }
//    if ( szCarAlias ) bRes &= LoadCarNoise(szCarAlias);
//    if ( szRevAlias ) {
//       if ( *szRevAlias == 0 ) szRevAlias = szCarAlias;
//       bRes &= LoadRevNoise(szRegion,szRevAlias);
//    }
//    if ( bLoadSea   ) bRes &= LoadSeaNoise(szRegion);
   return bRes;
}

void CGakNoises::AddSignForRev(
          float time/*ms*/, // time  время излучения сигнала    мс 
          const CMAcl *pAcl,
          float bearing,  // пеленг излучения волны, °
          float sector    // сектор излучения, °  0-не использовать                
          )      // излученный сигнал                   
{
   ASSERT(revnoise.IsCreated());
   TRevSign rs;
   rs.time  = time;
   rs.bearing = bearing;
   rs.sector= sector;

   m_SignList.AddTail(rs);
   m_SignList.GetTail().acl = *pAcl;
}


