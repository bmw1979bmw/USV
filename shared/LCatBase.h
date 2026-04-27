// LCatBase.h: interface for the LCatBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "LBase.h"
#include "LCatalog.h"
////////////////////////////////////////////////////////////

#pragma warning (disable: 4200)         
class M0Rays {
public:
   int    nRays;
   struct Ray {
      int   nRayNum;
      float time;
      float teta_in;
      float teta_out;
      float anom[]; // па/па???  по доку Па^2/Па^2 !!! 21.03.05 14:27
      static int getsize(int nfreq) { return 4+4+4+4+(4*nfreq); }
   } ray;
   static int getsize(int nrays, int nfreq) { return 4+(nrays*(Ray::getsize(nfreq)));}
   Ray *getray(int nray,int nf) { return (Ray *)(((BYTE *)this)+4+nray*(Ray::getsize(nf)));}
};
/*
class MXRays { // для активной лучёвки
public:
   int    nRays;
   struct Ray{
      int   nRayNum;
      int   nRayNum1;
      float time;
      float teta_out;
      float teta_in;
      float teta_in1;
      float teta_out1;
      float anom[];
      static int getsize(int nfreq) { return (7*4) + (4*nfreq); }
   } ray;
   static int getsize(int nrays, int nfreq) { return 4+(nrays*(Ray::getsize(nfreq)));}
   Ray *getray(int nray,int nf) { return (Ray *)(((BYTE *)this)+4+nray*(Ray::getsize(nf)));}
};*/
            
class MRays 
{  // класс для доступа к базам данных обоих типов
public:
   static BOOL bExtend;
   int    nRays;
   class  Ray 
   {
   public:
      int   nRayNum;
      union {
         struct {
            float time;
            float teta_in;
            float teta_out;
            float anom[];
         };
         struct {    //x
            int   nRayNum1;
            float xtime;
            float xteta_out;
            float xteta_in;
            float teta_in1;
            float teta_out1;
            float xanom[];
         };
      } t;

   public:
      int   &get_nRayNum1() { return t.nRayNum1; }
      float &get_time()     { return (MRays::bExtend)? t.xtime:t.time;}
      float &get_teta_in()  { return (MRays::bExtend)? t.xteta_in:t.teta_in;}
      float &get_teta_out() { return (MRays::bExtend)? t.xteta_out:t.teta_out; }
      float &get_teta_in1() {ASSERT(MRays::bExtend); return t.teta_in1; }
      float &get_teta_out1(){ASSERT(MRays::bExtend); return t.teta_out1; }
      float *get_anom()     { return (MRays::bExtend)? t.xanom : t.anom; }
      static int getsize(int nfreq) { return 4+4+4+4+(4*nfreq)+((MRays::bExtend)?12:0); }
   };
   static int getsize(int nrays, int nfreq) 
      { return 4+(nrays* (Ray::getsize(nfreq)) );}

   Ray *getray(int nray,int nf) { return (Ray *)(((BYTE *)this)+4+nray*(Ray::getsize(nf)));}

};
#pragma warning (default:4200)

////////////////////////////////////////////////////////////////////////
class LCatBase : public LBase  
{
public:
   MRays * GetMRays(UINT idx);
   BOOL DoRayProtocol(LPCTSTR lpszFilePath);
   CString m_sRayFileName;
   LCatalog *pCatalog;
   CFile * RayFile;
   DWORD dwRayFileData; // begin of data
   
   BOOL m_bRayCreation;
   BOOL m_bLoaded;

public:
   virtual void DeleteContents();
   LCatBase();
   virtual ~LCatBase();

public: // methods
   BOOL CreateCatalogHead(UINT nscales, const LScale *scales, UINT ndatascales,LPCTSTR filename);
   BOOL AddRecord(UINT idx, void *rec, DWORD reclen);
   BOOL ProcessCatCreation(BOOL bExtendRay=FALSE); // эта функция для Ray базы только!
   void FreeCatalogAddrs(void);
   void FreeCatalogAddr(UINT idx);
   virtual void * operator [] (UINT idx);
   
   BOOL SaveRay(LPCTSTR lpszPathName);
   BOOL SaveThinout(LPCTSTR lpszPathName);
   BOOL LoadRbase(CArchive & ar);
//   BOOL LoadRevRbase(CArchive & ar);
   BOOL LoadNew(CArchive & ar);
   virtual BOOL Load(CArchive & ar);
};

