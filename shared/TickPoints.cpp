// TickPoints.cpp: implementation of the CTickPoints class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gak.h"
#include "TickPoints.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTickPoints::CTickPoints()
{

}

CTickPoints::~CTickPoints()
{
   m_ptList.RemoveAll();
}

void CTickPoints::RemoveTicks()
{
   m_ptList.RemoveAll();
}

void CTickPoints::AddTick(float hor, float ver, int type, float val)
{
   TTickPoint p;
   p.ahor = hor;
   p.aver = ver;
   p.type = type;
   p.val  = val;
   m_ptList.AddTail(p);
}

void CTickPoints::DrawTicks(CDC &dc, int width, int height, CGraph2dData *d)
{
   // Last: -13
//   int n=4;
   CPen  pn(0,1,0x00FF), pn2(0,1,0xAA), pn3(0,1,0x0000AA00), *op;
   CBrush br(0x000000FF), *obr;
   int nX = d->nX;
   int nY = d->nY;

   dc.SelectStockObject(NULL_BRUSH);

   TCHAR buf[16];
   for( POSITION pos = m_ptList.GetHeadPosition(); pos ; ) {
      TTickPoint &p = m_ptList.GetNext(pos);
      int x = int ( width  * (nX-1)/nX * d->RangeXmm(p.ahor) + width/nX/2 );
      int y = int ( height * (nY-1)/nY * d->RangeYmm(p.aver) + height/nY/2);
      if ( d->m_dwStyle & GDS_NOTY ) y = height-y;
      switch ( p.type ) {
      case -12:  // верт палочка с цифрой - АСЦ
         dc.MoveTo(x,y);
         dc.LineTo(x,y+5);
         wsprintf(buf,_T("%d"),(int)p.val);
         dc.TextOut(x,y+5,buf);
         break;
      case -9: // уголки от квадратика
            dc.SetROP2(R2_XORPEN);
            dc.Ellipse(x-3,y-3,x+3,y+3);
      case -10: // квадратик XOR
            dc.SetROP2(R2_XORPEN);
            dc.Rectangle(x-3,y-3,x+3,y+3);
         	dc.SetROP2(R2_COPYPEN);
         break;
      case -8: // треугольник ниже точки на 6, острием вверх
         {
            POINT ss[4] = 
            {
               x-3,y+6+8,
               x,y+8,
               x+4,y+6+8,
               x-3,y+6+8
            };
            dc.Polyline(ss,4);
         }
         break;
      case -7: // треугольник с флоатом
         {
            _stprintf(buf,_T("%g"),p.val);
            dc.MoveTo(x-2,y); dc.LineTo(x+3,y);
            dc.MoveTo(x,y-2); dc.LineTo(x,y+3);
            dc.TextOut(x+2,y-7,buf);
         }
         break;
      case -6: // заполненный треугольник красным
         {
            obr = dc.SelectObject(&br);
            POINT ss[3] = 
            {
               x-3,y+6,
               x,y,
               x+4,y+6
            };
            dc.Polygon(ss,3);
            dc.SelectObject(obr);
         }
         break;
      case -5:  // заполненный квадратик красным
         {
            obr = dc.SelectObject(&br);
            dc.Rectangle(x-3,y-3,x+3,y+3);         
            dc.SelectObject(obr);
         }
         break;
      case 0: // квадратик
         dc.Rectangle(x-3,y-3,x+3,y+3);
      	break;
      case -1:
         {
         POINT node[3] = {
         
         /*node[0].x = */x-3,
         /*node[0].y =*/ y-3,
         /*node[1].x = */x-3,
         /*node[1].y = */y+3,
         /*node[2].x = */x,
         /*node[2].y = */y+3 
         };
         dc.Polygon(node,3);
         }
      	break;
      case -2:
         {
         POINT node[3] = {
         /*node[0].x = */x-3,
         /*node[0].y = */y+3,
         /*node[1].x = */x,
         /*node[1].y = */y-3,
         /*node[2].x = */x+3, 
         /*node[2].y = */y+3
         };
         dc.Polygon(node,3);
         }
      	break;
      case -11:
         op = dc.SelectObject(&pn2);
         goto _3;
      case -3:
         {
            op = dc.SelectObject(&pn);
_3:         dc.MoveTo(x-4,y); dc.LineTo(x+5,y);
            dc.MoveTo(x,y-4); dc.LineTo(x,y+5);
            dc.SelectObject(op);
         }
         break;
      case -4:
         {  
            op = dc.SelectObject(&pn);
            dc.MoveTo(x,0); dc.LineTo(x,height);
            dc.SelectObject(op);
         }
         break;
      case -13:
         {  
            op = dc.SelectObject(&pn3);
            dc.MoveTo(0,y); dc.LineTo(width,y);
            dc.SelectObject(op);
         }
         break;
      default: 
         {
            wsprintf(buf,_T("%d"),p.type);
            dc.MoveTo(x-2,y); dc.LineTo(x+3,y);
            dc.MoveTo(x,y-2); dc.LineTo(x,y+3);
            dc.TextOut(x+1,y+1,buf);
         }
      }
   }
}
