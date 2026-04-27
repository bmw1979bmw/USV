// WsgFile.cpp: implementation of the CWsgFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "wssgrp.h"
#include "WsgFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWsgFile::CWsgFile():
nMin(head[4]), nMax(head[5]), rnTableItems(head[2]),
m_bTblArrayInvalid(FALSE)
{
   m_GrpSizeMb = 700;
   m_table = NULL;
   m_nDigits = 1;
}

CWsgFile::~CWsgFile()
{
   DeleteContents();
}

void CWsgFile::DeleteContents()
{
   delete m_table;
   m_table = NULL;
   ClearTblArray();
   if ( Opened() ) file.Close();
}

void CWsgFile::Close()
{
   DeleteContents();
}

void CWsgFile::ClearTblArray()
{
   for ( int i=0; i<m_TblArray.GetSize() ;i++ ) {
      delete m_TblArray[i];
   }
   m_TblArray.SetSize(0);
}

void CWsgFile::InitNewFile(DWORD nTableItems)
{
   file.SetLength(0);
   file.SeekToBegin();

   if ( !nTableItems ) nTableItems = 2000;
   head[0] = '0GSW';  // сигнатура
   head[1] = 0;       // верси€
   // размер таблицы в DWORD[2] (по 8 байт)
   head[2] = nTableItems;
   head[3] = sizeof(head);// offset последней таблицы!
   head[4] = (DWORD)-1;// мин номер сценари€ в файле
   head[5] = 0;        // макс номер сценари€ в файле
   head[6] = 0;       // -резерв
   head[7] = 0;       // -резерв
   file.Write(head, sizeof head);
   DWORD *table = new DWORD[(head[2]+1)*2];
   DWORD tblen = (head[2]+1)*2*sizeof(DWORD);
   ZeroMemory(table, tblen);
   file.Write(table, tblen);
   delete [] table;

}

BOOL CWsgFile::Open(LPCTSTR szFileName, BOOL bEnableCreate,
                    BOOL bReadOnly, DWORD nTableItems
                    )
{
   m_sOriginalName = szFileName;
   m_bCreated = FALSE;

   BOOL bOK=FALSE;
   if ( bReadOnly ) {
      ASSERT( !bEnableCreate );
      bEnableCreate = FALSE;
      bOK = file.Open(szFileName, CFile::modeRead|CFile::shareDenyNone);
   } else {
      if ( bEnableCreate ) {
         bOK = file.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|
            CFile::modeReadWrite|CFile::shareExclusive);
      } else {
         bOK = file.Open(szFileName, CFile::modeReadWrite|CFile::shareExclusive);
      }
   }

   if ( !bOK ) {
      TRACE("Unable to open '%s'\r\n", szFileName);
      return 0;
   }

   DWORD filelen = (DWORD)file.GetLength();

   if ( filelen == 0 ) {
      if ( !bEnableCreate ) {
         TRACE("File too small '%s'\r\n", szFileName);
         return 0;
      }
      m_bCreated = TRUE;
      InitNewFile(nTableItems);
   } else if ( filelen < sizeof(head) ) {
      TRACE("Group file too small '%s'\r\n", szFileName);
      return 0;
   } else {
      file.Read(&head, sizeof head);
      if ( head[0] != '0GSW' || head[1] != 0 || 
         head[3] > file.GetLength()-(head[2]+1)*2*sizeof(DWORD)) 
      {
         TRACE("Bad header in group file '%s'\r\n", szFileName);
         return 0;
      }
   }
   int t=0;
   int i;
   for( i=0; i<_MAX_PATH && szFileName[i]; i++ ) {
      if ( szFileName[i] == '.' && t == 0 ) {
         t = i;
      } else if ( szFileName[i] == '\\' || szFileName[i] == '/') {
         t = 0;
      }
   }
   if ( t == 0 ) t = _MAX_PATH;

   for( i=0; i<_MAX_PATH && i < t && szFileName[i] ; i++) {
      title[i] = szFileName[i];
   }
   title[i] = 0;

   return TRUE;
}

