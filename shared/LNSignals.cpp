// LNSignals.cpp: implementation of the LSSignal, LMSignal classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LNSignals.h"
#include "MWave.h"
#include "GakStructs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LSSignal::LSSignal():
   Time(pData[0]), 
   Freq(pData[1])
{
   n=0;
   full=0;
   pData[0]=pData[1]=NULL;
}

LSSignal::LSSignal(LRange &range):
   Time(pData[0]), 
   Freq(pData[1])
{
   pData[0]=pData[1]=NULL;
   create(range);
}

LSSignal::LSSignal(LSSignal *base):
   Time(pData[0]), 
   Freq(pData[1])
{
   pData[0]=pData[1]=NULL;
   n=base->n;
   full=base->full;  
   low=base->low;
   ASSERT(n);

   allocate();
   clear();
}

LSSignal::~LSSignal()
{
   free();
}

BOOL LSSignal::create(const LRange &range)
{
   ASSERT( range.cPoints > 0 && range.cPoints < 32768);
   ASSERT( range.Low >= 0 && range.Low <= 200000 );
   ASSERT( range.Full>= 0 && range.Full <= 200000 );
   free();
   n=range.cPoints*2;
   full=range.Full*2;  
   low=range.Low;
   ASSERT(n);

   allocate();
   clear();
      
   return TRUE; 
}      
     
BOOL LSSignal::allocate()
{  
   ASSERT( n>0 && n < 65536);
   ASSERT(!pData[0] && !pData[1]);
   pData[0] = new Complex[n*2];
   pData[1] = pData[0]+n;
   return TRUE;  
}


/////////////////////////////////////////////////////////////////////////////

void LSSignal::add(Complex what, int POS, int L)
{
   Complex * pSign=pData[L];
   pSign[POS] += what;
}

void LSSignal::add(float re,float im, int POS, int L)
{
   pData[L][POS].re += re;
   pData[L][POS].im += im;
}
/////////////////////////////////////////////////////////////////////////////

void LSSignal::free()
{         
   delete pData[0];
   pData[0] = pData[1] = NULL;
}  

UINT LSSignal::impulse(int type, float length, float delay, float level, float currfreq, float band, BOOL bAnalitic, BOOL bTransfere)
{  // 2- не дошел, 1- дошел хотя бы часть, 0- через все
   UINT   nRet = 2;
   float  fo;
   float  df_dt;
   float  dt=1.0f/full;
   float  t;
   float  freq;
   long   j=(long)(delay/dt);
   
   switch(type) {
   case TYPE_TONE:{                         fo=currfreq-low;        break;}   
   case TYPE_LFM: {df_dt=band/length;       fo=currfreq-band/2-low; break;}   
   case TYPE_HFM: {df_dt=band/length/length;fo=currfreq-band/2-low; break;}   
   default:break;
   } 
   
   for ( int i=max(0,j); i<n ;i++ ) {   
      t=(i-j)*dt;
      if ( t >= length ) {
         nRet &= 1;   // может кто был
         break;
      }

      freq=fo;                   

      { 
         switch(type) {
         case TYPE_TONE:{                 break;}   
         case TYPE_LFM: {freq+=df_dt*t;   break;}
         case TYPE_HFM: {freq+=df_dt*t*t; break;}   
         default: break;
         }         
         
         if( !qlt0(freq) && freq <= full/2 ) {
            float Re,Im;
            if (bAnalitic) {
               Re = (float)(level*fsin(M_2PI*freq*t));   
               Im = (float)(level*fcos(M_2PI*freq*t)); 
            } else {
               Re = level;
               Im = 0;
            }
            add(Re,Im,i,0);       

            nRet = 1;
         } 
      } 
   }         
   return nRet;
}

UINT LSSignal::fromACLmax(CMAcl *pAcl)
{  // максимизирует спектр по всему входящему ACLю return число шагов по аклю

   clear(1);
   float tt=0;
   LSSignal sg(this);
   int k = 0;
   while(!sg.fromAcl(pAcl,tt*1000,1,NULL )) 
   {
      tt+=n/full;
      for (int i=0; i<n ;i++ ) 
      {
         if( Freq[i].pow() < sg.Freq[i].pow() ) Freq[i]=sg.Freq[i];
      }
      sg.clear(0); // чистим время только, т.к. акль перезаписывает спектр
      k++;
   }
   return k;
}

