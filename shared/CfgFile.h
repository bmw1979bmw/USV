// CfgFile.h: interface for the CCfgFile class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CMapStringToStringEx :public CMapStringToString 
{
public:
   LPCTSTR Lookup(LPCTSTR key) const
   {
	   ASSERT_VALID(this);
	   UINT nHash;
	   const CAssoc* pAssoc = GetAssocAt(key, nHash);
	   if (pAssoc == NULL)
		   return NULL;  // not in map
	   return pAssoc->value;
   }
};

#if _MSC_VER < 1300
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
#endif


class CCfgFileBase 
{
protected:
   CMapStringToStringEx m_Map;
   CString m_Name;
   BOOL m_bChanged;
   BOOL Load(CFile *file);
   void Save(CFile *file);

public:
   CCfgFileBase() : m_bChanged(0)
   {
      
   }

   virtual ~CCfgFileBase(){}


	LPCTSTR Get(LPCTSTR szKeyName, LPCTSTR szDefValue) const;
   float GetFloat(LPCTSTR szKeyName, float fDefValue)
   { LPCTSTR s = Get(szKeyName, ""); return float((*s)?strtod(s, NULL):fDefValue); }
   int   GetInt(LPCTSTR szKeyName, int nDefValue)
   { LPCTSTR s = Get(szKeyName, ""); return (*s)?strtol(s, NULL, 0):nDefValue; }
   DWORD GetDword(LPCTSTR szKeyName, DWORD dwDefValue)
   { LPCTSTR s = Get(szKeyName, ""); return (*s)?strtoul(s, NULL, 0):dwDefValue; }

   void Set(LPCTSTR szKeyName, LPCTSTR szValue);

   void Set(LPCTSTR szKeyName, DWORD Value)
   { CString s; s.Format("%u", Value); Set(szKeyName, s); }
   void SetHex(LPCTSTR szKeyName, DWORD Value)
   { CString s; s.Format("0x%08X", Value); Set(szKeyName, s); }
   void Set(LPCTSTR szKeyName, int   Value)
   { CString s; s.Format("%d", Value); Set(szKeyName, s); }
   void Set(LPCTSTR szKeyName, float Value)
   { CString s; s.Format("%g", Value); Set(szKeyName, s); }

   void Write(LPCTSTR szKeyName, int *pData, int nCnt)
   { CString s, t; while(nCnt--) { t.Format("%d ", *pData++); s+=t; } Set(szKeyName, s); }

   BOOL Read(LPCTSTR szKeyName, int *pData, int nCnt)
   { 
      LPCTSTR s = Get(szKeyName, "");
      if ( !*s ) return FALSE;
      while ( nCnt--) { *pData++ = strtol(s, (TCHAR**)&s, 0);}; 
      return TRUE;
   }

   class CCfgValue {
      CCfgFileBase *m_pBase;
      LPCTSTR      m_szName;
   public:
      CCfgValue(): m_pBase(NULL), m_szName(NULL) {  }

   public:
      void Set(CCfgFileBase *pBase, LPCTSTR szName)
      { m_pBase=pBase; m_szName = szName; }

      CString operator [](LPCTSTR szDefStr)
         { return m_pBase->Get(m_szName, szDefStr); }
      operator CString(void)
         { return m_pBase->Get(m_szName, ""); }

      DWORD operator [](const DWORD dwDefVal)
         { return m_pBase->GetDword(m_szName, dwDefVal); }
      operator DWORD()
         { return m_pBase->GetDword(m_szName, 0); }
      operator UINT() { return (DWORD)*this; }
      
      int   operator [](const int   nDefVal)
         { return m_pBase->GetInt(m_szName, nDefVal); }
      operator int()  
         { return m_pBase->GetInt(m_szName, 0); }

      float operator [](const float fDefVal)
         { return m_pBase->GetFloat(m_szName, fDefVal); }
      operator float()
         { return m_pBase->GetFloat(m_szName, 0); }

      void operator =(DWORD dw)
         { m_pBase->Set(m_szName, dw); }
      void operator =(UINT dw) 
         { m_pBase->Set(m_szName, (DWORD)dw); }
      void operator =(int dw) 
         { m_pBase->Set(m_szName, dw); }
      void operator =(float f) 
         { m_pBase->Set(m_szName, f); }
      void operator =(LPCTSTR str)
         { m_pBase->Set(m_szName, str); }

      void Write(int *pData, int nCnt) {

         m_pBase->Write(m_szName, pData, nCnt);
      }
      BOOL Read(int *pData, int nCnt)
      {
         return m_pBase->Read(m_szName, pData, nCnt);
      }

   } m_Val;

   CCfgValue &operator[] (LPCTSTR Name) 
      { m_Val.Set(this, Name); return m_Val; };

   static VOID CALLBACK sAPCProc(
      ULONG_PTR dwParam
   );

   static VOID CALLBACK sTimerProc( 
      HWND hwnd,
      UINT uMsg,
      UINT_PTR idEvent,
      DWORD dwTime
      );

   virtual void DoSave(){}
};

template<class TFile> class CCfgFile: public CCfgFileBase
{
public:
	explicit CCfgFile(LPCTSTR szFileName)
   {
      m_Name = szFileName;
      TFile file;
      if ( file.Open(szFileName, CFile::modeRead|CFile::shareDenyNone) ) {
          Load(&file);
      }
   }
	virtual ~CCfgFile()
   {
      DoSave();
   }
   virtual void DoSave(){
      if ( m_bChanged ) {
         TFile file;
         if ( file.Open(m_Name, CFile::modeWrite|CFile::modeCreate|CFile::shareDenyNone) ) {
             Save(&file);
         }
      }
   }
};

//////////////////////////////////////////////
// config для приложения - глобальный
// Singleton!
class CAppCfg : public CCfgFile<CFile> 
{
   class CModuleName: public CString
   {
   public:
      CModuleName()
      {
         this->CString::operator = ( AfxGetApp()->m_pszHelpFilePath );
         //GetModuleFileName(NULL, GetBuffer(MAX_PATH), MAX_PATH);
         //ReleaseBuffer();
         int t = ReverseFind('.');
         if ( t != -1 ) {
            GetBufferSetLength(t);
            ReleaseBuffer();
         }
         *this += _T(".conf");
      }
   };
//public:
   CAppCfg() : CCfgFile<CFile>(CModuleName()) 
   {
   }
   CAppCfg(const CAppCfg& src); // no impl 
   CAppCfg& operator=(const CAppCfg& src); // no impl

public:
   static CAppCfg& Instance() {
      static CAppCfg s_AppCfg;
      return s_AppCfg;
   }
};

#define g_Config (CAppCfg::Instance())