CFile & CWsgFile::GetFile()
{
   return file;
}

BOOL CWsgFile::CloseAndRenameSmart(CString *ps)
{
   file.Close();

   if ( m_nDigits <= 0 ) m_nDigits = 1;
   CString s;
   s.Format("%s.%.*u-%.*u.wss", title, m_nDigits, head[4], m_nDigits, head[5]);
   int i=1;
   while (!::MoveFile(m_sOriginalName, s) ) {
      s.Format("%s.%.*u-%.*u(%d).wss", title, m_nDigits, head[4], m_nDigits, head[5], i);
      i++;
      if ( i > 100 ) {
         return FALSE;
      }
   }
   if ( ps ) {
      *ps=s;
   }
   return TRUE;
}

void CWsgFile::AppendNum(DWORD nNum, DWORD off/*0-if filelength*/)
{
   m_bTblArrayInvalid = TRUE;
   DWORD *table;
   table = new DWORD[(head[2]+1)*2];
   DWORD tblen = (head[2]+1)*2*sizeof(DWORD);
   DWORD tblpos = head[3];
   if ( !tblpos ) tblpos = sizeof(head);
   table[1] = tblpos;
   do {
      file.Seek(tblpos = table[1], 0);
      file.Read(table, 2*sizeof(DWORD));
   } while ( table[1] );

   if ( table[0] >= head[2] ) {   // количество заполненных - все
      // записываем новую таблицу
      // записываем оффсет в старую таблицу
      file.Seek(tblpos+sizeof(DWORD), CFile::begin);
      tblpos = head[3] = (DWORD)file.GetLength();
      file.Write(&tblpos, sizeof DWORD);
      file.SeekToEnd();
      ZeroMemory(table, tblen);
      table[0] = 1; //  error was
      table[2] = nNum;
      table[3] = off ? off : (tblpos+tblen);
      file.Write(table, tblen);
   } else {
      DWORD t = table[0] + 1;
      file.Seek(tblpos, 0);
      file.Write(&t, sizeof(DWORD));
      t = off ? off : (DWORD)file.GetLength();
      file.Seek(tblpos+(table[0]+1)*2*sizeof(DWORD), 0);
      file.Write(&nNum, sizeof(DWORD));
      file.Write(&t, sizeof(DWORD));
   }

   if ( !(nNum & 0x80000000) ) {
      if ( head[4] > nNum ) head[4] = nNum;
      if ( head[5] < nNum ) head[5] = nNum;
   }

   WriteHead();

   delete [] table;
}

BOOL CWsgFile::AppendFile(DWORD nNum, void *pData, UINT nDataLen)
{
   m_bCreated = FALSE;
   AppendNum(nNum);
   file.SeekToEnd();
   file.Write(pData, nDataLen);
   return TRUE;
}

BOOL CWsgFile::AppendFile(DWORD nNum, CFile &srcfile)
{
   m_bCreated = FALSE;
   AppendNum(nNum);
   file.SeekToEnd();
   enum { BUF_SIZE = 0x10000 };
   BYTE *buf = new BYTE[BUF_SIZE];
   int nbytes;
   while ( (nbytes=srcfile.Read(buf, BUF_SIZE))>0 ) {
      file.Write(buf, nbytes);
   }
   delete [] buf;
   return TRUE;
}

