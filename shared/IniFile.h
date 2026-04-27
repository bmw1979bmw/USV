// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CIniValue {
public:
   CString m_FileName;
   CString m_sSection;
   LPCTSTR m_Name;
public:
   CString operator [](LPCTSTR szDefStr);
   DWORD operator [](const DWORD dwDefVal);
   int   operator [](const int   nDefVal);
   int   operator [](const UINT  nDefVal);
   float operator [](const float fDefVal);
   BOOL WriteArray(const float *pData, int nElements);
   int  ReadArray (float *pData, int nElements);
   int  ReadArrayNew (float **pData ); // с отхватом памяти
   BOOL WriteArray(int *pData, int nElements);
   int  ReadArray (int *pData, int nElements);
   int  ReadArrayNew (int **pData ); // с отхватом памяти

   BOOL Delete();
   BOOL operator =(DWORD dw);
   BOOL operator =(UINT dw) { return operator=((DWORD)dw); }
   BOOL operator =(int dw);
   BOOL operator =(float f);
   BOOL operator =(LPCTSTR str);
   operator CString(void);
   operator DWORD();
   operator float();
   operator int();
   operator UINT() { return (DWORD)*this; }
};

class CIniFile  
{
   CIniValue m_Val;
public:
   void Section(LPCTSTR szSection) {
      m_Val.m_sSection = szSection;
   }
	explicit CIniFile(LPCTSTR szFile=NULL);  // 0-для GetModuleFileName.ini
	virtual ~CIniFile();

   CIniValue &operator[] (LPCTSTR Name) 
      { m_Val.m_Name = Name; return m_Val; };
};

class CAppKeyI : public CIniFile 
{
public:
	explicit CAppKeyI(LPCTSTR szSection): CIniFile(NULL) {
		Section(szSection);
	}
};

