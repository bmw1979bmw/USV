// LSubCube.cpp: implementation of the LSubCube class.
// подкуб для конвертирования старых баз
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LScale.h"
#include "LSubCube.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LSubCube::LSubCube()
{
   pHead = NULL;
   nHeadLen = 0;
   nScales = 0;
   pScales = NULL;
   nTotal = 0;
   pData = NULL;
   nRecLen = 0;
}

void LSubCube::DeleteContents()
{
   delete pHead;
   delete [] pScales;
   delete pData;

   pHead = NULL;
   nHeadLen = 0;
   nScales = 0;
   pScales = NULL;
   nTotal = 0;
   pData = NULL;
   nRecLen = 0;
}

LSubCube::~LSubCube()
{
   DeleteContents();
}

void LSubCube::Serialize(CArchive & ar)
{
   if (ar.IsStoring()) {
   } else {
      Load(ar);
   }
}


BOOL LSubCube::Load(CArchive & ar)
{
   int i;
 
   nHeadLen = 0;

   ar.Read(&nHeadLen, size16);

   if ( nHeadLen ) {
      ASSERT(!pHead);
      pHead = new BYTE[nHeadLen];
      ar.Read(pHead, nHeadLen);
   }
   
   nScales = 0;

   ar.Read( &nScales, size16 );

   nTotal = 1;

   if ( nScales ) {
      ASSERT(!pScales);
      pScales = new LScale[nScales];
      for ( i=0; i<nScales ;i++ ) {
         pScales[i].ReadOld(ar);
         nTotal *= pScales[i].nLen;
      }
   }  
   
   nRecLen = 0;
   ar.Read( &nRecLen, size16 );

   ASSERT(nRecLen != 0);

   ASSERT(!pData);
   
   pData = new BYTE[nTotal*nRecLen];

   ar.Read(pData,nTotal*nRecLen);

   return TRUE;
}



void LSubCube::Dump(CDumpContext & dc) const
{
   dc << "HeadLen: " << nHeadLen << "\r\n";
   dc << "Head: " << (LPTSTR)pHead << "\r\n";
   dc << "nScales: " << nScales << "\r\n";
   int i;
   for ( i=0; i<nScales ;i++ ) {
      pScales[i].Dump(dc);
   }
   dc << "nRecLen: " << nRecLen << "\r\n";
   dc << "Data:[ "<< nTotal << " ]\r\n";
   for ( i=0; i<nTotal ;i++ ) {
      if ( nRecLen == 4 ) {
         TCHAR buf[32];
         wsprintf(buf, _T("%08X"), *(DWORD*)&pData[4*i]);
         dc << buf << " == " << *(float*)&pData[4*i] << "\r\n";
      } else {
         dc.HexDump(_T("."),&pData[nRecLen*i],nRecLen,32);
         if ( !(nRecLen & 3) ) {
            dc<<"\t == \r\n";
            for ( int j=0; j<nRecLen/4 ;j++ ) {
               dc << *(float *)&pData[nRecLen*i+(j*4)] << " ";
               if ( !(j&3) ) {
                  dc << "\r\n";
               }
            }
         }
      }
   }
}
