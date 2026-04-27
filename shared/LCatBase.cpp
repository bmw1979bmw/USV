// LCatBase.cpp: implementation of the LCatBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef QBASE
   #include "qbase.h"
#endif
#include "LCatBase.h"
#include "structures.h"
#include "TextArchive.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL MRays::bExtend=0;

LCatBase::LCatBase()
{
   pCatalog   = NULL;
   RayFile    = NULL;
}

LCatBase::~LCatBase()
{
   DeleteContents();
}

void LCatBase::DeleteContents()
{
   LBase::DeleteContents();

   delete [] pCatalog;  pCatalog   = NULL;

   if ( RayFile ) {
      RayFile->Close();
      delete RayFile;
      RayFile = NULL;
   }

   m_bRayCreation = FALSE;


}

void * LCatBase::operator[](UINT idx)
{
   ASSERT( idx < nRecords );
   
   if ( !(dwFlags & LBF_CATALOG) ) return LBase::operator[](idx);

   ASSERT ( nRecLen == 0 ); // when CATALOG - RecLen variate
   if ( pCatalog[idx].offset == 0xFFFFFFFF ) return NULL;

   if ( !pCatalog[idx].addr ) {
      pCatalog[idx].addr = new BYTE[pCatalog[idx].nRecLen];
      RayFile->Seek( dwRayFileData + pCatalog[idx].offset, CFile::begin );
      RayFile->Read( pCatalog[idx].addr, pCatalog[idx].nRecLen );
   }

   return pCatalog[idx].addr;
}

BOOL LCatBase::CreateCatalogHead(UINT nscales, const LScale * scales, UINT ndatascales, LPCTSTR filename)
{  // создает заголовок файла кат. базы
   // включающий каталог с нулевыми значениями
   // После этой функции нужно вызвать ProcessCatCreation для заполнения каталога
   ASSERT(nscales);

   DeleteContents();

   dwVersion = LBase::VERSION;

   m_bRayCreation = TRUE;


   dwFlags = LBF_CATALOG|LBF_CATCREATION;
   nScales = nscales;
   ASSERT(pScales==NULL);
   pScales = new LScale[nscales];

   nRecLen = 0;

   int nTotal = 1;
   UINT i;
   for ( i=0; i<nScales ;i++ ) {
      pScales[i] = scales[i];
      if ( i < ndatascales ) {
         nTotal *= pScales[i].nLen;
      } else {
         pScales[i].SetType(LScale::NOACTIVE);
      }
   }
   nRecords = nTotal;
   
   CFile file;
   if (!file.Open(filename,
      CFile::modeCreate|
      CFile::modeWrite| 
      CFile::shareDenyWrite)) return FALSE;

   m_sRayFileName = file.GetFilePath();

   dwRayFileData = 0;

   CArchive ar(&file,CArchive::store);

   
   ar << (short)'BQ';
   ar << (DWORD)VERSION;
   ar << dwFlags;

   dwRayFileData += 10;
   
   ar << nScales;
   dwRayFileData += 4;
   
   for ( i=0; i<nScales ;i++ ) {
      pScales[i].Serialize(ar);
      dwRayFileData += 1+4+pScales[i].nLen*4;
   }
   ar << (nRecLen=0);
   ar << nRecords; // for verifying
   dwRayFileData += 8;
   
   for ( i=0; i<nRecords ;i++ ) {
      ar << (DWORD)0;//pCatalog[i].offset;
      ar << (DWORD)0;//pCatalog[i].nRecLen;
      dwRayFileData += 8;
   }
   ar.Close();
   file.Close();

   return TRUE;
}

BOOL LCatBase::AddRecord(UINT idx, void * rec, DWORD reclen)
{
   if ( !m_bRayCreation ) return FALSE;
   if ( pCatalog[idx].offset != 0xFFFFFFFF ) return FALSE;
   RayFile->SeekToEnd();
   pCatalog[idx].offset = (DWORD)RayFile->GetPosition();
   pCatalog[idx].nRecLen = reclen;
   RayFile->Write(rec,reclen);
   pCatalog[idx].addr = NULL;
   return TRUE;
}


