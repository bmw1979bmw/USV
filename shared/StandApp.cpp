#include "stdafx.h"
#include "StandApp.h"
#include "FMExchange.h"

IMPLEMENT_DYNAMIC(CStandApp, CWinApp);

BEGIN_MESSAGE_MAP(CStandApp, CWinApp)
//{{AFX_MSG_MAP(CStandApp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CStandApp::CStandApp()
: m_pxchg(NULL),
m_nDebugExit(0)
{
}

int CStandApp::Run() 
{
   if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
   {
      // Not launched /Embedding or /Automation, but has no main window!
      TRACE0("Warning: m_pMainWnd is NULL in CWinApp::Run - quitting application.\n");
      AfxPostQuitMessage(0);
   }

   ASSERT_VALID(this);
   
   // for tracking the idle time state
   BOOL bIdle = TRUE;
   LONG lIdleCount = 0;
   
   // acquire and dispatch messages until a WM_QUIT message is received.
   for (;;)
   {
      
      while ( bIdle &&
         !::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
      {
         if ( m_pxchg ) m_pxchg->QuickWait();
         // call OnIdle while in bIdle state
         if (!OnIdle(lIdleCount++)) 
            bIdle = FALSE; // assume "no idle" state
         if ( m_pxchg ) m_pxchg->QuickWait();
      }
      
      if ( m_nDebugExit || m_msgCur.message == WM_QUIT ) return ExitInstance();

      // phase2: pump messages while available
      while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
      {
         // pump message, but quit on WM_QUIT
         if ( m_nDebugExit 
            || m_msgCur.message == WM_QUIT 
            || !CWinThread::PumpMessage() )
            return ExitInstance();
         
         // reset "no idle" state after pumping "normal" message
         if (IsIdleMessage(&m_msgCur))
         {
            bIdle = TRUE;
            lIdleCount = 0;
            if (!OnIdle(lIdleCount++)) bIdle = FALSE;
         }
         if ( m_pxchg ) m_pxchg->QuickWait();
      }

      if ( m_pxchg ) {
         // ждем сообщения или event
         m_pxchg->LongWait();
      } else {
         // просто ждем сообщения
         WaitMessage();
      }
   }
}

BOOL CStandApp::PumpMessage() // для модальных диалогов
{
   if ( m_nDebugExit ) return FALSE; 
   if ( m_pxchg ) m_pxchg->WaitForMessage();
   BOOL bRet = CWinThread::PumpMessage();
   return bRet;
}
