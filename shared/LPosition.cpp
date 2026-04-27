// LPosition.cpp: implementation of the LPosition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "QBase.h"

#include "LCube.h"

#include "LPosition.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPosition::LPosition()
{
   pPos = pMax = 0;
   nLen = 0;
}

LPosition::LPosition(LCube &base)
{
   pPos = pMax = 0;
   Assign(base);
}

void LPosition::Assign(LCube & base)
{
   if ( pPos ) delete pPos;
   if ( pMax ) delete pMax;

   nLen = base.nScales;
   if ( nLen ) {
      pPos = new int[nLen];
      pMax = new int[nLen];
   }
   int k = 0;
   for ( int i=0; i<nLen ;i++ ) {
      if ( ! base.pScales[i].IsActive() ) continue;
      pPos[k]=0;
      pMax[k] = base.pScales[i].nLen;
      k++;
   }
   nLen = k;
}

void LPosition::Create(int nlen)
{ 
   if ( pPos ) delete [] pPos;
   if ( pMax ) delete [] pMax;
   nLen = nlen;
   if ( nLen ) {
      pPos = new int[nLen];
      pMax = new int[nLen];
      ZeroMemory(pPos, nLen*sizeof(int));
      ZeroMemory(pMax, nLen*sizeof(int));
   } else {
      pPos = NULL;
      pMax = NULL;
   }
}

LPosition::~LPosition()
{
   delete pPos;
   delete pMax;
}

BOOL LPosition::IsAtLast(int idx)
{
   ASSERT(pPos); ASSERT(pMax);   ASSERT(idx < nLen);

   return pPos[idx] == pMax[idx]-1;
}

void LPosition::operator ++(int)
{
   ASSERT(pPos); ASSERT(pMax);
   pPos[nLen-1]++;
   for ( int i=nLen-1; i>=0 ;i-- ) {
      if ( pPos[i] == pMax[i] ) {
         pPos[i] = 0;
         if ( i ) {
            pPos[i-1]++;
         } 
      } else {
         break;
      }
   }
}

void LPosition::operator --(int)
{
   ASSERT(pPos); ASSERT(pMax);
   pPos[nLen-1]--;
   for ( int i=nLen-1; i>=0 ;i-- ) {
      if ( pPos[i] < 0 ) {
         pPos[i] = pMax[i]-1;
         if ( i ) {
            pPos[i-1]--;
         }
      } else {
         break;
      }
   }
}

LPosition::operator int() const
{
   ASSERT(pPos); ASSERT(pMax);

   int t = 0;
   for ( int i=0; i<nLen ;i++ ) {
      t*=pMax[i];
      t+=pPos[i];
   }
   return t;
}

int LPosition::GetSuperIdx(int n) const
{  // n - количество индексов для взятия координат подкуба
   // то же, что int от позиции с нулевыми координатами
   // большими или равными n
   ASSERT(pPos); ASSERT(pMax);

   int t = 0;
   for ( int i=0; i<nLen ;i++ ) {
      t*=pMax[i];
      if ( i < n ) t += pPos[i];
   }
   return t;

}

void LPosition::SetZero()
{
   ASSERT(pPos); ASSERT(pMax);
   ZeroMemory(pPos, nLen*sizeof(int));
}

int LPosition::operator =(int num)
{
   ASSERT(pPos); ASSERT(pMax);

   ZeroMemory(pPos, nLen*sizeof(int));

   int t  = num;
   for ( int i=nLen-1; i>=0 && t ;i-- ) {
      pPos[i] = t%pMax[i];
      t /= pMax[i]; 
   }
   return num;
}

int &LPosition::operator [ ](int idx)
{
   ASSERT(idx < nLen);
   ASSERT(pPos);
   ASSERT(pMax);    // m.b not
   return pPos[idx];
}

void LPosition::swap(int i, int j)
{
   ASSERT(i < nLen && j < nLen );
   ASSERT(pPos);
   ASSERT(pMax);
   int t;
   t = pPos[i]; pPos[i] = pPos[j]; pPos[j] = t;
   t = pMax[i]; pMax[i] = pMax[j]; pMax[j] = t;
}

void __cdecl LPosition::Set(int i0, ...)
{
   ASSERT(pPos);
   ASSERT(pMax);

   va_list marker;
   va_start( marker, i0 );
   //////////////////
   for ( int i=0; i<nLen ;i++) {
      ASSERT(i0 <= pMax[i]); 
      pPos[i] = i0;
      if ( i >= nLen ) break;
      i0 = (int) va_arg( marker, int ); 
   }
   ////////////////////
   va_end( marker );
}