BOOL LCatBase::ProcessCatCreation(BOOL bExtendRay)
{
   ASSERT(!m_sRayFileName.IsEmpty());

   {
   CFile file;
   if (!file.Open(m_sRayFileName,
      CFile::modeReadWrite|CFile::shareDenyWrite  
      )) return FALSE;

   CArchive ar(&file,CArchive::load);
   //========================
   __int16 t;
   ar.Read(&t, size16);
   if ( t != 'BQ' ) return FALSE;

   DeleteContents(); // поскольку есть неразрушенные шкалы из CreateCatHead
   LoadNew(ar); //здесь формируется RayFile
   //========================
   ar.Close();
   file.Close();
   }
   ASSERT(dwFlags&LBF_CATCREATION);

   dwFlags &= ~LBF_CATCREATION;

   if ( bExtendRay ) dwFlags |= LBF_EXTENDRAY;

   MRays::bExtend = bExtendRay;

   DWORD offs=0;
   int nf = GetScaleLen(3);
   int nRays;
   TRY {
   for ( UINT i=0; i<nRecords ;i++ ) {
      //int nRays;
      pCatalog[i].offset = offs;
      RayFile->Seek( dwRayFileData + offs, CFile::begin );
      RayFile->Read( &nRays, sizeof(int));
      if ( nRays == '\0=lN' ) {
         ASSERTM (0,_T("в файле запись 'Nl='"));
      }
      if ( nRays >= 200 ) {
         TCHAR buf[64];
         wsprintf(buf,_T("nRays=0x%X"),nRays);
         AfxMessageBox(buf);
         ASSERT(nRays < 200);
      }
      offs += pCatalog[i].nRecLen = 
      (
      MRays::getsize(nRays,nf)
      );
   }
   } CATCH (CFileException, e) {
      AfxMessageBox(_T("Плохой выходной файл"));
   } END_CATCH;


   RayFile->Seek( dwRayFileData - (nRecords*sizeof(DWORD)*2), CFile::begin );
                                  // catalog
   for ( UINT i=0; i<nRecords ;i++ ) {
      RayFile->Write(& pCatalog[i].offset, sizeof(DWORD));
      RayFile->Write(& pCatalog[i].nRecLen, sizeof(DWORD));
   }

   RayFile->Seek( 2+4, CFile::begin );
   RayFile->Write(&dwFlags, sizeof(DWORD));

   m_bRayCreation = 2; // why not?

   return TRUE;
}

void LCatBase::FreeCatalogAddrs()
{
   if ( !pCatalog ) return;
   for ( UINT i=0; i<nRecords ;i++ ) {
      delete pCatalog[i].addr;
      pCatalog[i].addr = NULL;
   }
}

void LCatBase::FreeCatalogAddr(UINT idx)
{
   if ( !pCatalog ) return;
   delete pCatalog[idx].addr;
   pCatalog[idx].addr = NULL;
}

