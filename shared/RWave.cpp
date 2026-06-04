// RWave.cpp: implementation of the CRWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RWave.h"
#include "stru_rgab.h"
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRWave::CRWave()
{
   m_nMPD = 0;        // кол-во элементов в м-ве П Д m_MPD 
   m_pMPD = NULL;     // м-в П Д m_MPD 
	m_nObn = 0;        // кол-во обнаруженных каналов БМ
	m_nObnKAS = 0;     // кол-во обнаруженных каналов БМ
	m_nObnKASDav = 0;  // кол-во под-х каналов БМ
	m_nObnBUR = 0;     // кол-во обнаруженных каналов БМ
	m_nObnBURDav = 0;  // кол-во под-х каналов БМ
	m_nPod = 0;        // кол-во подавляемых каналов БМ
   m_pObn = NULL;     // м-в обнаруженных каналов БМ
	m_pObnTip = NULL;  // м-в типов обнаруженных каналов БМ
	m_pPod = NULL;     // м-в подавляемых каналов БМ
   m_pObnKAS = NULL;  // 
   m_pObnBUR = NULL;  // 
   Store0();
}

CRWave::CRWave(const CRWave & w)
{
    Store0();
    nSourceId  = w.nSourceId; 
    nPtNum     = w.nPtNum; 
    nClass     = w.nClass;
    InitTime = w.InitTime;
    X=w.X;          
    Y=w.Y;          
    Z=w.Z;          
    K=w.K;          
    V=w.V;          
    L=w.L;          
    D=w.D;          
    Planer = w.Planer;
    
    N_K=w.N_K;         // номер канала
    newN_K=w.newN_K;         // номер канала
    Power=w.Power;     // мощность
    ch = w.ch;         // глубина цели
    Tip=w.Tip;       // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН
    NK_BM[0] = w.NK_BM[0];
    NK_BM[1] = w.NK_BM[1];
    for ( int i=0; i<128; i++ ) 
    {
		MOSP[i]    = w.MOSP[i];
		MAC[i]     = w.MAC[i];
		MNB[i]     = w.MNB[i];
		if ( i < 32 ) MPELENG[i] = w.MPELENG[i];
    }
	
    m_nMPD = w.m_nMPD;                    // кол-во элементов в м-ве П Д m_MPD 
    m_pMPD = NULL;
    if ( m_nMPD > 0 )  
    {                                   
       m_pMPD = new TPD;
       m_pMPD[0].P = w.m_pMPD[0].P;       // 
       m_pMPD[0].D = w.m_pMPD[0].D;       // 
       
       m_pMPD[0].NN = w.m_pMPD[0].NN;     // 
       m_pMPD[0].nSourceId = w.m_pMPD[0].nSourceId;   // 
       m_pMPD[0].nClass = w.m_pMPD[0].nClass;         // 
       m_pMPD[0].lev = w.m_pMPD[0].lev;   // 
       m_pMPD[0].Tim = w.m_pMPD[0].Tim;   // 
       m_pMPD[0].ns  = w.m_pMPD[0].ns;    // 
       m_pMPD[0].N_K = w.m_pMPD[0].N_K;   // 
       m_pMPD[0].ch  = w.m_pMPD[0].ch;    // 
   }

   // ---
	Vedom = w.Vedom;     // 0-ведомый   1-ведущий 
	m_TimSinch = w.m_TimSinch;    // время синхронизации - до начала паузы
	m_nObn = w.m_nObn;            // кол-во обнаруженных каналов БМ
   if ( m_nObn > 0 )  
   {                          
      m_pObn    = new int[m_nObn];
      m_pObnTip = new int[m_nObn];
      for ( int i=0; i<m_nObn; i++ ) 
      {
         m_pObn[i]    = w.m_pObn[i];
         m_pObnTip[i] = w.m_pObnTip[i];
      }
      //       m_pObn    = w.m_pObn;          // м-в обнаруженных каналов БМ
      //       m_pObnTip = w.m_pObnTip;       // м-в типов обнаруженных каналов БМ
   }
   else
   {
      m_pObn = NULL;       // м-в обнаруженных каналов БМ
      m_pObnTip = NULL;    // м-в типов обнаруженных каналов БМ
   }

   m_nPod = w.m_nPod;      // кол-во подавляемых каналов БМ
   if ( m_nPod > 0 )  
   {                          
      m_pPod    = new int[m_nPod];
      for ( int i=0; i<m_nPod; i++ ) 
      {
         m_pPod[i]    = w.m_pPod[i];
      }
      //       m_pObn    = w.m_pObn;          // м-в подавляемых каналов БМ
      //       m_pObnTip = w.m_pObnTip;       // м-в типов подавляемых каналов БМ
   }
   else
   {
      m_pPod = NULL;       // м-в подавляемых каналов БМ
   }
  
   // ---   для кассеты
	m_nObnKAS = w.m_nObnKAS;   // кол-во обнаруженных каналов БМ
	m_nObnKASDav = w.m_nObnKASDav;   // кол-во обнаруженных каналов БМ
   if ( m_nObnKAS > 0 )  
   {                          
      m_pObnKAS    = new TObnKAS[m_nObnKAS];
      for ( int i=0; i<m_nObnKAS; i++ ) 
      {
         m_pObnKAS[i]    = w.m_pObnKAS[i];
      }
   }
   else
   {
      m_pObnKAS = NULL;       // м-в обнаруженных каналов БМ
   }
   // ---

   // ---   для нового бурака
	m_nObnBUR = w.m_nObnBUR;   // кол-во обнаруженных каналов БМ
	m_nObnBURDav = w.m_nObnBURDav;   // кол-во обнаруженных каналов БМ
   if ( m_nObnBUR > 0 )  
   {                          
      m_pObnBUR    = new TObnBUR[m_nObnBUR];
      for ( int i=0; i<m_nObnBUR; i++ ) 
      {
         m_pObnBUR[i]    = w.m_pObnBUR[i];
      }
   }
   else
   {
      m_pObnBUR = NULL;       // м-в обнаруженных каналов БМ
   }
   // ---

   DN = w.DN;            // коэф. усиления антенны излучателя дБ
   HAnt = w.HAnt;        // высота радиоантенны излучателя  м
   OSP_P = w.OSP_P;      // 
   OSP_A = w.OSP_A;      // 
   MaxOSP_P = w.OSP_P;   // 
   MaxOSP_A = w.OSP_A;   // 
   Pom = 0;
   Level = 0.f;
   dele = 0;
   m_TE = 0;     // mc
}

