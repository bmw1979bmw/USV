#pragma once

#define __STANDFILE_H__

#ifdef QBASE //______doit
#define NO_STANDFILE
#endif

#ifdef NO_STANDFILE

typedef CFile CStandFile;
#pragma message("no StandFile support")

#else 

#include "WsgFile.h"
#include "CodeHex.h"

// файл из Stand.grp или одиночный

class CStandFile : public CFile 
{
#ifdef AFX_BIG_FILES
   typedef LONGLONG  LONG_BIG;
   typedef ULONGLONG DWORD_BIG;
   typedef ULONGLONG LONG_SEEK;
#else 
   typedef LONG  LONG_BIG;
   typedef DWORD DWORD_BIG;
   typedef LONG  LONG_SEEK;
#endif

   CMemFile m_MemFile;
//    BYTE *m_pMem;

   static BOOL OpenGrp();

public:   
   static BOOL ReOpenGrp();

public:
   static CWsgFile ms_Stand_grp;
   static BOOL     ms_bGrpOpenAttempt; // пытались открывать GRP
   static FILETIME ms_grpLastWriteTime;
   static int      ms_nGrpOpenPolicy; // Открывать только из grp
   //    static LPCTSTR  ms_szDefGrpFileName;
   static BOOL ExtractFile(LPCTSTR szFileName, CString &sOut);


public:
   CStandFile()//:  m_pMem(NULL)
   {
   
   }
	CStandFile(LPCTSTR lpszFileName, UINT nOpenFlags)
//      : m_pMem(NULL)
/*,      CFile(lpszFileName, nOpenFlags)*/
   {
	   ASSERT(AfxIsValidString(lpszFileName));
	   
	   m_hFile = /*(UINT_PTR)*/ hFileNull;
	   CFileException e;
	   if (!Open(lpszFileName, nOpenFlags, &e))
		   AfxThrowFileException(e.m_cause, e.m_lOsError, e.m_strFileName);
   } 

   ~CStandFile()
   {
      Close();
//      delete m_pMem;
   }

 	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

   static DWORD GetFileAttributes(LPCTSTR lpszFileName);

   virtual CFile* Duplicate() const 
   {  // для wsg не работает
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         return CFile::Duplicate();
      }
//       if ( m_MemFile.GetLength()/*  m_pMem*/ ) {
      AfxMessageBox("Do not insert raystruct to grp file!");
      AfxThrowNotSupportedException();
      //return m_MemFile.Duplicate();
   //  }
   }

	virtual LONG_SEEK Seek(LONG_BIG lOff, UINT nFrom)
   {  
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         return CFile::Seek(lOff, nFrom);
      }
//       if ( m_MemFile.GetLength()/*m_pMem*/ ) {
      return m_MemFile.Seek(lOff, nFrom);
//       }
   }

	virtual void SetLength(DWORD_BIG dwNewLen)
   {  // для wsg не работает
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         CFile::SetLength(dwNewLen);
         return;
      }
//       if ( m_MemFile.GetLength() /*m_pMem*/ ) {
      AfxMessageBox("SetLength is not supported for grp file!");
      //AfxThrowFileException(CFileException::generic);
      AfxThrowNotSupportedException();
//       }
   }
	virtual DWORD_BIG GetLength() const
   {  
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         return CFile::GetLength();
      }
      //if ( m_MemFile.GetLength() /*m_pMem*/ ) {
      return m_MemFile.GetLength();
     // }
   }

	virtual UINT Read(void* lpBuf, UINT nCount)
   {
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         return CFile::Read(lpBuf, nCount);
      }
      //if ( m_MemFile.GetLength() /*m_pMem*/ ) {
      return m_MemFile.Read(lpBuf,nCount);
      //}
   }

	virtual void Write(const void* lpBuf, UINT nCount)
   {  // для wsg не работает
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         CFile::Write(lpBuf, nCount);
         return;
      }
      //if ( m_MemFile.GetLength() /*m_pMem*/ ) {
      AfxMessageBox("Write is not supported for grp file!");
      //AfxThrowFileException(CFileException::generic);
      AfxThrowNotSupportedException();
   }

   virtual void Close()
   {
      if ( m_hFile != INVALID_HANDLE_VALUE ) {
         CFile::Close();
         return ;
      } 
//       if ( m_MemFile.GetLength() /*m_pMem*/ ) {
//          delete m_pMem; // если было Duplicate?
//          m_pMem = NULL;
      m_MemFile.Close();
//       } else {
//          CFile::Close();
   }

};

#endif // NO_STANDFILE