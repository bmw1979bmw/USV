// LBase.h: interface for the LBase class.
#pragma once

#include "LSubCube.h"
#include "LCube.h"
#include "LScale.h"
#include "LCatalog.h"

#pragma pack(push,Lbase)
#pragma pack(1)

#include "Types2.h"

//========================================================
#define  LBF_CATALOG 1
#define  LBF_EXTRADATA 2
#define  LBF_CATCREATION 4 /* признак, что создан заголовок */
#define  LBF_EXTENDRAY   8 /* MXRay вместо MRay */
#define  LBF_PA         16  /* если установлен, то Па, а не Па/v~Гц*/
#define  LBF_USER2      32
#define  LBF_USER3      64
#define  LBF_WBNSECTOR  128  /* использовать сектора излучения в wbn*/
#define  LBF_83SCALE    0x100 /* третьоктавная шкала */

#define LBASE_INLINE

struct TRbaseHead1
{               
   //char     id[7];       // идентификатор БД
   char     baseName[20];     // имя БД
   char     baseFile[20];     // имя файла БД
   char     velFile[20];      // имя файла ВРСЗ
   float    wind;             // скорость ветра ,м/с   
   char     profName[20];     // имя типа гидрологии
   __int16  nLayers;          // количество слоев в профиле
   float    prof[30*2];       // профиль ВРСЗ
};

// оболочка работы с файлами LCube

class LBase: public LCube  
{
protected:
   BOOL LoadOld(CArchive & ar);
   BOOL LoadNew(CArchive & ar);
   BOOL LoadRbase(CArchive & ar);  // LoadRevBase!!!
public:

   enum { MAXSCALES = 15 };
   LBase();
   virtual ~LBase();

   DWORD GetVersion() const { return dwVersion; }
   void Dump(CDumpContext& dc) const;
   virtual void DeleteContents();
   virtual BOOL  Load(CArchive &ar);
   void  Serialize(CArchive &ar);

public:

   enum { VERSION=4 };

   void SetExtraData(const void *pdata, DWORD len);
   bool m_bWasConverted;
   BOOL Create(int nscales, LScale *scales, int reclen, BOOL bUseData = FALSE);
   void About(CWnd *parent=NULL);
   void LoadFromFile(CFile *file);
   // with Exeptions
   void LoadFromFile(LPCTSTR filename);
   // без Exceptions
   BOOL LoadFromFileE(LPCTSTR filename,LPCTSTR szSubDir1=NULL/*,LPCTSTR szSubDir2=NULL*/);
   // c игнорированием
   BOOL LoadFromFileIgnore(LPCTSTR filename,LPCTSTR szSubDir=NULL, BOOL bAutoRetry=FALSE);

   DWORD dwVersion;
   DWORD dwFlags;
   
   LSubCube *pCube;      //   for old bases
   UINT     nSubCubes;   //   for old bases
   CString  sHead;
   int      nExtraDataLen;
   void    *pExtraData;
   UINT     nHeadLen;  
   
private:
   BOOL     Save(CArchive & ar);
};

#include "LPosition.h"

#pragma pack(pop,Lbase)

