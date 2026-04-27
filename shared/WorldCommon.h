#pragma once

#include "structures.h"
#include "RegsKey.h"
#include "FormInfo.h"
#include "GoodFunc.h"

#include "iAliases.h"

#ifndef MAX_STAND_NAMELEN
#define MAX_STAND_NAMELEN 31
#endif

#include "StandDefDir.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char hTHIS_FILE[] = __FILE__;
//#define THIS_FILE hTHIS_FILE
//#endif

#if WINVER!=0x501 || _WIN32_WINNT!=0x501
#error please add - C/С++ - Preprocessor: ,WINVER=0x501,_WIN32_WINNT=0x501
#endif

#ifdef _UNICODE
#error "No unicode supported!"
#endif

#define XSIGNATURE  'FMX0'

#define WM_MY_PROCESSDATA (WM_APP+8)

// флаговые данные - то что запрашивает клиент в поле XReq
// комбинируются через '|'
// только младшие 24 бита используются для запрашиваемых команд
// request
#define XREQ_CARRIER    0x00000001 /* XREQ_BUF0 мой носитель TXParams */
#define XREQ_WAVES      0x00000002 /* XREQ_BUF1 Waves - мои волны, 1 луч */
#define XREQ_TARGETS    0x00000004 /* XREQ_BUF2 targets - мои цели */
#define XREQ_ZPS        0x00000008 /* XREQ_BUF3 ЗПС */
// #define XREQ_BUF4       0x00000010 
// #define XREQ_BUF5       0x00000020
// #define XREQ_BUF6       0x00000040
// #define XREQ_BUF7       0x00000080
#define XREQ_SITUATION  0x00000100 /* все объекты - все XParams - ситуация */
#define XREQ_RADIOWAVES 0x00000200 /* радиоволны */
#define XREQ_MAGMOMENTS 0x00000400  /* магнитные моменты объектов */
// #define XREQ_ALLBUF3x    0x00000800
// #define XREQ_ALLBUF4x    0x00001000
// #define XREQ_ALLBUF5x    0x00002000
//#define XREQ_BIGINITCLIENT 0   /* убрать*/
#define XREQ_ASK_FORMULAR 0x00004000 /* отображать диалог формуляра запуска в world'e */
#define XREQ_5RAYS      0x00008000 /* модификатор для waves - все лучи(5), если не задан-1луч */
#define XREQ_Q_MOVE     0x00010000 /* квант на перемещение */
#define XREQ_Q_CALC     0x00020000 /* квант на вычисление полей */
#define XREQ_Q_LISTEN   0x00040000 /* квант на слушание гаком */
#define XREQ_Q_DRIVE    0x00080000 /* квант на управление носителем */

// системные запросы
#define XREQ_WSYSTEMS   0x00100000 /* XREQ_SYS0 список регистрированных объектов */        
#define XREQ_BUMP       0x00200000 /* XREQ_SYS1 столкновение */
  /* если дистанция столкновения задана <0 -то в команде выдаются 
  дополнительно координаты объекта, с которым столкнулись */
#define XCMD_INITCLIENT 0x00400000 /* XREQ_SYS2 формуляр для запуска клиента */            
#define XREQ_NEWTIME    0x00800000 /* XREQ_SYS3 новое время для систем документирования */ 
                    
        
/* флаги команд, требующих ответа  */ /*0x00CF0000*/ 
#define XFLAGS_RESPONSEABLE  (XREQ_Q_MOVE|XREQ_Q_CALC|XREQ_Q_LISTEN|XREQ_Q_DRIVE|XCMD_INITCLIENT|XREQ_NEWTIME)

#define XREQ_MAX        0x01000000 /* меньше него - запросы */
#define XREQ_ALL        0x00FFFFFF /* все биты запросов- для Response */

// для dwReq
#define XREQ_WAVES5        (XREQ_WAVES|XREQ_5RAYS) /* мои волны 5 лучей*/

/////////////////  КОМАНДЫ /////////////////////
#define xCMD_DEBUGGER_PRESENT  0x08000000 /* клиент запущен под отладчиком */
#define xCMD_CLOSE      0x10000000 /* закрыть систему, эмулируется в OnClose НЕ ПОСЫЛАТЬ!*/
#define XCMD_NEWCLIENT  0x20000000 /* от клиента серверу при соединении */
////////////////////////0x30000000
#define xCMD_RUN        0x40000000 /* запустить приложение (старый вариант)*/
#define XCMD_CMD        0x50000000 /* команды управления */
#define  XWORLD_CMD_STOP       0  /* параметры команды */
#define  XWORLD_CMD_QUANT      1
#define  XWORLD_CMD_FORWARD    2
#define  XWORLD_CMD_START      3
#define  XWORLD_CMD_BACKWARD   4
#define  XWORLD_CMD_CLOSEID    6  /* close object (dwid) */
#define  XWORLD_CMD_SETQUANT   7  /* установить длину кванта (dwQuant) */
#define  XWORLD_CMD_CREATEPOINT 8   /* (UINT nPtNum=0..63) создать точку приёма */
                                    /* затем используй SetXParamsPoint */
