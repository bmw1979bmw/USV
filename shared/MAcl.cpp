// MAcl.cpp: implementation of the CMAcl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextArchive.h"
#include "MAcl.h"
#include "float.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMAcl::CMAcl()
{
   nPacks = nSignals = 0;
   pPacks = NULL;
   pSignals = NULL;
}

CMAcl::CMAcl(const CMAcl &Acl)
{
   Copy(Acl);
}

void CMAcl::Copy(const CMAcl &Acl)
{
   nPacks = Acl.nPacks;
   nSignals = Acl.nSignals;
   if ( !nPacks || !nSignals ) return;

   pPacks = new TAclPack[nPacks];
   memcpy(pPacks,Acl.pPacks,nPacks*sizeof(TAclPack));
   pSignals = new TAclSignal[nSignals];
   memcpy(pSignals,Acl.pSignals,nSignals*sizeof(TAclSignal));
}

CMAcl::CMAcl(int npacks, int nsign)
{
   pPacks = NULL;
   pSignals = NULL;
   Create(npacks, nsign);
}

CMAcl::~CMAcl()
{
   delete pPacks;
   delete pSignals;
}

void CMAcl::Create(int npacks, int nsign)
{
   ASSERT(!pPacks);
   ASSERT(!pSignals);

   nPacks = npacks;
   nSignals = nsign;

   pPacks = new TAclPack[npacks];
   memset(pPacks,0,npacks*sizeof(TAclPack));
   pSignals = new TAclSignal[nsign];
   memset(pSignals,0,nsign*sizeof(TAclSignal));
}

void CMAcl::DeleteContents()
{
   delete pPacks;   pPacks = NULL;
   delete pSignals; pSignals = NULL;
   nPacks = 0;
   nSignals = 0;
}

#ifdef __TEXTARCHIVE_H__

void CMAcl::Serialize(CTextArchive & ar)
{
   if ( ar.IsStoring() ) {
      ar << nPacks ;//<< ";nPacks\r\n";
      int i;
      for ( i=0; i<nPacks ;i++ ) {
         ar << pPacks[i].Delay << pPacks[i].Level ;//<< "\r\n";
      }
      ar << nSignals;//<< ";nSignals\r\n";
      for ( i=0; i<nSignals ;i++ ) {
         ar << pSignals[i].type  << pSignals[i].length
            << pSignals[i].delay << pSignals[i].freq
            << pSignals[i].band  << pSignals[i].level
            << pSignals[i].code  ;//<< "\r\n";
      }
   } else {
      ASSERT(!pPacks && !pSignals);

      ar >> nPacks;
      ASSERT(nPacks);
      pPacks = new TAclPack[nPacks];
      int i;
      for ( i=0; i<nPacks ;i++ ) {
         ar >> pPacks[i].Delay >> pPacks[i].Level;
      }
      ar >> nSignals;
      ASSERT(nSignals);
      pSignals = new TAclSignal[nSignals];
      for ( i=0; i<nSignals ;i++ ) {
         ar >> pSignals[i].type  >> pSignals[i].length
            >> pSignals[i].delay >> pSignals[i].freq
            >> pSignals[i].band  >> pSignals[i].level
            >> pSignals[i].code;
      }
   }
}

#endif

void CMAcl::Serialize(CArchive & ar)
{
   if ( ar.IsStoring() ) {
      ar << nPacks;
      ar.Write(pPacks,nPacks*sizeof(TAclPack));
      ar << nSignals;
      ar.Write(pSignals,nSignals*sizeof(TAclSignal));
   } else {
      ASSERT(!pPacks && !pSignals);

      ar >> nPacks;
      ASSERT(nPacks);
      pPacks = new TAclPack[nPacks];
      ar.Read(pPacks,nPacks*sizeof(TAclPack));

      ar >> nSignals;
      ASSERT(nSignals);
      pSignals = new TAclSignal[nSignals];
      ar.Read(pSignals,nSignals*sizeof(TAclSignal));
   }
}

float CMAcl::GetMaxLevel(int *pidx) // Па
{
   float mp = 0;
   int i;
   for ( i=0; i<nPacks ;i++ ) {
      if ( pPacks[i].Level > mp ) mp = pPacks[i].Level;
   }
   float ms=pSignals[0].level;
   int idx=0;
   for ( i=1; i<nSignals ;i++ ) {
      if ( pSignals[i].level > ms ) {
         ms = pSignals[i].level;
         idx = i;
      }
   }
   if ( pidx ) *pidx = idx;
   return ms*mp;
}

float CMAcl::CalcLength()
{
   float tp = 0;
   int i;
   for ( i=0; i<nPacks ;i++ ) {
      if ( pPacks[i].Delay > tp ) tp = pPacks[i].Delay;
   }

   float ts=0;
   int idx=-1;
   for ( i=0; i<nSignals ;i++ ) {
      float end = pSignals[i].delay + pSignals[i].length;
      if ( end > ts ) {
         ts = end;
         idx = i;
      }
   }

   ASSERT(idx != -1);
   return tp + ts;
}

BOOL CMAcl::ReducePacks(int nMaxPacks)
{  // уменьшить количество пачек до указанного
   // вернуть TRUE, если уменьшили
   if ( nPacks <= nMaxPacks ) return FALSE;
   if ( nMaxPacks <= 2 ) return FALSE;


   int n = nPacks;
   while ( n > nMaxPacks ) {
      float level = FLT_MAX;
      int idx = 1;
      // сохраняем крайние !
      for ( int i=1; i<nPacks-2 ;i++ ) {
         if ( pPacks[i].Level > 0 &&  pPacks[i].Level < level ) {
            level = pPacks[i].Level;
            idx = i;
         }
      }
      pPacks[idx].Level = -1;
      n--;
   }

   TAclPack *pNewPacks = new TAclPack[nMaxPacks];
   n = 0;
   for ( int i=0; i<nMaxPacks ;i++ ) {
      while ( pPacks[n].Level < 0 && n < nPacks ) n++;
      if ( n >= nPacks ) { // ????
         delete [] pNewPacks;
         return FALSE;
      } else {
         pNewPacks[i] = pPacks[n];
      }
   }
   delete pPacks;
   nPacks = nMaxPacks;
   pPacks = pNewPacks;

   return TRUE;
}
