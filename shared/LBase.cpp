// LBase.cpp: implementation of the LBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "structures.h"
#include "TextArchive.h"

#include "LScale.h"
#include "LSubCube.h"
#include "LBase.h"
#include "GoodFunc.h"

#include "StandFile.h"

#ifdef QBASE
#include "BaseInfoDlg.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LBase::LBase()
{
   pCube      = NULL;
   pExtraData = NULL;

   DeleteContents();
}

void LBase::DeleteContents()
{
   LCube::DeleteContents();

   sHead.Empty();
   nHeadLen = 0;

   delete [] pCube;     nSubCubes   = 0;   pCube    = NULL;

   delete pExtraData;   pExtraData = NULL; nExtraDataLen = 0;

   dwFlags = 0;
   dwVersion = 0;
   m_bWasConverted = false;

}

LBase::~LBase()
{
   DeleteContents();
}

void LBase::Serialize(CArchive & ar)
{
   if (ar.IsStoring()) {
      if ( !Save(ar) )
         AfxThrowArchiveException(CArchiveException::badIndex,
            ar.GetFile()->GetFileName());
   } else {
      if ( !Load(ar) )
         AfxThrowArchiveException(CArchiveException::badIndex,
            ar.GetFile()->GetFileName());
   }
}


BOOL LBase::Load(CArchive & ar)
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


BOOL LBase::LoadNew(CArchive & ar)
{  // обычную только
   ar >> dwVersion;
   
   ar >> dwFlags;
   ASSERT ( !(dwFlags & LBF_CATALOG) );
   
   if (!(dwFlags & LBF_CATALOG) ) {
      ar >> sHead;
   }

   if ( dwFlags & LBF_EXTRADATA ) {
      ar >> nExtraDataLen;
      if ( nExtraDataLen ) {
         pExtraData = new BYTE[nExtraDataLen];
         ar.Read(pExtraData, nExtraDataLen);
      }
   }

   ar >> nScales;
   if ( nScales ) {
      pScales = new LScale[nScales];
      for ( UINT i=0; i<nScales ;i++ ) {
         pScales[i].Serialize(ar);
      }
   }
   ar >> nRecLen;
   ar >> nRecords; // for verifying

   {
      nDataLen = nRecLen*nRecords;
      pData = new BYTE[nDataLen];
      ar.Read(pData,nDataLen);
   }
   
   return TRUE;
}


BOOL LBase::Save(CArchive & ar)
{
   ASSERT (!(dwFlags & LBF_CATALOG) ); // каталогизированные базы тут не сохранять!
   
   ar << (short)'BQ';
   ar << (DWORD)VERSION;
   ar << dwFlags;

   ar << sHead;

   
   if ( dwFlags & LBF_EXTRADATA ) {
      ar << nExtraDataLen;
      if ( nExtraDataLen ) {
         ar.Write(pExtraData, nExtraDataLen);
      }
   }

   ar << nScales;
   for ( UINT i=0; i<nScales ;i++ ) {
      pScales[i].Serialize(ar);
   }
   ar << nRecLen;
   ar << nRecords; // for verifying

   ar.Write(pData,nDataLen); // пишем данные

   m_bWasConverted = false;

   return TRUE;
}

