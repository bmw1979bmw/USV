// FMExchange.cpp : implementation file
//

#include "stdafx.h"
#include "WorldCommon.h"
#include "FMExchange.h"
#include "StandApp.h"

#  ifdef DEMO
#define _WORLDFMX            "Demo_FM_9a_"
#define _WORLDSIMULATION     "Demo_Simulation_"
#define _WORLDNEWDATA        "Demo_NewData_"
#define _WORLDMUTEXSLOT      "Demo_MutexSlot_"
#define _WORLDSEMAPHOREDATA  "Demo_SemapData_"
#  else
#define _WORLDFMX            "World_FM_9a_"
#define _WORLDSIMULATION     "World_Simulation_"
#define _WORLDNEWDATA        "World_NewData_"
#define _WORLDMUTEXSLOT      "World_MutexSlot_"
#define _WORLDSEMAPHOREDATA  "World_SemapData_"
#  endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFMExchange

CFMExchange::CFMExchange()
{
   m_hFM = 0;
   m_PID = 0;
   m_FM = 0;
   m_pMySlot = 0;
   m_nInputLen = 0;
   m_nInputPID = 0;
   m_app = NULL;
   m_InputBuf.SetSize(0, TFMPool::N_BUFLEN);
   m_nHandles = 0;
   m_nFMSlotChanges = -1;

   _set_SSE2_enable(1);
}

CFMExchange::~CFMExchange()
{
   CloseHandles();
   if ( m_app ) m_app->m_pxchg = NULL;
}

void CFMExchange::WaitForMessage()
{
   // for client, для диалоговых окон, проверка на закрытие, из PumpMessage
   // Диалоговое окно будет закрыто только при окончании моделирования, 
   // но не по насильственному закрытию объекта командой 'CLOS' !
   if ( !this ) return;
   if ( !m_pMySlot ) return;
   if ( m_AllEventsNewData[1] ) return; //server
   //while ( !Wait(1) );
   if ( !m_nHandles ) return;
   
   do {   
      DWORD wait = MsgWaitForMultipleObjectsEx( 1, &m_hHandles[1],
         INFINITE, QS_ALLINPUT|QS_ALLPOSTMESSAGE,
         MWMO_INPUTAVAILABLE);
      
      if ( wait == WAIT_OBJECT_0+1 ) { //message
         break;
      }
      
      if ( wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED_0 ) {
         // somebody aborted 
         if ( m_pMySlot ) m_pMySlot->dwQuickCmd = 'CLOS';
         OnMsgClose(m_hHandles[1]);
         break;
      }
      
   } while ( 1 );
}

BOOL CFMExchange::WaitHandlesAndMessages( BOOL bLong )
{
   // true for message
   if ( !this ) return TRUE; // не кошерно
   if ( !m_pMySlot ) return TRUE;
   if ( !m_nHandles ) return TRUE;
   
   DWORD wait = 
      bLong 
      ? MsgWaitForMultipleObjectsEx( m_nHandles, m_hHandles,
      INFINITE, QS_ALLINPUT|QS_ALLPOSTMESSAGE,
      MWMO_INPUTAVAILABLE)
      : WaitForMultipleObjectsEx( m_nHandles, m_hHandles, 0, 0, 0);
   
   do {
      if ( wait == WAIT_OBJECT_0 + m_nHandles ) return TRUE;//break;// message occured
      if ( wait == WAIT_TIMEOUT ) break;
      if ( wait == WAIT_OBJECT_0 ) { OnMsgRead(); break; }
      if ( wait >= WAIT_OBJECT_0+1 && wait < WAIT_OBJECT_0 + m_nHandles ) {
         // somebody closed 
         OnMsgClose(m_hHandles[wait-WAIT_OBJECT_0]);
         break;
      }
      if ( wait >= WAIT_ABANDONED_0+1 && wait < WAIT_ABANDONED_0 + m_nHandles ) {
         // somebody aborted 
         OnMsgClose(m_hHandles[wait-WAIT_ABANDONED_0]);
         break;
      }
      
   } while ( 0 );
   
   if ( m_pMySlot && m_pMySlot->dwQuickCmd == 'CLOS' ) {
      OnMsgClose(0);
   }
   
   return FALSE;
}

