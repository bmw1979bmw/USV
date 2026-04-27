// structures.h: interface for the structures class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "math2.h"

struct TPattNoi
{
   DWORD    dwFlags;
   TFPoint  wbn[44]; // x-freq Гц, y-level Па/sqr(Гц) | Па
   TFPoint  nbn[44]; // x-freq Гц, y-level Па
};

struct TPattHns
{
   struct TWbn{
      float freq;    // Гц
      float lev;     // level Па
      float Rteta[19]; // гориз
      float Rfi[19];   // верт
   } wbn[44];
   struct TNbn{
      float freq;    // Гц
      float band;    // Гц
      float lev;     // level Па
      float Rteta[19]; // гориз
      float Rfi[19];   // верт
   } nbn[44];

#define HNS_WBNSTEP (sizeof(TPattHns::TWbn))
#define HNS_NBNSTEP (sizeof(TPattHns::TNbn))
};

struct TPatt2d
{  
   TFPoint  plane1[44]; // x-freq Гц, y-level Па/sqr(Гц)
   DWORD    dwReserved[4]; // not used
   float    fSector;    // сектор излучения на носу, если LBF_WBNSECTOR
   float    fUpSector;  // сектор излучения вверх
   float    fDownSector;// сектор излучения вниз
   LONG     nBlicks;    // число бликов в NBN used in monitor
   DWORD    dwFlags;    // used in fromWbn(LBF_PA)

   void CalcNBlicks(){
      nBlicks = 0;
      for ( int k=0; k<44 ;k++ ) {
         if ( !plane1[k].x ) break;
         nBlicks++;
      }
   }
}; 

#define CopyWbn(dest,src) memcpy(dest,src,sizeof(TPatt2d))
#define CopyNbn(dest,src) CopyWbn(dest,src)
#define CopyTF44(dest,src) memcpy(dest,src,44*sizeof(TFPoint))

struct TPatt1d
{  
   TFPoint  plane1[44];
}; 

struct TMParams { // motions parameters
   float    X;           // координата Х(широта)  ,км
   float    Y;           // координата Y(долгота) ,км
   float    Z;           // координата Z(глубина) ,м
   float    K;           // курс                  ,градусы
   float    V;           // скорость              ,узлы
   float    L;           // крен                  ,градусы
   float    D;           // дифферент             ,градусы   
   TMParams(){
      X=Y=Z=V=K=D=L=0;
   }
   TMParams(float x,float y,float z,float k,float v,float l,float d) {
      X=x;Y=y;Z=z;K=k;V=v;L=l;D=d;
   }
   /*
   float operator[](int idx) { return ((float *)this)[idx]; }
   */
   // позиция объекта для отображения в метрах
   operator CPoint() const { return CPoint((long)(X*1000),(long)(Y*1000));}
   // позиция объекта X и Z для отображения по глубине в метрах
   CPoint XZ() const { return CPoint((long)(X*1000),(long)(Z*1000));}

private:
   TMParams(const TMParams &src);  // не нужно передавать структуры через стек!
};

struct TFRect {
   float left;
   float top;
   float right;
   float bottom;
   float CenterX() const { return (left+right)/2; }
   float CenterY() const { return (top+bottom)/2; }
};

inline float distance(const TMParams *p1, const TMParams *p2) //km      2d
{
   float x = p1->X - p2->X;
   float y = p1->Y - p2->Y;
   return (float)sqrt(x*x+y*y);
}

inline float distance(const TMParams &p1, const TMParams &p2) //km      2d
{
   float x = p1.X - p2.X;
   float y = p1.Y - p2.Y;
   return (float)sqrt(x*x+y*y);
}

inline BOOL distance3dlt(const TMParams *p1, const TMParams *p2, float dist) //km
{  // истина, если расстояние м/ объектами меньше заданного
   dist*=dist;
   float x = p1->X - p2->X;
   x*=x;
   if ( x > dist ) return FALSE;
   float y = p1->Y - p2->Y;
   y*=y;
   if ( y > dist ) return FALSE;
   float z = (p1->Z - p2->Z)/1000.f;   // m => km
   z*=z;
   if ( z > dist ) return FALSE;
   return ((x+y+z) <= dist);
}

inline float distance3d(const TMParams *p1, const TMParams *p2) //km
{
   float x = p1->X - p2->X;
   float y = p1->Y - p2->Y;
   float z = (p1->Z - p2->Z)/1000.f;   // m => km
   return (float)sqrt(x*x+y*y+z*z);
}

inline void distances(float &dist2d, 
                         float &dist3d, 
                         const TMParams *p1, const TMParams *p2) //m
{  // вычисляет дистанции 2d и 3d в метрах 
   float x = (p1->X - p2->X)*1000;   // m
   float y = (p1->Y - p2->Y)*1000;   // m
   x = x*x + y*y;
   float z = p1->Z - p2->Z;   // m
   dist2d = fsqrt(x);
   dist3d = fsqrt(x+z*z);   // m
}

