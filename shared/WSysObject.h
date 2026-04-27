// WSysObject.h: interface for the CWSysObject class.
#pragma once

#include "structures.h"
#include "WorldSystems.h"

// базовый класс для SetMember & ImObject & CMObject
// производный от TXParams 
// содержит указатель на TWorldSystem и функции работы с ним

char *TextID(DWORD id,char *buf=NULL); // для отображения ID.POINT
 
class CWSysObject : public TXParams
{
protected:
   //CString m_sAlias;
   UINT    m_iAlias;

//    TWorldSystem * GetStaticSystem(){
//       static TWorldSystem s_pSystem;
//       return &s_pSystem;
//    }

public:
   UINT  Get_iAlias () const { return m_iAlias;/* pSystem->iAlias;*/}
   const CString &GetName () const { ASSERT(pSystem); return pSystem->sName_;}
   const CString &GetAlias_ () const { ASSERT(pSystem); return pSystem->sAlias_;}
   //const CString &GetAlias()  const { ASSERT(pSystem); return m_sAlias;}
   //const CString &GetExeName () const { ASSERT(pSystem); return pSystem->sExeName_;}
   UINT  GetClient_iAlias () const { ASSERT(pSystem); return pSystem->iAlias & IALIAS_MASK;}
   int   GetIconIdx () const { ASSERT(pSystem); return pSystem->nIconIdx;}
   int   GetIconCnt () const { ASSERT(pSystem); return pSystem->nIconCnt;}
   int   GetType    () const { ASSERT(pSystem); return pSystem->nType;}
   BOOL  IsSystem   () const { ASSERT(pSystem); return pSystem->IsSystem();}
   BOOL  IsCarrier  () const { ASSERT(pSystem); return pSystem->IsCarrier();}
   BOOL  IsCommand  () const { ASSERT(pSystem); return pSystem->IsCommand();}
   BOOL  IsEvent    () const { ASSERT(pSystem); return pSystem->IsEvent();}
   BOOL  IsGlobal   () const { ASSERT(pSystem); return pSystem->IsGlobal();}
   float GetBumpDist () const { ASSERT(pSystem); return pSystem->fBumpDist; }
   UINT  GetObjLength() const { ASSERT(pSystem); return pSystem->nLength; }
   float GetObjWidth () const { ASSERT(pSystem); return pSystem->fWidth; }
   UINT  GetObjClass () const { ASSERT(pSystem); return pSystem->nClass; }
   DWORD GetSysQuant () const { ASSERT(pSystem); return pSystem->dwQuant; }
   float GetMagMoment() const { ASSERT(pSystem); return pSystem->fMagMoment; }

   CWSysObject();
   virtual ~CWSysObject();

public:
   DWORD m_nCarrierID;
   TWorldSystem *pSystem;

   char *TextID(){ return ::TextID(m_nCarrierID); }
	BOOL SetupSystem(UINT iAlias/*LPCTSTR szAlias*/);
};

BOOL  IDText(DWORD &id, const char *buf); // для обратного преобразования ID.POINT
void AFXAPI DDX_TextID(CDataExchange* pDX, int nIDC, int& value);


// расширение для отображения дополнительных относительных параметров
class CWSysObjectEx : public CWSysObject 
{
public:
   enum {N_RELOBJIDS=4};
   
   DWORD m_nRelObjIDs[N_RELOBJIDS];  // Идs объектов, отн-но которых отображаются параметры 
   
   CWSysObjectEx() { memset(m_nRelObjIDs,0xFF,sizeof m_nRelObjIDs); }
};


//EOF