BOOL LCatBase::DoRayProtocol(LPCTSTR lpszFilePath)
{
   CFile file;
   if ( file.Open(lpszFilePath,CFile::modeCreate|CFile::modeWrite)){
      CTextArchive ar(&file,CArchive::store);
      /////////////////////////////////////////////////////////////
      int nfreq = GetScaleLen(3);
      for ( UINT i=0; i<nRecords ;i++ ) {
         MRays *prays = GetMRays(i);
         if ( !prays ) {
            ar << "! ошибка каталога базы !\r\n";
            continue;
         }
         ar << "Nrays: " << prays->nRays << "\r\n";
         for ( int iray=0; iray<prays->nRays ;iray++ ) {
            try{
               MRays::Ray *pray = prays->getray(iray,nfreq);
               if ( dwFlags&LBF_EXTENDRAY ) {
                  ar << " No:"       << pray->nRayNum;
                  ar << " No1:"      << pray->get_nRayNum1();
                  ar << " Time:"     << pray->get_time();
                  ar << " teta_in:"  << pray->get_teta_in(); 
                  ar << " teta_out:" << pray->get_teta_out(); 
                  ar << " teta_in1:" << pray->get_teta_in1(); 
                  ar << " teta_out1:"<< pray->get_teta_out1(); 
                  ar << "\r\n  Anom: ";
                  for ( int ifreq=0; ifreq<nfreq ;ifreq++ ) {
                     ar << pray->get_anom()[ifreq] << " ";
                  }
                  ar << "\r\n";
               } else {
                  ar << " No:"       << pray->nRayNum;
                  ar << " Time:"     << pray->get_time();
                  ar << " teta_in:"  << pray->get_teta_in(); 
                  ar << " teta_out:" << pray->get_teta_out(); 
                  ar << "\r\n  Anom: ";
                  for ( int ifreq=0; ifreq<nfreq ;ifreq++ ) {
                     ar << pray->get_anom()[ifreq] << " ";
                  }
                  ar << "\r\n";
               }
            } catch (...) {
            	RESETFP();
               AfxMessageBox(_T("Ошибки при записи в файл протокола."));
               FreeCatalogAddr(i);
               goto _ex;
            }
         }
         FreeCatalogAddr(i);
      }
_ex:   
      ar.Close();
      file.Close();
   }
   return TRUE;
}


MRays * LCatBase::GetMRays(UINT idx)
{
   MRays::bExtend = (dwFlags&LBF_EXTENDRAY);
   return (MRays *)((*this)[idx]);
}

BOOL LCatBase::SaveRay(LPCTSTR lpszPathName)
{
   if ( !m_bRayCreation ) {
      AfxMessageBox(_T("Можно сохранить только один раз только что созданную базу."));
      return FALSE;
   }

   RayFile->Close();
   delete RayFile;
   RayFile = NULL;
             
   if( ::MoveFileEx(m_sRayFileName,lpszPathName,MOVEFILE_COPY_ALLOWED 
      |MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH) ) {

      m_bRayCreation = FALSE;
      ASSERT(RayFile==NULL);
      RayFile = new CFile(lpszPathName,CFile::modeRead|CFile::shareDenyNone);
      return TRUE;
   } else {
      FormatMessageBox(GetLastError());

      ASSERT(RayFile==NULL);
      RayFile = new CFile(m_sRayFileName,CFile::modeRead|CFile::shareDenyNone);
      return FALSE;
   }
}

BOOL LCatBase::SaveThinout(LPCTSTR lpszPathName)
{
   CWaitCursor _wcr;

   LCatBase SavRay;
   
   if ( nScales != 4 ) {
      AfxMessageBox("nScales != 4",MB_ICONSTOP);
      return FALSE;
   }
   
   if ( !SavRay.CreateCatalogHead(4, pScales, 3, lpszPathName) ) {
      AfxMessageBox(MB_ICONSTOP,_T("Невозможно создать заголовок в файле '%s'"), lpszPathName);
      return FALSE;
   }
   
   int nTotalRays = 0;
   int nTotalDeleted = 0;

   MRays::bExtend = 0;

   {
      CFile file;
      if (!file.Open(lpszPathName,
         CFile::modeReadWrite|CFile::shareDenyWrite  
         )) return FALSE;
      
      file.SeekToEnd();
      
      int nf = GetScaleLen(3);
      int nRays;
      
      TRY {
         for ( UINT i=0; i<nRecords ;i++ ) {
            
            MRays *mr = GetMRays(i);
            nRays = mr->nRays;
            nTotalRays += nRays;
            if ( nRays <= 1 ) {
               file.Write(mr,MRays::getsize(nRays,nf));
               continue;
            }
            
            float mintime=FLT_MAX;  // sec
            int j;
            for ( j=0; j<nRays ;j++ ) {
               MRays::Ray *pray = mr->getray(j,nf);
               if ( pray->t.time < mintime ) mintime = pray->t.time;
            }
            
            int nGood = 0;
            for ( j=0; j<nRays ;j++ ) {
               MRays::Ray *pray = mr->getray(j,nf);
               float dtime = pray->t.time - mintime;
               if ( dtime < mintime*1.5f || dtime < 2 ) nGood++;
            }
            
            if ( nGood == nRays ) {
               file.Write(mr,MRays::getsize(nRays,nf));
               continue;
            }

            nTotalDeleted += nRays-nGood;
            
            file.Write(&nGood, sizeof(int));
            for ( j=0; j<nRays ;j++ ) {
               MRays::Ray *pray = mr->getray(j,nf);
               float dtime = pray->t.time - mintime;
               if ( dtime < mintime*1.5f || dtime < 2 ) {
                  file.Write(pray, pray->getsize(nf));
                  nGood--;
               }
            }
            ASSERT(!nGood);
         }
      } CATCH (CFileException, e) {
         AfxMessageBox(_T("Плохой выходной файл"));
         return FALSE;
      } END_CATCH;
      file.Close();
   }
   
   if ( !SavRay.ProcessCatCreation() ) {
      AfxMessageBox(MB_ICONSTOP,_T("Ошибка формирования структуры файла '%s'"), lpszPathName);
      return FALSE;
   }
   
   AfxMessageBox(0,_T("Удалено лучей: %d из %d"), nTotalDeleted, nTotalRays);

   return TRUE;
}