CRWave::~CRWave()
{
	if ( m_pMPD ) { delete m_pMPD; }
	if ( m_pObn ) { delete m_pObn; }
	if ( m_pObnTip ) { delete m_pObnTip; }
	if ( m_pPod ) { delete m_pPod; }
	if ( m_pObnBUR ) { delete m_pObnBUR; }
	if ( m_pObnKAS ) { delete m_pObnKAS; }
}

void CRWave::Store0()
{
   InitTime = 0;
   nClass = 0;
   Planer = 0;
   N_K   = 0;     // номер канала
   newN_K = -1;   // номер канала
   Power = 0;     // мощность
   ch = 0.0;      // глубина цели
   Tip= -1;       // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН
   ZeroMemory(&NK_BM,2*sizeof(int));
   ZeroMemory(&MOSP,128*sizeof(float));
   ZeroMemory(&MAC,128*sizeof(float));
   ZeroMemory(&MNB,128*sizeof(float));
   ZeroMemory(&MPELENG,32*sizeof(float));
   // ---
	Vedom = 0;              // 0-ведомый   1-ведущий 
   // 	m_nObn = 0;       // кол-во обнаруженных каналов БМ
   // 	m_pObn = NULL;    // м-в обнаруженных каналов БМ
   // 	m_nObnKAS = 0;    // кол-во обнаруженных каналов БМ
   // 	m_pObnKAS = NULL; // м-в обнаруженных каналов БМ
   // 	m_pObnTip = NULL; // м-в типов обнаруженных каналов БМ
   // 	m_nPod = 0;       // кол-во подавляемых каналов БМ
   // 	m_pPod = NULL;    // м-в подавляемых каналов БМ
	m_TimSinch = 0;         // время синхронизации - до начала паузы
   // ---

   DN = 0;           // коэф. усиления антенны излучателя дБ
   HAnt = 0;         // высота радиоантенны излучателя  м
	OSP_P = 0;        // 
	OSP_A = 0;        // 
	MaxOSP_P = 0;     // 
	MaxOSP_A = 0;     // 
	Pom = 0;
   Level = 0.f;
   dele = 0;
   m_TE = 0;

	nSourceId = 0;    // породил/отразил волну
   nPtNum = 0;       // номер точки ПП
	Tip = 0;          // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН 5-вертолет

   // ---  для Черешни
   RGBTipNK = 0;     // Тип обнаруженного канала РГБ
	RGBNK = 0;        // N обнаруженного канала РГБ
   RGBId = 0;        // Id РГБ
	RGBPower = 0;     // Пришедшая мощность на обнаруженном канале РГБ

   CherryId = 0;     // Id прибора
	CherryNK = 0;     // N обнаруженного канала Ч
	CherryTipNK = 0;  // Тип обнаруженного канала Ч
	CherryPower = 0;  // Пришедшая мощность на обнаруженном канале Ч
	CherryNPR = 0;    // N прибора Ч
   
	// Реальные K V из ситуации
   m_Kpl_Real = 0;   // Реальный К ПЛ
   m_Vpl_Real = 0;   // Реальная V ПЛ
   m_Xpl_Real = 0;   // Реальный X ПЛ
   m_Ypl_Real = 0;   // Реальная Y ПЛ
   m_Zpl_Real = 0;   // Реальная Z ПЛ
   m_Tpl_Real = 0;   // Реальная T ПЛ
   m_FLpl_Real = 0;  // 0-нет KV, 1-есть KV
}


