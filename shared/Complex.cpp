/////////////////////////////////////////////////////////////////////////////
//COMPLEX Implementation
#include "stdafx.h"
#include <stddef.h>
#include <stdlib.h>

#include "Complex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////


Complex& Complex::operator*=(const Complex& a)
{
   float Re=re*a.re-im*a.im,
      Im=re*a.im+im*a.re;
   
   re=Re;
   im=Im;
   
   return *this;
} 

#ifndef COMPLEX_INLINE
Complex Complex::operator*(Complex& a)
{
   return Complex ( a.re*re - a.im*im, a.re*im + a.im*re );
}
#endif

Complex operator*(const Complex& a, const Complex& b)
{
   return Complex ( a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re );
}


Complex& Complex::operator/=(const Complex& a)
{    
   //ASSERT(0); //#ERROR!
   __asm _emit 0xCC;
//    float mag=(float)sqrt(a.re*a.re+a.im*a.im);
//    re/=mag;
//    im/=mag;
//    /* xxx надо так! неправильно
//    float d = pow(a);
//    nre = (re*a.re + im*a.im)/d;  неправильно
//    nim = (im*a.re - re*a.im)/d;
//    */
   /* из stl */
   if ( (a.im<0?-a.im:a.im) < (a.re<0?-a.re:a.re) ) {
      float wr = a.im/a.re;
      float wd = a.re + wr*a.im;
      float nre = (re + im*wr)/wd;
      im = (im - re*wr)/wd;
      re = nre;
   } else if (a.im==0) {
      float t=1;
      t /= a.im;    //throw
   } else {
      float wr = a.re/a.im;
      float wd = a.im + wr*a.re;
      float nre = (re*wr + im)/wd;
      im = (im*wr - re)/wd;
      re = nre;
   }

   return *this;
}

Complex operator/(const Complex& a, const Complex& b)
{    
   Complex c(a);
   c /= b;
   return c;
   //ASSERT(0);// #ERROR!
//    __asm _emit 0xCC;
//    float mag=(float)sqrt(b.re*b.re+b.im*b.im);
//    return Complex(a.re/=mag,b.re/=mag);
}           


Complex operator+(const Complex& a, const Complex& b)
{
   return Complex(a.re +b.re, a.im + b.im);
}

Complex operator+(float a, const Complex & b)
{
   return Complex(a + b.re, b.im);
}

Complex operator+(const Complex & a, float b)
{
   return Complex(a.re + b, a.im);
}
Complex operator-(const Complex& a, const Complex& b)
{
   return Complex(a.re - b.re, a.im - b.im);
}

Complex operator-(float a, const Complex& b)
{
   return Complex(a - b.re, -b.im);
}

Complex operator-(const Complex& a, float b)
{
   return Complex(a.re - b, a.im);
}

Complex operator*(float a, const Complex& b)
{
   return Complex(b.re*a, b.im*a);
}

Complex operator/(const Complex& a, float b)
{
   return Complex(a.re/b, a.im/b);
}

int operator==(const Complex& a, const Complex& b)
{
   return a.re == b.re && a.im == b.im;
}

int operator!=(const Complex& a, const Complex& b)
{
   return a.re != b.re || a.im != b.im;
}

#ifndef COMPLEX_INLINE

float Complex::abs()
{
   return (float)sqrt(re*re+im*im);
}

float Complex::pow()
{
   return (float)(re*re+im*im);
}

#endif

void Complex::swap(Complex &src)
{
   float tim,tre;
   tim = im;
   tre = re;
   im = src.im;
   re = src.re;
   src.im = tim;
   src.re = tre;
}

