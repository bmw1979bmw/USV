#pragma once

// stand directory 
// Singleton! 
// *no thread safe creation

class CStandDefDir 
{
   CString m_sStandDir; // нет косой на конце!
   CString m_sStandName;
   BOOL m_bCorrectPath;
   CString m_sLastWsName;

   CStandDefDir()
   {
      m_bCorrectPath = TRUE;
      TCHAR buf[MAX_PATH];
      DWORD d = GetModuleFileName(NULL, buf, MAX_PATH);
      if ( d && GetLongPathName(buf,buf,MAX_PATH) ) {  
         if ( buf[1] != ':' ) {
            m_bCorrectPath = FALSE;
         } else {
            TCHAR c = buf[3];
            buf[3] = 0;
            UINT ntype = GetDriveType(buf);
            buf[3] = c;
            if ( ntype != DRIVE_FIXED ) {
               m_bCorrectPath = FALSE;
            }
         }

         char *p = strrchr(buf,'\\');
         if ( p ) {
            strcpy(p+1,"StandDir.cfg");
            CFile f; 
            CString s;
            if ( f.Open(buf,CFile::modeRead|CFile::shareDenyNone)){
               int t = min((DWORD)f.GetLength(),MAX_PATH);
               if ( t > 0 ) {
                  f.Read(buf,t);
                  while ( t > 0 && buf[t-1] <= 0x20 ) t--;
                  buf[t]=0;
               }
            } else {
               *p = 0;
            }
            m_sStandDir = buf;
            p = strrchr(buf,'\\');
            if ( p ) {
               // добавлено по причине неправильного вычисления
               // GetLongPathName - имя папки не меняется на оригинальний регистр
               // поэтому ищем эту папку на диске и берем ее имя как есть
               WIN32_FIND_DATA wfd;
               HANDLE h = FindFirstFile(buf,&wfd);
               if ( h != INVALID_HANDLE_VALUE ) {
                  m_sStandName = wfd.cFileName;
                  FindClose(h);
               } else {
                  m_sStandName = p+1;
               }
              // AfxMessageBox(m_sStandName);
            }
         }
      }
      m_sLastWsName = m_sStandDir + "\\last_ws_" + m_sStandName;

      m_bCorrectPath = m_bCorrectPath 
         && (m_sStandName.GetLength() <= MAX_STAND_NAMELEN);
   }
   //virtual ~CStandDefDir();
   CStandDefDir(const CStandDefDir& src);  // no impl
   CStandDefDir& operator=(const CStandDefDir& src);  // no impl

public:
   static CStandDefDir& Instance() {
      static CStandDefDir s_StandDefDir;
      return s_StandDefDir;
   }

   BOOL IsEmpty() { return m_sStandDir.IsEmpty(); }

   BOOL SetDefDir()
   { 
      if ( m_sStandDir.IsEmpty() ) return FALSE;
      if ( SetCurrentDirectory(m_sStandDir) ) return TRUE;
      FormatMessageBox(GetLastError(),"StandDir: " + m_sStandDir);
      return FALSE;
   }
   const CString &GetName() { return m_sStandName; }
   const CString &GetLastWsName() { return m_sLastWsName; }
   const CString &GetDefDir() { return m_sStandDir; }
   BOOL IsCorrectPath() { return m_bCorrectPath; }
};

//extern CStandDefDir g_StandDefDir;
#define g_StandDefDir (CStandDefDir::Instance())

// inline BOOL StandGetDefDir(CString &s){ return g_StandDefDir.GetDefDir(s); }
inline const CString &StandGetDefDir(){ return g_StandDefDir.GetDefDir(); }
LPCTSTR StandGetDefDir_(); // extern
inline BOOL StandSetDefDir() { return g_StandDefDir.SetDefDir(); } 
inline const CString &StandGetName() { return g_StandDefDir.GetName(); } 
inline const CString &StandGetLastWsName() { return g_StandDefDir.GetLastWsName(); } 


///////