UINT LSSignal::fromACLmaxR(CMAcl *pAcl,BOOL bTranslate,
      CMWave::TWRayAcl *o)
{  // максимизирует спектр по всему входящему ACLю 
   // только реальная часть спектра

   // может нужно выбрать максимальную пачку????
   clear(1);
   float tt=0;
   float step = n/full;
   LSSignal sg(this);
   int k = 0;
   while(!sg.fromAcl(pAcl,tt*1000.f,bTranslate,o))  
   {
      tt+=step;
      for (int i=0; i<n ;i++ ) 
      {
         float f = sg.Freq[i].pow();
         if( Freq[i].re < f ) Freq[i].re=f;
      }
      sg.clear(0); // чистим время только, т.к. fromacl перезаписывает спектр
      k++;
   }

   for ( int i=0; i<n ;i++ ) {
      Freq[i].re = fsqrt(Freq[i].re);
   }

   return k;
}

UINT LSSignal::corrACLaver(float *dd, float Delay,float Taver)
{
   float dt = n/full;
   float d = Delay - Taver;
   if ( d > dt ) return 1; // вышло Taver

   if ( qlt0(d)/* < 0*/ ) { // всё в Taver
      *dd = dt/Taver;
   } else { // часть в Taver
      *dd = (dt-d)/Taver;
   }

   return 0;   
}

void LSSignal::create(LSSignal * from)
{
   n=from->n;
   full=from->full;
   low=from->low;
   
   if ( n ) {   
      allocate();
      copy(from);
   }
}

void LSSignal::copy(LSSignal * from)
{
   copy(from,0);  
   copy(from,1);  
}

void LSSignal::copy(LSSignal * from, int L)
{
   Complex *pSign=pData[L];
   Complex *pFrom=from->pData[L];
   memcpy(pSign, pFrom, n*sizeof(Complex));
}

Complex LSSignal::corr(LSSignal  &with,int L)
{        // abs->Па^2
   mul(with,L);
   return summ(L);
}    

float LSSignal::corr_idx_inplace(LSSignal  &with,int L, int maxidx)
{  // умножение на сдвинутый на индекс сигнал для вычисленного из Доплера индекса
   // Меняет оригинальный сигнал! Нельзя вызывать в цикле!
   ASSERT(L==1);
   Complex * pWith=with.pData[L];
   Complex * pSign=pData[L];
   int j=maxidx;
   Complex Summ;
   Summ.re=Summ.im=0;
   for ( int i=0; i<n ;i++ ) {
      pSign[i] *= pWith[(i+j)%n];
      Summ += pSign[i];
   }
   return Summ.abs(); // Па^2
}

float LSSignal::corr_idx_copy(LSSignal  &with,int L, int maxidx)
{  // умножение на сдвинутый на индекс сигнал для вычисленного из Доплера индекса
   // Не меняет оригинальный сигнал - делает копию
   ASSERT(L==1);
   Complex * pWith=with.pData[L];
   LSSignal S(this);
   S.copy(this,1);
   Complex * pSign = S.pData[L];
   int j=maxidx;
   Complex Summ;
   Summ.re=Summ.im=0;
   for ( int i=0; i<n ;i++ ) {
      pSign[i] *= pWith[(i+j)%n];
      Summ += pSign[i];
   }
   return Summ.abs(); // Па^2
}

// Па^2
float LSSignal::corr_max(LSSignal  &with,int L,BOOL bMulSig, int &maxidx)
{  // ищет максимум от произведения сдвинутого спектра относительно базового
   // для когерентной обработки
   ASSERT(L==1);
   Complex * pWith=with.pData[L];
   Complex * pSign=pData[L];

   float lmax=0,f;
   int imax;
   Complex Summ;

   int j;
   for ( j=0; j<n ;j++ ) {
      Summ.re=Summ.im=0;
      for ( int i=0; i<n ;i++ ) {
         Summ += pSign[i]*pWith[(i+j)%n];
      }
      f = Summ.abs(); // Па^2
      if ( f > lmax ) {
         lmax = f;
         imax = j;
      }
   }

   if ( bMulSig ) {
      j=imax;
      for ( int i=0; i<n ;i++ ) {
        pSign[i] *= pWith[(i+j)%n];
      }
   }
   maxidx = imax;
   return lmax;
}