BOOL CWsgFile::AppendFileCRC(DWORD nNum, CFile &srcfile, ULONGLONG *pnWritten)
{  // добавл€ем файл с кодированием буфера
   m_bCreated = FALSE;
   AppendNum(nNum);
   DWORD d = ((DWORD)file.SeekToEnd()) ^ nNum;
   enum { BUF_SIZE = 0x10000 };
   BYTE *buf = new BYTE[BUF_SIZE];
   int nbytes;
   while ( (nbytes = srcfile.Read(buf, BUF_SIZE)) > 0 ) {
      for ( int i=0; i<nbytes ;i+=4 ) {
         *(DWORD*)&buf[i] ^= d;
         d = *(DWORD*)&buf[i];
      }
      file.Write(buf, nbytes); // (nbytes+3)&~3);
      if ( pnWritten ) *pnWritten += nbytes;
   }
   delete [] buf;
   return TRUE;
}

BOOL CWsgFile::AppendFileBegin(DWORD nNum)
{  // добавить номер, следом за ней должен быть вызван
   // вывод информации в GetFile()
   m_bCreated = FALSE;
   AppendNum(nNum);
   file.SeekToEnd();
   return TRUE;
}

void CWsgFile::EnumBegin()
{
   delete m_table;
   m_tblen = (head[2]+1)*2*sizeof(DWORD);
   m_table = new DWORD[(head[2]+1)*2];
   file.Seek(sizeof(head), 0);
   file.Read(m_table, m_tblen);
   m_nIndex = 0;
}

BOOL CWsgFile::EnumNext(DWORD &nNum, DWORD &dwOff, DWORD &dwSize)
{
   if ( m_table[0] == 0 ) return FALSE;

   if ( m_nIndex < m_table[0]-1 ) { // не последний
      nNum = m_table[(m_nIndex+1)*2];
      dwOff = m_table[(m_nIndex+1)*2+1];
      dwSize = m_table[(m_nIndex+2)*2+1] - m_table[(m_nIndex+1)*2+1];
   } else { // последний
      nNum = m_table[(m_nIndex+1)*2];
      dwOff = m_table[(m_nIndex+1)*2+1];
#pragma warning(disable: 4146)
     dwSize = -m_table[(m_nIndex+1)*2+1];
#pragma warning(default: 4146)
      if ( m_table[1] ) { // off to next
         file.Seek(m_table[1], 0);
         file.Read(m_table, m_tblen);
         m_nIndex = (DWORD)-1;   // будет ноль
         dwSize += m_table[3];
      } else {
         m_table[0] = 0;
         dwSize += (DWORD)file.GetLength();
      }
   }
   m_nIndex++;
   return TRUE;
}

void CWsgFile::EnumEnd()
{
   delete m_table;
   m_table = NULL;
}

///////////////////////////////////////////////////////////////////////

BOOL CWsgFile::TruncateLast(DWORD &dwResultNum)
{
   DWORD tblpos = head[3];
   DWORD table[2];

   file.Seek(tblpos, 0);
   file.Read(table, 2*sizeof(DWORD));

   while ( table[0] == 0 ) { // empty
      if ( !RemoveLastTable() ) return FALSE;
      tblpos = head[3];
      file.Seek(tblpos, 0);
      file.Read(table, 2*sizeof(DWORD));
   }

   file.Seek(tblpos+(table[0])*2*sizeof(DWORD), 0);
   DWORD nExperimentNum;
   file.Read(&nExperimentNum, sizeof(DWORD));
   dwResultNum = nExperimentNum;
   DWORD offcut;
   file.Read(&offcut, sizeof(DWORD));
   file.SetLength(offcut);
   DWORD t = 0;
   file.Seek(tblpos+(table[0])*2*sizeof(DWORD), 0);
   file.Write(&t, sizeof(DWORD));
   file.Write(&t, sizeof(DWORD));
   
   t = table[0]-1;
   file.Seek(tblpos, 0);
   file.Write(&t, sizeof(DWORD));

   if ( t /*able[0]*/ == 0 ) {
      RemoveLastTable();
   }

   UpdateMinMax();

   WriteHead();

   return TRUE;
}