#define  XWORLD_CMD_DELETEPOINT 9   /* для удаления точки (int ptNum)*/
#define  XWORLD_CMD_CHANGEXREQ 10   /* изменить m_dwReq для сокета (dwclear, dwset) */
#define  XWORLD_CMD_ENDEXPERIMENT 11   /* завершить эксперимент (BOOL bDontSave) */
#define  XWORLD_CMD_RESERVEMAXID  12   /* зарезервировать ID для своих объектов */
#define  XWORLD_CMD_SETTICK       13   /* установить шаг таймера (DWORD). 0=Runtime -1=Idle *=ms */
#define  XWORLD_CMD_RUN          0x20  /* запустить приложение (новый вариант) */
#define  XWORLD_CMD_RUNDECOY     0x40  /* запустить мульку */

// для функции RunProgram
#define  XRUN_FLAG_DEBUG 1             /* отладка программы */
#define  XRUN_ASK_FORMULAR XREQ_ASK_FORMULAR /* перед запуском программы отобразить формуляр */

#define XCMD_RESPONSE   0x60000000 /* отклик клиента */
#define XCMD_BUFFER     0x70000000 /* работа с буферами 0..7 или -1 - радио */
// #define  XBUF_SET        0   /* для совместимости со старыми */
// #define  XBUF_APPEND     1   /* для совместимости со старыми */
#define  XBUF_SETEX      2   /* версия со счетчиком */
#define  XBUF_APPENDEX   3   /* версия со счетчиком */
#define XCMD_SENDCOMMAND 0x80000000 /* слать команду системе */  

// XCMD_CMD для монитора
#define XMON_CMD_CREATEWAVE      'WAVE'
#define XMON_CMD_SETSUPERNOISE   'SUPR'
#define XMON_CMD_QUERYKEELSTEPS  'QKS'
/*
параметры XMON_CMD_QUERYKEELSTEPS
   DWORD nCarrierID;
   UINT  iAlias;
   float X;   // координаты точки км
   float Y;   //                  км
   float K;   // курс излучателя
   float dD;  // дистанция до боковых точек, м (5)
   DWORD reserved1;
   DWORD reserved2;
   DWORD reserved3;
*/
#define XMON_CMD_ANSWERKEELSTEPS 'AQKS' 
// обрабатывается запрашивавшим XMON_CMD_QUERYKEELSTEPS
/* параметры */
struct TMonAnswerQKS 
{
  float p[3];    // вероятности обнаружения (левая(-dD),центр,правая(+dD))
  BOOL bInConus; // находимся в конусе
  float psc;     // для конуса - процент достижения им конечной ширины (0..1)
  float MinDist; // дистанция до ближайшего следа
  DWORD reserv1;
  DWORD reserv2;
  DWORD reserv3;
  DWORD reserv4;
  DWORD reserv5;
};

// структура, приходящая с XREQ_BUMP - параметры объекта, с которым столкнулись
struct TBumpStruct
{
   DWORD XBUMP;      // == XREQ_BUMP
   DWORD nObjClass;  // класс
   DWORD nCarrierID; // ID
   float X;          // координаты 
   float Y;
   float Z;
   float K;
   DWORD nObjLength; // длина объекта
   float nObjWidth;  // ширина объекта
   float V;          // скорость
   DWORD reserved;
   DWORD reserved2;
   DWORD reserved3;
};

// XCMD_CMD для документирования
#define XDOC_CMD_MSG             'DOCM'
#define xDOC_CMD_FRM             'FRM'
#define xDOC_CMD_STARTUP         'STUP'
#define XDOC_CMD_DONTSAVE        'SAVN'
//#define XDOC_CMD_xxxxx   'xxxx'

// for compatibility
#define XREQ_WORLD_Q_MOVE   XREQ_Q_MOVE
#define XREQ_WORLD_Q_CALC   XREQ_Q_CALC
#define XREQ_WORLD_Q_LISTEN XREQ_Q_LISTEN
#define XREQ_WORLD_Q_DRIVE  XREQ_Q_DRIVE
#define XREQ_WORLD_CARRIER  XREQ_CARRIER    /* мой носитель */
#define XCMD_WORLD_BUMP     XREQ_BUMP
//
#define SUPERNOISE           0x80000000