void LSSignal::mulpart(LSSignal &with, int startPos, int nPoints, int L)
{
   ASSERT(n == with.n);
   Complex * pWith=with.pData[L];
   Complex * pSign=pData[L];
   
   if(!nPoints||nPoints>n) nPoints=n;                 
   for(int i=0;i<nPoints;i++) 
      if(i+startPos<n)
         pSign[i+startPos]*=pWith[i];
      else break;
      
}

void LSSignal::mul(LSSignal &with, int L)
{
   ASSERT(n == with.n);
   Complex * pWith=with.pData[L];
   Complex * pSign=pData[L];
   for(int i=0;i<n;i++) pSign[i]*=pWith[i];
}

void LSSignal::mulR(LSSignal &with, int L)
{
   ASSERT(n == with.n);
   Complex * pWith=with.pData[L];
   Complex * pSign=pData[L];
   for(int i=0;i<n;i++) pSign[i]*=pWith[i].re;
}

void LSSignal::mul2(LSSignal & A, LSSignal & B, int L)
{
   ASSERT(n == A.n && n == B.n);
   Complex * pA=A.pData[L];
   Complex * pB=B.pData[L];
   Complex * pC=pData[L];
   
   for(int i=0;i<n;i++)
      pC[i]=pA[i]*pB[i];
   
}

void LSSignal::mul2CR(LSSignal & A, LSSignal & B, int L)
{  // умнож. A.complex на B.re
   ASSERT(n == A.n && n == B.n);
   Complex * pA=A.pData[L];
   Complex * pB=B.pData[L];
   Complex * pC=pData[L];
   
   for(int i=0;i<n;i++)
      pC[i]=pA[i]*(pB[i].re);
   
}

void LSSignal::mul(float t,int L)
{
   Complex * pC=pData[L];
   
   for(int i=0;i<n;i++)
      *pC++ *= t;
}

float LSSignal::power(int L)
{
   Complex * pSign=pData[L]; 
   
   float SummPower=0;
   
   for(int i=0;i<n;i++) 
      SummPower+= pSign[i].pow(); 
   
   if(!L)SummPower/=n; 

   return SummPower;   
}

float LSSignal::subpower(int L, int nIdxLo, int nIdxHi)
{  // мощность поддиапазона
   ASSERT(L==1); // только из спектра
   ASSERT(nIdxLo >= 0 && nIdxHi < n/2);
   // индексы поддиапазона
   float pow = 0;
   for ( int i=nIdxLo; i<=nIdxHi ;i++ ) {
      pow += Freq[i].pow();
      if ( i ) pow += Freq[n-i].pow();
   }
   return pow;
}

float LSSignal::subpowerR(int L, int nIdxLo, int nIdxHi)
{  // мощность поддиапазона из Re только!
   ASSERT(L==1); // только из спектра
   ASSERT(nIdxLo >= 0 && nIdxHi < n/2);
   // индексы поддиапазона

   float pow = 0;
   for ( int i=nIdxLo; i<=nIdxHi ;i++ ) {
      pow += Freq[i].re * Freq[i].re;
      if ( i ) pow += Freq[n-i].re*Freq[n-i].re;
   }
   return pow;
}

Complex LSSignal::summ(int L)
{
   Complex * pSign=pData[L]; 
   Complex Summ(0,0);

   for(int i=0;i<n;i++) Summ+=pSign[i]; 

   if ( !L ) Summ /= (float)(n); 

   return Summ;
}

float LSSignal::summR(int L)
{
   Complex * pSign=pData[L]; 
   float Summ = 0;

   for(int i=0;i<n;i++) Summ+=pSign[i].re; 

   if ( !L ) Summ /= (float)(n/*-pos*/); 

   return Summ;
}

Complex LSSignal::subsumm(int L, int nIdxLo, int nIdxHi)
{  // сумма поддиапазона
   Complex * pSign=pData[L]; 
   ASSERT(nIdxLo >= 0 && nIdxHi < n/2);

   Complex Summ(0,0);

   for ( int i=nIdxLo; i<=nIdxHi ;i++ ) {  
      Summ += pSign[i];
      if ( i ) Summ += pSign[n-i];
   }
   return Summ;
}

float LSSignal::subsummR(int L, int nIdxLo, int nIdxHi)
{  // сумма поддиапазона  из Re только!
   Complex * pSign=pData[L]; 
   ASSERT(nIdxLo >= 0 && nIdxHi < n/2);

   float Summ=0;

   for ( int i=nIdxLo; i<=nIdxHi ;i++ ) {
      Summ += pSign[i].re;
      if ( i ) Summ += pSign[n-i].re;
   }
   return Summ;
}


