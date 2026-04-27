// MainDoc.h : interface of the CMainDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDOC_H__366DCE9B_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
#define AFX_MAINDOC_H__366DCE9B_1140_11D5_B73F_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GakStructs.h"

struct TFormularUSV // 
{
         BOOL bUpr;
         float     KURS_CMD ;  //float заданный  курс
         float     A_ZACLAD ;//float курс угол  закладки  рулей
         int     SU_KURS  ;//int тип управления по курсу
         int       ZIRC;//int направл циркуляции 1 вправо ,др-влево
         UINT      T_CMD_K;
         float     SPEED_CMD; ////float скорость
         int       SU_SPEED; //;//int тип управления по  скорости
         UINT      T_CMD_V;
         BOOL bOtn;//признак относительности курса
          int  Type_Logic;
    float     SPEED_PR_CMD;//DIFF_CMD;  //float глуб - дифферент
    float     SPEED_DG_CMD;//DIFF_CMD;  //float глуб - дифферент

BOOL bDemo;  //флаг демонстрации ИД

FI_BEGIN 
   FI_VAR(BOOL,  "признак наличия управления",0)
   FI_VAR(float,"заданный  курс,°",0)
   FI_VAR(float,"угол  закладки  рулей,°",0)
   FI_VAR(int,  "тип управления по курсу",0)
   FI_VAR(int,  "направление циркуляции 1 вправо ,др-влево",0)
   FI_VAR(int,  "время подачи команды изменения курса,mc",0)

   FI_VAR(float,"скорость 1,уз",0)
   FI_VAR(int,  "тип управления по скорости 1",0)
   FI_VAR(int,  "время подачи команды изменения скорости 1,мс",0)
   FI_VAR(BOOL,  "признак относительности управления по курсу",0)
    FI_VAR(BOOL,  "тип действий 0 - нет 1 -прорыв 2- слежение ",0)
    FI_VAR(float,"скорость при прорыве,уз",0)
    FI_VAR(float,"скорость преследования,уз",0)

   FI_VAR(BOOL,  "диалоги ИД",0)

   
   
   FI_COMMAND("БЭНК: курс",'KURS')
      FI_VAR(float ,"заданный  курс",0)
      FI_VAR(float ,"угол  закладки  рулей",0)
      FI_VAR(int ,"тип управления по курсу",0)
      FI_VAR(int ,"напр. циркуляции: 1 - вправо, 0 - влево",0)
      FI_VAR(BOOL ,"относительность курса",0)
   FI_COMMAND("БЭНК: скорость",'SPD')
      FI_VAR(float ,"скорость",0)
      FI_VAR(int   ,"тип управления по скорости",0)



   FI_COMMAND("БЭНК: начать прорыв (1), преследование (2)",'PRV')
    FI_VAR(BOOL,  "тип действий: 0 - нет, 1 - прорыв, 2 - слежение",0)
    FI_VAR(float ,"скорость",0)
   FI_END;






};

class CMainDoc : public CDocument
{
protected: // create from serialization only
   CMainDoc();
   DECLARE_DYNCREATE(CMainDoc)
   TTrassReport *m_pTR;
   int m_nTrassReport;

         struct TFM {
            float Y;
            float p;
      float dp;
      float sp; // сигнал/помеха
      CString sName; // имя объекта
      UINT nNum; // номер объекта
   } *m_pTFM;
   UINT m_nTFM; //  m_nTFM не может быть отрицательной (кол.-во эл.-тов массива), д.б. беззнаковой (UINT)


// Attributes
public:


   BOOL b_PGN;  //признак преследования
   BOOL b_1Contact_PL;
   BOOL b_1Contact_OPL;
   BOOL b_END_OPL;//признак окончания работы всех помех
    UINT Time_Out_Contact;
   float SPEED_SEARCH,SPEED_DGN;
   BOOL bSetPror;
   UINT time_observ;
   TXParams m_En;// параметры движения цели << World
   float KURS_NULL;   
   int Time_Reset;
//   float SPEED_SEARCH;
   BOOL b_Class_VP;
   int type_moving;
   
   
   BOOL bCMD_K;
   BOOL bCMD_V ;

   float ostatok;
static const float ANGLE_EPSILON; // Допуск сравнения углов в градусах (0.01° - ок. 0.6 мили на 100 км)
   float time_speed_kurs_cmd;
   float d_SPEED_KURS;
   float FIX_DV;       //возможное изменени 
   float delta_speed;
   float delta_speed_r;
   float  dV0;
   float   dV0MAX;
   float dv;
   float usk0;
   float k0;
   BOOL f_new_cmd;
   float T_S;