BOOL CWsgFile::RemoveLastTable()
{
   DWORD tblpos = head[3];
   DWORD table[2];
   DWORD tblfind = tblpos;

   file.Seek(tblpos = sizeof(head), 0);
_1:
   file.Read(table, 2*sizeof(DWORD));
   if ( table[1] == 0 ) {
      TRACE("Error: Offset to the next table is zero!\r\n");
      return FALSE;
   }
   if ( table[1] != tblfind ) {
      file.Seek(tblpos = table[1], 0);
      goto _1;
   }

   DWORD t = 0;
   file.Seek(tblpos+sizeof(DWORD), 0);
   file.Write(&t, sizeof(DWORD));
   file.SetLength(tblfind);
   head[3] = tblpos;

   WriteHead();

   return TRUE;
}

void CWsgFile::UpdateMinMax()
{
   DWORD nMin = (DWORD)-1, nMax = 0;

   DWORD tblpos = sizeof head;
   DWORD tblen = (head[2]+1)*2*sizeof(DWORD);
   DWORD *table = new DWORD[(head[2]+1)*2];

   while ( tblpos ) {
      file.Seek(tblpos, 0);
      file.Read(table, tblen);
      int n = table[0];
      for ( int i=0; i<n ;i++ ) {
         DWORD num = table[(i+1)*2];
         if ( nMin > num ) nMin = num;
         if ( nMax < num ) nMax = num;
      }
      tblpos = table[1];
   }

   head[4] = nMin;// мин номер сценари€ в файле
   head[5] = nMax; // макс номер сценари€ в файле

   delete [] table;
}

BOOL CWsgFile::CloseAndRenameOversize(BOOL bAdditionalCondition)
{
   // переименовать и переоткрыть
   // возвращает FALSE, если нужно переименовать, а никак
   if ( bAdditionalCondition || file.GetLength()/1024 >= m_GrpSizeMb*1024) { 
      // ограничение на длину группы
      // надо бы переименовать
      if ( !CloseAndRenameSmart() ) return FALSE;
   }
   return TRUE;
}

BOOL CWsgFile::RenameOversizeAndReopenOriginal()//CheckForRenameSmart()
{  // переименовать и переоткрыть
   // возвращает FALSE, если нужно переименовать, а никак

   if ( file.GetLength()/1024 >= m_GrpSizeMb*1024) { // ограничение на длину группы
      DWORD nTableItems = head[2]; // same size
      // надо бы переименовать
      if ( !CloseAndRenameSmart() ) return FALSE;
      if ( !Open(m_sOriginalName, TRUE, FALSE, nTableItems) ) return FALSE;
   }
   return TRUE;
}

void CWsgFile::WriteHead()
{
   file.SeekToBegin();
   file.Write(head, sizeof head);
}

BOOL CWsgFile::ExtractAll(LPCTSTR szExt)
{
   BOOL bRet = TRUE;
   EnumBegin();
   DWORD nNum, dwOff, dwSize;
   char name[128];
   CByteArray ba;
   enum { BUF_SIZE = 0x10000 };
   ba.SetSize(BUF_SIZE);
   BYTE *buf = ba.GetData();
   UINT nbytes;
   while ( EnumNext(nNum, dwOff, dwSize) ) {
      CFile f;
      wsprintf(name, "%08u%s", nNum, szExt);
      if ( f.Open(name, CFile::modeCreate|
         CFile::modeWrite|CFile::shareExclusive) ) 
      {
         file.Seek(dwOff, 0);
         while ( (nbytes = file.Read(buf, BUF_SIZE))>0 ) {
            f.Write(buf, min(nbytes, dwSize));
            if ( nbytes > dwSize ) break;
            dwSize -= nbytes;
         }
      } else {
         bRet = FALSE;
      }
   }
   EnumEnd();
   return bRet;
}