float LSSignal::idxpower(int idx, int L)
{
   ASSERT(L==1);
   float pow = Freq[idx].pow();
   if ( idx /* != 0 */ ) {
      pow += Freq[n-idx].pow();
   }
   return pow;
}

void LSSignal::clear()
{
   memset(pData[0],0,2*n*sizeof(Complex));
}

void LSSignal::clear(int L)
{
   memset(pData[L],0,n*sizeof(Complex));
}

void LSSignal::shift(float tay)
{
   Complex * pSign=pData[1];
   
   for(int i=0;i<n/2;i++) 
   {
      float freq=low+i*full/n;
      Complex delay(fcos(M_2PI*freq*tay),-fsin(M_2PI*freq*tay));
      pSign[i]*=delay;
      pSign[n-i-1]*=delay;
   }  
}

void LSSignal::mulconj(LSSignal * with, int startPos, int nPoints, int L)
{
   Complex * pWith=with->pData[L];
   Complex * pSign=pData[L];
   
   if(!nPoints||nPoints>n) nPoints=n;                 
   for(int i=0;i<nPoints;i++) {
      if(i+startPos<n)
         pSign[i+startPos]*=Complex(pWith[i].re,-pWith[i].im);
      else break;
   }
}

//переход из временной области в частотную или наоборот
void LSSignal::transfere(int from, int to)
{
   copy(from,to);
   FFT(to);
}

//скопировать временную область в частотную или наоборот
void LSSignal::copy(int from, int to)
{
   Complex * pFrom=pData[from];
   Complex * pTo=pData[to];
   memcpy(pTo, pFrom, n*sizeof(Complex));
}

//вычисление БПФ ( во временной или частотной области)
void LSSignal::FFT(int L)
{
   Complex * s=pData[L];    
   FFT2(s,L);
}   

void LSSignal::FFT2(Complex *s, int L)
{
   
   float PI=M_PI;
   
   int i=0;
   int j=0;
   int k=0;
   
   Complex r(1,0),W(1,0),w(1,0);
   
   //L- область в которую переход(0-временная,значит обратное БПФ
   //,1-в частотную,значит прямое)
   
   if(L) //т.е. переход в частотную область- прямое БПФ      
   {
      for(i=0;i<n;i++)
         s[i]/=(float)n;
      PI = -PI;
   } 
   j=0;
   for ( i=0; i<n-1 ; i++, j+=k ) {
      if ( i < j ) {
         r=s[j];
         s[j]=s[i];
         s[i]=r;
      }

      k=n/2; 
      
      while ( k <= j ) {
         j -= k;
         k /= 2;
      }
   } 


   int l=1;
   int p=0;
   int q=0;
   
   while ( l < n ) {
      p=l;
      l+=l;
      W=Complex(1.,0);
      w=Complex(fcos((float)(PI/p)),fsin((float)(PI/p)));
      for ( j=1; j<=p ;j++ ) {
         for ( i=j; i<=n ;i+=l ) {
            q=i+p;
            r=s[q-1]*W;
            s[q-1]=s[i-1]-r;
            s[i-1]+=r;
         }
         W *= w;
      }
   }
}

void LSSignal::xtransfere(int from, int to)
{
   copy(from,to);
   XFFT(to);
}
//вычисление БПФ ( во временной или частотной области) Y
void LSSignal::XFFT(int L)
{
   Complex * s=pData[L];    
   float PI=M_PI;
   
   int i=0;
   int j=0;
   int k=0;
   
   Complex r(1,0),W(1,0),w(1,0);
   
   //L- область в которую переход(0-временная,значит обратное БПФ
   //,1-в частотную,значит прямое)
   
   if(L) //т.е. переход в частотную область- прямое БПФ      
   {
      for(i=0;i<n;i++)
         s[i]/=(float)n;
      PI = -PI;
   } 
   j=0;
   for ( i=0; i<n-1 ; i++, j+=k ) {
      if ( i < j ) {
         s[j].swap(s[i]);
      }

      k=n/2; 
      
      while ( k <= j ) {
         j -= k;
         k /= 2;
      }
   } 


   int l=1;
   int p=0;
   int q=0;
   
   while ( l < n ) {
      p=l;
      l+=l;
      W=Complex(1.,0);
      w=Complex(fcos((float)(PI/p)),fsin((float)(PI/p)));
      for ( j=1; j<=p ;j++ ) {
         for ( i=j; i<=n ;i+=l ) {
            q=i+p;
            r=s[q-1]*W;
            s[q-1]=s[i-1]-r;
            s[i-1]+=r;
         }
         W *= w;
      }
   }

}

