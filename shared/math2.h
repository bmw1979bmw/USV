#ifndef __MATH__H__2__
#define __MATH__H__2__

#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_E         2.71828182845904523536f
#define M_LOG2E     1.44269504088896340736f
#define M_LOG10E    0.434294481903251827651f
#define M_LN2       0.693147180559945309417f
#define M_LN10      2.30258509299404568402f
#define M_PI        3.14159265358979323846f
#define M_PI_2      1.57079632679489661923f
#define M_PI_4      0.785398163397448309616f
#define M_1_PI      0.318309886183790671538f
#define M_2_PI      0.636619772367581343076f
#define M_2_SQRTPI  1.12837916709551257390f
#define M_SQRT2     1.41421356237309504880f
#define M_SQRT_2    0.707106781186547524401f  /* 1/sqrt(2)*/

#endif

#define M_4_PI      1.2732395447351626861510701069801f
#define M_1_SQRTPI  0.564189583547756286948f
#define M_1_SQRT2   0.707106781186547524401f  /* 1/sqrt(2)*/
#define M_PI_180     (M_PI/180.0f)
#define M_180_PI     (180.0f/M_PI)
#define M_2PI       6.28318530717958647692f
#define M_2PI_3     2.094395102393195492308f
#define M_4PI       12.56637061435917295385f
#define M_SQRT_PI_2 1.2533141373155002512078826424055f /* (sqrt(M_PI_2)) */
               
#define M_0_23  0.23156333016903367669330684408867f /* sqr6(2)-1/sqr6(2)  */
     
#define GrRd (M_PI/180.0f)
#define RdGr (180.0f/M_PI)
#define KnKm (1.852f/3600.0f) /* узлы в км/с */
#define KnMe (1852.f/3600.0f)
#define MeKn (3600.0f/1852.f)
#define KmKn (3600.0f/1.852f)
#define MiSe (1.0f/1000.0f)
#define KmMe (1000.0f)
#define MeKm (1.0f/1000.0f)
#define SeKm (1.500f) /* seconds to km 1.500 */
#define SeMe (1500.f) /* seconds to m 1500 */

//#define
#ifndef CONTROL87PARAMS
#define CONTROL87PARAMS   \
      _EM_INEXACT         \
      |_EM_UNDERFLOW      \
      |_EM_DENORMAL       \
      |_EM_INVALID        
#endif
 
inline void CONTROL87()
{
// To make it work, you need to compile with /EHa !!! 
   _control87( 
      CONTROL87PARAMS
      ,_MCW_EM|_MCW_IC );
}

//#define 
inline void RESETFP()
{ 
   _clearfp();
   _control87(0,0);
   _fpreset();
   CONTROL87();
      //_control87(t,t/*_MCW_EM*/);
}

inline double distance(double x, double y)
{
   return sqrt(x*x+y*y);
}

inline float distance(float dx, float dy)
{
   return (float)sqrt(dx*dx+dy*dy);
}

inline float pow2(float val)
{
   return val*val;
}

struct TFPoint
{       
   float x;
   float y;
};

inline float distance(const TFPoint &p0, const TFPoint &p1)
{
   float dx = p0.x - p1.x;
   float dy = p0.y - p1.y;
   return (float)sqrt(dx*dx+dy*dy);
}


UINT locate(float val,float *scale,UINT N,float *delta1,float *delta2);
UINT locate(float val,TFPoint *scale,UINT N,float *delta1,float *delta2);
float inter(float left,float right,float delta1,float delta2);
float interloc(float val,const float *x,UINT N,const float *y); //Y
float interloccomb(float val,const TFPoint *scalex,float *y,UINT N); // x из шкалы y-из y
float interloce(float val,const float *x,UINT N,const float *y); // above edges
float interloc(float val,const TFPoint *scale,UINT N); // Y
float interloce(float val,const TFPoint *scale,UINT N); // above edges
float in_limits(float x, float low, float up); 
float interlocx0(float val,const TFPoint *scale,UINT N); // scale[i].x не равно нулю (меньше 44 полос)
float interloc_koef(float val,const float *x,UINT N,const float *y,int &ii,int &ii1,float &dd);
float interloc_calc( int ii, int ii1, float dd, float *y);
float interloce_koef(float val,const float *x,UINT N,const float *y,int &ii,int &ii1,float &dd);
float interloc_calc( int ii, int ii1, float dd, float *y);
//---------------------------------------------------------------------------
extern long rands_oldrand;
inline void set_rand(long t) { rands_oldrand = t;}
inline long get_rand() { return rands_oldrand; }
int  rands(void);
float rand758(void);
float rnd();
float random(float vmax);
float random(float low, float high);
void  randomize(void);
//генератор случайных чисел по Гауссу(0,1)
float  gauss();
float gauss3s(float sigma);

// рандомизация шума (Па^2)
float NoiseRand(int N/*сумма*/,float N1/*усреднение*/);
long NoiseRand_Get();
void NoiseRand_Set(long new_rands_oldrand);
//---------------------------------------------------------------------------

// void *operator new ( size_t stAllocateBlock, BOOL b );

#define fsqrt(a) ((float)sqrt(a))
#define fsin(a) ((float)sin(a))
#define fcos(a) ((float)cos(a))
#define ffabs(a) ((float)fabs(a))
#define ffmod(a,b) ((float)fmod(a,b))
#define fpow(a,b) ((float)pow(a,b))
#define flog10(a) ((float)log10(a))

// в градусах
#define fsin_gr(a) ((float)sin(GrRd*(a)))
#define fcos_gr(a) ((float)cos(GrRd*(a)))
#define sin_gr(a) (sin(GrRd*(a)))
#define cos_gr(a) (cos(GrRd*(a)))
#define sinG(a) (sin(GrRd*(a)))
#define cosG(a) (cos(GrRd*(a)))

float bearing2(float dx,float dy);

// радиальная скорость сближения объектов - в единицах V1, V2
float RadVel( float dx, float dy, float K1,float V1, float K2, float V2);
float RadVelP(float p/*пеленг,рад*/, float K1,float V1, float K2, float V2);

// модуль курсового угла на источник волны - для эха
float AbsCourseAngle(float meX, float meY, float  meK, float tX, float tY);

// курсовой угол на источник 
float CourseAngle(float meX, float meY, float  meK, float tX, float tY);

// угловой размер источника
float AngleSize(float meX, float meY, float tX, float tY, float tK, float tLen);

// угол от -180 до 180
float Angle180(float a);

// float меньше нуля (целочисленный, быстрый, исходя из формы хранения float'a)
inline BOOL qlt0(float t) { return (*(DWORD*)&t) & 0x80000000ul; }

// целочисленный, быстрый fabs, исходя из формы хранения float'a
inline void _Qfabs(float &t) { *(DWORD*)&t &= 0x7FFFfffful; }  //на месте

inline float qfabs(float t) 
{
   *(DWORD*)&t &= 0x7FFFfffful;
   return t;
}

float bearing_summ(float);

inline float bearing_delta(float a)
{
   //if ( a < 0 ) a = -a;
   a = fabs(a);
   if ( a > 180.f ) a = 360.f - a;
   return a;
} 

float angle_middle(float a, float b); // средний угол между двумя с учетом круга
float angle_grade(float beg, float end, float grade ); // угол от первого до второго по мин дистанции c процентом grade (0..1)
float angle_step(float beg, float end, float step ); // угол от первого до второго шагом по мин дистанции
 


#endif