BOOL LBase::LoadOld(CArchive & ar)
{
   UINT i;
   //----------------------------
   
   bool OLD_CONVERT = 
#ifdef QBASE
      (AfxMessageBox(IDS_DOCONVERT,MB_YESNO)==IDYES);
#else
   TRUE;
#endif
   
   m_bWasConverted = OLD_CONVERT;
   
   if ( nHeadLen ) {
      BYTE *pHead;
      pHead = new BYTE[nHeadLen];
      ar.Read(pHead, nHeadLen);
      sHead = pHead;
      delete [] pHead;
   } else {
      sHead.Empty();
   }
   //----------------------------
   nScales = 0;
   ar.Read( &nScales, size16 );
   
   nSubCubes = 1;
   
   if (nScales) {
      ASSERT(pScales == 0);
      if ( OLD_CONVERT ) {
         pScales = new LScale[MAXSCALES]; 
      } else {
         pScales = new LScale[nScales];
      }
      for ( i=0; i<nScales ;i++ ) {
         pScales[i].ReadOld(ar);
         nSubCubes *= pScales[i].nLen;
      }
   }
   //----------------------------
   
   if ( OLD_CONVERT ) { 
      int nCubeTotal = 1;
      uint16 t,headlen,nscales,nreclen;
      ar >>  headlen;
      ASSERT(!headlen);
      ar >> nscales;
      if ( nscales ) {
         for ( i=0; i<nscales ;i++ ) {
            pScales[nScales].ReadOld(ar);
            nCubeTotal *= pScales[nScales].nLen;
            nScales++;
            ASSERT(nScales <= MAXSCALES);
         }
      }
      ar >> nreclen;
      
      nRecLen = nreclen;
      nRecords = nSubCubes*nCubeTotal;
      nDataLen = nRecords*nRecLen;
      pData = new BYTE [nDataLen];
      BYTE *p = pData;
      ar.Read(p,nCubeTotal*nRecLen);
      p += nCubeTotal*nRecLen;
      
      for ( i=1; i<nSubCubes ;i++ ) { // другие кубы
         ar >> t;
         ASSERT(!t);
         ar >> t;
         ASSERT(t == nscales);
         for ( int j=0; j<nscales ;j++ ) {
            LScale sc;
            sc.ReadOld(ar);
            ASSERT( sc == pScales[nScales-nscales+j]);
         }
         ar >> t;
         ASSERT(t==nreclen);
         ar.Read(p,nCubeTotal*nRecLen);
         p += nCubeTotal*nRecLen;
      }
      
   } else {
      
      ASSERT(!pCube);
      
      pCube = new LSubCube[nSubCubes];
#ifdef _DEBUG
      int nCubeScales=0;
#endif
      for ( i=0; i<nSubCubes ;i++ ) {
         pCube[i].Serialize(ar);
#ifdef _DEBUG
         if ( !i ) {
            nCubeScales = pCube[0].nScales;
         } else {
            ASSERT(nCubeScales == pCube[0].nScales);
         }
#endif
         
      }  
   }
   
   /////////////////////////////////////////
if ( !OLD_CONVERT ) {
#ifdef QBASE
   About();
#endif
   return FALSE;
}
   
   return TRUE;
}

