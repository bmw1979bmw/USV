#pragma once

#if WINVER!=0x501 || _WIN32_WINNT!=0x501
#error please add - C/С++ - Preprocessor: ,WINVER=0x501,_WIN32_WINNT=0x501
#endif

#include "FMSync.h"
#include "ArcBuf.h"
#include "WorldCommon.h"
#include "StandApp.h"
#include "ByteArrayEx.h"

#define MAX_FM_OBJECTS   (MAXIMUM_WAIT_OBJECTS-2) /*1 newdata, 1 message*/

// структура, лежащая поверх FileMapping для обмена
struct TFMPool
{
   TCHAR szMutexServer[64];  // mutex server working  WorldSimulation
   TCHAR szEventNewData_[64];  //
   TCHAR szMutexSlot[64];
   TCHAR szSemaphoreData1024[64];

   enum { N_SLOTS=(MAX_FM_OBJECTS+1), N_DIRS=1024, N_BUFLEN = 8192 };

   LONG    m_nSlotChanges;// были изменения в слотах по добавлению/удалению объектов
                          // если изменений не было - можно не ждать MutexSlot
   // ячейка под процесс
   struct TSlot {
      DWORD PID;             // кому (нулевой - сервер)
      DWORD nLastReadOrder;  // последняя прочитанная - доступно только для читателя
      DWORD nLastWrittenOrder; // последняя записанная
      DWORD dwQuickCmd; // быстрая команда 'CLOS'
      DWORD Reserved1;
      DWORD Reserved2;
      DWORD Reserved3;
      DWORD Reserved4;

      void Init2()
      {
         nLastReadOrder = 0;
         nLastWrittenOrder = 0;
         dwQuickCmd = 0;
      }

      BOOL TryOccupy_m(DWORD nPID)  // занять слот, если можно @m_MutexSlot
      {
          // m_MutexSlot controlled
         if ( PID ) return FALSE;
         PID = nPID;
//             InterlockedCompareExchange((LONG*)&PID, nPID, 0) == 0 ) {
         Init2();
         return TRUE;
//          }
//         return FALSE;
      }

      void Release_m() // освободить слот  @m_MutexSlot controlled
      {
         Init2();   
         PID = 0;
//         InterlockedExchange((LONG*)&PID,0);
      }

      DWORD GetLastWriteOrder_m(LONG nCount) { 
         // зарезервировать для записи указанного числа буферов
         // вернуть последнее значение, увеличить его на число
         return InterlockedExchangeAdd((LONG*)&nLastWrittenOrder,nCount);
      }

   } Slot[N_SLOTS];

   // описатели куска данных
   struct TDirectory {
      DWORD PID;     // кому
      DWORD PIDfrom; // от кого
      DWORD nOrder;  // порядок
      DWORD nLen;    // длина

      BOOL TryOccupy(/*DWORD nPID*/) {
         if ( InterlockedCompareExchange((LONG*)&PID, /*nPID*/ ~0, 0) == 0 ) {
            nOrder = 0;
            nLen = 0;
            return TRUE;
         }
         return FALSE;
      }
      void Release()
      {
         nLen = 0;
         nOrder = 0;
         InterlockedExchange((LONG*)&PID,0);
      }
   } Dir[N_DIRS];
   // куски данных (отдельно, чтобы были выровнены вместе)
   BYTE Data[N_DIRS][N_BUFLEN];

   /////////////////////////////////////////////////

   void Clear(){ ZeroMemory(this, sizeof TFMPool); }

   //функции с *_m прикрыты m_MutexSlot'ом 

   TSlot *OccupyServerSlot_m(DWORD PID) // m_MutexSlot controlled
   {
      InterlockedIncrement(&m_nSlotChanges);
      if ( Slot[0].TryOccupy_m(PID) ) return &Slot[0];
      return NULL;
   }

   TSlot *OccupySlot_m(DWORD PID) // m_MutexSlot controlled
   {
      InterlockedIncrement(&m_nSlotChanges);
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].TryOccupy_m(PID) ) return &Slot[i];
      }
      return NULL;
   }

   int GetNextBufIndex(TSlot *pSlot)
   {
      // вернуть номер следующего буфера для чтения
      // если нет: -1,  // если не заполнена длина: тупо ждать
      DWORD nLast = pSlot->nLastReadOrder;
      DWORD nPID  = pSlot->PID;

      for ( int i=0; i<N_DIRS ;i++ ) {
         if ( Dir[i].PID != nPID ) continue;
         if ( Dir[i].nOrder != nLast ) continue;
         pSlot->nLastReadOrder++;
         return i;
      }
      return -1;
   }

   int CalcAvailableDataForPID(DWORD PID)
   {  // for debug
      int n = 0;
      for ( int i=0; i<N_DIRS ;i++ ) {
         if ( Dir[i].PID != PID ) continue;
         n++;
      }
      return n;
   }

   int CalcFreeSlots()
   {  // for debug
      int n = 0;
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].PID ) continue;
         n++;
      }
      return n;
   }

   BOOL GetLastWriteOrderForPID_m(DWORD PID, LONG nCount, DWORD *pOrder ) 
   { 
      if ( !pOrder ) return FALSE;
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].PID != PID ) continue;
         *pOrder = Slot[i].GetLastWriteOrder_m(nCount);
         return TRUE;
      }
      return FALSE;
   }

   int FindPIDSlot(DWORD PID)
   {
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].PID != PID ) continue;
         return i;
      }
      return -1;
   }

   BOOL QuickCmdToPID_m(DWORD PID, DWORD nQuckCmd)
   {
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].PID != PID ) continue;
         Slot[i].dwQuickCmd = nQuckCmd;
