#pragma once

#ifndef NO_DRAWHOOK

class CDontDrawHook 
{
   HHOOK m_Hook;
   static LRESULT CALLBACK my_CallWndProc(
      int nCode,
      WPARAM wParam,
      LPARAM lParam
      )
   {
      CWPSTRUCT *cw = (CWPSTRUCT *)lParam;
      if ( nCode == HC_ACTION /*>= 0 */ ) {
         if ( cw->message == WM_CREATE ) {
            DWORD style = GetWindowLong( cw->hwnd, GWL_STYLE);
            if ( style & WS_CHILD ) {  
               if ( style & WS_VISIBLE )ShowWindow(cw->hwnd,SW_HIDE);
               SetWindowPos(cw->hwnd,0,1280,1024,0,0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER );
            } else {
               DestroyIcon((HICON) SetClassLong(cw->hwnd,GCL_HICON,0));  
               DestroyIcon((HICON) SetClassLong(cw->hwnd,GCL_HICONSM,0));
               SetMenu(cw->hwnd,NULL);
            }
         } else if ( cw->message == WM_PAINT ) {
            ValidateRect(cw->hwnd, NULL);
            DWORD style = GetWindowLong( cw->hwnd, GWL_STYLE);
            if( style & WS_CHILD) {
               ShowWindow(cw->hwnd,SW_HIDE);
            } else {
               if ( GetWindowTextLength(cw->hwnd) ) {
                  SetWindowText(cw->hwnd,"");
               }
            }
         }
        // return 1;
      }
      return CallNextHookEx(0, nCode, wParam, lParam);
   }
   
public:
   CDontDrawHook() : m_Hook(NULL) {}

   void Set()
   {
      m_Hook = SetWindowsHookEx(
         WH_CALLWNDPROC,
         my_CallWndProc,
         NULL,
         GetCurrentThreadId());
   }
   ~CDontDrawHook()
   {
      if ( m_Hook ) UnhookWindowsHookEx(m_Hook);
   }
};

#endif 

//EOF