BOOL LBase::LoadRbase(CArchive & ar)   // LoadRevBase!!!
{
bool OLD_CONVERT = 
#ifdef QBASE
   (AfxMessageBox(IDS_DOCONVERT,MB_YESNO)==IDYES);
#else 
   TRUE;
#endif

   m_bWasConverted = OLD_CONVERT;

   dwFlags |= LBF_EXTRADATA;
#pragma pack(push,TAnomInfo)
#pragma pack(1)
   //struct /* TAnomInfo */
   //{                  //! -2 - only calling from Load
   //   char     id[7 - 2 ];       // идентификатор БД
   //   char     baseName[20];     // имя БД
   //   char     baseFile[20];     // имя файла БД
   //   char     velFile[20];      // имя файла ВРСЗ
   //   float    wind;             // скорость ветра ,м/с   
   //   char     profName[20];     // имя типа гидрологии
   //   __int16  cLayers;          // количество слоев в профиле
   //   float    prof[30*2];       // профиль ВРСЗ
   //} 
   TRbaseHead1 Head;


   ASSERT ( sizeof(Head) == (333-7) );

   ar.Read(&Head, 5); // остаток поля id
   ar.Read(&Head, sizeof(Head));
      sHead.Format(_T("DB:%hs; File:%hs; VSSO:%hs; Wind:%f; HydType:%s; nLayers:%d"),
         Head.baseName,
         Head.baseFile,
         Head.velFile,
         Head.wind,
         Head.profName,
         (int)Head.nLayers);
   //-----------------------
   SetExtraData(&Head,sizeof(Head));

   short   unk1;
   ar >> unk1;

   //-----------------------
   nRecords = 1;
   nRecLen = sizeof(float);
   nScales = 5;
   ASSERT(pScales==NULL);
   pScales = new LScale[5];
   int16 t;
   //--------0
   ar >> t;   //Ns   Глубины
   pScales[0].Create(t);
   nRecords *= pScales[0].nLen;
   //--------3
   ar >> t;   //Nt   шкала времени
   pScales[4].Create(t);
   nRecords *= pScales[4].nLen;
   //--------1
   ar >> t;   //Nf   частоты
   pScales[1].Create(t);
   nRecords *= pScales[1].nLen;
   //--------2
   ar >> t;   //Nl   длит-ти
   pScales[2].Create(t);
   nRecords *= pScales[2].nLen;
   //--------4
   ar >> t;   //Nu  Углы
   pScales[3].Create(t);
   nRecords *= pScales[3].nLen;
   //////////////////////////////////////
   float dT;
   ar >> dT;  dT /= 2.0f;

   {  
      float m;
      ar>>m;
      ar>>m;
   }

   //0...2
   UINT i;
   for ( i=0; i<4 ;i++ ) {
      pScales[i].ReadRaw(ar);
   }
   // 3
   for ( i=0; i<pScales[4].nLen ;i++ ) {
      pScales[4].pScale[i] = dT*i;
   }
   //
   /////////////////////////////////////////////////////////
   // таперь читаем catalog 
   nDataLen = sizeof(float)*nRecords;
   ASSERT(pData==NULL);
   pData = new BYTE[nDataLen];
   ZeroMemory(pData,nDataLen);

   if (ar.Read(pData, nDataLen) != nDataLen) {
#ifdef QBASE
      AfxMessageBox(IDS_ERRFILELENGTH);
#endif
   }
   //--------------------------------
   if ( !OLD_CONVERT ) {
#ifdef QBASE
      About();
#endif
      return FALSE;
   }
   return TRUE;
#pragma pack(pop,TAnomInfo)
}

#ifndef _DEBUG
CDumpContext& CDumpContext::operator<<(float f)
{
   char szBuffer[32];
   _gcvt(f, FLT_DIG, szBuffer);
   *this << szBuffer;
   return *this;
}

CDumpContext& CDumpContext::operator<<(double d)
{
   char szBuffer[32];
   _gcvt(d, DBL_DIG, szBuffer);
   *this << szBuffer;
   return *this;
}
#endif


void LBase::Dump(CDumpContext & dc) const
{
   //dc << "HeadLen: " << nHeadLen << "\r\n";
   dc << sHead << "\r\n";
   dc << "nScales: " << (int)nScales << "\r\n";
   int i;
   for ( i=0; i<(int)nScales ;i++ ) {
      dc << "scale["<<i<<"]:---------------------\r\n";
      pScales[i].Dump(dc);
   }
   if ( pCube ) {
      dc << "nSubCubes: " << (int)nSubCubes << "\r\n";
      for ( i=0; i<(int)nSubCubes ;i++ ) {
         dc << "cube["<<i<<"]:---------------------\r\n";
         pCube[i].Dump(dc);
      }
   } else {
      dc << "nDataLen: " << nDataLen << "\r\n";
      dc << "nRecords: " << nRecords << "\r\n";
      dc << "nRecLen: " << nRecLen << "\r\n";
      
   }
}
///////////////////////////////////////////////////

void LBase::SetExtraData(const void * pdata, DWORD len)
{
   if ( pExtraData ) delete pExtraData;
   pExtraData = (void *)new BYTE[nExtraDataLen = len];
   memcpy(pExtraData,pdata, len);
}

//////////////////////////////////////////////////

void LBase::LoadFromFile(LPCTSTR filename)
{
   CStandFile file(filename, CFile::modeRead|CFile::shareDenyNone);
   LoadFromFile(&file);
   file.Close();
}