//#pragma chMSG("плохо. может измениться PID пока присваиваем.") 
         return TRUE;
      }
      return FALSE;
   }

   int OccupyBuffer()
   {
      for ( int i=0; i<N_DIRS ;i++ ) {
         if ( Dir[i].PID ) continue;
         if ( !Dir[i].TryOccupy() ) continue;
         return i;
      }
      return -1;
   }

   DWORD GetServerPID(){ return Slot[0].PID; } 
   
   void CleanUpForPID_m(DWORD nPID) //? прибираемся
   {  //? вызывает сервер при закрытии процесса, чтоб удалить ссылки на этот PID
      if ( !nPID ) return;
      for ( int i=0; i<N_SLOTS ;i++ ) {
         if ( Slot[i].PID != nPID ) continue;
         Slot[i].Init2();
         Slot[i].PID = 0;
         break;
      }
      CleanUpDirsForPID_m(nPID);
   }

   LONG CleanUpDirsForPID_m(DWORD nPID) // прибираемся
   {  // удалить буферы для этого PID
      if ( !nPID ) return 0;
      LONG n=0;
      for ( int i=0; i<N_DIRS ;i++ ) {
         if ( Dir[i].PID == nPID ) {
            Dir[i].PID = 0;
            n++;
         }
      }
      return n;
   }
   

};


/////////////////////////////////////////////////////////////////////////////
// CFMExchange

class CFMExchange 
{
public:
	CFMExchange();

   DWORD  m_PID;

   HANDLE   m_hFM;
   TFMPool *m_FM;
   TFMPool::TSlot *m_pMySlot;

   CWMutex     m_MutexServer;
   CWEvent     m_EventNewData;

   CWMutex     m_MutexSlot;
   CWSemaphore m_SemaphoreData1024;

   // для сервера все, для клиента только нулевой
   CWEvent     m_AllEventsNewData[1+MAX_FM_OBJECTS];  //SERVER+CLIENTS

   // для OnReceive
   CByteArrayEx m_InputBuf;
   DWORD        m_nInputPID;  // от кого пришел 
   DWORD        m_nInputLen;
   
//   BOOL     m_bServer;   see  m_AllEventsNewData[1] for server

   HANDLE    m_hHandles[1+MAX_FM_OBJECTS];  // NewDataEvent + CLIENT's hProcess
   UINT      m_nHandles;

   CString     m_sCause;

   LONG    m_nFMSlotChanges;// были изменения в слотах по добавлению/удалению объектов

   virtual void ErrorStream() = 0;
   virtual void OnReceive() = 0;
   virtual void OnClose(HANDLE hParam) = 0;

   CStandApp *m_app;

   void QuickWait()  // просто проверить состояние m_hHandles
   {  WaitHandlesAndMessages(0);  }
   
   void LongWait() // ждать m_hHandles или message
   {  WaitHandlesAndMessages(1);  }

   void WaitForMessage();

   BOOL WaitHandlesAndMessages(BOOL bLong);


   void OnMsgRead();

   void OnMsgClose(HANDLE hParam);

   int GetFreeBufferIndex();

   BOOL WaitForSlot();

   BOOL SendToPID(DWORD PID, const void *pBuffer, DWORD nLength);

   BOOL SendToServer(const void *pBuffer, DWORD nLength)
   {  return SendToPID(m_FM->GetServerPID(), pBuffer, nLength); }
   
   BOOL SendToServer(CArcBuf &ab)
   {  return SendToPID(m_FM->GetServerPID(), ab.GetBuf(), ab.GetLen()); }
   
   BOOL SendToPID(DWORD PID, CArcBuf &ab)
   {  return SendToPID(PID, ab.GetBuf(), ab.GetLen()); }
   
   BOOL QuickCmdToPID(DWORD PID, DWORD nQuckCmd);
   
   void CloseHandles();

   void Destroy();

   BOOL CreateClient ( LPCTSTR szStandName /*stand name*/ );
   BOOL CreateServer ( LPCTSTR szStandName /*stand name*/ );

	virtual ~CFMExchange();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
