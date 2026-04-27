// CmdLine.h: interface for the CCmdLine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CCmdLine : public CCommandLineInfo  
{
public:
   CCmdLine();
   virtual ~CCmdLine();

   // добавить параметры. ƒл€ проверки, заданы ли они, смотри m_pParams[i]
   // ≈сли параметр оканчиваетс€ на ':', m_pParams [i] будет = int после ':'
   void  AddParameters(int nParams, LPCTSTR *szParArr);

public:
   virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

// user defined parameters
   LPCTSTR *m_szParArr;
   int     *m_pParams;
   int      m_nParams;
// common parameters
//__declspec(deprecated) DWORD m_dwFormId;   // obsolete  /F:1111111
   DWORD    m_nID;        //  /1
   CString  m_strListName;
};

extern CCmdLine g_cmdInfo;

