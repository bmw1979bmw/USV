//Complex .h : Declares the COMPLEX classes .
//

#ifndef __COMPLEX_H__
#define __COMPLEX_H__
#include <math2.h>
/////////////////////////////////////////////////////////////////////////////
#define COMPLEX_INLINE
class  Complex 
{
   
public:
   float re,
      im;
   // constructors
   Complex(float Re, float Im=0) { re = Re; im = Im; }
   Complex(){}
   
   // Binary Operator Functions
   friend Complex  operator+(const Complex&, const Complex&);
   friend Complex  operator-(const Complex&, const Complex&);
   friend Complex  operator*(const Complex&, const Complex&);   
   Complex operator*(const Complex& a);
   Complex operator*(float b);

   friend Complex  operator/(const Complex&, const Complex&);
   
   friend Complex  operator-(float, const Complex&);
   friend Complex  operator+(float, const Complex&);
   friend Complex  operator*(float, const Complex&);
   
   friend Complex  operator+(const Complex&, float);
   friend Complex  operator-(const Complex&, float);
   friend Complex  operator/(const Complex&, float);
   
   friend int      operator==(const Complex&, const Complex&);
   friend int      operator!=(const Complex&, const Complex&);
   
   BOOL NonZero(){ return re != 0 || im != 0; }

   Complex&  operator+=(const Complex&a){ re += a.re; im += a.im; return *this; }
   Complex&  operator-=(const Complex&a){ re -= a.re; im -= a.im; return *this; }
   Complex&  operator*=(const Complex&a); 
   Complex&  operator/=(const Complex&a);

   Complex&  operator+=(float a){ re +=a; return *this; }
   Complex&  operator-=(float a){ re -=a; return *this; }
   Complex&  operator*=(float a){ re*=a; im*=a; return *this; }
   Complex&  operator/=(float a){ re/=a; im/=a; return *this; }
   
   Complex   operator+() { return *this; }
   Complex   operator-() { return Complex(-re, -im); }

   float abs(void);
   float pow(void);
   void swap(Complex &src);
   
//    friend float real(Complex&);
//    friend float imag(Complex&);
   
};

#ifdef COMPLEX_INLINE
inline Complex Complex::operator*(const Complex& a)
{
   return Complex ( a.re*re - a.im*im, a.re*im + a.im*re );
}

inline Complex Complex::operator*(float b)
{
   return Complex(re*b, im*b);
}

inline float Complex::abs()
{
   return fsqrt(re*re+im*im);
}

inline float Complex::pow()
{
   return (float)(re*re+im*im);
}

#endif


#endif///////////////////////////////////////////////////////////////////////