// GakStructs.h: interface for the GakStructs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAKSTRUCTS_H__4F4B1D7A_0928_11D4_9C8F_0060970CEBA8__INCLUDED_)
#define AFX_GAKSTRUCTS_H__4F4B1D7A_0928_11D4_9C8F_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
////////////////////////////////////////////////////////////////////////////
#include "structures.h"
#include "TextArchive.h"

struct DETECT_REPORT
{
   BOOL bTraced;         // признак трассировки
   float t;              //время (задержка прихода),сек 
   DWORD nObjId;   //carrierid
   int nRClass;
   float dist;

   int nMaxFR;         // номер ЧД с максимальным С/П
   struct DetFR {
      BOOL  bfill;     //признак заполнения формуляра
      float teta;      //пеленг,град            
      float fi;        //вертикальный угол,град 
      float SNR;       //сигнал/помеха
      float SIG;       //сигнал
      float NOI;       //помеха
      float sigma;     // точность пеленгования 
   } FR[3];
};

// struct xDETECT_REPORT
// {
//    BOOL bTraced;         // признак трассировки
//    float t;              //время (задержка прихода),сек 
//    // дистанция = t/2*1.5f  км
//    BOOL  bfill;      //признак заполнения формуляра
//    float teta;       //пеленг,град            
//    float fi;         //вертикальный угол,град 
//    float SNR;        //сигнал/помеха
//    float SIG;        //сигнал
//    float NOI;        //помеха
// };

typedef CList <DETECT_REPORT, DETECT_REPORT&> CReportList;

struct TOGSDet {
   BOOL bTraced;
   float freq;
   float teta;
   float fi;
   float SNR;
   float level;// Па
   float noi;  // Па
   int   type; // тип посылки
   float len;  // длина посылки
   UINT  nRClass; // реальный класс
   float Rdist; // реальная дист. до объекта
   float band;  // частотная полоса
};

typedef CList<TOGSDet,TOGSDet&> COGSReportList;

struct TKLReport {
   float level;
   float teta;
   UINT  nClass;
   float dist;
};

typedef CList <TKLReport, TKLReport&> CKLReportList;

struct FAN // веер
{
   int   cPoints;
   float Low;        
   float Full;
   float dF() { return Full/(cPoints-1); }
   float operator [](int i) { 
      ASSERT(i>=0 && i<cPoints);
      return Low+i*Full/(cPoints-1); }
};

struct TTrassReport
{
   int  nNum;       // номер трассы
   float NumTime;   // абс время присвоения номера
   UINT nClass;     // класс объекта
   float ClassProb; // вероятность классификации
   float time;      // время последнего отсчета, мс
   float teta;      // ку
   float snr;       // osp
   float vis;       // ВИСигнала ОСПнов-ОСПРстар  0 нет данных == нет изменения
   float vip;       // ВИПеленга tetaNew-tetaOld (КУ) 0 нет данных
   float dtime;     // время последнего отсчета дистанции
   float dist;      // дистанция (ШП - реальная для отладки)
   float sigma;     // точность пеленгования 
};

struct TTrassReportBA
{
   int   nNum;      // номер объекта или 0 для множественных
   float NumTime;   // время
   UINT  nClass;    // 777 для БА
   float un1;       // -
   float time;      // время
   float teta;      // ку
   float SK0;       // погрешность вблизь, м
   float SK1;       // погрешность вдаль,  м, -1 = бесконечность
   float SKA;       // погрешность по углу в ширину,  м
   float dtime;     // время
   float dist;      // дистанция, м
   float PK;        // критерий определения дистанции 0-100
};

#include "DebugFlags.h"
////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_GAKSTRUCTS_H__4F4B1D7A_0928_11D4_9C8F_0060970CEBA8__INCLUDED_)