//вычисление сопряженного(временная и частотная область)
void LSSignal::conj()
{
   conj(0);
   conj(1);
}

//вычисление сопряженного(временная или частотная область)
void LSSignal::conj(int L)
{
   Complex *pSign=pData[L];
   for(int i=0;i<n;i++)  {
      pSign[i].im = -pSign[i].im;
   }
}

//добавить сигнал(временная или частотная область) с весом
void LSSignal::add(LSSignal * what, int L, Complex * weight, int K)
{
   Complex * pWhat=what->pData[L];
   Complex * pSign=pData[L];
   
   int j=0;          
      for(int i=0;i<n;i++)
      {
         Complex a(0,0);//Complex(0,0);
         if(weight) a=pWhat[i]*weight[j]; 
         else       a=pWhat[i];   
         
         pSign[i]+=a;
         j++;
         j%=K;  
      } 
}

//добавить сигнал (частотная область) с ХН как коэф. к pow Па^2
void LSSignal::add1hn(const LSSignal *what, float XH)
{
   const Complex * pWhat=what->pData[1];
   Complex * pSign=pData[1];
   
   float p = sqrt(XH);
   ASSERT(n == what->n);
   for ( int i=0; i<n ;i++) {
      pSign[i] += p*pWhat[i];
      /*
         re+re, im+im
         pow = re*re+im*im

      */
   } 
}

// вернуть мощность от произведения спектра на комплексно сопряженный
float LSSignal::mul1conjpow(const LSSignal * with)
{
   const Complex * pWith=with->pData[1];
   const Complex * pSign=pData[1];
   
   float p=0;
   for(int i=0;i<n; i++) {
      p += (pSign[i] * Complex(pWith[i].re,-pWith[i].im)).pow();
   }
   return p;
}



//добавить сигнал(временная или частотная область)
void LSSignal::add(LSSignal * what, int L)
{
   Complex * pWhat=what->pData[L];
   Complex * pSign=pData[L];
   
   for(int i=0;i<n;i++)
   {
      pSign[i]+=pWhat[i];
   } 
}

// сложить спектры сигналов по мощности - в реальную часть   Y.
void LSSignal::pow_add(LSSignal * what)
{
   Complex * pWhat=what->Freq;
   
   for ( int i=0; i<n ;i++ ) {
      Freq[i].re = fsqrt(Freq[i].pow() + pWhat[i].pow());   
      Freq[i].im = 0;
   } 

}

//добавить сигнал(временная и частотная область)
void LSSignal::add(LSSignal * what, Complex * weight, int K )
{                              
   add(what,0,weight,K);
   add(what,1,weight,K);
}

void LSSignal::SpreadPower(float power /* Па^2 */, int L)
{  // размазать мощность по отсчетам
   if ( L == 1 ) {
      ASSERT(n>2);
      float p = fsqrt(power/(n-2));
      for ( int i=0; i<n ;i++ ) {
         Freq[i] = p;
      }
      Freq[0] = Freq[n/2] = 0; // чтоб сумма idxpower равнялась power
   } else {
      ASSERT(0); //??? how?
   }

}

void LSSignal::SpreadPower_re_idx(float power /* Па^2 */, int L)
{  // размазать мощность по отсчетам в первую половину, в реальную часть Па2
   if ( L == 1 ) {
      clear(1);
      ASSERT(n>2);      //                   01234567 8 9 10 11 12 13 14 15
      float p = power*2/n;  // поделить на n/2
      for ( int i=0; i<n/2 ;i++ ) {
         Freq[i].re = p;
      }
   } else {
      ASSERT(0); //??? how?
   }
}

void LSSignal::SpreadPowerAdd(float power /* Па^2 */, int L)
{  // размазать мощность по отсчетам (суммировать с имеющимся)
   if ( L == 1 ) {
      ASSERT(n>2);
      float p = fsqrt(power/(n-2));
      for ( int i=0; i<n ;i++ ) {
         Freq[i] += p;
      }
      Freq[0] = Freq[n/2] = 0; // чтоб сумма idxpower равнялась power
   } else {
      ASSERT(0); //??? how?
   }

}

