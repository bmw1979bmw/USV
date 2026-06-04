// RWave.h: interface for the CRWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWAVE_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_)
#define AFX_RWAVE_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stru_rgab.h"
//#include "LNSignals.h"

struct TObnBUR
{   // копия в RWave
    int    NK;      
    int    Tip;       
    int    Rgab;   // обнаружение данным прибором
    float  Dist; // дистанция до РГБ
    float  Power; // полученная мощность

     float  P;      
     float  D;      
//     int    Obn;    // есть обнаружение по ГА    
//     float  Lev;    // уровень подавляемого РС
     float  M;        // мощность

    // Обнаружение 2-м
    int    BRgab;    // подавление 2-м прибором
    float  BDistObn; // дистанция до РГБ
    int    NPrO;      // номер прибора

    // Подавление 2-м
    int    BPod;     // подавление 2-м прибором
    float  BDistPod; // дистанция до РГБ
    int    NPrP;      // номер прибора

//     float  BLev; // уровень подавляемого РС 2-м прибором
//     int    BObn; // обнаружение 2-м прибором без подавления 
};
 
struct TObnCHERRY
{   // копия в RWave
    int    NK;      
    int    Tip;       
    int    Rgab;      // обнаружение данным прибором
//    float  Dist;      // дистанция до РГБ
    float  Power;     // полученная мощность
    float  M;         // мощность
    // Подавление 2-м
    int    BPod;      // подавление 2-м прибором

    int    SourceId; // Id

    int    RGBTipNK;  // Тип обнаруженного канала РГБ
	 int    RGBNK;     // N обнаруженного канала РГБ
    int    RGBId;     // Id РГБ
    float  RGBPower; // Пришедшая мощность на обнаруженном канале РГБ

    int    CherryTipNK;  // Тип обнаруженного канала Ч
	 int    CherryNK;     // N обнаруженного канала Ч
    int    CherryId;     // Id прибора Ч
	 float  CherryPower;  // Пришедшая мощность на обнаруженном канале Ч
};
 
struct TObnRATOBORA
{   // копия в RWave
    int    NK;      
    int    Tip;       
    int    Rgab;      // обнаружение данным прибором
//    float  Dist;      // дистанция до РГБ
    float  Power;     // полученная мощность
    float  M;         // мощность
    // Подавление 2-м
    int    BPod;      // подавление 2-м прибором

    int    SourceId;  // Id

    int    RGBTipNK;  // Тип обнаруженного канала РГБ
	 int    RGBNK;     // N обнаруженного канала РГБ
    int    RGBId;     // Id РГБ
    float  RGBPower; // Пришедшая мощность на обнаруженном канале РГБ

    int    CherryTipNK;  // Тип обнаруженного канала Ч
	 int    CherryNK;     // N обнаруженного канала Ч
    int    CherryId;     // Id прибора Ч
	 float  CherryPower;  // Пришедшая мощность на обнаруженном канале Ч
	 int    CherryNPR;     // N прибора Ч
};

struct TObnKAS
{   // копия в RWave
    int    NK;      
    int    Tip;       
    int    Rgab;   // обнаружение данным прибором
    float  Dist; // дистанция до РГБ

     float  P;      
     float  D;      
//     int    Obn;    // есть обнаружение по ГА    
//     float  Lev;    // уровень подавляемого РС

    // Обнаружение 2-м
    int    BRgab; // подавление 2-м прибором
    float  BDistObn; // дистанция до РГБ

    // Подавление 2-м
    int    BPod; // подавление 2-м прибором
    float  BDistPod; // дистанция до РГБ

//     float  BLev; // уровень подавляемого РС 2-м прибором
//     int    BObn; // обнаружение 2-м прибором без подавления 

};
 


class CRWave : public TMParams // TMParams объекта от точки рождения волны:
{
	int operator =(const CRWave &); // no implementation
public:
	
public:
	void Serialize(CArchive &ar);
	CRWave();
	CRWave(const CRWave &w);
	virtual ~CRWave();
   void Store0();
	
