// GoodFunc.cpp: implementation of the GoodFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GoodFunc.h"
#include "RegsKey.h"

// определите NO_STANDFILE для использования функций без этого класса
#ifdef NO_STANDFILE
typedef CFile CStandFile;
#pragma message("GoodFunc: no StandFile support")
#else 
#include "StandFile.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GoodFunc::GoodFunc()
{
}

int AFXAPI AfxMessageBox( UINT nType, LPCTSTR lpszfmt, ... )
{
   CString s;
   va_list args;
   va_start(args, lpszfmt);
   s.FormatV(lpszfmt, args);
   va_end(args);

   return AfxMessageBox(s, nType);
}

void AFXAPI DDX_Hex(CDataExchange* pDX, int nIDC, DWORD& value)
{
   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
   TCHAR szT[32];
   if (pDX->m_bSaveAndValidate)
   {
      ::GetWindowText(hWndCtrl, szT, 32 );

      DWORD dw;
      TCHAR *ep;
      dw = _tcstoul(szT, &ep, 16);

      if ( *ep ) {
         AfxMessageBox(_T("Введите шестнадцатиричное число"));
         pDX->Fail();
      }
      value = dw;
   }
   else
   {
      wsprintf(szT, _T("%08X"), value);
      AfxSetWindowText(hWndCtrl, szT);
   }
}

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

void AFXAPI DDX_Text0(CDataExchange* pDX, int nIDC, float& value)
{  // if input is empty or bad - assume 0
   AFX_STATIC BOOL AFXAPI _AfxSimpleFloatParse(LPCTSTR lpszText, double& d);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d;
		if (!_AfxSimpleFloatParse(szBuffer, d))
		{
			value = 0;
      } else {
		   value = (float)d;
      }
	} else {
		_stprintf(szBuffer, _T("%.*g"), 6, value);
		AfxSetWindowText(hWndCtrl, szBuffer);
	}
}

void AFXAPI DDV_Pow2Int(CDataExchange* pDX, int value )
{
   if ( !pDX->m_bSaveAndValidate ) return;

   if ( value > 0 && ((value&(value-1)) == 0) ) goto _ex;

   AfxMessageBox(_T("Число должно быть степенью 2"), MB_ICONEXCLAMATION);
   pDX->Fail();
_ex:;
}

void StdMouseNotifyParent(HWND hWnd, UINT code, int x, int y)
{
   NMMOUSE nm;

   nm.hdr.hwndFrom = hWnd;
   nm.hdr.idFrom = ::GetDlgCtrlID(hWnd);
   nm.hdr.code = code;
   nm.pt.x = x;
   nm.pt.y = y;
   nm.dwHitInfo = nm.dwItemData = nm.dwItemSpec = 0;

   ::SendMessage(::GetParent(hWnd), WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm);
}

void FormatMessageStr(CString &S, DWORD dwMsgId) 
{
   LPTSTR lpMsgBuf;
   FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dwMsgId,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      NULL 
      );
   S = lpMsgBuf;
   LocalFree( lpMsgBuf );
}

int FormatMessageBox(DWORD dwMsgId, LPCTSTR szNote, UINT nType)
{
   CString S;
   FormatMessageStr(S, dwMsgId);
   S += _T("\n");
   S += szNote;
   return AfxMessageBox( S, nType );
}


// MyTaskBarAddIcon - adds an icon to the taskbar status area. 
#if 0
BOOL MyTaskBarAddIcon(HWND hwnd, UINT uMsg,  UINT uID, UINT idicon, LPTSTR lpszTip) 
{ 
    BOOL res;
    NOTIFYICONDATA tnid;
    HICON hicon = AfxGetApp()->LoadIcon(idicon);
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd; 
    tnid.uID = uID; 
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnid.uCallbackMessage = uMsg;
    tnid.hIcon = hicon;
    if (lpszTip) 
        lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
    else 
        tnid.szTip[0] = '\0'; 
 
    res = Shell_NotifyIcon(NIM_ADD, &tnid); 
 
    if (hicon) 
       DestroyIcon(hicon); 

    return res; 
} 
 
// MyTaskBarDeleteIcon - deletes an icon from the taskbar 
//     status area. 
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID) 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd; 
    tnid.uID = uID; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
    return res; 
} 
#endif
 
int AFXAPI SetDlgItemForm( CWnd *w, UINT id, LPCTSTR lpszfmt, ... )
{
   CString s;
   va_list args;
   va_start(args, lpszfmt);
   s.FormatV(lpszfmt, args);
   va_end(args);

   return SetDlgItemText(w->m_hWnd, id, s);
}

