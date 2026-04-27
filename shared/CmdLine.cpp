// CmdLine.cpp: implementation of the CCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCmdLine g_cmdInfo;


CCmdLine::CCmdLine()
:   
//m_dwFormId(0),
m_nID((DWORD)-1),
m_nParams(0),
m_pParams(NULL),
m_szParArr(NULL)
{
}

void CCmdLine::AddParameters(int nParams, LPCTSTR *szParArr)
{
   // добавить параметры. Для проверки, заданы ли они, смотри m_pParams[i]
   // Если параметр оканчивается на ':'или'=', m_pParams [i] будет = int после ':'
   // Вызывать только один раз и от статического szParArr!!!
   ASSERT(!m_pParams);
   m_nParams = nParams;

   m_pParams = new int[nParams];
   ZeroMemory(m_pParams, nParams*sizeof(int));
   m_szParArr = szParArr;
}

CCmdLine::~CCmdLine()
{
   delete m_pParams;
}

/////      ParseCommandLine
void CCmdLine::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
   if (bFlag) {
      if (_tcsicmp(pszParam, _T("Unregister")) == 0 ) {
         m_nShellCommand = AppUnregister;
      } else if ( pszParam[0] >= _T('0') && pszParam[0] <= _T('9') ) {
         m_nID = _tcstoul(pszParam, NULL, 0);
      } else {
         for ( int i=0; i<m_nParams ;i++ ) {
            int len = lstrlen(m_szParArr[i]);
            if ( _tcsnicmp(pszParam, m_szParArr[i], len) == 0 ) {
               TCHAR cend = m_szParArr[i][len-1];
               if ( cend == ':' || cend == '=' ) {
                  m_pParams[i] = _tcstol(&pszParam[len], 0, 0);
               } else {
                  m_pParams[i] = 1;
               }
               break;
            }
         }
      }
   } else if ( pszParam[0] == '@' ) {
      if (m_strListName.IsEmpty())
         m_strListName = &pszParam[1];
   } else {
      if (m_strFileName.IsEmpty())
         m_strFileName = pszParam;
   }

   if (bLast) {
      if (m_nShellCommand == FileNew && !m_strFileName.IsEmpty())
         m_nShellCommand = FileOpen;
   }
}