inline float bearing2(const TMParams &me, const TMParams &target) 
{
   return bearing2(target.X - me.X, target.Y - me.Y);
}

// в градусах
inline float bearing_G(const TMParams &me, const TMParams &target) 
{
   return RdGr*bearing2(me, target);

}
// вертикальный пеленг на объект: +вверх -вниз   // сиречь угол места,градусы
inline float vbearing(const TMParams *p1, const TMParams *p2)
{  // p1-Me, p2-object
   float d = distance(p1,p2);
   float dz = p1->Z - p2->Z;
   return (float)(atan2(dz,d*1000)*RdGr);
}

inline float vbearing(float Dist2d_m, const TMParams *p1, const TMParams *p2)
{  // Dist_m-дистанция в метрах,  p1-Me, p2-object
   float dz = p1->Z - p2->Z;
   return (float)(atan2(dz,Dist2d_m)*RdGr);
}

inline float vbearing(float Dist2d_m,float Z1,float Z2)
{  // Dist_m-дистанция в метрах,  p1-Me, p2-object
   float dz = Z1 - Z2;
   return (float)(atan2(dz,Dist2d_m)*RdGr);
}

inline float vbearing3d(float Dist3d_m, const TMParams *p1, const TMParams *p2)
{  // Dist3d_m-дистанция трехмерная в метрах,  p1-Me, p2-object
   // не очень хорошо близко к нулю
   if ( !Dist3d_m ) return 90;
   float dz = p1->Z - p2->Z;
   return (float)(asin(dz/Dist3d_m)*RdGr);
}

inline float CourseAngle(const TMParams &me, const TMParams &target)
{  // курсовой угол на источник волны
   // ok
   float angle = (float)(RdGr*(atan2(target.X - me.X, target.Y - me.Y) - (GrRd*me.K)));
   //angle = ffmod(angle, 180);
   angle = ffmod(angle+360+360, 360);
   if ( angle > 180 ) angle -= 360;
   return angle;
}

// акустические флаги объекта
#define AF_REFLECTOR          1    /* отражатель - есть эхопортрет */
#define AF_STANDBY            2    /* признак, что флаги могут меняться.
                                      объект будет отображаться в мониторе 
                                      и к нему будут тянуться волны,
                                      даже если нет других флагов */
#define AF_RECEIVER           4    /* приемник */
#define AF_ALL_LOWER    (AF_REFLECTOR|AF_STANDBY|AF_RECEIVER)

#define AF_STRANGER         0x10    /* для DEMO - признак свой/чужой */

#define AF_DEBUG           0x100    /* для отладки */

// flags short versions
#define AF_ECHO AF_REFLECTOR
#define AF_RECV AF_RECEIVER

struct TXParams: public TMParams /* структура обмена world-monitor situation */
{
   DWORD dwNoises;  // флаги шумов, up to 32
   DWORD dwAcFlags; // акустические флаги объекта
   TXParams() : dwNoises(1), dwAcFlags(5) {}

   DWORD *TXArr(){ return (DWORD*)static_cast<TMParams*>(this); }
   float *TFArr(){ return (float*)static_cast<TMParams*>(this); }

   static UINT Count(){ return sizeof(TXParams)/4; }

private:
   TXParams(const TXParams &src); // не нужно передавать структуры через стек!
};

#ifdef _DEBUG
#  define INFO ::AfxTrace("%s(%i): ",__FILE__,__LINE__); ::AfxTrace
#else
#  if _MSC_VER >= 1300
#    define INFO __noop
#  else
#    define INFO ((void)0)
#  endif
#endif

/////////////////////////////////////////////////////////////////
#define for_each(l,m) for( POSITION pos##m = (l).GetHeadPosition(); pos##m && (m = (l).GetNext(pos##m))!=0; ) 
#define for_each_(l,m) for(         pos##m = (l).GetHeadPosition(); pos##m && (m = (l).GetNext(pos##m))!=0; ) 
#define for_each1(l,m) for( POSITION pos1##m = (l).GetHeadPosition(); pos1##m && (m = (l).GetNext(pos1##m))!=0; ) 
#define for_each2(l,m) for( POSITION pos2##m = (l).GetHeadPosition(); pos2##m && (m = (l).GetNext(pos2##m))!=0; ) 

#define for_eachx(l,m)  for( POSITION pos0 = (l).GetHeadPosition(); (pos = pos0)!=0 && (m = (l).GetNext(pos0))!=0;)

#define for_eacht(l,t,m) for( POSITION pos##m = (l).GetHeadPosition(); pos##m && (m = (t)(l).GetNext(pos##m))!=0; ) 

#define for_arr(a, T, p) for(T *p = a.GetData(), *e##a =  a.GetData()+a.GetSize();p<e##a;++p)
//#define for_this(m)   for( POSITION pos##m = GetHeadPosition(); pos##m && (m = GetNext(pos##m))!=0; ) 

/////////////////////////////////////////////////////////////////

