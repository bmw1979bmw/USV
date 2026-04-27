// RegsKey.h: interface for the CRegsKey class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CRegsKey  
{
   class CRegsValue {
      LPCTSTR m_Name;
      HKEY    m_hKey;
      LONG    m_lRes;
   public:
      CString operator [](LPCTSTR szDefStr);
      DWORD operator [](const DWORD dwDefVal);
      int   operator [](const int   nDefVal);
      float operator [](const float fDefVal);
      BOOL Read(void *pData, DWORD nBytes);
      BOOL ReadBuf(void *pData, DWORD nBytes);
      BOOL Write(void *pData, DWORD nBytes);
      BOOL Delete();
      BOOL operator =(DWORD dw);
      BOOL operator =(UINT dw) { return operator=((DWORD)dw); }
      BOOL operator =(int dw) { return operator=((DWORD)dw); }
      BOOL operator =(float f) { return operator = (*(DWORD*)&f); };
      BOOL operator =(LPCTSTR str);
      operator CString(void);
      operator DWORD();
      operator float();
      operator int() { return (DWORD)*this; }
      operator UINT() { return (DWORD)*this; }
   
      CRegsValue() : m_hKey(0), m_Name(NULL), m_lRes(0) {}
      void Set(HKEY hKey, LPCTSTR Name) { m_hKey=hKey; m_Name = Name; }
   
   };

   CRegsValue m_Val;
public:
   CRegsKey(){m_hKey = NULL;}
   ~CRegsKey(){Close();}

// Attributes
public:
   operator HKEY() const {return m_hKey;}
   HKEY m_hKey;

// Operations
public:
   BOOL EnumKey(CString &sName, DWORD dwIndex);
   BOOL OpenDn(LPCTSTR lpszKeyName);
   CRegsValue &operator[] (LPCTSTR Name) 
      { m_Val.Set(m_hKey, Name); return m_Val; };

   BOOL Create(HKEY hKeyParent, LPCTSTR lpszKeyName);
   BOOL CreateDn(LPCTSTR lpszKeyName); // спуститься ниже на раздел
   BOOL Open(HKEY hKeyParent, LPCTSTR lpszKeyName);
   BOOL Close();

   LONG DeleteSubKey(LPCTSTR lpszSubKey);
   LONG RecurseDeleteKey(LPCTSTR lpszKey);
};


inline BOOL CRegsKey::CRegsValue::Delete()
{
   return RegDeleteValue(m_hKey, m_Name) == ERROR_SUCCESS;
}

inline LONG CRegsKey::DeleteSubKey(LPCTSTR lpszSubKey)
{
   _ASSERTE(m_hKey != NULL);
   if ( !m_hKey ) return 0;
   return RegDeleteKey(m_hKey, lpszSubKey);
}

class CAppKey : public CRegsKey
{
public:
   explicit CAppKey(LPCTSTR szKey = NULL, BOOL bUser = TRUE );
};