#if 0
BOOL SelectComputer(HWND hWnd, CString &s)
{
   typedef DWORD (WINAPI *LPFNServerBrowseDialogA0)(HWND hwnd, CHAR *pchBuffer,
	DWORD cchBufSize);

	LPFNServerBrowseDialogA0	lpfn;
	HMODULE						hMod = NULL;
   CString st;
   DWORD ret = 0;

	if((hMod = LoadLibrary(TEXT("ntlanman.dll"))) &&
		(lpfn = (LPFNServerBrowseDialogA0)GetProcAddress(hMod,
			"ServerBrowseDialogA0")))
	{
      ret = !(*lpfn)(hWnd, st.GetBuffer(MAX_COMPUTERNAME_LENGTH+5), 
         MAX_COMPUTERNAME_LENGTH+5);
      st.ReleaseBuffer();
      if ( ret ) {
         s = st;
      }
	}
	else
	{
		AfxMessageBox("Unable to load 'ntlanman.dll'", MB_ICONEXCLAMATION);
	}

	if (hMod) FreeLibrary(hMod);
   return ret;
}
#endif

int IsNumber(TCHAR B)
{  // >=3-int, >=2-float
   if ( B >= _T('0') && B <= _T('9') ) return 3;
   if ( B == _T('-') || B == _T('+') ) return 3;
   if ( B == _T('.') ) return 2;
   if ( B == _T('e') || B == _T('E') ) return 1;     // e не может быть первой!
   return 0;
}

BOOL FromFileRead(LPCTSTR szFileName, void *pData, UINT uDataLen, BOOL bMessages)
{
   CStandFile file;
   if ( file.Open(szFileName, CFile::modeRead|CFile::shareDenyNone)){
      if ( file.Read ( pData, uDataLen ) == uDataLen ) return TRUE;
      if ( bMessages ) AfxMessageBox(MB_ICONSTOP, "Неверная длина файла '%s'", szFileName);
   } else {
      if ( bMessages ) AfxMessageBox(MB_ICONSTOP, "Невозможно открыть '%s'", szFileName);
   }
   return FALSE;
}

BOOL ToFileWrite(LPCTSTR szFileName, const void *pData, UINT uDataLen, BOOL bMessages)
{
   CFile file;
   if ( file.Open(szFileName, CFile::modeWrite|CFile::modeCreate)){
      TRY {
         file.Write ( pData, uDataLen );
      } CATCH_ALL(e) {
	     if ( bMessages ) AfxMessageBox(MB_ICONSTOP, "Невозможно записать в файл '%s'", szFileName);
         return FALSE;
      } END_CATCH_ALL;
      return TRUE;
   } else {
      if ( bMessages ) AfxMessageBox(MB_ICONSTOP, "Невозможно создать '%s'", szFileName);
   }
   return FALSE;
}

BOOL _FromResOrFileRead(CFile &file, LPCTSTR szFileName, void *pData, UINT uDataLen)
{
   if ( file.Open(szFileName, CFile::modeRead|CFile::shareDenyNone)){
      if ( file.Read ( pData, uDataLen ) == uDataLen ) return TRUE;
   }
   HINSTANCE hInst = AfxGetResourceHandle();
   do {
      HRSRC hrsrc = FindResource(hInst, szFileName, "file");
      if ( !hrsrc ) break;
      HGLOBAL hg = LoadResource(hInst, hrsrc);
      if ( !hg ) break;
      DWORD rsize = SizeofResource(hInst, hrsrc);
      memcpy(pData, LockResource(hg), min(uDataLen, rsize) );

      return TRUE;
   } while ( 0 );

   AfxMessageBox(MB_ICONSTOP, "Невозможно открыть '%s'"
                 " или загрузить его из ресурсов", szFileName);
   return FALSE;
}

BOOL FromResOrFileRead(LPCTSTR szFileName, void *pData, UINT uDataLen)
{
   CStandFile file;
   return _FromResOrFileRead(file, szFileName, pData, uDataLen);
}

BOOL FromResOrFileReadNonGrp(LPCTSTR szFileName, void *pData, UINT uDataLen)
{
   CFile file;
   return _FromResOrFileRead(file, szFileName, pData, uDataLen);
}

DWORD GetExeTimeDateStamp(HMODULE hMod)
{
   try {
   // Берём TimeDataStamp из PE-header'a текущего EXE-модуля
   BYTE *p;
   if ( hMod ) {
      p = (BYTE*)hMod;
   } else {
      p = (BYTE*)AfxGetInstanceHandle(); // usually 400000
   }
   return  *(DWORD*) &p[ *(DWORD*)&p[0x3c] + 8];
   } catch(...) {
      return 0x80000002;
   }
}

BOOL WhatsThisHelp(CWnd* pWnd, CPoint point, int IDD)
{
   pWnd->ScreenToClient(&point);
   HWND hControl = ::ChildWindowFromPoint (pWnd->m_hWnd, point);
   if ( !hControl ) return FALSE;
   int id = ::GetDlgCtrlID(hControl);
   if ( id == 0 || id == -1 ) return FALSE;
   
   WORD t[8] = { (WORD)id, 0, (WORD)id, 0x8000|(WORD)IDD, 0, 0, 0, 0};
   if ( !
     ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, HELP_CONTEXTMENU, 
             (DWORD) &t)
             ) {
      FormatMessageBox(GetLastError());
      return FALSE;
   }; 
   return TRUE;
}

