// LCube.cpp: implementation of the LCube class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LScale.h"
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

LCube::LCube()
{
   pScales = NULL;
   pData = NULL;

   DeleteContents();
}

void LCube::DeleteContents()
{
   delete [] pScales;   nScales  = 0;   pScales  = NULL;
   delete   pData;      nDataLen = 0;   pData    =  NULL;
   nRecLen  = 0;
   nRecords = 0;
}

LCube::~LCube()
{
   DeleteContents();
}

BOOL LCube::Create(UINT nscales, const LScale * scales, UINT nreclen)
{
   ASSERT(nscales);

   DeleteContents();

   nScales = nscales;
   pScales = new LScale[nscales]; 

   nRecLen = nreclen;

   int nTotal = 1;
   for ( UINT i=0; i<nScales ;i++ ) {
      pScales[i] = scales[i]; // check!
      nTotal *= pScales[i].nLen;
   }
   nRecords = nTotal;
   nDataLen = nRecords*nRecLen;
   pData = new BYTE[nDataLen];
   ZeroMemory(pData,nDataLen);

   return TRUE;
}

void LCube::Dump(CDumpContext & dc) const
{
   dc << "nScales: " << nScales << "\r\n";
   UINT i;
   for ( i=0; i<nScales ;i++ ) {
      pScales[i].Dump(dc);
   }
   dc << "nRecLen: " << nRecLen << "\r\n";
   dc << "Data:[ "<< nRecords << " ]\r\n";
   for ( i=0; i<nRecords ;i++ ) {
      if ( nRecLen == 4 ) {
         TCHAR buf[32];
         wsprintf(buf, _T("%08X"), *(DWORD*)&pData[4*i]);
         dc << buf << " == " << *(float*)&pData[4*i] << "\r\n";
      } else {
         dc.HexDump(_T("."),&pData[nRecLen*i],nRecLen,32);
         if ( !(nRecLen & 3) ) {
            dc<<"\t == \r\n";
            for ( UINT j=0; j<nRecLen/4 ;j++ ) {
               dc << *(float *)&pData[nRecLen*i+(j*4)] << " ";
               if ( !(j&3) ) {
                  dc << "\r\n";
               }
            }
         }
      }
   }
}

void * LCube::operator [](UINT idx)
{
   if ( !pData ) return NULL;
   ASSERT(/*idx >= 0 &&*/ idx < nRecords );
   return pData+(nRecLen*idx);
}

// #ifndef LCUBE_INLINE
// 
// int LCube::GetScaleLen(UINT idx) const
// {
//    ASSERT(pScales);
//    ASSERT(idx < nScales);
//    return pScales[idx].nLen;
// }
// 
// float * LCube::GetScale(UINT idx) const
// {
//    ASSERT(pScales);
//    ASSERT(idx < nScales);
//    return pScales[idx].pScale;
// }
// #endif
////////////////////////FIND///////////////////////////////////

int LCube::vFindIdx(float *pFirst)
{
   LPosition pos(*this);
   float first = *pFirst;
   va_list marker;
   va_start( marker, (*pFirst) );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales;j++ ) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i++] = pScales[j].GetNearest(first);
      if ( i >= nScales ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );              
   
   return pos;
}

void * LCube::FindRecord(float first, ...)
{
   if ( !nScales ) return NULL;
   if ( !pData ) return NULL;

   LPosition pos(*this);
   
   va_list marker;
   va_start( marker, first );    
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales;j++ ) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i++] = pScales[j].GetNearest(first);
      if ( i >= nScales ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );             

   return operator [](/*vFindIdx(&first)*/ pos);
}

int LCube::FindRecordIdx(float first, ...)
{
   if ( !nScales ) return NULL;

   LPosition pos(*this); // here pos == 0 !
   va_list marker;
   va_start( marker, first );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales;j++) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i++] = pScales[j].GetNearest(first);
      if ( i >= nScales ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );              
   return pos;
}

int LCube::FindRecordIdxEx(float *deltas, float first, ...)
{
   if ( !nScales ) return NULL;
   LPosition pos(*this); // here pos == 0 !
   va_list marker;
   va_start( marker, first );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales;j++) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i] = pScales[j].GetNearest(first,&deltas[i]);
      if ( ++i >= nScales ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );              
   return pos;
}

int LCube::FindSubCubeIdxEx(float *deltas,UINT nVals, float first,...)
{
   if ( !nScales ) return NULL;
   LPosition pos(*this); // here pos == 0 !
   va_list marker;
   va_start( marker, first );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales; j++) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i++] = pScales[j].GetNearest(first,deltas?&deltas[i]:NULL);
      if ( i >= nVals ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );              
   return pos;
}

int LCube::FindSubCubeIdx(UINT nVals, float first,...)
{
   if ( !nScales ) return NULL;
   LPosition pos(*this); // here pos == 0 !
   va_list marker;
   va_start( marker, first );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales; j++) {
      if ( !pScales[j].IsActive() ) continue;
      pos[i++] = pScales[j].GetNearest(first);
      if ( i >= nVals ) break;
      first = (float) va_arg( marker, double ); 
   }
   ////////////////////
   va_end( marker );              
   return pos;
}

void * LCube::GetRecord(UINT idxfirst,...)
{
   if ( !nScales ) return NULL;
   if ( !pData ) return NULL;

   LPosition pos(*this);
   va_list marker;
   va_start( marker, idxfirst );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales; j++) {
      if ( !pScales[j].IsActive() ) continue;
      ASSERT(idxfirst < (UINT)pos.pMax[i]);
      pos[i++] = idxfirst;
      if ( i >= nScales ) break;
      idxfirst = va_arg( marker, UINT);   
   }
   ////////////////////
   va_end( marker );              
   
   return operator [](pos);
}

int LCube::GetSubCubeIdx(UINT nVals, UINT idxfirst,...)
{
   if ( !nScales ) return NULL;
   ASSERT(nVals <= nScales);
   LPosition pos(*this); // here pos == 0 !
   va_list marker;
   va_start( marker, idxfirst );     
   //////////////////
   UINT i=0;
   for ( UINT j=0;j<nScales;j++ ) {
      if ( !pScales[j].IsActive() ) continue;
      ASSERT(idxfirst < (UINT)pos.pMax[i]);
      pos[i++] = idxfirst;
      if ( i >= nVals ) break;
      idxfirst = va_arg( marker, UINT); // мб взятие за пределами стека
   }
   ////////////////////
   va_end( marker );              
   return pos;
}


void LCube::DebugFill()
{
   ASSERT(pData);
   float t = 0;
   for ( UINT i=0; i<nDataLen/4 ;i++ ) {
      ((float*)pData)[i] = t;
      t++;
   }
}

void LCube::CheckAscendScales()
{
   for ( UINT j=0; j<nScales; j++ ) pScales[j].CheckAscendType();
}
