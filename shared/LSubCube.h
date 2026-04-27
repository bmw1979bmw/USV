// LSubCube.h: interface for the LSubCube class.
// подкуб для конвертирования старых баз
//////////////////////////////////////////////////////////////////////
#pragma once

#include "LScale.h"

class LSubCube  
{
public:
   void Dump(CDumpContext& dc) const;
   int nRecLen;
   BOOL Load(CArchive & ar);
   BYTE * pData;
   int nTotal;
   BYTE * pHead;
   int nHeadLen;
   LScale * pScales;
   int   nScales;
   void DeleteContents();
   void Serialize(CArchive & ar);
   LSubCube();
   virtual ~LSubCube();
};

