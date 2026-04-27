// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile(LPCTSTR szFile)
{
   if ( szFile ) {
      m_Val.m_FileName = szFile;
   } else {
      m_Val.m_FileName = AfxGetApp()->m_pszHelpFilePath;
//      GetModuleFileName(NULL,m_Val.m_FileName.GetBuffer(MAX_PATH),MAX_PATH);
//      m_Val.m_FileName.ReleaseBuffer();
      int t = m_Val.m_FileName.ReverseFind('.');
      if ( t != -1 ) {
         m_Val.m_FileName.GetBufferSetLength(t);
         m_Val.m_FileName.ReleaseBuffer();
      }
      m_Val.m_FileName+=_T(".ini");
   }
}

CIniFile::~CIniFile()
{

}

CIniValue::operator DWORD()
{
   return GetPrivateProfileInt(m_sSection,m_Name,0,m_FileName);
}

CIniValue::operator float()
{
   CString s;
   GetPrivateProfileString(m_sSection,m_Name,"0",s.GetBuffer(64),64,m_FileName);
   s.ReleaseBuffer();
   return (float)_tcstod(s,NULL);
}

CIniValue::operator int()
{
   return (*this)[(int)0];
}

DWORD CIniValue::operator [](const DWORD dwDefVal)
{
   return GetPrivateProfileInt(m_sSection,m_Name,dwDefVal,m_FileName);
}

float CIniValue::operator [](const float fDefVal)
{
   CString s;
   TCHAR t[64];
   sprintf(t,"%g",fDefVal);
   GetPrivateProfileString(m_sSection,m_Name,t,s.GetBuffer(64),64,m_FileName);
   s.ReleaseBuffer();
   return (float)_tcstod(s,NULL);
}

int CIniValue::operator [](const int nDefVal)
{
   return GetPrivateProfileInt(m_sSection,m_Name,nDefVal,m_FileName);
}

int CIniValue::operator [](const UINT nDefVal)
{
   return GetPrivateProfileInt(m_sSection,m_Name,nDefVal,m_FileName);
}

CString CIniValue::operator [](LPCTSTR szDefStr)
{
   CString s;
   GetPrivateProfileString(m_sSection,m_Name,szDefStr,
      s.GetBuffer(2048),2048,m_FileName);
   s.ReleaseBuffer();
   return s;
}

CIniValue::operator CString (void)
{
   return CIniValue::operator[]("");
}

BOOL CIniValue::operator =(LPCTSTR str)
{
   return WritePrivateProfileString(m_sSection,m_Name,str,m_FileName);;
}

BOOL CIniValue::operator =(DWORD dwValue)
{
   TCHAR t[16];
   wsprintf(t,"%u",dwValue);
   return WritePrivateProfileString(m_sSection,m_Name,t,m_FileName);
}

BOOL CIniValue::operator =(int nValue)
{
   TCHAR t[16];
   wsprintf(t,"%d",nValue);
   return WritePrivateProfileString(m_sSection,m_Name,t,m_FileName);
}

BOOL CIniValue::operator =(float fValue)
{
   TCHAR t[64];
   sprintf(t,"%g",fValue);
   return WritePrivateProfileString(m_sSection,m_Name,t,m_FileName);
}

BOOL CIniValue::WriteArray(const float * pData, int nElements)
{
   CString s;
   TCHAR buf[64];
   s.Format("%d ",nElements);
   for ( int i=0; i<nElements; i++ ) {
      sprintf(buf,"%g ",pData[i]);
      s+=buf;
   }
   return (*this)=s;
}

int CIniValue::ReadArray(float * pData, int nElementsMax)
{
   CString s = *this;
   if ( s.IsEmpty() ) return 0;

   int nElements=0;

   LPTSTR sz = (LPTSTR)(LPCTSTR)s;
   LPTSTR szTok;
   szTok = _tcstok(sz," ");
   nElements = _tcstol(szTok,NULL,0);
   szTok = _tcstok(NULL," ");
   for ( int i=0; i<nElements && i < nElementsMax ; i++ ) {
      if ( szTok ) {
         pData[i] = (float)_tcstod(szTok,NULL);
         szTok = _tcstok(NULL," ");
      }
      else pData[i]=0;
   }
   return nElements;
}

int CIniValue::ReadArrayNew(float **pData)
{
   *pData = NULL;
   CString s = *this;
   if ( s.IsEmpty() ) return 0;

   int nElements=0;

   LPTSTR sz = (LPTSTR)(LPCTSTR)s;
   LPTSTR szTok;
   szTok = _tcstok(sz," ");
   nElements = _tcstol(szTok,NULL,0);
   if ( !nElements ) return 0;
   *pData = new float[nElements];
   szTok = _tcstok(NULL," ");
   for ( int i=0; i<nElements ; i++ ) {
      if ( szTok ) {
         (*pData)[i] = (float)_tcstod(szTok,NULL);
         szTok = _tcstok(NULL," ");
      }
      else (*pData)[i]=0;
   }
   return nElements;
}

int CIniValue::WriteArray(int * pData, int nElements)
{
   CString s;
   TCHAR buf[17];
   s.Format("%d ",nElements);
   for ( int i=0; i<nElements; i++ ) {
      wsprintf(buf,"%d ",pData[i]);
      s+=buf;
   }
   return (*this)=s;
}

int CIniValue::ReadArray(int *pData, int nElementsMax)
{
   CString s = *this;
   if ( s.IsEmpty() ) return 0;

   int nElements=0;

   LPTSTR sz = (LPTSTR)(LPCTSTR)s;
   LPTSTR szTok;
   szTok = _tcstok(sz," ");
   nElements = _tcstol(szTok,NULL,0);
   szTok = _tcstok(NULL," ");
   for ( int i=0; i<nElements && i < nElementsMax ; i++ ) {
      if ( szTok ) {
         pData[i] = _tcstol(szTok,NULL,0);
         szTok = _tcstok(NULL," ");
      }
      else pData[i]=0;
   }
   return nElements;
}


int CIniValue::ReadArrayNew(int **pData)
{
   *pData = NULL;
   CString s = *this;
   if ( s.IsEmpty() ) return 0;

   int nElements=0;

   LPTSTR sz = (LPTSTR)(LPCTSTR)s;
   LPTSTR szTok;
   szTok = _tcstok(sz," ");
   nElements = _tcstol(szTok,NULL,0);
   if ( !nElements ) return 0;
   *pData = new int[nElements];
   szTok = _tcstok(NULL," ");
   for ( int i=0; i<nElements ; i++ ) {
      if ( szTok ) {
         (*pData)[i] = _tcstol(szTok,NULL,0);
         szTok = _tcstok(NULL," ");
      }
      else (*pData)[i]=0;
   }
   return nElements;
}