BOOL LBase::LoadFromFileIgnore(LPCTSTR filename, LPCTSTR szSubDir, BOOL bAutoRetry)
{                                 // для bAutoRetry должен быть CMsgBoxHook
   int nRes = IDRETRY;
   CString lf;
   if ( szSubDir ) { lf = szSubDir; lf += "\\"; lf += filename;}

   UINT mbFlags = bAutoRetry?
      (MB_ABORTRETRYIGNORE|MB_RIGHT|MB_DEFBUTTON2|MB_ICONEXCLAMATION)
      : (MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);

   while ( nRes == IDRETRY ) {
      CStandFile file;
      if ( !file.Open(filename, CFile::modeRead|CFile::shareDenyNone) 
         && (!szSubDir || !file.Open(lf, CFile::modeRead|CFile::shareDenyNone)) 
         ) {
         if ( szSubDir ) {
            nRes = AfxMessageBox(mbFlags,
               _T("Невозможно открыть файл:\r\n'%s'\r\nили\r\n'%s'"),
               filename,lf);
         } else {
            nRes = AfxMessageBox(mbFlags,
               _T("Невозможно открыть файл '%s'"),filename);
         }
         if ( nRes == IDABORT ) return FALSE;
         // переоткрыть grp файл. Его могут заменить!
#ifndef NO_STANDFILE
         if ( nRes == IDRETRY ) CStandFile::ReOpenGrp();
#endif
         continue;
      }

      TRY {
         LoadFromFile(&file);
         break;
      } CATCH_ALL(e) {
         TCHAR buf[128];
         e->GetErrorMessage(buf,128);
         //wsprintf(buf,_T("Ошибки при загрузке файла '%s'\nПовторить?"), e->m_strFileName);
         nRes = AfxMessageBox(buf, mbFlags);
         if ( nRes == IDABORT ) return FALSE;
#ifndef NO_STANDFILE
         if ( nRes == IDRETRY ) CStandFile::ReOpenGrp();
#endif
      } END_CATCH_ALL;
   }
   return TRUE;
}


BOOL LBase::LoadFromFileE(LPCTSTR filename,LPCTSTR szSubDir1/*,LPCTSTR szSubDir2*/)
{
   CString lf1;//,lf2;
   if ( szSubDir1 ) { lf1 = szSubDir1; lf1 += "\\"; lf1 += filename; }
//   if ( szSubDir2 ) { lf2 = szSubDir2; lf2 += "\\"; lf2 += filename; }

   CStandFile file;
   if ( !file.Open(filename, CFile::modeRead|CFile::shareDenyNone) 
      && (!szSubDir1 || !file.Open(lf1, CFile::modeRead|CFile::shareDenyNone)) 
//      && (!szSubDir2 || !file.Open(lf2, CFile::modeRead|CFile::shareDenyNone))
      ) 
      return FALSE;

   TRY {
      LoadFromFile(&file);
   } 
   CATCH_ALL(e) 
   {
      return FALSE;
   } 
   END_CATCH_ALL;
  
   return TRUE;
}

void LBase::LoadFromFile(CFile *file)
{
   CArchive ar(file, CArchive::load);
   Serialize(ar);
}

#ifdef QBASE
void LBase::About(CWnd * parent)
{
   CBaseInfoDlg dlg(parent);
   dlg.pBase = this;
   dlg.m_nScales = nScales;
   dlg.m_sHead = sHead;
   dlg.m_dwFlags = dwFlags;
   if ( dlg.DoModal() == IDOK ) {
      sHead = dlg.m_sHead;
      dwFlags = dlg.m_dwFlags;
   }
}
#endif

BOOL LBase::Create(int nscales, LScale * scales, int nreclen, BOOL bUseData)
{
   void *pdataold = pData;
   UINT ndatalen = nDataLen;
   if ( bUseData ) {
       pData = NULL;  // чтоб не удаляла
   }
   DeleteContents();

   LCube::Create(nscales,scales,nreclen);

   if ( bUseData && pdataold ) {
      memcpy(pData,pdataold,min(nDataLen,ndatalen));
      delete pdataold;
   } 
   
   dwVersion = LBase::VERSION;

   return TRUE;
}


