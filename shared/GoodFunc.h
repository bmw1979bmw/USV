// GoodFunc.h: interface for the GoodFunc class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning (3: 4189 4505 4727 4702 4705 4706)

class GoodFunc  
{
public:
	GoodFunc();

};
float Ball2Me(UINT nBall); // баллы в высоту волн (в метры)

// look for ColorAdjustLuma in shlwapi.dll
COLORREF WINAPI TryColorAdjustLuma(COLORREF clrRGB,int n,BOOL fScale);

BOOL WhatsThisHelp(CWnd* pWnd, CPoint point, int IDD);

int AFXAPI AfxMessageBox( UINT nType, LPCTSTR lpszfmt, ... );

#define ShowDlgItem(id,sw) ::ShowWindow(::GetDlgItem(m_hWnd,id),sw)
#define EnableDlgItem(id,bEn) ::EnableWindow(::GetDlgItem(m_hWnd,id),bEn)
#define IsDlgItemVisible(id) ::IsWindowVisible(::GetDlgItem(m_hWnd,id))

void AFXAPI AfxSetWindowText(HWND hWndCtrl, LPCTSTR lpszNew);
void AFXAPI DDX_Hex(CDataExchange* pDX, int nIDC, DWORD& value);
void AFXAPI DDX_Text0(CDataExchange* pDX, int nIDC, float& value);
int AFXAPI SetDlgItemForm( CWnd *w, UINT id, LPCTSTR lpszfmt, ... );

void AFXAPI DDV_Pow2Int(CDataExchange* pDX, int value );

void StdMouseNotifyParent(HWND hWnd, UINT code, int x, int y);

void FormatMessageStr(CString &S,DWORD dwMsgId);
int FormatMessageBox(DWORD dwMsgId,LPCTSTR szNote=NULL, UINT nType = MB_OK);

// MyTaskBarAddIcon - adds an icon to the taskbar status area. 
BOOL MyTaskBarAddIcon(HWND hwnd, UINT uMsg, UINT uID, UINT idicon, LPTSTR lpszTip);

// MyTaskBarDeleteIcon - deletes an icon from the taskbar 
//     status area. 
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID);

BOOL SelectComputer(HWND hWnd, CString &s);

#ifdef _DEBUG
#define ASSERTM(expr,msg) \
   do { if (!(expr) && \
     (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg))) \
      _CrtDbgBreak(); } while (0)
#else
   #define ASSERTM(expr,msg)
#endif

#define LO_WORD(i) (*(WORD *)&i)
#define HI_WORD(i) (*((WORD *)&i+1))

int IsNumber(TCHAR B);

BOOL FromFileRead(LPCTSTR szFileName, void *pData, UINT uDataLen,BOOL bMessages=TRUE);
BOOL ToFileWrite(LPCTSTR szFileName, const void *pData, UINT uDataLen,BOOL bMessages=TRUE);
BOOL FromResOrFileRead(LPCTSTR szFileName, void *pData, UINT uDataLen);
BOOL FromResOrFileReadNonGrp(LPCTSTR szFileName, void *pData, UINT uDataLen);

DWORD GetExeTimeDateStamp(HMODULE hMod = NULL);

void TimeDateStampToFileTime( DWORD timeDateStamp, LPFILETIME pFileTime );

void ScreenMsg(int y,LPCSTR szMsg,BOOL bAlways=FALSE,DWORD Delay=20);

#define PRESSED_CTRL    (GetAsyncKeyState(VK_CONTROL) & 0x8000)
#define PRESSED_ALT     (GetAsyncKeyState(VK_MENU) & 0x8000)
#define PRESSED_SHIFT (GetAsyncKeyState(VK_SHIFT)&0x8000)


void Make83Scale(float *f);

#define chCHR2(i) #i
#define chCHR(i) chCHR2(i)
#define chMSG(j)  message(__FILE__ "(" chCHR(__LINE__) ") :         <" j )
#define chMSG0(j)  message(__FILE__ "(" chCHR(__LINE__) ") : " j )
#define chINT(j)  message(__FILE__ "(" chCHR(__LINE__) ") : " chCHR(j) )

void BlinkScrollLock(void);

// BOOL PowerOff(BOOL bHibernate=FALSE); // ¬џ Ћё„»“№ питание компа. “–”≈ если удалось.
BOOL PowerOff(BOOL bHibernate=FALSE, BOOL bRestart=FALSE);// ¬џ Ћё„»“№ питание компа. TRUE если удалось.

// оканчиваетс€ ли строка на указанный паттерн
int __cdecl strend(const char *str, const char *end);

void ChangeEqualSign(char *buf);
void SkipBrokenChars(CString &s);

/*
float sign(float f)
{
   if ( f > 0 ) return 1;
   if ( f == 0 ) retrun 0;
   return -1;
}
*/