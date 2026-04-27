#include "stdafx.h"
#include <time.h>
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT locate(float val, const float *scale, UINT N, float *delta1, float *delta2)
{
   float left, right;
   left=scale[0];
   if(val<=left)
   { 
      *delta1=0;
      *delta2=1;
      return 0;
   } 
   right=scale[N-1];
   if(val>=right) 
   {
      *delta1=1;
      *delta2=0;
      return N-1;
   }
   
   for(UINT i=0;i<N-1;i++)
   { 
      left=scale[i];
      right=scale[i+1];
      if(val>=left&&val<=right) 
      {   
         *delta1=val- left;
         *delta2=right-val;
         return i;
      }
   }  
   return 0;   
}

float interloc(float val, const float *x, UINT N, const float *y) 
{ //Y
   if ( val <= x[0] ) { 
      return y[0];
   } 
   if ( val >= x[N-1] ) {
      return y[N-1];
   }
   for( UINT i=0;i<N-1;i++) { 
      if( val < x[i+1] ) {
         return (y[i+1]-y[i])*
            (val-x[i])/(x[i+1]-x[i]) + y[i];
      }
   }  
   return 0;   
}

float interloce(float val, const float *x, UINT N, const float *y)  // above edges
{ //Y
   UINT i;
   for( i=0; i<N-2; i++ ) { 
      if( val < x[i+1] ) {
         return (y[i+1]-y[i])*
            (val-x[i])/(x[i+1]-x[i]) + y[i];
      }
   }  
   // here i = N-1;
   return (y[i+1]-y[i])*
      (val-x[i])/(x[i+1]-x[i]) + y[i];
}

float in_limits(float x, float low, float up)
{
   if ( x < low ) return low;
   if ( x > up ) return up;
   return x;
}

// interloc_koef
// interloc_calc
// - используются для быстрого вычисления interloc для известных частот.
// Частоте сопоставляется структура типа { ii, ii1, dd }, которая заполняется
// функцией interloc_koef, потом при изменении шкалы y по этим данным
// вычисляется значение в interloc_calc. см. Monitor\CMonWave::DampTFPoint83
float interloc_koef(float val, const float *x, UINT N, const float *y, 
                   int   &ii,   // i
                   int   &ii1, // i+1
                   float &dd)  // (val-x[i])/(x[i+1]-x[i])
{ // ищет коэффициенты для дальнейшего быстрого вычисления interloc_calc
   if ( val <= x[0] ) { 
      ii = ii1 = 0;
      dd = 0;
      return y[0];
   } 
   if ( val >= x[N-1] ) {
      ii = ii1 = N-1;
      dd = 0;
      return y[N-1];
   }
   for( UINT i=0;i<N-1;i++) { 
      if( val < x[i+1] ) {
         ii = i;
         ii1 = i+1;
         dd = (val-x[i])/(x[i+1]-x[i]);
         return (y[ii1]-y[ii])*dd+y[ii];
      }
   }  
   ASSERT(0);
   return 0;
}
// версия - за краями тоже
float interloce_koef(float val, const float *x, UINT N, const float *y, 
                   int   &ii,   // i
                   int   &ii1, // i+1
                   float &dd)  // (val-x[i])/(x[i+1]-x[i])
{
   UINT i;
   for( i=0;i<N-2;i++) { 
      if( val < x[i+1] ) {
         ii = i;
         ii1 = i+1;
         dd = (val-x[i])/(x[i+1]-x[i]);
         return (y[ii1]-y[ii])*dd+y[ii];
      }
   }  
   ii = i;
   ii1 = i+1;
   dd = (val-x[i])/(x[i+1]-x[i]);
   return (y[ii1]-y[ii])*dd+y[ii];
}

float interloc_calc( int ii, int ii1, float dd, float *y)
{
   return (y[ii1]-y[ii])*dd+y[ii];
}


float interloc(float val, const TFPoint *scale, UINT N) 
{ //Y
   if ( val <= scale[0].x ) { 
      return scale[0].y;
   } 
   if ( val >= scale[N-1].x ) {
      return scale[N-1].y;
   }
   for( UINT i=0;i<N-1;i++) { 
      if( /* scale[i].x <= val &&*/ val <= scale[i+1].x ) { // шкала возрастающая плиз
         return (scale[i+1].y-scale[i].y)*
            (val-scale[i].x)/(scale[i+1].x-scale[i].x) + scale[i].y;
      }
   }  
   return 0;   
}