void CFMExchange::OnMsgRead()
{
   
   if ( !m_pMySlot ) return; // destroyed
   
   while ( 1 ) {
      int idx = m_FM->GetNextBufIndex(m_pMySlot);
      if ( idx < 0 ) break;
      
      DWORD nLen = m_FM->Dir[idx].nLen;
      ASSERT(nLen);
      
      BYTE *data = m_FM->Data[idx];
      m_nInputPID = m_FM->Dir[idx].PIDfrom;
      
      if ( m_InputBuf.GetSize() == 0 ) {
         if ( nLen < 8 || nLen > TFMPool::N_BUFLEN ) {
            continue; //???
            /// ErrorStream(0);// ASSERT(0); // terrible!!!
         }
         DWORD *DWData = (DWORD*)data;
         DWORD dwSignature = DWData[0];
         if ( dwSignature != XSIGNATURE ) {
            ErrorStream();
         }
         m_nInputLen = DWData[1];
         m_InputBuf.Append(data + 8, nLen - 8);
         /* TODO for speed up
         if ( nLen < TFMPool::N_BUFLEN ) {
         то не m_InputBuf.Write, а m_InputBuf.Attach !
               }
         */
         
      } else {
         m_InputBuf.Append(data, nLen);
      }
      m_FM->Dir[idx].Release();
      m_SemaphoreData1024.Release();
      
      if ( (UINT)m_InputBuf.GetSize() >= m_nInputLen ) {  //??
         OnReceive();
         m_InputBuf.SetSize(0, TFMPool::N_BUFLEN);
      }
   }
}

void CFMExchange::OnMsgClose( HANDLE hParam )
{
   
   if ( !this ) return;
   if ( !m_pMySlot ) return; // destroyed
   OnClose(hParam);
}

int CFMExchange::GetFreeBufferIndex()
{
   HANDLE hh[2] = { m_SemaphoreData1024, m_MutexServer };
   int nh = 2;
   while ( 1 ) {
      DWORD wait = WaitForMultipleObjects(nh, hh, FALSE, 5000);
      
      switch ( wait ) {
      case WAIT_TIMEOUT: // loop 5 sec
         break;
      case WAIT_ABANDONED_0:
      case WAIT_OBJECT_0 + 1:
      case WAIT_ABANDONED_0 + 1:
      default:
         return -1;
      case WAIT_OBJECT_0:
         {
            int idx = m_FM->OccupyBuffer();
            if ( idx >= 0 ) return idx;
            //???
         }
      }
   }
}

BOOL CFMExchange::WaitForSlot()
{
   // ждать слотовый мьютекс. Вызвать m_MutexSlot.Release() если TRUE.
   if ( !m_MutexSlot ) return FALSE;
   if ( !m_MutexServer ) return FALSE;
   
   HANDLE hh[2] = { m_MutexSlot, m_MutexServer };
   int n = m_AllEventsNewData[1]? 1:2;
   DWORD wait = WaitForMultipleObjectsEx( n, hh, 0, INFINITE, 0);
   switch ( wait ) {
   case WAIT_OBJECT_0:
      return TRUE;
   case WAIT_OBJECT_0+1:
      m_MutexServer.Release();
      //return FALSE;
   }
   //  AfxMessageBox(MB_TOPMOST, "!WaitForSlot=%X", wait);
   return FALSE;
}

BOOL CFMExchange::SendToPID( DWORD PID, const void *pBuffer, DWORD nLength )
{
   DWORD head[2] = { XSIGNATURE, nLength };
   
   int nRecords = (nLength + 8 + TFMPool::N_BUFLEN - 1)/TFMPool::N_BUFLEN;
   
   DWORD nOrder;
   //#pragma chMSG("для скорости проверить без него")
   
   if ( m_nFMSlotChanges != m_FM->m_nSlotChanges ) {
      if ( !WaitForSlot() ) {
         //            AfxMessageBox("!WaitForSlot", MB_TOPMOST);
         return FALSE;
      }
      // нехорошо. Надо ждать MutexSlot и MutexServer INFINITE
      m_nFMSlotChanges = m_FM->m_nSlotChanges;
      BOOL bOK = m_FM->GetLastWriteOrderForPID_m(PID, nRecords, &nOrder); 
      m_MutexSlot.Release();
      if ( !bOK ) return FALSE;
   } else {
      if ( !m_FM->GetLastWriteOrderForPID_m(PID, nRecords, &nOrder) ) {
         return FALSE;
      }
   }
   
   /*
   может сбиться порядок записей!!!
   тут может упасть
    или при чтении вернет нулевую длину
    тогда GetLastRead нужно выполнить еще раз для того же
   */
   for ( int i=0; i<nRecords && nLength ;i++ ) {
      int idx = GetFreeBufferIndex();
      if ( idx < 0 ) return FALSE;
      
      BYTE *buf = m_FM->Data[idx];
      TFMPool::TDirectory *dir = &m_FM->Dir[idx];
      
      dir->nOrder = nOrder + i;
      dir->PIDfrom = m_PID;
      
      DWORD nLen;    // шаг по данным
      DWORD nCurLen; // размер текущего буфера
      if ( !i ) {  // в первый кусок пишем заголовок с длиной
         memcpy(buf, head, 8);
         nLen = min(nLength, TFMPool::N_BUFLEN-8);
         buf += 8;
         nCurLen = nLen + 8;
      } else {
         nLen = min(nLength, TFMPool::N_BUFLEN);
         nCurLen = nLen;
      }
      memcpy(buf, pBuffer, nLen);
      dir->nLen = nCurLen;
      pBuffer = ((BYTE*)pBuffer) + nLen;
      nLength -= nLen;
      InterlockedExchange((LONG*)&dir->PID, PID);
   }
   
   // TODO     мютекс на добавление и удаление пид
   
   int idx = m_FM->FindPIDSlot(PID);
   if ( idx >= 0 ) m_AllEventsNewData[idx].Set();
   
   return TRUE;
}

