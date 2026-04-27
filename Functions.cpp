// Functions.cpp: implementation of the Love Functions.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Functions.h"


LPCTSTR Status[] = { _T("Выключена"),
                     _T("Включена"),
                     _T("Работает"),
                     _T("Есть результаты"),
                     _T("Остановлена"),
                     _T("Необходим маневр"),
                     _T("Однократный расчет"),
                     _T("Завершен однократный расчет"),
};

double Sea2Math(double Angle)
{  // пересчет из математической в морскую системы координат
	double A;
	A = M_25PI-Angle;
	if(A>=M_2PI) return A-M_2PI;
	return A;
}

void DoubleValueOut(CString& s, int Sdr, double value)
{  // вывод Double без знака

   if ( value < 0. ) {
      s = _T("?");
   } else {
      switch ( Sdr ) {
      case 0: 
         s.Format("%.0f", value);
      	break;
      case 1:
         s.Format("%.1f", value);
      	break;
      case 2:
         s.Format("%.2f", value);
      	break;
      case 3:
         s.Format("%.3f", value);
      	break;
      default: 
         s.Format("%g", value);
      }
   }           
}

void DoubleValueOutS(CString& s, int Sdr, double value )
{  // вывод Double со знаком
   switch ( Sdr ) {
   case 0: 
      s.Format("%.0f", value);
      break;
   case 1:
      s.Format("%.1f", value);
      break;
   case 2:
      s.Format("%.2f", value);
      break;
   case 3:
      s.Format("%.3f", value);
      break;
   case 4:
      s.Format("%.4f", value);
      break;
   case 5:
      s.Format("%.5f", value);
      break;
   default: 
      s.Format("%f", value);
   }
}

void TimeValueOut(CString& s, double t)
{  // вывод времени чч:мм:с
   if ( t > 0. ) {
      if ( t <= 60. ) {
         if ( t < 10. ) {
            s.Format("00:00:0%1d", (int)t);
         } else {
            s.Format("00:00:%2d", (int)t);
         }
      } else {
         int m, h;
         if ( t <= 3600. ) {
            m = (int)(t/60.);
            s.Format("00:%2d:%2d", m, (int)(t - (double)m * 60.) );
         } else {
            h = (int)(t/3600.);
            m = (int)( (t - (double)h * 3600.) / 60.);
            s.Format("%2d:%2d:%2d", h, m, 
               (int)( t - (double)h * 3600. - (double)m * 60.) );
         }
      }
   }
}
               
double ChangeAngle(double an, double an1, double an2)
{  // определение min угла и направления поворота с an на an1,an2
   double ang1, ang2;
   ang1 = an1;
   ang2 = an2;

   if ( an > M_PI ) an = M_PI - an;
   if ( an1 > M_PI ) an1 = M_PI - an1;
   if ( an2 > M_PI ) an2 = M_PI - an2;

   an1 -= an;
   an2 -= an;

   if ( fabs(an1) <= fabs(an2) ) {
      return ang1;
   } else {
      return ang2;
   }
}

double Laplas(double x)
{
   double a;
   float f[3891]; 
//       = { 
// #include "laplas.dat"
//    };

   int n = (int)fabs(1000. * x);
   (n >= 3891) ? a = 0.5 : a = f[n];
   if ( x < 0 ) return 0.5 - a;
   else return 0.5 + a;
}

void NormAngle(double& Angle)
{
   _asm wait;
   while ( Angle >= M_2PI ) Angle -= M_2PI;
   while ( Angle < 0. ) Angle += M_2PI;
}

void NormKU(double& Angle)
{
   if ( fabs(Angle) <= M_PI ) {
      return;
   } else {
      while( fabs(Angle) >= M_PI ) {
         if ( Angle < 0 ) {
            Angle += M_2PI;
         } else {
            Angle -= M_2PI;
         }
      }
   }
}

void ClearTGak( TGAK& f)
{
   f.Class = 0;     // Класс цели
   f.ppk = -1.;    // Рпк
   f.t = -1.;      // текущее время
   f.p = -1.;
   f.dp = -1.;
   f.d = -1.;
   f.dd = -1.;
   f.h = -1.;
   f.dh = -1.;
   f.bloss = FALSE;     // признак потери контакта
   f.bmove = FALSE;     // признак изменения режима движения
}

void TGak2TNewData( TGAK f, TNewData& g)
{
   g.t = f.t;                      
   g.p = f.p;                          
   g.d = f.d;                          
   g.k = -1.;                          
   g.v = -1.;                          
   g.h = f.h;                          
   g.dp = f.dp;                          
   g.dd = f.dd;                          
   g.dk = -1.;                          
   g.dv = -1.;                          
   g.dh = f.dh;                          
   g.bloss = f.bloss;                    
   g.bmove = f.bmove;
   g.Class = f.Class;
   g.ppk = f.ppk;
}             

void ClearTSudCommand( TSudCommand& f)
{
   f.Prior = -1;
   f.bChange = FALSE;
   f.K = -1.;
   f.V = -1.;
   f.H = -1.;
   f.D = -1.;
   f.T = -1.;
}
    
/* классы объектов
0 - неклассифицировано
10 - НК
20 - ПЛ
30 - мина
40 - оружие
*/
void TEformNoDataCopy( EForm& d, EForm& s)
{
   d.t = -1.;                      
   d.p = -1.;                          
   d.d = -1.;                          
   d.k = -1.;                          
   d.v = -1.;                          
   d.h = -1.;                          
   d.bloss = TRUE;                    
   d.bmove = s.bmove;                    
   d.bkpd = 0;//FALSE;                      
   d.x = -1.;                          
   d.y = -1.;
}             
    
void ClearTProblemStatus( TProblemStatus& f)
{
   f.Change = FALSE;
   f.bASC = FALSE;
   f.ProgDis = 0;   
   f.bProgDis = FALSE;
   f.Avoid = 0;  
   f.bAvoid = FALSE;
   f.DistDis = 0;   
   f.bDistDis = FALSE;
   f.OcupPos = 0;   
   f.bOcupPos = FALSE; 
   
}

double MinArc(double k1, double k2)
{
   double dk;
   if ( k1 > M_PI ) k1 -= M_2PI;
   if ( k2 > M_PI ) k2 -= M_2PI;
   dk = k1 - k2;
   if ( dk > M_PI ) return M_2PI - dk;
   if ( dk < -M_PI ) return M_2PI + dk;
   return dk;
}

void AFXAPI AfxSetWindowText(HWND hWndCtrl, LPCTSTR lpszNew);

void AFXAPI DDX_TextId(CDataExchange* pDX, int nIDC, UINT nId)
{
	if (pDX->m_bSaveAndValidate) return;

   CString s;
   if ( !s.LoadString(nId) ) return;

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);

	AfxSetWindowText(hWndCtrl, s);
	
}

double KU(double p, double k)
{
   double q = p - k;
   q = fmod(double(q + M_4PI), double(M_2PI));
   if ( q > M_PI ) q -= M_2PI;
   return q;
}