float interloccomb(float val, const TFPoint *scalex, float *y, UINT N) 
{ //Y
   if ( val <= scalex[0].x ) { 
      return y[0];
   } 
   if ( val >= scalex[N-1].x ) {
      return y[N-1];
   }
   for( UINT i=0;i<N-1;i++) { 
      if( /* scale[i].x <= val &&*/ val <= scalex[i+1].x ) { // шкала возрастающая плиз
         return (y[i+1]-y[i])*
            (val-scalex[i].x)/(scalex[i+1].x-scalex[i].x) + y[i];
      }
   }  
   return 0;   
}

float interloce(float val, const TFPoint *scale, UINT N) 
{ //Y
   UINT i;
   for( i=0;i<N-2;i++) { 
      if( /* scale[i].x <= val &&*/ val <= scale[i+1].x ) { // шкала возрастающая плиз
         return (scale[i+1].y-scale[i].y)*
            (val-scale[i].x)/(scale[i+1].x-scale[i].x) + scale[i].y;
      }
   }  
   return (scale[i+1].y-scale[i].y)*
      (val-scale[i].x)/(scale[i+1].x-scale[i].x) + scale[i].y;
}

float interlocx0(float val, const TFPoint *scale, UINT N) 
{ //Y
   while ( scale[N-1].x == 0 ) N--;
   if ( val <= scale[0].x ) { 
      return scale[0].y;
   } 
   if ( val >= scale[N-1].x ) {
      return scale[N-1].y;
   }
   for( UINT i=0;i<N-1;i++) { 
      if( /* scale[i].x <= val &&*/ val < scale[i+1].x ) { // шкала возрастающая плиз
         return (scale[i+1].y-scale[i].y)*
            (val-scale[i].x)/(scale[i+1].x-scale[i].x) + scale[i].y;
      }
   }  
   return 0;   
}

UINT locate(float val, TFPoint *scale, UINT N, float *delta1, float *delta2)
{
   float left, right;
   left=scale[0].x;
   if(val<=left)
   { 
      *delta1=0;
      *delta2=1;
      return 0;
   } 
   right=scale[N-1].x;
   if(val>=right) 
   {
      *delta1=1;
      *delta2=0;
      return N-1;
   }
   
   for(UINT i=0;i<N-1;i++)
   { 
      left=scale[i].x;
      right=scale[i+1].x;
      if(val>=left&&val<=right) 
      {   
         *delta1=val- left;
         *delta2=right-val;
         return i;
      }
   }  
   return 0;   
}

float inter(float left, float right, float delta1, float delta2)
{
   return(left+(right-left)*delta1/(delta1+delta2));
}  

// в градусах
float bearing_summ(float a )
{  
   if ( a < 0 ) a += 360.f;
   else if ( a >= 360.f ) a -= 360.f;
   return a;  
}             

long rands_oldrand=1;

int  rands(void) 
{
//microsoft
   /*
   return ((
       (rands_oldrand = rands_oldrand * 214013L + 2531011L) 
       >> 16)
       & 0x7fff   WM_CREATE
       );  
    */
// Borland
   
   return ((
       (rands_oldrand = 0x15a4e35L * rands_oldrand + 1)
       >> 16) 
       & 0x7fff 
       );
   
}
void randomize(void) 
{
   rands_oldrand = (long)time(NULL);
}

float random(float vmax) 
{
   return (float)rands()*vmax/32768.f;
}

float random(float low, float high)
{
   return low+random(high-low);
}

float  gauss()       // gaussbor see RandTest
{ // гауссовский датчик с sigma == 1
  
   static const 
      float  ai[3] =  {2.515517f , 0.802853f,  0.010328f },
      bi[4] =  {1.f,   1.432788f,  0.189269f, 0.001308f };
   
   float  w = 0, xz = 0;
   float  rn;
   rn = random(1);
   if ( rn == 0 || rn == 1) return 0;
   
   float t;
   if ( rn >= 0.5f ) t = 1-rn;
   else t = rn;
   w = (float)sqrt( log(1.0/( t*t )));

   xz = w - (ai[0] + w*(ai[1] + ai[2]*w))/
      (bi[0] + w*(bi[1] + w*(bi[2] + bi[3]*w)));
   
   if (rn > 0.5f) return  -xz;
   else           return   xz ;
}

float gauss3s(float sigma)
{ // гауссовский датчик ограниченный 3мя сигмами
   float tx = gauss()*sigma;
   float t = tx;
   t = fabs(t);
   if( t > 3*sigma ) t = 3*sigma;
   if ( tx < 0 ) return -t;
   return t;
}

