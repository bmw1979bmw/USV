#pragma once

// #define MB_HOOK   0x00200000L

// обратный отсчет на кнопке по умолчанию в MessageBox'е
// Singleton! 
class CMsgBoxHook {

   HHOOK hHook;
   HWND  hDlg;      
   int   nCount;
   TCHAR szButtonText[128];
   int   nRLen;
   UINT  nDefButton;
   UINT  nMaxCounter;
   BOOL  bBeeper;

   CMsgBoxHook()
   {
      nCount = 0;
      hHook = NULL;
      hDlg = NULL;      
      nRLen = 0;
      nDefButton = 0;
      nMaxCounter = 9;
      bBeeper = FALSE;
   }

   CMsgBoxHook(const CMsgBoxHook& src);  // no impl
   CMsgBoxHook& operator=(const CMsgBoxHook& src);  // no impl

   VOID CALLBACK TimerProc( HWND hwnd, UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
   {
      if ( hwnd != hDlg ) return;
      if ( idEvent != 1111 ) return;
         
      if ( !--nCount ) {
         EndDialog(hwnd, nDefButton);
         KillTimer(hwnd,1111);
      } else {
         wsprintf( szButtonText + nRLen, " (%d)", nCount);
         SetDlgItemText(hwnd, nDefButton, szButtonText);
         if ( bBeeper ) Beep(3520,100);
      }
   }

   static VOID CALLBACK sTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
   {
      Instance().TimerProc(hwnd,uMsg,idEvent,dwTime);
   }

   static LRESULT CALLBACK sCallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
   {
      return Instance().CallWndRetProc(nCode, wParam, lParam);
   }

   LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
   {
      if ( nCode != HC_ACTION ) return ::CallNextHookEx(0, nCode, wParam, lParam);
      CWPSTRUCT *msg = (CWPSTRUCT *)lParam;
      if ( msg->message == WM_INITDIALOG ) {
         hDlg = msg->hwnd;
         //HWND hRetry = GetDlgItem(msg->hwnd,IDBTN);
         HWND hChild = 0;
         for ( UINT i=0; i<5/*nDefButton+1*/ ;i++ ) {
            hChild = FindWindowEx(hDlg, hChild, "BUTTON",NULL);
            // можно 
            if ( !hChild ) break;
            if ( GetWindowLong(hChild, GWL_STYLE) & BS_DEFPUSHBUTTON ) break;
         }
         if ( hChild ) {
            nDefButton = GetDlgCtrlID(hChild);
            nCount = nMaxCounter;
            nRLen = GetWindowText(hChild,szButtonText,128);
            wsprintf( szButtonText + nRLen, " (%d)", nCount);
            SetWindowText(hChild,szButtonText);
            SetTimer(msg->hwnd, 1111, 1000, sTimerProc);
         }
         ::UnhookWindowsHookEx(hHook);
         hHook = NULL;
      }
   
      return ::CallNextHookEx(0, nCode, wParam, lParam);
   }

public:
   static CMsgBoxHook& Instance() {
      static CMsgBoxHook s_MsgBoxHook;
      return s_MsgBoxHook;
   }

   int DoMessageBox(CWinApp *pApp, LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
   {
      hDlg = NULL;
      if ( nType & MB_RIGHT ) {
         nType &= ~MB_RIGHT;
         nDefButton = (nType&MB_DEFBUTTON4)>>8;
         hHook = ::SetWindowsHookEx( 
            WH_CALLWNDPROC, sCallWndRetProc, NULL, ::GetCurrentThreadId());
         if ( !(nType & MB_HELP) ) {
            if ( nIDPrompt ) {
               // рассматриваем nIDprompt как задержку 
               // если в ней MB_HELP, то со звуковым сигналом
               if ( nIDPrompt & MB_HELP ) {
                  nIDPrompt &= ~MB_HELP;
                  bBeeper = TRUE;
               }
               nMaxCounter = nIDPrompt;
               nIDPrompt = 0;
            }
         }
      }
      int nRet = pApp->CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
      if (hHook) { 
         ::UnhookWindowsHookEx(hHook); 
         hHook = NULL;
      }
      return nRet;
   }

};
#define g_MsgHook (CMsgBoxHook::Instance())

// как пользоваться 
/* переопределить виртуальную функцию в приложении 

int CMainApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt) 
{
   return g_MsgHook.DoMessageBox(this,lpszPrompt, nType, nIDPrompt);
}

в вызове AfxMessageBox поставить флаги 
	MB_RIGHT 
	и один из: MB_DEFBUTTON1 MB_DEFBUTTON2 MB_DEFBUTTON3 MB_DEFBUTTON4

*/