BOOL CWsgFile::ExtractNum(DWORD nNum, LPCTSTR szExt, LPCTSTR szDir)
{
   DWORD dwOff;
   DWORD dwSize;
   if ( !FindNum(nNum, dwOff, &dwSize) ) return FALSE;

   char name[MAX_PATH];
   CByteArray ba;
   enum { BUF_SIZE = 0x10000 };
   ba.SetSize(BUF_SIZE);
   BYTE *buf = ba.GetData();
   UINT nbytes;

   CFile f;
   if ( szDir && (CreateDirectory(szDir, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) ) {
      wsprintf(name, "%s\\%08u%s", szDir, nNum, szExt);
   } else {
      wsprintf(name, "%08u%s", nNum, szExt);
   }

   if ( !f.Open(name, CFile::modeCreate|
      CFile::modeWrite|CFile::shareExclusive) ) 
   {
      return FALSE;
   }
   
   file.Seek(dwOff, 0);
   while ( (nbytes = file.Read(buf, BUF_SIZE))>0 ) {
      f.Write(buf, min(nbytes, dwSize));
      if ( nbytes > dwSize ) break;
      dwSize -= nbytes;
   }

   return TRUE;
}

BOOL CWsgFile::FindNum(DWORD nNum, DWORD &dwOff, DWORD *pdwSize)
{
   if ( !Opened() ) return FALSE;

   if ( m_bTblArrayInvalid ) {
      ClearTblArray();
      m_bTblArrayInvalid = FALSE;
   }

// сначала ищем в массиве
   DWORD off=sizeof(head);
   for ( int i=0; i<m_TblArray.GetSize() ;i++ ) {
      DWORD *p = m_TblArray[i];
      UINT n = p[0];
      off = p[1]; // на след таблицу
      for ( UINT j=0; j<n ;j++ ) {
         if ( p[(j+1)*2] == nNum ) {
            dwOff =  p[(j+1)*2+1];
            if ( pdwSize ) {
               if ( j == n-1 ) { // последний в таблице
                  if ( i == m_TblArray.GetSize()-1 ) { // последн€€ загруженна€ !!!
                     if ( off == NULL ) { // последн€€ таблица в файле
                        *pdwSize =  (DWORD)file.GetLength() - dwOff;
                     } else {
                        file.Seek(off, 0);
                        DWORD *table = new DWORD[(head[2]+1)*2];
                        file.Read(table, sizeof(DWORD)*(head[2]+1)*2);
                        m_TblArray.Add(table);
                        if ( table[0] ) {
                           *pdwSize = table[3] - dwOff;
                        } else {
                           *pdwSize =  (DWORD)file.GetLength() - dwOff;
                        }
                     }
                  } else {
                     p = m_TblArray[i+1];
                     if ( p[0] ) {
                        *pdwSize = p[3] - dwOff;
                     } else {  //  пуста€ след таблица
                        *pdwSize =  (DWORD)file.GetLength() - dwOff;
                     }
                  }
               } else {
                  *pdwSize = p[(j+2)*2+1] - dwOff;
               }
            }
            return TRUE;
         }
      }
   }

// ищем, загружа€ таблицы и добавл€€ их в массив
   while ( off  ) {
      file.Seek(off, 0);
      DWORD *table = new DWORD[(head[2]+1)*2];
      file.Read(table, sizeof(DWORD)*(head[2]+1)*2);
      m_TblArray.Add(table);
      off = table[1];
      UINT n = table[0];

      for ( UINT j=0; j<n ;j++ ) {
         if ( table[(j+1)*2] == nNum ) {
            dwOff = table[(j+1)*2+1];
            if ( pdwSize ) {
               if ( j == n-1 ) { // последний в таблице
                  if ( off == NULL ) { // последн€€ таблица в файле
                     *pdwSize = (DWORD)file.GetLength() - dwOff;
                  } else {
                     file.Seek(off, 0);
                     DWORD *table = new DWORD[(head[2]+1)*2];
                     file.Read(table, sizeof(DWORD)*(head[2]+1)*2);
                     m_TblArray.Add(table);
                     if ( table[0] ) {
                        *pdwSize = table[3] - dwOff;
                     } else {
                        *pdwSize =  (DWORD)file.GetLength() - dwOff;
                     }
                  }
               } else {
                  *pdwSize = table[(j+2)*2+1] - dwOff;
               }
            }
            return TRUE;
         }
      }
   } 
   return FALSE; // not found
}

BOOL CWsgFile::LoadFileCRC(DWORD dwCode, CMemFile &memFile)
{
   DWORD dwOff, dwLen;
   if ( !FindNum(dwCode, dwOff, &dwLen) ) return FALSE;
   if ( !dwLen ) return FALSE;

   BYTE *pMem;
   void *pMax;
   memFile.SetLength(dwLen);
   memFile.SeekToBegin();
   if ( !memFile.GetBufferPtr(CMemFile::bufferWrite, (dwLen+3)&~3, (void**)&pMem, &pMax) )
      return FALSE;

   BYTE *buf = pMem;
   CFile *pFile = GetFile().Duplicate();
   ASSERT(pFile);
   pFile->Seek(dwOff, 0);
   pFile->Read(buf, dwLen);
   delete pFile;
   
   DWORD d = dwOff ^ dwCode;
   DWORD t;
   for ( UINT i=0; i<dwLen ;i+=4 ) {
      t = *(DWORD*)&buf[i];
      *(DWORD*)&buf[i] ^= d;
      d = t;
   }

   return TRUE;
}

BOOL CWsgFile::LoadFullList(CDWordArray &arr)
{  // загрузить полный список
   if ( !Opened() ) return FALSE;

   if ( m_bTblArrayInvalid ) {
      ClearTblArray();
      m_bTblArrayInvalid = FALSE;
   }

   DWORD off=sizeof(head);
// сначала ищем в массиве
   for ( int i=0; i<m_TblArray.GetSize() ;i++ ) {
      DWORD *p = m_TblArray[i];
      UINT n = p[0];
      off = p[1]; // на след таблицу
      for ( UINT j=0; j<n ;j++ ) {
         DWORD d = p[(j+1)*2];
         arr.Add(d);
      }
   }
// ищем, загружа€ таблицы и добавл€€ их в массив
   while ( off  ) {
      file.Seek(off, 0);
      int size = (head[2]+1)*2;
      DWORD *table = new DWORD[size];
      ZeroMemory(table, size*sizeof(DWORD));   
      //^ при поврежденных Read не прочитает целиком!

      file.Read(table, size*sizeof(DWORD));
      m_TblArray.Add(table);
      off = table[1];
      UINT n = table[0];
      for ( UINT j=0; j<n ;j++ ) {
         DWORD d = table[(j+1)*2];
         arr.Add(d);
      }
   }

   return TRUE;
}

class CFileExy : public CFile
{
public:
   void SetCloseOnDelete(BOOL bCloseOnDelete){m_bCloseOnDelete = bCloseOnDelete;}
   BOOL GetCloseOnDelete(){ return m_bCloseOnDelete;}
};

void CWsgFile::Duplicate(CFile &f)
{
	HANDLE hFile;
	if (!::DuplicateHandle(::GetCurrentProcess(), (HANDLE)f.m_hFile,
		::GetCurrentProcess(), &hFile, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		CFileException::ThrowOsError((LONG)::GetLastError());
	}
#ifdef AFX_BIG_FILES
   file.m_hFile = hFile;
#else
	file.m_hFile = (UINT)hFile;
#endif
	ASSERT(file.m_hFile != file.hFileNull);
	((CFileExy*)&file)->SetCloseOnDelete( ((CFileExy*)&f)->GetCloseOnDelete() );
}


DWORD CWsgFile::SeekToNum(DWORD num)
{
   if ( !Opened() ) return 0;
   DWORD off;
   if ( !FindNum(num, off) ) return 0;
   return (DWORD)file.Seek(off, 0);
}

BOOL CWsgFile::CheckSequence(CDWordArray &arr)
{
   if ( !Opened() ) return FALSE;
   CDWordArray a;
   LoadFullList(a);
   DWORD nmin = 0x7FFFFFFF;
   DWORD nmax = 0;
   int ta = a.GetSize();
   if ( !ta ) return FALSE;
   int i;
   for ( i=0; i<ta ;i++ ) {
      if ( a[i] & 0x80000000 ) continue;
      if ( nmin > a[i] ) nmin = a[i];
      if ( nmax < a[i] ) nmax = a[i];
   }
   if ( nmin > nmax ) return FALSE;
   CByteArray b;
   int t;
   b.SetSize(t = nmax - nmin + 1);
   ZeroMemory(b.GetData(), t);
   for ( i=0; i<ta ;i++ ) {
      if ( a[i] & 0x80000000 ) continue;
      b[a[i]-nmin] = 1;
   }
   int ncnt=0;
   for ( i=0; i<t ;i++ ) {
      if ( !b[i] ) ncnt++;
   }
   if ( !ncnt ) return FALSE;
   arr.SetSize(ncnt);
   int k = 0;
   for ( i=0; i<t ;i++ ) {
      if ( !b[i] ) {
         arr[k++] = i+nmin;
      }
   }
   return TRUE;
}

BOOL CWsgFile::CorrectHeaderMinMax()
{  // пересчитать мин и мах в заголовок
   CDWordArray a;
   LoadFullList(a);
   DWORD nmin = 0x7FFFFFFF;
   DWORD nmax = 0;
   int ta = a.GetSize();
   if ( !ta ) return FALSE;
   for ( int i=0; i<ta ;i++ ) {
      if ( a[i] & 0x80000000 ) continue;
      if ( nmin > a[i] ) nmin = a[i];
      if ( nmax < a[i] ) nmax = a[i];
   }
   if ( nmin == nMin && nmax == nMax ) return FALSE;
   nMin = nmin;
   nMax = nmax;
   WriteHead();
   return TRUE;
}

BOOL CWsgFile::RepairBoundaryErrors(CDWordArray &arr, BOOL bViewOnly)
{  // try locate and fix files between tables
   CDWordArray a;
   LoadFullList(a);
   int n = rnTableItems;
   BOOL bRepaired = FALSE;

   for ( int k=n-1; k+1 < a.GetSize(); k += n) {
      if ( a[k] + 2 != a[k+1] ) continue;
      // possible error
      DWORD ofk0, ofk2;
      if ( FindNum(a[k]+1, ofk0) ) continue;
      // нет его в файле
      if ( !FindNum(a[k], ofk0) || !FindNum(a[k+1], ofk2) ) continue;
      if ( ofk0 >= ofk2 ) continue;
      // найдены окружающие
      DWORD dwSignature;
      file.Seek(ofk0, 0);
      file.Read(&dwSignature, sizeof(DWORD));
      DWORD of = ofk0+4;
      if ( (dwSignature & 0xFFFFFF) != 'SSW' ) continue;
      BYTE buf[4096];
      buf[0] = buf[1] = buf[2] = 0;
      int nbytes;
      while ( (nbytes = file.Read(&buf[3], 4096-3))>0 && of < ofk2) {
         for ( int i=0; i<nbytes && of < ofk2; i++, of++ ) {
            if ( *(DWORD *)&buf[i] == dwSignature ) {
               if ( !bViewOnly ) {
                  AppendNum(a[k]+1, of-3);
               }
               arr.Add(a[k]+1);
               bRepaired = TRUE;
               goto _1;
            }
         }
         buf[0] = buf[4096-3];
         buf[1] = buf[4096-2];
         buf[2] = buf[4096-1];
      }
_1:;
   }

   return bRepaired;
}