void TimeDateStampToFileTime( DWORD timeDateStamp, LPFILETIME pFileTime ) 
{ 
   __int64 t1970 = 0x019DB1DED53E8000; // Magic... GMT... Don't ask.... 
   __int64 timeStampIn100nsIncr = (__int64)timeDateStamp * 10000000; 
   __int64 finalValue = t1970 + timeStampIn100nsIncr; 
   memcpy( pFileTime, &finalValue, sizeof( finalValue ) ); 
} 

float Ball2Me(UINT nBall)  // баллы в высоту волн (в метры)
{
   static const float Hm[10] = {
      0,
      0.25f,
      0.5f,
      1.f,
      1.625f,
      2.75f,
      4.75f,
      7.25f,
      9.75f,
      11.f
   };
   if ( nBall > 9 ) nBall = 9;
   return Hm[nBall];
}

void ScreenMsg(int y, LPCSTR szMsg, BOOL bAlways, DWORD Delay)
{
   if (bAlways || (GetAsyncKeyState(VK_SHIFT) & 
      GetAsyncKeyState(VK_CONTROL) & 0x8000) ) {
      CClientDC dc(NULL);
      int len = lstrlen(szMsg);
      for ( int i=0; i<len ;i++ ) {
         dc.TextOut(0, y, &szMsg[len-i-1], i+1);
         GdiFlush();
         if ( Delay ) Sleep(Delay);
      }
   }
}

// look for ColorAdjustLuma in shlwapi.dll
COLORREF WINAPI TryColorAdjustLuma(COLORREF clrRGB, int n, BOOL fScale)
{
   COLORREF clr;
   typedef COLORREF (WINAPI *tColorAdjustLuma)(COLORREF clrRGB, int n, BOOL fScale);
	HMODULE hMod = NULL;
   tColorAdjustLuma ColorAdjustLuma = NULL;
   hMod = LoadLibrary("shlwapi.dll");

	if( hMod ) 
      ColorAdjustLuma = (tColorAdjustLuma)GetProcAddress(hMod, "ColorAdjustLuma");

   if ( ColorAdjustLuma ) {
      clr = ColorAdjustLuma(clrRGB, n, fScale);
   } else {
      clr = clrRGB;
      BYTE *b=(BYTE *)&clr;
      for ( int i=0; i<3 ;i++ ) {
         DWORD t = b[i];
         if ( n > 0 ) {
            t=t*n/70;
         } else {
            t=t*70/(-n);
         }
         if ( t>255 ) t=255;
         b[i]=(BYTE)t;
      }
   }
   if ( hMod ) FreeLibrary(hMod);
   return clr;
}

void Make83Scale(float *f)
{
   float Ffo[10]={5.0f, 6.3f, 8.0f, 10.0f, 12.5f, 16.0f, 20.0f, 25.0f, 31.5f, 40.0f};
   
   int n=0, i, j;   
   for(i=0;i<5;i++) {
      for(j=0; j<10 && n<44 ;j++, n++) { 
         f[n] = Ffo[j];
         Ffo[j] *= 10.0f;
      }
   }                
}

void BlinkScrollLock(void)
{
   keybd_event( VK_SCROLL, 0x46, KEYEVENTF_EXTENDEDKEY | 0, 0 ); 
   
   // Simulate a key release
   keybd_event( VK_SCROLL,
      0x46,
      KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
      0);
}

//BOOL PowerOff(BOOL bHibernate) // ВЫКЛЮЧИТЬ питание компа. ТРУЕ если удалось.
BOOL PowerOff(BOOL bHibernate, BOOL bRestart)// ВЫКЛЮЧИТЬ питание компа. TRUE если удалось.
{
//    if ( bHibernate ) {
//       if ( !IsPwrHibernateAllowed() ) return FALSE;
//    }

   HANDLE hToken; 
   TOKEN_PRIVILEGES tkp;  // Get a token for this process.  
   if (!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
        return FALSE; // Get the LUID for the shutdown privilege.  
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,         
                     &tkp.Privileges[0].Luid);  
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
// Get the shutdown privilege for this process.  
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);  
// Cannot test the return value of AdjustTokenPrivileges.  
   if ( GetLastError() != ERROR_SUCCESS ) return FALSE;
// Shut down the system and force all applications to close.  
   if ( bHibernate ) {
      return SetSystemPowerState(FALSE, TRUE);
   }
   return InitiateSystemShutdownEx(NULL, NULL, 0, TRUE, bRestart, 0);
}

// оканчивается ли строка на указанный паттерн
int __cdecl strend(const char *str, const char *end)
{ 
   size_t L1 = strlen(str);
   size_t L2 = strlen(end);
   if ( L1 < L2 ) return 1;
   return stricmp(str+L1-L2, end);
}

void ChangeEqualSign(char *buf)
{  // заменить символ равно в названии параметра для работы с ini-файлом 
   char *p = buf;
   while ( *p ) {
      if ( *p == '=' ) *p = '@';
      p++;
   }
}

void SkipBrokenChars(CString &s)
{  // недопустимые в именах файлов
   int n = s.GetLength();
   LPCTSTR sz = s;
   for ( int i=0; i<n ;i++ ) {
      if ( strchr("\\/|<>:*?\"", sz[i]) ) s.SetAt(i, '_');
   }
} 


