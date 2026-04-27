// WorldClient.h: interface for the CWorldClient class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FMExchange.h"
#include "WorldCommon.h"
#include "DebugFlags.h"

class CWorldClient;

#ifdef GTEST
extern DWORD g_dwTest;
#endif

#define override

///////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(4)
struct TSituation // XREQ_SITUATION + GetSituation
{
   DWORD    nCarrierID;
   UINT     iAlias;
   TXParams tx;
   DWORD    dwXReqSumm;
   DWORD    nObjClass; //443: г/а класс объекта 
};
#pragma pack(pop)

class CPortWorld {
public:
   // dummy 
   // для совместимости с 9L
};

///////////////////////////////////////////////////////////////////
class CWorldClient : public CFMExchange, public CWorldClientData   
{
//public:
protected:
   CCmdTarget *m_pTarg;
   int         m_nSerializeID;
   DWORD       a_xcmd;
   DWORD       a_xcmd_last; // последняя бывшая команда
   CArchive   *a_arc;

public:
   CString     m_sAlias;        // мой алиас
   DWORD       m_nCarrierID;    // ид носителя
   CString     m_sCarAlias;     // алиас носителя
   CString     m_sFileName;     // имя ws файла расстановки в World'e
   DWORD       m_dwDepth;       // глубина моря для заданного региона,м
   BOOL        m_bWinterFlag;   // признак зимы
   int         m_nBettaType;    // для затухания тип района 0-обычный, 1-японского моря, 2-для отладки; CMonWave::GetBetta
   CHAR        m_szRegion[64];  // имя региона
   float       m_FresnelZone;   // зона Френеля
   BOOL        m_bInitClientOK; // пришел INITCLIENT

   DWORD m_dwMyXReq;      // флаги запросов - что запросили в Create

   DWORD m_nScenNumber;   // текущий сценарий
   DWORD m_nScenBeginNum; // номер первого сценария 
   DWORD m_nScenEndNum;   // последний сценарий
   int   m_nUseScenList;  // использование списка экспериментов 
//    BOOL  m_bRunMin;       // был запущен минимизированно - отключить отрисовку

   CString m_ScenTitle0;
   CString m_sSavePath;

   CString m_sStandDir;
   CString m_sStandName;

protected:
   ////////////////////////////////////////
   virtual void ErrorStream() override;
   virtual void OnReceive() override;
   virtual void OnClose(HANDLE hParam) override;

public:
   LPCTSTR GetAlias() const { return m_sAlias; }
   void ExitApp();      // выход из приложения. Только in notify от xworld.
   void ExitAppSmart(); // выход из приложения

   //команды в World 
   void WorldStop() { SendToServer(XCMD_CMD, XWORLD_CMD_STOP); } // стоп - красный квадрат
   void WorldQuant() { SendToServer(XCMD_CMD, XWORLD_CMD_QUANT); } // один шаг
   void WorldForward() { SendToServer(XCMD_CMD, XWORLD_CMD_FORWARD); } // быстрее
   void WorldStart() { SendToServer(XCMD_CMD, XWORLD_CMD_START); } // пуск-зеленый треугольник
   void WorldBackward() { SendToServer(XCMD_CMD, XWORLD_CMD_BACKWARD); } // медленнее
   void CloseID(DWORD nID) { SendToServer2(XCMD_CMD, XWORLD_CMD_CLOSEID, nID); } //завершить ID
   void SetQuantSize(DWORD dwQuant)  // 0- по умолчанию (из типа системы)
   { SendToServer2(XCMD_CMD, XWORLD_CMD_SETQUANT, dwQuant); }
   BOOL CreatePoint(UINT nPtNum) // создать точку приёма-передачи
   { return SendToServer2(XCMD_CMD,XWORLD_CMD_CREATEPOINT,nPtNum); }
   BOOL DeletePoint(UINT nPtNum) // удалить точку приёма-передачи
   { return SendToServer2(XCMD_CMD,XWORLD_CMD_DELETEPOINT,nPtNum); }
   void ChangeXReq(DWORD dwClear,DWORD dwSet); // изменить m_dwReq для сокета
   void EndExperiment(BOOL bDontSave)
   { SendToServer2(XCMD_CMD, XWORLD_CMD_ENDEXPERIMENT, bDontSave); }
   BOOL ReserveMaxID(DWORD nMaxID) // зарезервировать ID для своих объектов
   {  ASSERT(nMaxID < 0xFFFF);
      return SendToServer2(XCMD_CMD, XWORLD_CMD_RESERVEMAXID, nMaxID); }
   ////////////////////////////

   void Docs(DWORD nMsgID, LPCTSTR szFmt, ...);
   void DocsDontSaveWss();

   void RunProgram(
      UINT iAlias,             // номер алиаса, кого запускать
      DWORD dwFormLen = 0,     // длина формуляра в байтах 
      void *pForm = NULL,      // формуляр
      DWORD nID = DWORD(-1),   // опциональный ID
      LPCTSTR szArguments = NULL, // опциональная строка запуска программы
      DWORD dwDebugFlags = 0      // отладка XRUN_FLAG_DEBUG, XRUN_ASK_FORMULAR
      ); // если iAlias == 0, то имя исполняемого файла содержится в szArguments 


   // запуск имитированного объекта (мульки)
   void RunDecoy(UINT iAlias, // номер алиаса, кого запускать
      DWORD nID,
      TXParams *pTX);

   // установить координаты в объект
   void SetXParams(TXParams *pMe)
   {  SetBuffer(0, pMe, sizeof(TXParams)); }

