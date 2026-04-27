#include "stdafx.h"
#include "StandFile.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWsgFile CStandFile::ms_Stand_grp;

LPCTSTR StandGetDefDir_(); 

// пытаться открывать GRP:  0-once(однократно), 1-always(всегда), 2-never
BOOL     CStandFile::ms_bGrpOpenAttempt = FALSE; 
int      CStandFile::ms_nGrpOpenPolicy = 0;        // присваивает модель при необходимости
FILETIME CStandFile::ms_grpLastWriteTime = {0, 0};

#define SINGLE_STAND_GRP

BOOL CStandFile::OpenGrp()    // static 
{
   if ( ms_Stand_grp.Opened() ) return TRUE;

   if ( ms_bGrpOpenAttempt ) return FALSE;    // never
   ms_bGrpOpenAttempt = TRUE;

      CString s = StandGetDefDir_();
#ifdef SINGLE_STAND_GRP
      s += "\\Stand.grp";
      WIN32_FIND_DATA wfd;
      HANDLE hFind = FindFirstFile(s, &wfd);
      if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
      FindClose(hFind);
      ms_grpLastWriteTime = wfd.ftLastWriteTime;
#else      
//       s += "\\*.grp";
//       WIN32_FIND_DATA wfd;
//       HANDLE hFind = FindFirstFile(s, &wfd);
//       if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
//       ms_grpLastWriteTime = wfd.ftLastWriteTime;
//       s = StandGetDefDir_();
//       s += '\\';
//       s += wfd.cFileName;
//       BOOL bMulty = FindNextFile(hFind, &wfd);
//       FindClose(hFind);
//       
//       if ( bMulty ) {
//          SetLastError(ERROR_ALREADY_EXISTS);
//          AfxMessageBox("Множественные grp не поддерживаются", MB_ICONSTOP);
// КАК?
//          return FALSE;
//       }
#endif
      
   return ms_Stand_grp.Open(s, 0, 1);
}

BOOL CStandFile::ReOpenGrp() // static
{
   ms_Stand_grp.Close();
   ms_bGrpOpenAttempt = FALSE; 
   ms_grpLastWriteTime.dwHighDateTime = 
      ms_grpLastWriteTime.dwLowDateTime = 0;

   return OpenGrp();
}

BOOL CStandFile::ExtractFile(LPCTSTR szFileName, CString &sOut) // static 
{
   // bNewest  - только если новее, иначе всегда ????????????????/
   
   DWORD dwCode = CodeHex(szFileName);

   sOut = StandGetDefDir_();
   char buf[16];
   wsprintf(buf, "\\%08X.tmp", dwCode);
   sOut += buf;

   WIN32_FIND_DATA wfd;
   HANDLE hFind = FindFirstFile(sOut, &wfd);
   //zz bool bFound = false;
   if ( hFind != INVALID_HANDLE_VALUE ) {
      FindClose(hFind);
      if (  ((ULARGE_INTEGER &)wfd.ftLastWriteTime).QuadPart
         == ((ULARGE_INTEGER &)ms_grpLastWriteTime).QuadPart ) 
      {
         return TRUE;
      }
      //zz bFound = true;
   }

   if ( !OpenGrp() )  return FALSE;  

   CStandFile file;
   if ( !file.Open(szFileName, CFile::modeRead) ) return FALSE;

   CFile out;
   
   if ( !out.Open(sOut, CFile::modeCreate|CFile::modeWrite) ) return FALSE;
   TRY {
      void *pBuf, *pMax;
      file.m_MemFile.SeekToBegin();
      file.m_MemFile.GetBufferPtr(CMemFile::bufferRead, 0, &pBuf, &pMax);
      out.Write(pBuf, /*file.m_pMem,*/ (UINT)file.m_MemFile.GetLength());
   } CATCH_ALL(e) {
      return FALSE;
   } END_CATCH_ALL;
   SetFileTime(out, &ms_grpLastWriteTime, &ms_grpLastWriteTime, &ms_grpLastWriteTime);
   out.Close();
   return TRUE;
}

BOOL CStandFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags,
                  CFileException* pError)
{
//    m_MemFile.Close(); уходит m_nGrowBytes = 0   // 07.05.2018 16:12
//    m_MemFile.Attach(0, 0, 1024); тогда autodelete уходит!!!

//   ASSERT(!m_pMem);
   if ( ms_nGrpOpenPolicy == 0 ) {
      if ( CFile::Open(lpszFileName, nOpenFlags, pError) ) {
         return TRUE;
      }
   }
   
   if ( !OpenGrp() )  return FALSE;  
   
   LPCTSTR name = lpszFileName;
   char cur[MAX_PATH];
   
   {
      LPCTSTR def = StandGetDefDir_();
      int ndef = strlen(def);
      GetFullPathName(name, MAX_PATH, cur, NULL);
      if ( strnicmp(cur, def, ndef) == 0 ) {
         name = cur+ndef+1;
      }
   }
   
   {
      if ( nOpenFlags&(modeWrite|modeReadWrite|modeCreate) ) return FALSE;
      
      DWORD dwCode = CodeHex(name);
      //DWORD dwLen;
      if ( !ms_Stand_grp.LoadFileCRC(dwCode, m_MemFile) ) return FALSE;
      //m_MemFile.Attach(m_pMem, dwLen);

// 19.03.2018 11:12
//       DWORD dwOff, dwLen;
//       if ( !ms_Stand_grp.FindNum(dwCode, dwOff, &dwLen) ) return FALSE;
//       if ( !dwLen ) return FALSE;
//       dwLen = (dwLen+3)&~3;
//       
//       m_pMem = new BYTE[dwLen];
//       m_MemFile.Attach(m_pMem, dwLen);
//       
//       BYTE *buf = m_pMem;
//       CFile *pFile = ms_Stand_grp.GetFile().Duplicate();
//       ASSERT(pFile);
//       pFile->Seek(dwOff, 0);
//       pFile->Read(buf, dwLen);
//       delete pFile;
//       
//       DWORD d = dwOff ^ dwCode;
//       DWORD t;
//       for ( UINT i=0; i<dwLen ;i+=4 ) {
//          t = *(DWORD*)&buf[i];
//          *(DWORD*)&buf[i] ^= d;
//          d = t;
//       }
   }
   return TRUE;
}

DWORD CStandFile::GetFileAttributes(LPCTSTR lpszFileName)
{
   if ( ms_nGrpOpenPolicy == 0 ) {
      DWORD attr = ::GetFileAttributes(lpszFileName);
      if ( attr != ~0 ) return attr;
   }

   if ( !OpenGrp() ) return (DWORD)-1;
   
   LPCTSTR name = lpszFileName;
   char cur[MAX_PATH];
   
   {
      LPCTSTR def = StandGetDefDir_();
      int ndef = strlen(def);
      GetFullPathName(name, MAX_PATH, cur, NULL);
      if ( strnicmp(cur, def, ndef) == 0 ) {
         name = cur + ndef + 1;
      }
   }
   
   DWORD dwCode = CodeHex(name);
   DWORD dwOff, dwLen;
   if ( !ms_Stand_grp.FindNum(dwCode, dwOff, &dwLen) ) return (DWORD)-1;
   
   return FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_READONLY;
}