BOOL LCatBase::Load(CArchive & ar)
{
   DeleteContents();

   __int16 t;
   ar.Read(&t, size16);

   if ( t == 'bR' ) {
      return LoadRbase(ar);
   } else if ( t == 'BQ' ) {
      return LoadNew(ar);
   } else {
      nHeadLen = t;
      if ( nHeadLen > 128 ) return FALSE;
      return LoadOld(ar);
   }
}

BOOL LCatBase::LoadRbase(CArchive &)
{
   AfxMessageBox(_T("Форматы старых версий не поддерживаются"));
   return FALSE;
}

///////////////////////////////////////////////////

BOOL LCatBase::LoadNew(CArchive & ar)
{  // и каталогизированную и обычную
   ar >> dwVersion;
   
   ar >> dwFlags;
   if ( (dwFlags & LBF_CATALOG) && dwVersion < 4 ) return FALSE;
   
   dwRayFileData = 2+4+4;

   if (!(dwFlags & LBF_CATALOG) ) {
      ar >> sHead;
   }

   if ( dwFlags & LBF_EXTRADATA ) {
      ar >> nExtraDataLen;
      dwRayFileData += 4;
      if ( nExtraDataLen ) {
         ASSERT(pExtraData==NULL);
         pExtraData = new BYTE[nExtraDataLen];
         ar.Read(pExtraData, nExtraDataLen);
         dwRayFileData += nExtraDataLen;
      }
   }

   ar >> nScales;
   dwRayFileData += 4;
   if ( nScales ) {
      ASSERT(pScales==NULL);
      pScales = new LScale[nScales];
      for ( UINT i=0; i<nScales ;i++ ) {
         pScales[i].Serialize(ar);
         dwRayFileData += 1+4+pScales[i].nLen*4;
      }
   }
   ar >> nRecLen;
   ar >> nRecords; // for verifying
   dwRayFileData += 8;

   if ( dwFlags & LBF_CATALOG ) {
      ASSERT(nRecLen==0);
      ASSERT(pCatalog==0);
      ASSERT(pCatalog==NULL);
      pCatalog = new LCatalog[nRecords];
      for ( UINT i=0; i<nRecords ;i++ ) {
         ar >> pCatalog[i].offset;
         ar >> pCatalog[i].nRecLen;
         pCatalog[i].addr = NULL;
         dwRayFileData += 8;
      }
      RayFile = ar.GetFile()->Duplicate();
      m_bRayCreation = FALSE;
   } else {
      nDataLen = nRecLen*nRecords;
      ASSERT(pData==NULL);
      pData = new BYTE[nDataLen];
      ar.Read(pData,nDataLen);
   }
   
   return TRUE;
}
