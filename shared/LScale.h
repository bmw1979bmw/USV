// LScale.h: interface for the LScale class.
#pragma once

#include "TextArchive.h"

#define size16 sizeof(__int16)
// шкала в LBase
class LScale  
{
public:
   void Serialize( CArchive & ar);
   LScale();
   virtual ~LScale();
   enum { NOACTIVE = 0x80, INCREASE=1, DECREASE=2 };

public:
   void Attach(int nlen, float *pData);
   void Detach();
   void CheckAscendType();
   float operator [](UINT idx);
   LScale(const LScale &scale);
   void operator = (const LScale &scale);
   BOOL ChangeLen(UINT nlen);
   UINT GetNearest(float f, float *pdelta = NULL ) const;
   void ReadOld(CArchive &ar);
   void SetType(int ntype){nType = (BYTE)ntype;}
   UINT ReadRaw(CArchive & ar);
   void Create(UINT nlen);
   void Create(UINT nlen, float First, float Step);
   BOOL operator == (LScale &sc);
   void Dump(CDumpContext& dc) const;
   BOOL IsActive() const { return !(nType&NOACTIVE); }

   void  WriteAsText(CTextArchive &ar, char *sComment);
public:
	int PointToIndex(int x, int nx);
	float GetLast(void) const;
   void Not();
   void Mirror(UINT nDummy=0);
   void DeleteAt(UINT idx);
   void InsertBefore(UINT idx);
   
   BYTE nType;

   UINT nLen;
   float * pScale;
};