//####################################################################

LMSignal::LMSignal()
{
   m=0;
   pSign = NULL;
}

LMSignal::LMSignal(LRange &range,int M)
{
   pSign = NULL;
   Create(range,M);
}

void LMSignal::Create(LRange &range, int M)
{
   if ( pSign ) delete [] pSign;
   m = M;
   pSign = new LSSignal[M];
   for ( int i=0; i<M ;i++ ) {
      pSign[i].create(range);
   }
}

LSSignal & LMSignal::operator[](int idx)
{
   ASSERT(idx >= 0 && idx < m );
   ASSERT(pSign);
   return pSign[idx];
}

void LMSignal::Copy(LMSignal & src,int L)
{
   ASSERT(pSign);
   ASSERT(m == src.m);
   for ( int i=0; i<m ;i++ ) {
      pSign[i].copy(&src[i],L);
   }
}

LMSignal::~LMSignal()
{
   delete [] pSign;
}

void LMSignal::Clear(int L)
{  // чистит время(0) / спектр(1) у всех
   ASSERT(pSign);
   for ( int i=0; i<m ;i++ ) {
      pSign[i].clear(L);
   }
}

void LMSignal::Clear() // чистит и время и спектр у всех
{
   ASSERT(pSign);
   for ( int i=0; i<m ;i++ ) {
      pSign[i].clear();
   }
}


float interlocx02(float val,TFPoint *scale,UINT N, float *anom) 
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
         float yi = scale[i].y;
         float yi1 = scale[i+1].y;
         if ( anom ) {
            yi *= anom[i];
            yi1 *= anom[i+1];
         }
         return (yi1-yi)*
            (val-scale[i].x)/(scale[i+1].x-scale[i].x) + yi;
      }
   }  
   return 0;   
}

// возвращает мощность - Па^2
float LSSignal::fromWaveNoi (CMWave *w,    // из волны  TypeNOISE
                             BOOL bClear,  // очистить перед суммированием шумов
                             BOOL bWbn,    // делать из Wbn
                             BOOL bNbn,    // делать из Nbn
                             BOOL bIdxPow, // Freq[i].re = Па^2 для i=0..n/2 (половина)
                             BOOL bPow,    // Freq[i].re = Па^2 для всех (симметрично) 
                             BOOL bRandom, // делать шум как случайный процесс (Gauss)
                             // иначе только в реальную часть спектра
                             BOOL bUseRay, // использовать лучевку (множить на аномалию)
                             int nray )      // номер луча
{
   ASSERT(w->TypeNOISE());
   
   bUseRay = bUseRay && w->nRays;
   if ( bUseRay ) ASSERT( nray < w->nRays );
   
   float power = 0;
   if ( bClear ) {
      clear(1); // Freq
   } else {
      ASSERT( bPow || bIdxPow );
      // если не чистим, то суммирование должно быть в Па^2 !!!
   }
   
   float  dF =  full/n; // шаг по частоте
   
   CMWave::TWRayNoi *o;
   if ( bUseRay ) o = w->GetRayNoi(nray);
   
   // суммируем всё до кучи по мощности Па^2
   for ( int j=0; j<w->NoisesListLen ;j++ ) {
      if ( bWbn ) {
         TPatt2d *pWBN = w->NoisesList[j].pWbn;

         float *awbn = NULL;
         if ( bUseRay ) awbn = o->L[j][0];
         // в реальную часть спектра
         for ( int i=0; i < n/2 ;i++ ) {
            float freq=low+i*dF;   
            float Level = interlocx02 ( freq, pWBN->plane1, 44 , awbn );
            
            Level = Level*Level*dF; // Па^2
            // Па^2/Гц -> Па
            // если данные из сид - интегральный уровень Па
            // привести к полосе Па/v~Гц:
            if ( pWBN->dwFlags & LBF_PA ) Level /= (M_0_23*freq); // Па^2

            // Па^2
            power += Level;
            if ( i == 0 || bIdxPow ) {
               Freq[i].re += Level;
            } else {
               float re = Level/2;//M_SQRT2;
               Freq[i].re += re;
               Freq[n-i].re += re;
            }    
         } 
      } 
      if ( bNbn ) {
         TPatt2d *pNBN = w->NoisesList[j].pNbn;
         
         float *anbn;
         if ( bUseRay ) anbn = o->L[j][1];
         
         for( int i=0; i<pNBN->nBlicks ;i++) {
            
            float freq=pNBN->plane1[i].x-low;
            if ( qlt0(freq) ) continue;
            
            int k=(int)(freq/dF);
            if( /*k >= 0 &&*/ k < n/2 ) {
               float Level=pNBN->plane1[i].y;   // Па/v~Гц 
               
               if ( bUseRay ) Level *= anbn[i];
               
               Level = Level*Level*dF; // Па^2
               power += Level;
               if ( k == 0 || bIdxPow ) {
                  Freq[k].re += Level;
               } else {
                  float re = Level/2;//M_SQRT2;
                  Freq[k].re += re;
                  Freq[n-k].re += re;
               }
            }
         }
      }
   }
   
   if ( !bIdxPow && !bPow )  { // переходим к Па
      for ( int i=0; i < n ;i++ ) {
         Freq[i].re = fsqrt(Freq[i].re);
      }
   }
   
   if ( bRandom ) { 
      for ( int i=0; i<n ;i++ ) {
         Freq[i].im = Freq[i].re*gauss()/M_SQRT2;
         Freq[i].re *= gauss()/M_SQRT2;
      }
   }
   return power;
}