BOOL CFMExchange::QuickCmdToPID( DWORD PID, DWORD nQuckCmd )
{
   
   if ( !WaitForSlot() ) return FALSE;
   BOOL bRet = m_FM->QuickCmdToPID_m(PID, nQuckCmd); 
   int idx = m_FM->FindPIDSlot(PID);
   m_MutexSlot.Release();
   if ( idx >= 0 ) m_AllEventsNewData[idx].Set();
   return bRet;
}

void CFMExchange::CloseHandles()
{
   m_nHandles = 0;
   TRY {
      m_MutexServer.Release();
      m_MutexServer.Close();
      
      if ( m_AllEventsNewData[1] ) { // server, именно 1
         m_EventNewData.Detach();
         //eBreak.Close();
         for ( int i=0; i<1+MAX_FM_OBJECTS ;i++ ) {
            m_AllEventsNewData[i].Close();
         }
      } else { // клиент
         m_AllEventsNewData[0].Close();
         m_EventNewData.Close();
      }
      m_MutexSlot.Release();
      m_MutexSlot.Close();
      m_SemaphoreData1024.Close();
      
      if ( m_FM ) { UnmapViewOfFile( (void *)m_FM ); m_FM = NULL; }
      if ( m_hFM ) { CloseHandle(m_hFM); m_hFM = 0; }
   } CATCH_ALL(e) {
      TRACE("!CloseHandles\n");
      TCHAR buf[128];
      e->GetErrorMessage(buf, 128);
      if ( AfxMessageBox(MB_ABORTRETRYIGNORE|MB_TOPMOST, "CloseHandles\n%s", buf) 
         == IDRETRY ) 
      {
         _asm int 3;
      }
   } END_CATCH_ALL;
}

void CFMExchange::Destroy()
{
   if ( m_app ) m_app->m_pxchg = NULL;
   if ( m_pMySlot ) {
      if ( m_FM ) {
         InterlockedIncrement(&m_FM->m_nSlotChanges);
         LONG n = m_FM->CleanUpDirsForPID_m(m_pMySlot->PID);
         if ( n ) m_SemaphoreData1024.Release(n);
      }
      m_pMySlot->Release_m();
      m_pMySlot = NULL;
   }
   CloseHandles();
}

BOOL CFMExchange::CreateClient( LPCTSTR szStandName /*stand name*/ )
{
   ASSERT(strlen(szStandName)<32);
   
   Destroy(); 

   CWinApp *app = AfxGetApp();
   if ( !app->IsKindOf(RUNTIME_CLASS(CStandApp))) {
      m_sCause = "!CStandApp";
      return FALSE;
   }
   m_app = (CStandApp*)app;
   m_app->m_pxchg = this;

   m_PID = GetCurrentProcessId();

   char buf[64];
   strcpy(buf, _WORLDFMX);
   strcat(buf, szStandName);
   HANDLE hFM;
   hFM = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, buf);
   if ( !hFM ) {
      m_sCause = "Cеанс моделирования не запущен, или другая версия обмена.\n";
      m_sCause += buf;

      return FALSE;
   }
   m_FM = (TFMPool*)MapViewOfFile(hFM, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TFMPool));
   CloseHandle(hFM);
   if ( !m_FM ) { 
      m_sCause = "!MapViewOfFile";  
      return FALSE; 
   }
   
   if ( !m_FM->GetServerPID() ) {
      m_sCause = "Server PID == 0";  
      return FALSE; 
   }
   
   if ( !m_MutexServer.Open(MUTEX_ALL_ACCESS, m_FM->szMutexServer) ) {
      m_sCause = "!Server mutex";
      return FALSE;
   }
   
   if ( !m_MutexSlot.Open(MUTEX_ALL_ACCESS, m_FM->szMutexSlot) ) {
      m_sCause = "!Mutex slot";
      return FALSE;
   }
   if ( !WaitForSlot() ) {
      m_sCause = "Невозможно занять свободный буфер для объекта (объектов > 62 ?)";
      return FALSE;
   }
   
   m_pMySlot = m_FM->OccupySlot_m(m_PID);

   m_MutexSlot.Release();

   if ( !m_pMySlot ) {
      m_sCause = "Нет свободных буферов для объектов (объектов > 62 ?)";
      return FALSE;
   }

   strcpy(buf, m_FM->szEventNewData_);
   int nLen = strlen(buf);
   wsprintf(buf+nLen, "%d", m_pMySlot - m_FM->Slot);  //TODO %02d
   if ( !m_EventNewData.Open(EVENT_ALL_ACCESS, buf) ) {
      m_sCause = "!NEW DATA EVENT";
      m_sCause += buf;
      return FALSE;
   }
   strcpy(buf+nLen, "0");       //TODO 00
   if ( !m_AllEventsNewData[0].Open(EVENT_ALL_ACCESS, buf) ) {
      m_sCause = "!NEW DATA EVENT [0]";
      return FALSE;
   }
   
   if ( !m_SemaphoreData1024.Open(SEMAPHORE_ALL_ACCESS, m_FM->szSemaphoreData1024) ) {
      m_sCause = "!Data semaphore";
      return FALSE;
   }

   m_hHandles[0] = m_EventNewData;
   m_hHandles[1] = m_MutexServer;
   m_nHandles = 2;
   
   return TRUE;
}

