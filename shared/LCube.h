// LCube.h: interface for the LCube class.
#pragma once

#include "LScale.h"

#define  LCUBE_INLINE

class LCube
{
public:  // data
   UINT     nRecords;
   UINT     nRecLen;
   UINT     nDataLen;
   BYTE    *pData;
   LScale  *pScales;
   UINT     nScales;

public: // functions
   void * GetData() const { return pData; }
   // поиск //////////////////////////////////////////
//   int FindRecordPos(LPosition &pos,float first,...); // позици€ - внешн€€
   int __cdecl FindRecordIdx(float first,...);
   int __cdecl FindRecordIdxEx(float *deltas,float first,...);
   int __cdecl FindSubCubeIdx(UINT nVals, float first,...);
   int __cdecl FindSubCubeIdxEx(float *deltas,UINT nVals, float first,...);
   void * __cdecl FindRecord(float first, ...);
   void * __cdecl GetRecord(UINT idxfirst, ...);
   int __cdecl GetSubCubeIdx(UINT nVals, UINT idxfirst,...);
   int vFindIdx(float *pFirst);
   ///////////////////////////////////////////////////
   virtual void * operator [] (UINT idx);
   /// инфо //////////////////////////////////////////
   // float * шкала - массив
   float * GetScale(UINT idx) const {
      ASSERT(pScales);
      ASSERT(idx < nScales);
      return pScales[idx].pScale;
   }

   int GetScaleLen(UINT idx) const {
      ASSERT(pScales);
      ASSERT(idx < nScales);
      return pScales[idx].nLen;
   }


public: // methods

   BOOL Create(UINT nscales, const LScale * scales, UINT nreclen);

   BOOL IsCreated() { return pData != NULL; }

public:
	void CheckAscendScales();
	void DebugFill();
   void Dump(CDumpContext& dc) const;
   virtual void DeleteContents();
   LCube();
   virtual ~LCube();

protected:
   LCube( const LCube & ); // no implementation!

};

// #ifdef LCUBE_INLINE
// 
// inline int LCube::GetScaleLen(int idx) const
// {
//    return pScales[idx].nLen;
// }
// 
// inline float * LCube::GetScale(int idx) const
// {
//    return pScales[idx].pScale;
// }
// 
// #endif