BOOL LSSignal::fromAcl(CMAcl * pAcl, // TRUE - можно удалять
                       float Delay,     //ms
                       BOOL bTranslate,
                       CMWave::TWRayAcl *o) 
{
   BOOL bCanDel = TRUE;

   float  dT=1.0f/full; // шаг по времени

   for ( int is=0; is<pAcl->nSignals ;is++ ) {
      float freq  =pAcl->pSignals[is].freq;           //Hz
      float band  =pAcl->pSignals[is].band;           //Hz
      float length = pAcl->pSignals[is].length/1000.f;// c
      int type = pAcl->pSignals[is].type;
      float  f0;
      float  df_dt;
      switch(type) {
      case TYPE_TONE:{                         f0=freq-low;        break;}   
      case TYPE_LFM: {df_dt=band/length/2;       f0=freq-band/2-low; break;}   
      case TYPE_HFM: {df_dt=band/length/length/2;f0=freq-band/2-low; break;}   
      case TYPE_NOISE: {                       f0=freq-band/2-low; break;}   
      default:
         ASSERT(0);
         continue; //по сигналам - другие не умеем
      } 
      if ( f0 > full/2 ) continue;
      for ( int ip=0; ip<pAcl->nPacks ;ip++ ) {
         float delay =pAcl->pSignals[is].delay + pAcl->pPacks[ip].Delay-Delay;//ms
         delay /= 1000.f; // c    от начала врем.выборки
         int ibeg; // начальный индекс во врем.выборки
         int iend; // конечный индекс во врем.выборки

         float level =pAcl->pSignals[is].level * pAcl->pPacks[ip].Level; //Pa
         if ( o ) level*=o->L[is];

         if ( type == TYPE_NOISE ) {
            clear(1); // используем спектр
            // используем индексы для индексации спектральной выборки
            ibeg = int(f0*n/full);
            if ( ibeg < 0 ) ibeg = 0;
            if ( ibeg >= n ) ibeg = n-1;
            iend = int((f0+band)*n/full);
            if ( iend < 0 ) continue;
            if ( iend >= n ) iend = n-1;
            float tl = level*fsqrt(full/n/band)/M_SQRT2;
            for ( int i=ibeg; i<=iend ;i++ ) {           //<= 4.12.2008
               Freq[i].re = tl*gauss();
               Freq[i].im = tl*gauss();
            }
            FFT2(Freq,0);  // во временную область
            // теперь во Freq - временные отсчеты
         }

         ibeg = int(delay/dT);
         if ( ibeg >= n ) {
            bCanDel = FALSE; // не нужно удалять волну
            continue;
         }
         iend = int((delay+length)/dT);
         if ( iend < 0 ) continue;
         int nbeg = ibeg;
         if ( ibeg < 0 ) ibeg = 0;
         if ( iend > n ) {
            iend = n;
            bCanDel = FALSE; // не нужно удалять волну - ещё что-то есть
         }
         if ( type == TYPE_NOISE ) {
            float tl=0;
            if (  ibeg < iend  ) { 
               for ( int i=ibeg; i<iend ;i++ ) tl += Freq[i].pow();
               if (  tl  ) {     
                  tl = fsqrt(level*level*(iend-ibeg)/tl);
                  for (  i=ibeg; i<iend ;i++ ) {
                     Time[i]+=Freq[i]*tl;
                  }
               }
            }
         } else {
            for ( int i=ibeg; i<iend ;i++ ) {
               float t = (i-nbeg)*dT; // время в сигнале
               float f=f0;
               if ( type == TYPE_LFM ) {
                  f += df_dt*t;
               } else if ( type == TYPE_HFM ) {
                  f += df_dt*t*t;
               }
               
             //  if( f >= 0 && f <= full/2 ) 
               {
                  f *= M_2PI*t;
                  Time[i].re += level*fsin(f);
                  Time[i].im += level*fcos(f);
               }
            }
         }
      }
   }

   if ( bTranslate ) transfere(0,1);

   return bCanDel;
}
    