  float KURS_TEC;
  float SPEED_KURS_CMD;
    /////////управляющее  воздействие/////////////////////////
float A_ZACLAD ;           //курс угол  закладки  рулей
float KURS_CMD ;        //заданный  курс
float SPEED_CMD;        //скорость
int SU_KURS;             //тип управления по курсу
int SU_SPEED;            //тип управления по  скорости
int ZIRC;                //направл циркуляции 1 вправо ,др-влево
//////////////////данные  по  проекту///////////////

int CLASS_PL;            //класс(проект) ПЛ 1- 885 2- 971 3- ЛА  4-СВ
float SPEED_MAX;     //максимальная  скорость ПЛ
float SPEED_MIN;     //минимальная скорость ПЛ


/////////////////////////////////////////////////////////////
/////////////////////// КУРС ////////////////////////////////
/////////////////////////////////////////////////////////////
///////////////////коэффициенты  по  курсу////////////

const static struct KURS_CLASS_PL
{
float K_S_1_KURS[2];
float K_S_2_KURS[2];
float K_S_3_KURS[2];
float K_TG_1_KURS[2];
float K_TG_2_KURS[2];
float K1_KURS[2];
float K2_KURS[2];
float K3_KURS[2];
float RK1,RK2,RK3;
}  kk_kurs;


float D_TIME;           //приращение  времени
//float TIME;             //время

//////////////////////приращиение  курса/////////////////////

float KURS_UP;          //текущее приращение  курса
float KURS_UP_EXIT;     //текущее приращение  курса  расчета  момента  автовыхода
float KURS_EXIT;        //курс в расчете автовыхода
float KURS_UP_LIM;      //предельное расчетное  значение приращения курса
float KURS_UP_LIM_AVTO; //предельное значение приращения курса  при  автоматическом  выходе  с  циркуляции
BOOL avto_exit;          //флаг  автовыхода  с  циркуляции
float A_ZACLAD_TEC;        //текущий  угол  закладки
float DIFF_TEC;         //текущий местный дифферент
BOOL perechet;           //флаг пересчета заданного курса
BOOL rachet_exp;         //флаг расчета ехр  (может меняться  в SPEED())
float x;                //расчетный коэффиц ехр в расчете  приращения курса   
BOOL speed_kurs;
BOOL speed_kurs_exit;    //флаг изменения  скорости  на циркуляции
BOOL speed_kurs_antre;   //флаг изменения  скорости  на циркуляции
float time_speed_kurs;  // относительное время циркуляции
float SPEED_K_FIX;      // скорость на начале циркуляции
float speed_tec;        //текущая  скорость  при расчете  изменения  скорости (не  всегда  присваивается)
BOOL avto_exit_speed ;   // флаг расчета  увеличения  скорости  на  выходе  с циркуляции
BOOL flag_1;
float time_speed_exit;
float A_ZACLAD_s;
int ZIRC_TEC;  //текущее значение  циркуляции  
float SPEED_CMD_OLD; //сохраняемое  значение  команды для изменения  скорости  при  циркуляции
float  time_bius;  //время  для симуляции  команд биуса

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/////////////////////////ГЭУ/////////////////////////////////



/////////////////////////////////////////////////////////////
/////////////////////// CКОРОСТЬ ////////////////////////////////
/////////////////////////////////////////////////////////////
///////////////////коэффициенты  по  скорости////////////
const static struct SPEED_CLASS_PL
{
float sk1[8],sk2[8];
float sk3,sk4,sk5,sk6,sk7,sk8,
sk9,sk10,sk11,sk12,sk13,sk14,sk15,sk16;}
kk_speed;

/////////////////////////////////////////////////////////////////
BOOL rachet_pow;     // флаг расчета pow
float xx;           // расчетная  величина pow
float SPEED_CMD_MAX;//максимальная скорость по  состоянию ГЭУ
////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////// ГЛУБИНА ////////////////////////////////
/////////////////////////////////////////////////////////////
//////////коэффициенты  по  глубине/////////////////////////
/////////////////////////////////
BOOL avto_exit_extr;
// Operations

// Operations
public:
 void Move(void);
 void OnKURS();     //расчет курса
 void OnSPEED();    //расчет скорости
 void OnPOWER();    //расчет ГЭУ
 void OnTRAEKT();   //определение  координат,дист после  обсервации,время  после обсервации
 float RP(int);    //текущее прир мощности
 void SPEED_KURS_EXIT(); //скорость при заходе  на  циркуляцию
 void SPEED_KURS_ANTRE();//скорость  при  выходе с циркуляции 
// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMainDoc)
   public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
   //}}AFX_VIRTUAL

// Implementation
public:
//	void Transmitt(float angle, float sector,float level,float freq,float length);

//   void OneRayProcessing(float fi, float teta, LSSignal *pWave);
   virtual ~CMainDoc();
//   void UpdateCarNoise(float Depth,float Speed);
//   void UpdateSeaNoise(float Depth,float Wspeed);
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   CWorldClient   xworld;
public:
	void _3_Pointing_PR();
	void _1_Searching_PR();
	void Comand_V();
	void Comand_K();
	void _3_Pointing();
	void _1_Searching();
   	void Step(double t);

   TXParams          m_Me;    // мои координаты
   BOOL              m_bBump;
   UINT              m_TimeStep;
   UINT              m_Time;
   TFormularUSV      m_form;


// Generated message map functions
protected:
   //{{AFX_MSG(CMainDoc)
   afx_msg void OnPortWorld();
   afx_msg void OnDebugFlags(UINT nID);
   afx_msg void OnUpdateDebugFlags(CCmdUI* pCmdUI);
//	afx_msg void OnUpdateIndicatorNum(CCmdUI* pCmdUI);
//	afx_msg void OnRun();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDOC_H__366DCE9B_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
