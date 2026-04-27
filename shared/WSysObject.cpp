// WSysObject.cpp: implementation of the CWSysObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WSysObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char *TextID(DWORD id, char *buf)  // для отображения ID.POINT
{
   static char _buf[32];
   if ( !buf ) buf = _buf;
   DWORD w = POINTNUM(id);
   if ( id != (DWORD)-1 && w ) { // point!
      wsprintf(buf, "%d.%d", CARRID(id), w);
   } else {
      wsprintf(buf, "%d", id);
   }
   return buf;
}

BOOL  IDText(DWORD &id, const char *buf) // для обратного преобразования ID.POINT
{
   int a;
   UINT b;
   char *ep;
   a = strtol(buf, &ep, 10);
   if ( *ep == 0 ) {
   } else if ( *ep == '.' && a > 0 ){
      b = strtoul(ep+1, &ep, 10);
      if ( b == 0 || b > 32767 ) {
         AfxMessageBox("Point number must be from 0 to 32767");
         return FALSE;
      }
      a = POINTID(a, b);
   } else {
      AfxMessageBox("Введите ID объекта или точки(ID.POINT)");
      return FALSE;            // throws exception
   }
   id = a;
   return TRUE;
}

void AFXAPI DDX_TextID(CDataExchange* pDX, int nIDC, int& value)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, 32);
      DWORD a;
      if ( !IDText(a, szBuffer) ) pDX->Fail();

      value = a;
      TextID(value, szBuffer);
		AfxSetWindowText(hWndCtrl, szBuffer);
	}
	else
	{
      TextID(value, szBuffer);
		AfxSetWindowText(hWndCtrl, szBuffer);
	}
}

CWSysObject::CWSysObject()
: pSystem(NULL), //TODO ? pSystem по умолчанию = static unknown system?
m_iAlias((UINT)-1), 
m_nCarrierID((UINT)-1) 
//: pSystem(GetStaticSystem())
{
}

CWSysObject::~CWSysObject()
{
#ifdef _DEBUG
   pSystem=NULL;
#endif
}

BOOL CWSysObject::SetupSystem(UINT iAlias /*LPCTSTR szAlias*/)
{  // если iAlias == -1 переустановить свою систему

   ASSERT(iAlias != 0xCDCDCDCD);

   if ( iAlias != (UINT)-1 ) {
      m_iAlias = iAlias;
   } else { // szAlias == NULL
      if ( m_iAlias == (UINT)-1 /*.IsEmpty()*/ ) { 
//    неверно ! для world == 0
         // если не было системы - нечего переустанавливать
         ASSERT(!pSystem);
         return TRUE;
      }
   }

   //ASSERT(!m_sAlias.IsEmpty());
   ASSERT(m_iAlias != ~0);

   pSystem = g_Systems.Lookup(m_iAlias);

   return (BOOL)pSystem;
}
