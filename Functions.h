// Functions.h: interface for the Love Functions.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTIONS_H__C6106009_D686_11D3_A3DD_D91261EEA670__INCLUDED_)
#define AFX_FUNCTIONS_H__C6106009_D686_11D3_A3DD_D91261EEA670__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <math2.h>
#define M_PI4		0.78539816339744830961
#define M_PI2    	1.57079632679489661923
#define M_3PI4		2.35619449019234492884
#define M_5PI4		3.92699081698724154807
#define M_3PI2		4.71238898038468985769
#define M_7PI4		5.49778714378213816730
#define M_25PI		7.85398163397448309615

#define M_PI3		1.04719755119659774615
#define M_2PI3		2.09439510239319549230


#define M_SQ2PI   2.50662827463100050241      // sqrt(2PI)
#define M_SQ2     1.41421356237309504880      // sqrt(2)


#define M_180PI  57.29577951308232087679
#define M_PI180   0.01745329251994329576

#define WL        0.00872664625997164788  // средняя угловая скорость при повороте

#define MaxSigmaDProgDisc 0.3

#define MAXTASKS 9

struct TGakForm {
   int  nNum;       // номер трассы
   int  nObj;       // номер объекта
   UINT nClass;     // класс объекта
   int  nClassOk;   // вероятность классификации
   double time;     // время последнего отсчета, мс
   double p;        // Пеленг
   double dp;       // Производная пеленга (изменение - ?)
   double d;        // Дистанция
   double dd;       // Производная дистанции (изменение - ?)
   float osp;       // Сигнал/Шум
   float vis;       // Величина изменения скорости
   float vip;       // Величина изменения пеленга
   BOOL bUse;
}; // формуляр от ГАК

struct TForm{
   double x;
   double y;
   double h;
   double k;
   double v;
   double kr;
   double dif;
}; // Carrier

struct TObjLocation{
   double x;
   double y;
   double h;
   double k;
   double v;
}; // Enemy

struct EForm{
   double t;     // текущее время
   double p;     // П    
   double d;     // Д    
   double k;     // К    
   double v;     // V     
   double h;     // Н    
   BOOL bloss;   // признак потери контакта
   BOOL bmove;   // признак изменения режима движения
   BOOL bkpd;    // признак наличия ЭДЦ
   double x;     // координаты объекта
   double y;
}; // формуляр объекта Outsider для хранения данных (БИУС)

struct TOutForm {
   double t;     // текущее время
   double t0;    // время обнаружения объекта
   double tpk;   // время потери контакта
   double p;     // П    
   double dp;    // СКО П
   double d;     // Д    
   double dd;    // СКО Д
   double k;     // К    
   double dk;    // СКО К
   double v;      // V     
   double dv;     // СКО V 
   double h;      // Н    
   double dh;     // СКО Н
   double vip;    // ВИП      
   double dvip;   // СКО ВИП  
   double vir;    // ВИР      
   double dvir;   // СКО ВИР  
   BOOL bkpd;      // признак наличия ЭДЦ
   BOOL bloss;     // признак потери контакта
   BOOL bmove;     // признак изменения режима движения
}; // формуляр объекта Outsider (БИУС)

struct TGAK{
   int Class;     // Класс цели
   double ppk;    // Рпк
   double t;      // текущее время
   double p;      // П
   double dp;     // СКО П
   double d;      // Д           
   double dd;     // СКО Д       
   double h;      // Н           
   double dh;     // СКО Н       
   BOOL bloss;     // признак потери контакта
   BOOL bmove;     // признак изменения режима движения
}; // формуляр объекта ГАК

struct TNewData{
   int Class;     // Класс цели
   double ppk;    // Рпк
   double t;      // текущее время
   double p;      // П
   double dp;     // СКО П
   double d;      // Д           
   double dd;     // СКО Д       
   double h;      // Н           
   double dh;     // СКО Н       
   double k;      // K           
   double dk;     // СКО K       
   double v;      // V           
   double dv;     // СКО V       
   BOOL bloss;     // признак потери контакта
   BOOL bmove;     // признак изменения режима движения
}; // новые данные от источников

