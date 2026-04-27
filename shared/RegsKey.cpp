// RegsKey.cpp: implementation of the CRegsKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegsKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// CRegsKey

BOOL CRegsKey::Close()
{
   LONG lRes = ERROR_SUCCESS;
   if (m_hKey != NULL)
   {
      lRes = RegCloseKey(m_hKey);
      m_hKey = NULL;
   }
   return lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::Create(HKEY hKeyParent, LPCTSTR lpszKeyName)
{
   ASSERT(hKeyParent != NULL);
   if ( !hKeyParent ) return FALSE;
   DWORD dw;
   HKEY hKey = NULL;
   LONG lRes = RegCreateKeyEx(hKeyParent, lpszKeyName, 0,
      REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
   if (lRes == ERROR_SUCCESS)
   {
      Close();    ///закрыли старый
      m_hKey = hKey;
   }
   return lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::CreateDn(LPCTSTR lpszKeyName)
{  // создать/открыть на 1 вниз от текущего
   ASSERT(m_hKey != NULL);
   if ( !m_hKey ) return FALSE;
   DWORD dw;
   HKEY hKey = NULL;
   LONG lRes = RegCreateKeyEx(m_hKey, lpszKeyName, 0,
      REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
   if (lRes == ERROR_SUCCESS)
   {
      Close();    ///закрыли старый
      m_hKey = hKey;
   }
   return lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName)
{
   ASSERT(hKeyParent != NULL);
   if ( !hKeyParent ) return FALSE;
   HKEY hKey = NULL;
   LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyName, 0, KEY_ALL_ACCESS, &hKey);
   if (lRes == ERROR_SUCCESS)
   {
      VERIFY( Close() );   ///закрыли старый
      m_hKey = hKey;
   }
   return lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::OpenDn(LPCTSTR lpszKeyName)
{  // открыть на 1 вниз от текущего
   ASSERT(m_hKey != NULL);
   if ( !m_hKey ) return FALSE;
   HKEY hKey = NULL;
   LONG lRes = RegOpenKeyEx(m_hKey, lpszKeyName, 0, KEY_ALL_ACCESS, &hKey);
   if (lRes == ERROR_SUCCESS)
   {
      VERIFY( Close() );   ///закрыли старый
      m_hKey = hKey;
   }
   return lRes == ERROR_SUCCESS;
}

LONG CRegsKey::RecurseDeleteKey(LPCTSTR lpszKey)
{
   CRegsKey key;
   LONG lRes = key.Open(m_hKey, lpszKey);

   if (lRes != ERROR_SUCCESS)  return lRes;

   FILETIME time;
   TCHAR szBuffer[256];
   DWORD dwSize = 256;
   while (RegEnumKeyEx(key.m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
      &time)==ERROR_SUCCESS)
   {
      lRes = key.RecurseDeleteKey(szBuffer);
      if (lRes != ERROR_SUCCESS) return lRes;
      dwSize = 256;
   }
   key.Close();
   return DeleteSubKey(lpszKey);
}

CRegsKey::CRegsValue::operator DWORD()
{
   DWORD dwValue = 0;
   DWORD dwType = NULL;
   DWORD dwCount = sizeof(DWORD);
   m_lRes = RegQueryValueEx(m_hKey, m_Name, NULL, &dwType,
      (LPBYTE)&dwValue, &dwCount);
   if ( m_lRes == ERROR_SUCCESS ) {
      if ( dwType != REG_DWORD || dwCount != sizeof(DWORD)) {
         m_lRes = ERROR_BAD_FORMAT;
      }
   }

   return dwValue;
}

CRegsKey::CRegsValue::operator float()
{
   DWORD dwValue = ( DWORD ) *this;
   return *(float *)&dwValue;
}

DWORD CRegsKey::CRegsValue::operator [](const DWORD dwDefVal)
{
   DWORD dw;
   dw = ( DWORD ) *this;
   if ( m_lRes != ERROR_SUCCESS ) {
      dw = dwDefVal;
   }
   return dw;
}

float CRegsKey::CRegsValue::operator [](const float fDefVal)
{
   float f;
   f = ( float ) *this;
   if ( m_lRes != ERROR_SUCCESS ) {
      f = fDefVal;
   }
   return f;
}

int CRegsKey::CRegsValue::operator [](const int nDefVal)
{
   int n;
   n = ( int ) *this;
   if ( m_lRes != ERROR_SUCCESS ) {
      n = nDefVal;
   }
   return n;
}

CString CRegsKey::CRegsValue::operator [](LPCTSTR szDefStr)
{
   CString strValue;
   strValue = ( CString ) *this;
   if ( m_lRes != ERROR_SUCCESS ) {
      strValue = szDefStr;
   }
   return strValue;
}

CRegsKey::CRegsValue::operator CString (void)
{
   DWORD dwType = NULL;
   CString strValue;
   DWORD dwCount;

   m_lRes = RegQueryValueEx(m_hKey, m_Name, NULL, &dwType,
      NULL, &dwCount);
   if (m_lRes == ERROR_SUCCESS) {
      ASSERT((dwType == REG_SZ) || (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));
      m_lRes= RegQueryValueEx(m_hKey, m_Name, NULL, &dwType,
         (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
      strValue.ReleaseBuffer();
   }
   
   return strValue;
}

BOOL CRegsKey::CRegsValue::operator =(LPCTSTR str)
{
   ASSERT(str != NULL);
   ASSERT(m_hKey != NULL);
   if ( !str ) return FALSE;
   if ( !m_hKey ) return FALSE;
   m_lRes = RegSetValueEx(m_hKey, m_Name, NULL, REG_SZ,
      (BYTE * const)str, (_tcslen(str)+1)*sizeof(TCHAR));

   return m_lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::CRegsValue::operator =(DWORD dwValue)
{
   ASSERT(m_hKey != NULL);
   if ( !m_hKey ) return FALSE;
   m_lRes = RegSetValueEx(m_hKey, m_Name, NULL, REG_DWORD,
      (BYTE * const)&dwValue, sizeof(DWORD));
   return m_lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::CRegsValue::Write(void * pData, DWORD nBytes)
{
   m_lRes = RegSetValueEx(m_hKey, m_Name, NULL, REG_BINARY,
      (LPBYTE)pData, nBytes);
   return m_lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::CRegsValue::Read(void * pData, DWORD nBytes)
{
   DWORD dwType;
   m_lRes = RegQueryValueEx(m_hKey, m_Name, NULL, &dwType,
      (LPBYTE)pData, &nBytes);
   return m_lRes == ERROR_SUCCESS;
}

BOOL CRegsKey::CRegsValue::ReadBuf(void * pData, DWORD nBytes)
{
   DWORD dwType;
   DWORD nToRead = nBytes;
   m_lRes = RegQueryValueEx(m_hKey, m_Name, NULL, &dwType,
      (LPBYTE)pData, &nBytes);
   if ( m_lRes != ERROR_SUCCESS ) return FALSE;
   if ( nToRead != nBytes ) return FALSE;
   return TRUE;
}

CAppKey::CAppKey(LPCTSTR szKey, BOOL bUser)
{
   ASSERT(AfxGetApp()->m_pszRegistryKey != NULL);
   ASSERT(AfxGetApp()->m_pszProfileName != NULL);
   if ( Open(bUser?HKEY_CURRENT_USER:HKEY_LOCAL_MACHINE, _T("Software")) ) {
      if ( CreateDn(AfxGetApp()->m_pszRegistryKey) ) {
         if (CreateDn (AfxGetApp()->m_pszProfileName ) ) {
            if ( szKey ) CreateDn(szKey);
         }
      }
   }
}

BOOL CRegsKey::EnumKey(CString & sName, DWORD dwIndex)
{
   TCHAR buf[256];
   DWORD dwLen = 256;
   if ( RegEnumKeyEx(m_hKey, dwIndex, buf, &dwLen, NULL, NULL, NULL, NULL)
      != ERROR_SUCCESS ) return FALSE;
   sName = buf;
   return TRUE;
}
