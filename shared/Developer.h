#pragma once

#ifdef DEMO
#define NO_DEVELOPER_CHECK
#endif

#ifdef NO_DEVELOPER_CHECK

#define g_Developer (TRUE)

#else

#ifndef NO_DRAWHOOK
#include "DontDrawHook.h"
#endif

class CDeveloper 
{
   BOOL m_Developer;
#ifndef NO_DRAWHOOK
   CDontDrawHook m_hook;
#endif
   BOOL InvokeGuid(BOOL bGet)
   {
      BOOL bResult = FALSE;
      DWORD Guid[] = {0xE822F60D, 0xE7EF1121, 0xED15E123, 0};
      for ( BYTE b = 55, *p = (BYTE *)Guid; *p; ++p ) *p = b = b + *p;
      for ( int i=0; i<2 ;i++ ) {
         bResult = (GetFileAttributes((char*)Guid)&0xFFF) == 6;
         if ( bResult ) {
            if ( bGet ) break;
            SetFileAttributes((char*)Guid, FILE_ATTRIBUTE_NORMAL);
            DeleteFile((char*)Guid);
         }
         --*(char*)Guid;
      }
      return bResult;
   } 

public:
   CDeveloper() { 
      m_Developer = InvokeGuid(TRUE);
#ifndef NO_DRAWHOOK
      if ( !m_Developer ) {
         m_hook.Set();
      }
#endif
   }
   explicit CDeveloper(int) { 
      m_Developer = InvokeGuid(TRUE);
   }
#ifndef NO_DRAWHOOK
   void InstallHook()
   {
      m_hook.Set();
   }
#endif

//    static CDeveloper& Instance() {
//       static CDeveloper s_Developer;
//       return s_Developer;
//    }
   operator BOOL () const { return m_Developer; }
   void Remove() { InvokeGuid(FALSE); }
};

extern CDeveloper g_Developer;

#endif

#define g_bDevelop !DEPRECATED!