BOOL LSSignal::fromWaveAcl (  // TRUE - можно удалять
                           CMWave *w,    // из волны  TypeACL
                           UINT Time,    // текущее время (шагом dT = n/full)
                           BOOL bClear,  // очистить вначале
                           BOOL bTranslate, // БПФ сделать
                           BOOL bRandom, // для типа3 делать шум как случайный процесс (Gauss) ???
                                         // иначе как тон
                           BOOL bUseRay, // использовать лучевку
                           int nray      // номер луча
                           )
{
   ASSERT(w->TypeACL());

   bUseRay = bUseRay && w->nRays;

   if ( bUseRay ) ASSERT( nray < w->nRays );

   CMWave::TWRayAcl *o = w->GetRayAcl(nray);

   if ( bClear ) clear(0); // Time
   
   float Delay = (float)Time - o->EnterTime - n*1000.f/full ; // ms от начала акля 

   return fromAcl(w->pAcl,Delay,bTranslate,bUseRay?o:0);
}

// максимизирует спектр по всему входящему ACLю 
// в реальную часть спектра
BOOL LSSignal::fromWaveAclMaxR(  // TRUE - можно удалять
                               CMWave *w,      // из волны  TypeACL
                               BOOL bRandom/*=FALSE*/,// для типа3 делать шум как случайный процесс (Gauss)
                               // иначе как тон
                               BOOL bUseRay/*=TRUE*/,// использовать лучевку
                               int nray/*=0 */  // номер луча
                               )
{
   ASSERT(w->TypeACL());

   bUseRay = bUseRay && w->nRays;

   if ( bUseRay ) ASSERT( nray < w->nRays );

   CMWave::TWRayAcl *o = bUseRay? w->GetRayAcl(nray) : NULL;
   /**/
   clear(1);
   float tt=0;
   float step = n/full;
   LSSignal sg(this);

   while(!sg.fromAcl(w->pAcl,tt*1000.f,1,o))
   {
      tt+=step;
      for (int i=0; i<n ;i++ ) 
      {
         float f = sg.Freq[i].pow();
         if( Freq[i].re < f ) Freq[i].re=f;
      }
      sg.clear(0); // чистим время только, т.к. fromacl перезаписывает спектр
   }

   for ( int i=0; i<n ;i++ ) {
      Freq[i].re = fsqrt(Freq[i].re);
   }

   return TRUE;
}


// присуммировать в сигнал максимальные мощности всего акля в реальную часть спектра
UINT LSSignal::fromWaveAclPowR(  // TRUE - можно удалять
                               CMWave *w,      // из волны  TypeACL
                               BOOL bUseRay/*=TRUE*/,// использовать лучевку
                               int nray/*=0 */  // номер луча
                               )
{
   ASSERT(w->TypeACL());
   
   bUseRay = bUseRay && w->nRays;
   
   if ( bUseRay ) ASSERT( nray < w->nRays );
   
   CMWave::TWRayAcl *o = bUseRay? w->GetRayAcl(nray) : NULL;
   /**/
   clear(1);
   float tt=0;
   float step = n/full;
   LSSignal sg(this);
   
   UINT k = 0;
   while(!sg.fromAcl(w->pAcl,tt*1000.f,1,o))
   {
      tt+=step;
      for (int i=0; i<n ;i++ ) 
      {
         float f = sg.Freq[i].pow();    // перезаписываем максимальный
         if( Freq[i].re < f ) Freq[i].re=f;
      }
      sg.clear(0); // чистим время только, т.к. fromacl перезаписывает спектр
      k++;
   }
   
   return k;
}