   // установить координаты для точки приёма-передачи
   void SetXParamsPoint(UINT nPtNum, TXParams *pMe)
   { SetBuffer(POINTID(m_nCarrierID,nPtNum),0,pMe,sizeof(TXParams));}
  
   // послать команду системе
   void SendCommand(UINT iAlias, DWORD nID, DWORD cmd, void *pdata, DWORD nlen);
   void SendCommand(UINT iAlias, DWORD nID, DWORD cmd, CArcBuf &ab);
   // работа с буферами
   void SetBuffer(DWORD nID, int nBufNum, void *pdata, DWORD nlen,DWORD nCount=1);
   void AppendBuffer(DWORD nID, int nBufNum, void *pdata, DWORD nlen,DWORD nCount=1);
   void SetBufferEmpty(DWORD nID, int nBufNum);
   
   // переопределения для своего носителя ////////////////////
   // послать команду системе
   void SendCommand(UINT iAlias, DWORD cmd, void *pdata, DWORD nlen)
   { SendCommand(iAlias, m_nCarrierID, cmd, pdata, nlen); }
   void SendCommand(UINT iAlias, DWORD cmd, CArcBuf &ab)
   { SendCommand(iAlias, m_nCarrierID, cmd, ab);}
   // работа с буферами
   void SetBuffer(int nBufNum, void *pdata, DWORD nlen,DWORD nCount=1) 
   { SetBuffer(m_nCarrierID, nBufNum, pdata, nlen, nCount); }
   void AppendBuffer(int nBufNum, void *pdata, DWORD nlen,DWORD nCount=1)
   { AppendBuffer(m_nCarrierID, nBufNum, pdata, nlen, nCount);}
   void SetBufferEmpty(int nBufNum) // свой
   { SetBufferEmpty(m_nCarrierID,nBufNum); }
   ///////////////////////////////////////////////////////////
   void dummy(){}
#define SendToServerBegin(xcmd) dummy();CArcBuf _ab; _ab << (DWORD)xcmd
#define SendToServerEnd()       SendToServer(_ab);
   
   BOOL SendToServer(DWORD xcmd, DWORD param);
   BOOL SendToServer2(DWORD xcmd, DWORD param1, DWORD param2);
using CFMExchange::SendToServer;
   void SendResponse(DWORD rcmd)
   { SendToServer(XCMD_RESPONSE, rcmd); }
   DWORD         GetXCmd() const { return a_xcmd; }
   CArchive &GetArchive() const { /*ASSERT(a_arc);*/ return *a_arc; }
   
   UINT GetSituation(const TSituation **ts); // валидно при поступлении XREQ_SITUATION
   DWORD *GetXCmdData(); // валидно при поступлении XCMD_CMD

   const TBumpStruct *GetBumpStruct(); // валидно при поступлении XREQ_BUMP

protected:
   void NotifyCmdTarget();
   BOOL CheckLatAlias(LPCTSTR szAlias);

   virtual void PostWmClose();

public:

//    __declspec(deprecated)  
//    /* содержит неиспользуемые параметры, оставлена для совместимости */
//    BOOL Create(
//       CCmdTarget *pTarg,    // чей OnCmdMsg
//       UINT     nSerializeID,// параметр OnCmdMsg
//       DWORD    dwReq,       // запрашиваемые данные XREQ_xxx
//       DWORD    nCarrierID,  // ид своего носителя, if bCarrier -> -1
//       
//       LPCTSTR  szAlias,     // alias - его файлы
//       LPCTSTR  szName,      // имя для отображения
//       BOOL     bCarrier,    // 1-плавает 0-система
//       DWORD    ,            // НЕ ИСПОЛЬЗУЕТСЯ
//       LPCTSTR  ,            // НЕ ИСПОЛЬЗУЕТСЯ
//       UINT     nIdBitmap,   // битмапка для отображения 34x16
//       UINT     nIdBitmapSel,// селектир. битмапка -if _bCarrier 
//       DWORD    dwQuant,     // квант работы
//       float    fBumpDist,   // дистанция столкновения, км  -if _bCarrier 0.01f
//       UINT     nLength,     // длина объекта, м            -if _bCarrier 
//       DWORD    ,            // НЕ ИСПОЛЬЗУЕТСЯ
//       UINT     nClass,      // класс объекта       -if _bCarrier
//       float    MagMoment=0, // магнитный момент -if _bCarrier  кА*м^2 - вертикальный/поперечный
//       float    fWidth = 0   // ширина объекта (корма), м  для кильв.следа -if _bCarrier 
//       );

   BOOL Create(
      CCmdTarget *pTarg,    // чей OnCmdMsg
      UINT     nSerializeID,// параметр OnCmdMsg
      DWORD    dwReq,       // запрашиваемые данные XREQ_xxx
      DWORD    nCarrierID,  // ид своего носителя, if bCarrier -> -1
      UINT     iAlias,      // Алиас
      BOOL     bCarrier,    // 1-плавает 0-система
      UINT     nIdBitmap,   // битмапка для отображения 34x16
      UINT     nIdBitmapSel,// селектир. битмапка -if _bCarrier 
      DWORD    dwQuant,     // квант работы
      float    fBumpDist,   // дистанция столкновения, км  -if _bCarrier 0.01f
      UINT     nLength,     // длина объекта, м            -if _bCarrier 
      UINT     nClass,      // класс объекта носителя/ для систем- разрешение запуска дубля
      float    MagMoment=0, // магнитный момент -if _bCarrier  кА*м^2 - вертикальный/поперечный
      float    fWidth = 0   // ширина объекта (корма), м  для кильв.следа -if _bCarrier 
      );
   
   CWorldClient();
   void Destroy();
   virtual ~CWorldClient();
   
};