float NoiseRand(int N/*сумма*/, float N1/*усреднение*/) 
{   // как коэффициент к сумме квадратов амплитуд (Па^2)

// рандомизация шума
// быстрая реализация следующей формулы случайного распределения
// (приближенная). Сумма по мощности усредненная. (Па^2)
// {
//    float _1_253 = M_SQRT_PI_2;
//    
//    f = 0;
//    f = 0;
//    for ( int j=0; j<d_N ;j++ ) {  // сумма
//       float t = 0;
//       for ( int k=0; k<d_N1 ;k++ ) { // усреднение
//          float d = d_B*reley()/_1_253;
//          t += d*d;
//       }
//       t /= d_N1;
//       f += t;
//    }
// }

   return ffabs (1 + gauss()/sqrt(N1*N) );
}             

long NoiseRand_Get()
{
   return rands_oldrand;
}

void NoiseRand_Set(long new_rands_oldrand)
{
   rands_oldrand = new_rands_oldrand;
}

//---------------------------------------------------------------------------
float bearing2(float dx, float dy)
{
   //! dx = target.x - we.x
   //! dy = target.y - we.y
   float   pel=(float)atan2(dx, dy);
   if ( pel < 0 ) pel += M_2PI;
   return pel;
}
// радиальная скорость сближения объектов - в единицах V1, V2     2d
float RadVel( float dx, float dy, float K1, float V1, float K2, float V2)
{
   //! dx = target.x - we.x
   //! dy = target.y - we.y
   float p = (float)atan2(dx, dy);
   return   (float)( cos(K1*GrRd-p)*V1 - cos(K2*GrRd-p)*V2 );
}
// радиальная скорость объектов в 3D будет Vr3d = Vr*cos(vbearing)


// радиальная скорость сближения объектов - в единицах V1, V2     2d
// для быстроты, с заранее вычисленным пеленгом в радианах
float RadVelP( float p/*пеленг, рад*/, float K1, float V1, float K2, float V2)
{
   return (float)( cos(K1*GrRd-p)*V1 - cos(K2*GrRd-p)*V2 );
}

float AbsCourseAngle(float meX, float meY, float  meK, float tX, float tY)
{ // модуль курсового угла на источник волны - для эха
   // me-мы   t-источник
   float angle = (float)fabs((RdGr*(atan2(tX - meX, tY - meY) - (GrRd*meK))));
   angle = ffmod(angle, 360);
   if ( angle > 180 ) angle = 360-angle;
   return angle;
}

float CourseAngle(float meX, float meY, float  meK, float tX, float tY)
{  // курсовой угол на источник волны
   // ok
   float angle = (float)(RdGr*(atan2(tX - meX, tY - meY) - (GrRd*meK)));
   //angle = ffmod(angle, 180);
   angle = ffmod(angle+360+360, 360);
   if ( angle > 180 ) angle -= 360;
   return angle;
}

// угловой размер источника
float AngleSize(float meX, float meY, float tX, float tY, float tK, float tLen/*длина источ*/)
{
   // t-target

   float D = distance(meX-tX, meY-tY)*KmMe;
   float Ky = CourseAngle(tX, tY, tK, meX, meY);
   float dd = fabs(tLen*fcos(GrRd*Ky)/2);
   float h =  tLen*fsin(GrRd*Ky)/2;

   return float( RdGr*(fabs(atan2(h, D-dd))+fabs(atan2(h, D+dd))));
}

// угол от -180 до 180
float Angle180(float a)
{
   if ( a > 180 ) a -= 360;
   return a;
}


float angle_middle(float a, float b)
{ // средний угол между двумя с учетом круга
   float d = (a+b)/2;
   if ( fabs(a-b) > 180 ) {
      d += 180;
      if ( d >= 360 ) d -= 360;
   }
   return d;
}

float angle_grade(float beg, float end, float grade )
{ // угол от первого до второго по мин дистанции c процентом grade (0..1)

   // 300->200
   // 300 + (200-300)*grade

   // 300->10
   // 300 + (10-300)*grade
   // должен 300->370
   // 300 + (370-300)*grade

   // 10->300
   // 10 + (300+360-10)*grade
   // 

//    НЕВЕРНО
//    float d;
//    if ( fabs(beg-end) > 180 ) {
//       d = beg + (end + 360 - beg)*grade;
//       if ( d >= 360 ) d -= 360;
//    } else {
//       d = beg + (end - beg)*grade;
//    }
//    ================

   float delta = end - beg;
   if ( fabs(delta) < 180 ) return beg + delta * grade;
   
   if ( delta < 0 ) delta += 360;
   else             delta -= 360;
	float d = beg + delta * grade;
   if ( d >= 360 ) d -= 360;
   return d;
}


float angle_step(float beg, float end, float step )
{ // угол от первого до второго шагом по мин дистанции
   
   float d;
   float delta = end - beg;
   if ( fabs(delta) > 180 ) {
	   if ( delta > 0 ) step = -step;
      d = beg + step;
      if ( d >= 360 ) d -= 360;
   } else {
      d = beg + step;
   }
   return d;
}
