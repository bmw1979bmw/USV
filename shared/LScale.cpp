// LScale.cpp: implementation of the LScale class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LScale.h"

#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LScale::LScale()
{
   nLen = 0;
   pScale = NULL;
   nType = 0;
}

LScale::LScale(const LScale & scale)
{
   Create(scale.nLen);
   if ( nLen ) {
      memcpy(pScale,scale.pScale, nLen*sizeof(float));
   }
}
void LScale::operator = (const LScale & scale)
{
   Create(scale.nLen);
   if ( nLen ) {
      memcpy(pScale,scale.pScale, nLen*sizeof(float));
   }
}

void LScale::Create(UINT nlen)
{
   delete pScale;
   pScale = NULL;
   nLen = nlen;
   if ( nlen ) {
      pScale = new float[nlen];
   }
}
void LScale::Create(UINT nlen, float First, float Step)
{
   Create(nlen);
   for ( UINT i=0; i<nlen ;i++ ) {
      pScale[i] = First+i*Step; // более точно, не накапливается ошибка
   }
}

LScale::~LScale()
{
   delete pScale;
}

void  LScale::Attach(int nlen, float *pData)
{
   ASSERT(nlen && pData);
   delete pScale;
   nLen = nlen;
   pScale = pData;
}

void  LScale::Detach()
{
   pScale = NULL;
   nLen = 0;
}

void LScale::Serialize(CArchive & ar)
{
   if (ar.IsStoring()) {
      ar << nType;  
      ar << nLen;
      if ( nLen ) {
         ar.Write(pScale, nLen*sizeof(float));
      }
      
   } else {
      ar >> nType;  
      ar >> nLen;
      Create(nLen);
      if ( nLen ) {
         ReadRaw(ar);
      }
   }
}

void LScale::CheckAscendType()
{
   nType &= ~INCREASE;

   ASSERT(pScale);
   ASSERT(nLen);
   float t = pScale[0];
   for ( UINT i=1; i<nLen ;i++ ) {
      if ( t >= pScale[i] ) return;
      t = pScale[i];
   }
   nType |= INCREASE;
}

float LScale::operator [](UINT idx)
{
   ASSERT(pScale);
   ASSERT(idx<nLen);
   return pScale[idx];
}

void LScale::Dump(CDumpContext & dc) const
{
   dc << "\tnType: " << nType << "\r\n";
   dc << "\tnLen: " << nLen << "\r\n";
   for ( UINT i=0; i<nLen ;i++ ) {
      dc << "\t val[" << i << "] : " << pScale[i] << "\r\n";
   }
}

BOOL LScale::operator ==(LScale & sc)
{
   if ( nLen != sc.nLen ) return FALSE;
   for ( UINT i=0; i<nLen ;i++ ) {
      if ( pScale[i] != sc.pScale[i] ) return FALSE;
   }
   return TRUE;
}

UINT LScale::ReadRaw(CArchive & ar)
{
   return ar.Read(pScale, nLen* sizeof(float));
}

void LScale::ReadOld(CArchive & ar)
{
   nType = 0;
   nLen = 0;
   ar.Read(&nLen, size16);
   pScale = new float[nLen];
   ReadRaw(ar);
}



UINT LScale::GetNearest(float f, float *pdelta ) const
{  // возвращает индекс ближайшего к данному значению элемента шкалы
   ASSERT(pScale);
   ASSERT(nLen);

   UINT idx;

   {
      UINT i;
      idx = 0;
      float mind = FLT_MAX;

      for ( i=0; i<nLen ;i++ ) {
         float delta = fabs(pScale[i]-f);
         if ( delta < mind ) {
            mind = delta;
            idx = i;
         } else {
            if (nType & INCREASE) {  // дельта стала увеличиваться
               break;
            }
         }
      }
   }
   if ( pdelta ) {
      *pdelta = pScale[idx]-f;
   }

   return idx;
}

void * ReCalloc(void *pold, UINT nOldLen, UINT nNewLen, UINT elemsize)
{                                               
   void *p;
   if ( nOldLen == nNewLen ) return pold;
   p = new BYTE[elemsize*nNewLen];
   UINT lencopy;
   if ( nOldLen < nNewLen ) {
      lencopy = nOldLen;
      ZeroMemory(p,elemsize*nNewLen);
   } else {
      lencopy = nNewLen;
   }
   if (pold) {
      memcpy(p, pold, elemsize*lencopy);
      delete pold;
   }
   return p;
}

BOOL LScale::ChangeLen(UINT nlen)
{
   if ( nLen == nlen ) return FALSE;

   if ( nlen ) {
      if ( pScale ) {
         pScale = (float *) ReCalloc(
            pScale, nLen, nlen, sizeof(float));
      } else {
         pScale = new float[nlen];
         ZeroMemory(pScale,nlen*sizeof(float));
      }
   } else { //0
      delete pScale;
      pScale = NULL;
   }
   nLen = nlen;

   return TRUE;
}



void LScale::InsertBefore(UINT idx)
{
   if ( idx >= nLen ) return;

   ChangeLen(nLen+1);
   UINT j;
   for ( j=nLen-1; j>idx ;j-- ) {
      pScale[j] = pScale[j-1];
   }
   pScale[j] = 0;

}

void LScale::DeleteAt(UINT idx)
{
   if ( nLen < 2 ) return;
   if ( idx >= nLen ) return;

   for ( UINT j=idx; j<nLen-1 ;j++ ) {
      pScale[j] = pScale[j+1];
   }
   ChangeLen(nLen-1);
}

void LScale::Mirror(UINT /*nDummy*/)
{
   int k=nLen;
   ChangeLen(nLen*2);
   for ( int i=0; i<k ;i++ ) {
      pScale[nLen-1-i] = -pScale[i];
   }
}

void LScale::Not()
{
   for ( UINT i=0; i<nLen ;i++ ) {
      pScale[i] = -pScale[i];
   }
}

void LScale::WriteAsText(CTextArchive &ar, char *sComment)
{
   ar << nLen << sComment << "\r\n";
   for ( UINT i=0; i<nLen ;i++ ) {
      ar << pScale[i]  << " ";
   }
   ar << "\r\n";
}

float LScale::GetLast() const
{
   return pScale[nLen-1];
}

int LScale::PointToIndex(int x, int nx)
{  // вычисляет индекс в шкале по точке x на равномерном графике
   // при этом длина графика nx точек

   ASSERT(pScale);

   float Xmin = pScale[0];
   float Xmax = pScale[nLen-1];
   float t =  x*(Xmax-Xmin)/nx  + Xmin;

   return GetNearest(t);
}