BOOL CFMExchange::CreateServer( LPCTSTR szStandName /*stand name*/ )
{
   ASSERT(strlen(szStandName)<32);
   
   Destroy(); 

   CWinApp *app = AfxGetApp();
   if ( !app->IsKindOf(RUNTIME_CLASS(CStandApp))) {
      m_sCause = "!CStandApp";
      return FALSE;
   }
   m_app = (CStandApp*)app;
   m_app->m_pxchg = this;


   m_PID = GetCurrentProcessId();
   
   char buf[64];
   strcpy(buf, _WORLDFMX);
   strcat(buf, szStandName);

   m_hFM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
      PAGE_READWRITE, 0, sizeof(TFMPool), buf);

   if ( !m_hFM ) {
      FormatMessageBox(GetLastError(), buf, MB_TOPMOST);
      m_sCause = "!WORLDFMX";
      return FALSE;
   }

   if ( GetLastError() == ERROR_ALREADY_EXISTS ){
      CloseHandle(m_hFM);
      m_hFM = 0;
      m_sCause = "!WORLDFMX: ERROR_ALREADY_EXISTS\nкто-то держит сеанс.";
//      EnumAllProcesses(m_sCause);
      return FALSE;
   }

   m_FM = (TFMPool*)MapViewOfFile(m_hFM, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TFMPool));
   if ( !m_FM ) { 
      m_sCause = "!MapViewOfFile";  
      return FALSE; 
   }
   m_FM->Clear();

   m_pMySlot = m_FM->OccupyServerSlot_m(m_PID); //для сервера можно без m_MutexSlot

   if ( !m_pMySlot ) {
      m_sCause = "!Set server slot";  
      return FALSE;
   }

   strcpy(m_FM->szMutexServer, _WORLDSIMULATION);
   strcat(m_FM->szMutexServer, szStandName);
   
   if ( !m_MutexServer.CreateNew(1, m_FM->szMutexServer) ) {
      m_sCause = "!SERVER MUTEX\nкто-то держит сеанс";
//      EnumAllProcesses(m_sCause);
      return FALSE;
   }
   
   strcpy(buf, _WORLDNEWDATA);
   strcat(buf, szStandName);
   strcat(buf, "_");    // 13.12.2018 16:50
   strcpy(m_FM->szEventNewData_, buf);
                           
   int nLen = strlen(buf);
   for ( int i=0; i<1+MAX_FM_OBJECTS ;i++ ) {
      wsprintf(buf+nLen, "%d", i);  //TODO %02d
      if ( !m_AllEventsNewData[i].CreateNew(0, 0, buf) ) {
         m_sCause = "!NEW DATA EVENT ";
         m_sCause += buf;
         return FALSE;
      }
   }
   m_EventNewData.Attach(m_AllEventsNewData[0]);
   
   strcpy(m_FM->szMutexSlot, _WORLDMUTEXSLOT);
   strcat(m_FM->szMutexSlot, szStandName);

   if ( !m_MutexSlot.CreateNew(0, m_FM->szMutexSlot) ) {
      m_sCause = "!Slot semaphore ";
      m_sCause += m_FM->szMutexSlot;
      return FALSE;
   }

   strcpy(m_FM->szSemaphoreData1024, _WORLDSEMAPHOREDATA);
   strcat(m_FM->szSemaphoreData1024, szStandName);
   
   if ( !m_SemaphoreData1024.CreateNew(TFMPool::N_DIRS, TFMPool::N_DIRS, m_FM->szSemaphoreData1024) ) {
      m_sCause = "!Data semaphore ";
      m_sCause += m_FM->szSemaphoreData1024;
      return FALSE;
   }

   m_hHandles[0] = m_EventNewData;
   m_nHandles = 1;

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