struct TSudCommand{
   int Prior;     // приоритет объекта (зависит от класса)
   BOOL bChange;  // признак изменения
   double K;      // К маневра
   double V;      // V маневра
   double H;      // Н маневра
   double D;      // Д маневра
   double T;      // Т маневра
}; // структура данных для управления носителем

struct TTask {
   LPCTSTR sName;
   BOOL bStatus;  // изменение статуса
   int Status;
};

struct TProblemStatus{
// BOOL: 0 - No change, 1 - int cnanged
// int: 0 - не включена, 1 - включена, 2 - работает, 3 - есть результаты 
   TTask TTasks[MAXTASKS]; // массив задач
/* 0 - N пеленгов
   1 - Фильтр калмана
   2 - дистанция обнаружения противника
   3 - прогноз обнаружения
   4 - уклонение
   5 - занятие позиции
   6 - изменение позиции
   7 - расчет ОВПС
   8 - атака по пеленгу
*/
   BOOL Change; // признак изменения статуса
   BOOL bASC;  // автосопровождение
   int DistDis;   // дистанция обнаружения противника
   BOOL bDistDis;
   int ProgDis;   // прогноз обнаружения
   BOOL bProgDis;
   int Avoid;   // уклонение
   BOOL bAvoid; 
   int OcupPos;   // занятие позиции
   BOOL bOcupPos; 
};

struct TCoord {
   double x;
   double y;  
};

struct TZalp {
   int npu; // кол-во ТА, ПУ
   int bz;  // тип БЗ
   int tube[6]; // номер ПУ
};

struct TDTForm {
   double w;     // угол первого отворота
   double a;     // угол разведения
   double Draz;  // дистанция разведения
   double Dssn;  // дальность действия ССН
   double Dmar;  // дальность маршевого участка (включения ССН)
   double Hmar;  // глубина маршевого участка 
   double H;     // глубина поиска
   double Hot;   // глубина ограничения верха
   double Hob;   // глубина ограничения низа
   double Dp;    // дистанция поиска по прямой
   BOOL bLeft;   // знак циркуляции после поиска по прямой
};

struct TDTChar {
   LPCTSTR sName; // тип торпеды
   double D0;    // дистанция участка выхода
   double Dr;    // дистанция от выхода на курс до начала разведения
   double Dmax;  // максимальная дальность хода
   double Dmin;  // минимальная дальность применения
   double Drmin; // минимальная дальность разведения
   double Dssnmin; // минимальная дальность CCH
   double Dssnmax; // максимальная дальность CCH
   double Hmax;    // максимальная глубина применения
   double Vmar;    // маршевая скорость
   double D2s[2];  // дистанция вторичного поиска (ПЛ, НК)
   double om;      // угловая скорость рад/с
};

extern LPCTSTR Status[];

void NormAngle(double& Angle);
void NormKU(double& Angle);
double Sea2Math(double Angle);
void DoubleValueOut(CString& s, int Sdr, double value);
void DoubleValueOutS(CString& s, int Sdr, double value);
void TimeValueOut(CString& s, double t);
double ChangeAngle(double an, double an1, double an2);
double Laplas(double x);
void ClearTGak( TGAK& f);
void TGak2TNewData( TGAK g, TNewData& f);
void ClearTSudCommand( TSudCommand& f);
void ClearTProblemStatus( TProblemStatus& f);
void TEformNoDataCopy( EForm& d, EForm& s);
double MinArc(double k1, double k2); // минимальный угол поворота с к1 на к2
double KU(double p, double k);

#endif // !defined(AFX_FUNCTIONS_H__C6106009_D686_11D3_A3DD_D91261EEA670__INCLUDED_)
           
