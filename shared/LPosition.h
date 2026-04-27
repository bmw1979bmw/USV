// LPosition.h: interface for the LPosition class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "LCube.h"

class LCube;
// позиция в LCube
class LPosition    
{
private:  // no implementation:
        LPosition (const LPosition&);
   void operator= (const LPosition&);

public:
   void Create(int nlen);
	BOOL IsAtLast(int idx);// на последнем месте - pPos[idx] == pMax[idx]-1
   int GetSuperIdx(int n) const;
   int &operator [](int idx);
   BOOL WasAssigned() const { return !!pPos; }
   void Assign(LCube &base); // инициализация из базы
   int operator = (int num);
   void SetZero();
   LPosition();
   operator int() const;
   void operator --(int);
   void operator++(int);
   int * pMax;    // максимальные значения pPos + 1 (длины шкал)
   int * pPos;    // позиция в шкале (быстрее изменяется последняя)
   int nLen;
   explicit LPosition(LCube &base);
   virtual ~LPosition();
   void  swap(int i, int j);
   void __cdecl Set(int i0, ...);
};