#define POINTID(CarrierID,PointNum) ((DWORD)MAKELONG(CarrierID,PointNum))
#define POINTNUM(CarrierID)  HIWORD(CarrierID)
#define ISPOINTID(id) (id!=DWORD(-1) && POINTNUM(id))
#define CARRID(id) LOWORD(id)
#define SAMEIDS(id1,id2) (CARRID(id1)==CARRID(id2))

#ifndef ASSERTM
#ifdef _DEBUG
#define ASSERTM(expr,msg) \
   do { if (!(expr) && \
     (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg))) \
      _CrtDbgBreak(); } while (0)
#else
   #define ASSERTM(expr,msg)
#endif
#endif

// описание формуляра запуска
class CFormularInfo 
{
public:
   struct TFormularEntry { // описание одного поля
      UINT type;    // FITYPE_
      UINT ncmd;    //команда 
      CString name;
   };

protected:
   CArray<TFormularEntry,TFormularEntry&> aInfo;

   void Copy(const CFormularInfo &src)
   {
      aInfo.Copy(src.aInfo);
   }
   CFormularInfo( const CFormularInfo &src){ Copy(src); }

public:
   CFormularInfo(){}
   void operator =( const CFormularInfo &src){ Copy(src); }

   int GetCount() const { return aInfo.GetSize(); }
   const TFormularEntry *GetData() const { return aInfo.GetData(); }

   const TFormularEntry & operator[](int idx) const { return aInfo[idx]; }
   TFormularEntry & operator[](int idx) { return aInfo[idx]; }

   void Create(UINT nitems)
   {
      aInfo.SetSize(nitems);
   }
   
   void Destroy() { aInfo.SetSize(0); }

   void Serialize(CArchive &ar)
   {
      if ( ar.IsStoring() ) {
         UINT nItems = aInfo.GetSize();
         ar << nItems;
         for ( UINT i=0; i<nItems ;i++ ) {
            ar << aInfo[i].type;
            ar << aInfo[i].ncmd;
#ifdef DONT_SAVE_PARAMETER_TEXT
            { 
               CString s;
               ar << s;
            }
#else
            ar << aInfo[i].name;
#endif
         }
      } else {
         UINT nItems;
         ar >> nItems;
         if ( nItems ) { 
            //5- ar >> nItems;
            Create(nItems);
            for ( UINT i=0; i<nItems ;i++ ) {
               ar >> aInfo[i].type;
               if ( aInfo[i].type < FITYPE_COMMAND ) {
                  UINT nlen;
                  ar >> nlen;
                  if ( nlen > 1 ) {
                     ASSERT(0);
//                      CString s;
//                      ar >> s;
//                      UINT type = aInfo[i].type;
//                      for ( UINT j=0; j<nlen ;j++ ) {
//                         aInfo[i+j].type = type;
//                         aInfo[i+j].ncmd = 0;
//                         aInfo[i+j].name.Format("%s [%d]",s,j);
//                      }
//                      i += nlen-1;
                  }
//                   else {
                     aInfo[i].ncmd = 0;
                     ar >> aInfo[i].name;
//                   }
               } else {
                  ar >> aInfo[i].ncmd;
                  ar >> aInfo[i].name;
               }
            }
         }
      }
   }
};

/// тип системы объекта-участника 
struct TWorldSystem {
   
   enum { CUR_VERSION = 1 };

   UINT iAlias; // номер алиаса в Dict
   // нижние FFF биты - алиас, верхние 4 бита - номер команды минус 1
   // а имена вторичны, создаются в CWorldSystems::UpdateNames()
   // sExeName не нужно, т.к. exe теперь соответствует алиасу
   // как хранить алиас, которому команда?

   CString  sAlias_;    // создаётся из iAlias и словаря
   CString  sName_;     // создаётся из iAlias и словаря, для команд задается при регистрации

#define TYPE_SYSTEM  0
#define TYPE_CARRIER 1
#define TYPE_COMMAND 2
#define TYPE_EVENT   3
#define TYPE_GLOBAL  4   /* World */

   int      nType;         // 1 - носитель, 0 - система, 2 - команда, 3- событие, 4-глобальный
   int      nIconIdx;      // номер картинки в ImgList
   DWORD    nIconCnt;      // количество картинок в ImgList  // 16.10.2018
   DWORD    dwQuant;       // максимальная длина кванта для работы системы 0-любая
   float    fBumpDist;     // дистанция столкновения, км 3d, XYZKlw объекта
   UINT     nLength;       // длина для объекта, м
   UINT     nClass;        // класс объекта. Для систем - разрешение запуска дубликата 13.09.2018
   DWORD    dwVersion;     // CRC для перерегистрации или версия
   float    fMagMoment;    // магнитный момент объекта  кА*м^2 - вертикальный
   float    fWidth;        // ширина объекта (корма), м  для кильв.следа

