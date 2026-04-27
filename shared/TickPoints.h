// TickPoints.h: interface for the CTickPoints class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TICKPOINTS_H__DC063236_7FF0_11D5_B7C5_0050DAC48A11__INCLUDED_)
#define AFX_TICKPOINTS_H__DC063236_7FF0_11D5_B7C5_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphDisp.h"

class CTickPoints  
{
public:
	void DrawTicks(CDC &dc, int width, int height, CGraph2dData *d);
	void RemoveTicks(void);
	void AddTick(float hor, float ver, int type=0, float val=0);
	CTickPoints();
	virtual ~CTickPoints();

   struct TTickPoint
   {
      float ahor;
      float aver;
      int   type;
      float val;
   };
   typedef CList<TTickPoint, TTickPoint &> CTickList;
   CTickList m_ptList;

};

#endif // !defined(AFX_TICKPOINTS_H__DC063236_7FF0_11D5_B7C5_0050DAC48A11__INCLUDED_)