	//////////////////////////////////////////////////////////////////
	UINT     nSourceId;  // породил/отразил волну
   int      nPtNum;     // номер точки ПП
	UINT     Tip;       // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН 5-вертолет
	UINT     nClass;     // класс источника
	UINT     InitTime;   // время рождения волны, мс
	int      Planer;     // служебные команды от вертолета активному РГАБ
	// 100 - акт. посылка    101 - смена канала   102 - доразмотка троса
	int      N_K;        // номер канала РГАБ ( для посылки и приема РВ )
	int      newN_K;     // для смены номера канала у акт. РГАБ  или № Черешня
	float    Power;      // мощность
	float    ch;         // глубина цели
	// номера каналов для Черешня
	int      NK_BM[2]; // номера каналов для Черешня
	
	float    MAC[128];   // передача сигналов
	float    MNB[128];   // передача шумов
	int      m_nMPD;     // кол-во элементов в м-ве П Д m_MPD 
	TPD     *m_pMPD;     // м-в П Д m_MPD 


   // --- для синхронизации
	int      Vedom;      // 0-ведомый   1-ведущий 
   UINT     m_TimSinch; // время синхронизации - до начала паузы
	int      m_nObn;     // кол-во обнаруженных каналов БМ
	int     *m_pObn;     // м-в обнаруженных каналов БМ
	int     *m_pObnTip;  // м-в типов обнаруженных каналов БМ
	int      m_nPod;     // кол-во подавляемых каналов БМ
	int     *m_pPod;     // м-в подавляемых каналов БМ

   // ---   для кассеты
	int      m_nObnKASDav;    // число подавляемых
	int      m_nObnKAS;    // м-в обнаруженных каналов БМ
	TObnKAS     *m_pObnKAS;    // м-в типов обнаруженных каналов БМ
   // ---

   // ---  для нового Бурака
	int      m_nObnBURDav;    // число подавляемых
	int      m_nObnBUR;    // м-в обнаруженных каналов БМ
	TObnBUR  *m_pObnBUR;    // м-в типов обнаруженных каналов БМ
   // ---

	float    MOSP[128];      // передача ОСП
	float    MPELENG[32];   // передача пеленга
	
	float    DN;          // коэф. усиления антенны излучателя дБ
	float    HAnt;        // высота радиоантенны излучателя  м
	float    OSP_P;       // порог ОSP
	float    OSP_A;       // порог ОSP
	float    MaxOSP_P;       // текущее ОСП
	float    MaxOSP_A;       // текущее ОСП

	float    Level;      // уровень принятого сигнала для БМ
	int      Pom;        // признак помехи

	int      dele;       // служебный
	UINT     m_TE;       // единое время для синхронизации Кассета, мс
                        // для 101 и Poseidon - время прихода луча 125 в 101 и передача в PS

   // ---  для Черешни
   int      RGBTipNK;  // Тип обнаруженного канала РГБ
	int      RGBNK;     // N обнаруженного канала РГБ
   int      RGBId;     // Id РГБ
	float    RGBPower;  // Пришедшая мощность на обнаруженном канале РГБ

   int      CherryId;     // Id прибора
	int      CherryNK;     // N обнаруженного канала Ч
	int      CherryTipNK;  // Тип обнаруженного канала Ч
	float    CherryPower;  // Пришедшая мощность на обнаруженном канале Ч
	int      CherryNPR;     // N прибора Ч
   
	// Реальные K V из ситуации
   float    m_Kpl_Real;     // Реальный К ПЛ
   float    m_Vpl_Real;     // Реальная V ПЛ
   float    m_Xpl_Real;     // Реальный X ПЛ
   float    m_Ypl_Real;     // Реальная Y ПЛ
   float    m_Zpl_Real;     // Реальная Z ПЛ
   float    m_Tpl_Real;     // Реальная T ПЛ
   int      m_FLpl_Real;    // 0-нет KV, 1-есть KV

//	float    Dist;      // дистанция Ч - РГБ

	// диаграмма излучения /опциональная/ ! по горизонтали
	//   int m_nPattLen;
	// TFPoint *m_pPatt;   
	// x - модуль угла, град; y - коэффициент к Па 0..1 при угле меньше x
	//   void CreatePattern(int nlen);
	
};


typedef CTypedPtrList<CPtrList, CRWave *> CRWaveList;


#endif // !defined(AFX_RWAVE_H__02873BD5_E8F7_11D3_9C6A_0060970CEBA8__INCLUDED_)