void CRWave::Serialize(CArchive & ar)
{
    if ( ar.IsStoring() ) 
    {
        ar << nSourceId    // породил/отразил волну
            << nPtNum      // номер точки ПП
            // TMParams объекта от точки рождения волны
            << Tip         // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН
            << InitTime
            << nClass
            << X           // координата Х(широта)  ,км
            << Y           // координата Y(долгота) ,км
            << Z           // координата Z(глубина) ,м
            << K           // курс                  ,градусы
            << V           // скорость              ,узлы
            << L           // крен                  ,градусы
            << D           // дифферент             ,градусы   
            // Reflect
            << Planer      // команда от самолета - вертолет
            
            << N_K         // номер канала
            << newN_K      // новый номер канала
            << Power       // мощность
            << ch          // глубина цели
            << DN          // коэф.усиления радиоантенны
            << HAnt        // высота радиоантенны
            << OSP_P       // 
            << OSP_A       // 
            << MaxOSP_P    // 
            << MaxOSP_A    // 

            << Level       // 
            << Pom         // 
            << m_TE        // 
            << dele        // 
            << m_nMPD      // кол-во элементов в м-ве П Д m_MPD 
            << Vedom       // ведущий/ведомый
         	<< m_TimSinch  // время синхронизации - до начала паузы
            << m_nObn      // кол-во обнаруженных каналов
            << m_nObnKAS   // кол-во обнаруженных каналов
            << m_nObnKASDav // кол-во обнаруженных каналов
            << m_nObnBUR   // кол-во обнаруженных каналов
            << m_nObnBURDav // кол-во обнаруженных каналов
            << m_nPod      // кол-во подавляемых каналов

            << RGBTipNK    // ---  для Черешни   Тип обнаруженного канала РГБ
	         << RGBNK       // N обнаруженного канала РГБ
            << RGBId       // Id РГБ
	         << RGBPower    // Пришедшая мощность на обнаруженном канале РГБ
            << CherryId    // Id прибора
	         << CherryNK    // N обнаруженного канала Ч
	         << CherryTipNK // Тип обнаруженного канала Ч
	         << CherryPower; // Пришедшая мощность на обнаруженном канале Ч

//  	         << m_pMPD.P
// 	         << m_pMPD.NN
// 	         << m_pMPD.D
// 	         << m_pMPD.nSourceId
// 	         << m_pMPD.nClass
// 	         << m_pMPD.lev
// 	         << m_pMPD.Tim
// 	         << m_pMPD.ns
// 	         << m_pMPD.N_K
// 	         << m_pMPD.ch; 
        ar.Write(NK_BM, 2*sizeof(int));  // номера каналов для Черешня
        ar.Write(MOSP, 128*sizeof(float));  // передача ОСП
        ar.Write(MAC, 128*sizeof(float));  // передача сигналов
        ar.Write(MNB, 128*sizeof(float));  // передача шума
        ar.Write(MPELENG, 32*sizeof(float));  // передача ОСП пеленга
		
 		if ( m_nMPD ) 	ar.Write(m_pMPD, 1/*m_nMPD*/ * sizeof(TPD)); 
 		if ( m_nObn ) 	ar.Write(m_pObn, m_nObn * sizeof(int)); 
 		if ( m_nObnKAS ) 	ar.Write(m_pObnKAS, m_nObnKAS * sizeof(TObnKAS)); 
 		if ( m_nObnBUR ) 	ar.Write(m_pObnBUR, m_nObnBUR * sizeof(TObnBUR)); 
 		if ( m_nObn ) 	ar.Write(m_pObnTip, m_nObn * sizeof(int)); 
 		if ( m_nPod ) 	ar.Write(m_pPod, m_nPod * sizeof(int)); 
    }
    else
    {
		//      DWORD t;
		ar >> nSourceId  // породил/отразил волну
         >> nPtNum     // номер точки ПП
			// TMParams объекта от точки рождения волны
			>> Tip        // тип источника 0-БМ 1-ПНН 2-ПН 3-АНН 4-АН
			>> InitTime
			>> nClass
			>> X          // координата Х(широта)  ,км
			>> Y          // координата Y(долгота) ,км
			>> Z          // координата Z(глубина) ,м
			>> K          // курс                  ,градусы
			>> V          // скорость              ,узлы
			>> L          // крен                  ,градусы
			>> D          // дифферент             ,градусы   
			>> Planer     // команда от самолета - вертолет
			
			>> N_K        // номер канала
			>> newN_K     // новый номер канала
			>> Power      // мощность
			>> ch         // глубина цели
			>> DN         // коэф.усиления радиоантенны
			>> HAnt       // высота радиоантенны
			>> OSP_P           // 
			>> OSP_A           // 
			>> MaxOSP_P        // 
			>> MaxOSP_A        // 

			>> Level           // 
			>> Pom             // 
			>> m_TE            // 
			>> dele            // 
			>> m_nMPD          // кол-во элементов в м-ве П Д m_MPD 
         >> Vedom           // ведущий/ведомый
         >> m_TimSinch      // время синхронизации - до начала паузы
         >> m_nObn          // кол-во обнаруженных каналов
         >> m_nObnKAS       // кол-во обнаруженных каналов
         >> m_nObnKASDav    // кол-во обнаруженных каналов
         >> m_nObnBUR       // кол-во обнаруженных каналов
         >> m_nObnBURDav          // кол-во обнаруженных каналов
         >> m_nPod          // кол-во подавляемых каналов

         >> RGBTipNK        // ---  для Черешни   Тип обнаруженного канала РГБ
	      >> RGBNK           // N обнаруженного канала РГБ
         >> RGBId           // Id РГБ
	      >> RGBPower        // Пришедшая мощность на обнаруженном канале РГБ
         >> CherryId        // Id прибора
	      >> CherryNK        // N обнаруженного канала Ч
	      >> CherryTipNK     // Тип обнаруженного канала Ч
	      >> CherryPower;    // Пришедшая мощность на обнаруженном канале Ч
         
//ASSERT(DN<0);
//_asm wait;

//  	      >> m_pMPD.P
// 	      >> m_pMPD.NN
// 	      >> m_pMPD.D
// 	      >> m_pMPD.nSourceId
// 	      >> m_pMPD.nClass
// 	      >> m_pMPD.lev
// 	      >> m_pMPD.Tim
// 	      >> m_pMPD.ns
// 	      >> m_pMPD.N_K
// 	      >> m_pMPD.ch; 
		ar.Read(NK_BM, 2*sizeof(int));
		ar.Read(MOSP, 128*sizeof(int));
		ar.Read(MAC,  128*sizeof(float));  // передача сигналов
		ar.Read(MNB,  128*sizeof(float));  // передача шума
		ar.Read(MPELENG,   32*sizeof(float));  // передача ОСП пеленга
		

 		if ( m_nMPD ) 
 		{
 			//ASSERT(m_pMPD == NULL);
 			m_pMPD = new TPD[m_nMPD];
 			ar.Read(m_pMPD, 1/*m_nMPD*/*sizeof(TPD));
 		}

 		if ( m_nObn ) 
 		{
 			m_pObn = new int[m_nObn];
 			ar.Read(m_pObn, m_nObn*sizeof(int));
 			m_pObnTip = new int[m_nObn];
 			ar.Read(m_pObnTip, m_nObn*sizeof(int));
 		}
 		if ( m_nObnKAS ) 
 		{
 			m_pObnKAS = new TObnKAS[m_nObnKAS];
 			ar.Read(m_pObnKAS, m_nObnKAS*sizeof(TObnKAS));
 		}
 		if ( m_nObnBUR ) 
 		{
 			m_pObnBUR = new TObnBUR[m_nObnBUR];
 			ar.Read(m_pObnBUR, m_nObnBUR*sizeof(TObnBUR));
 		}
 		if ( m_nPod ) 
 		{
 			m_pPod = new int[m_nPod];
 			ar.Read(m_pPod, m_nPod*sizeof(int));
 		}
    
    }
}


