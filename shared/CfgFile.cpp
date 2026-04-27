#include "stdafx.h"
#include "CfgFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CCfgFileBase::Load(CFile *file)
{
   DWORD len = (DWORD)file->GetLength();
   char *pFile = new char[len+1];
   file->Read(pFile, len);
   pFile[len] = 0;
   file->Close();

   int nStr = 1;
   char *p = pFile;
   int n = len;
   while(n--) {
      if ( *p++ == '\n' ) nStr++;
   }

   p = pFile;
   const char *pe = p+len;

   int i;
   for ( i=0; i<nStr && p < pe ;i++ ) {
      while ( p < pe && ( *p == ' ' || *p == '\t' ) ) p++;

      char *szKey = p;
      char *szVal = NULL;

      while ( p < pe ) {
         if ( *p == '\n' ) {
            *p++ = 0;
            break;
         }
         if ( *p == '\t') *p = 32;
         else if ( *p == '\r') *p = 0;
         else if ( *p == '=' ) {
            if ( !szVal ) {
               *p = 0;
               szVal = p+1;
            }
         }
         p++;
      }

      if ( !szKey[0] 
         || szKey[0] == ';' 
         || !szVal 
         || szVal >= pe ) 
      {
         continue;
      } 
         
      char *c = szVal;
      while ( *c == 32 ) c++;
      szVal = c;

      c = szKey + strlen(szKey) - 1;
      while ( c > szKey && *c == 32 ) *c-- = 0;
      
      if ( *szVal ) m_Map.SetAt(szKey, szVal);
   }

   delete [] pFile;
   return TRUE;
}

void CCfgFileBase::Save(CFile *file)
{
   POSITION pos = m_Map.GetStartPosition();
   CString sKey, sVal;

   while ( pos ) {
      m_Map.GetNextAssoc(pos, sKey, sVal);
      sKey += '=' +  sVal + "\r\n";
      file->Write((LPCTSTR)sKey, sKey.GetLength());
   }

   m_bChanged = FALSE;
}

LPCTSTR CCfgFileBase::Get(LPCTSTR szKeyName, LPCTSTR szDefValue) const
{
   LPCTSTR szVal = m_Map.Lookup(szKeyName);
   return szVal ? szVal : szDefValue;
}

void CCfgFileBase::Set(LPCTSTR szKeyName, LPCTSTR szValue)
{  // main set function
   m_Map.SetAt( szKeyName, szValue );
#ifdef APP_CONFIG      //??
   if ( !m_bChanged ) QueueUserAPC(sAPCProc, GetCurrentThread(), (DWORD)this);
#endif
   m_bChanged = TRUE;
}

VOID CALLBACK CCfgFileBase::sAPCProc(ULONG_PTR dwParam)
{
   reinterpret_cast<CCfgFileBase*>(dwParam)->DoSave();
}