   CFormularInfo Frm;

   void Serialize(CArchive &ar);
   BOOL IsSystem()  const { return nType == TYPE_SYSTEM; }
   BOOL IsCarrier() const { return nType == TYPE_CARRIER; }
   BOOL IsCommand() const { return nType == TYPE_COMMAND; }
   BOOL IsCommandPlus() const { return nType >= TYPE_COMMAND; }
   BOOL IsEvent  () const { return nType == TYPE_EVENT; }
   BOOL IsGlobal () const { return nType == TYPE_GLOBAL; }
   //BOOL __declspec(property(get=IsCarrier)) bCarrier;
   BOOL EnabledSystemDuplicates() const { return nClass != 0; }


   CMap<CString, LPCTSTR, UINT, UINT> m_ParamMap;
   CStringArray                       m_ParamArr;
   void MakeParamMap() 
   {
      if ( !m_ParamMap.IsEmpty() ) return;
      m_ParamArr.SetSize(0);

      void ChangeEqualSign(char *buf);

      UINT n = Frm.GetCount();
      if ( !n ) return;
      m_ParamArr.Add("Frm_bUse");
      m_ParamMap.SetAt(m_ParamArr[0],0);
      CString s;
      for ( UINT i=0; i<n ;i++ ) {
         if ( Frm[i].type == FITYPE_COMMAND ) break;
         // s = "Frm." + Frm[i].name;
         s.Format("Frm[%d].%s", i, Frm[i].name); //457
         s.TrimRight(); // могут быть пробелы в конце названия  >458

         ChangeEqualSign(s.GetBuffer(0));
         VERIFY( m_ParamArr.Add(s) == (int)i+1 );
         m_ParamMap.SetAt(m_ParamArr[i+1], i+1 /*| (Frm.pInfo[i].type<<16)*/);
      }
   }
private:
   void operator =(const TWorldSystem &src) 
   {
      ASSERT(0);
      iAlias = src.iAlias;
      sName_       = src.sName_      ;  //TODO check
      sAlias_      = src.sAlias_     ;  //TODO check
      nType       = src.nType      ;       
      nIconIdx    = src.nIconIdx   ;    
      nIconCnt    = src.nIconCnt   ;    
      dwQuant     = src.dwQuant    ;     
      fBumpDist   = src.fBumpDist  ;   
      nLength     = src.nLength    ;     
      nClass      = src.nClass     ;      
      dwVersion   = src.dwVersion  ;   
      fMagMoment  = src.fMagMoment ;  
      fWidth      = src.fWidth     ;      
      Frm         = src.Frm        ;
   }
public:
   void FullCopyFrom(const TWorldSystem &src) 
   {
    //  ASSERT(0);
      iAlias = src.iAlias;
      sName_       = src.sName_    ;  //TODO check
      sAlias_      = src.sAlias_   ;  //TODO check
      nType       = src.nType      ;       
      nIconIdx    = src.nIconIdx   ;    
      nIconCnt    = src.nIconCnt   ;    
      dwQuant     = src.dwQuant    ;     
      fBumpDist   = src.fBumpDist  ;   
      nLength     = src.nLength    ;     
      nClass      = src.nClass     ;      
      dwVersion   = src.dwVersion  ;   
      fMagMoment  = src.fMagMoment ;  
      fWidth      = src.fWidth     ;      
      Frm         = src.Frm        ;
   }

   void UpdateFrom(const TWorldSystem &src)
   {
      // имена остаются прежними
      iAlias = src.iAlias;
      nType       = src.nType      ;       
      nIconIdx    = src.nIconIdx   ;    
      nIconCnt    = src.nIconCnt   ;
      dwQuant     = src.dwQuant    ;     
      fBumpDist   = src.fBumpDist  ;   
      nLength     = src.nLength    ;     
      nClass      = src.nClass     ;      
      dwVersion   = src.dwVersion  ;   
      fMagMoment  = src.fMagMoment ;  
      fWidth      = src.fWidth     ;      
      Frm         = src.Frm        ;
   }
};

class CWorldClientData : public TWorldSystem 
{
public:
   void Serialize(CArchive &ar);
   void Send(CArchive &ar);
   void Receive(CArchive &ar);

   BOOL LoadFormInfo(
      const TFormInfo *pFormInfo = NFormInfo::pFormInfo, 
      DWORD dwFormLen = NFormInfo::dwFormLen);

   CImageList m_Images;  // две картинки объекта: селектированный и нет
//    DWORD      m_dwFormId; // DWORD dwFormId - id формуляра запуска

   CFormularInfo Cmd; // дополнительные команды для регистрации
                      // if ( Cmd.pInfo ) dwSomeFlags = 0x100;
};

////////////////////////////////////////////////////////


