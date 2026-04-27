// OwnDrawChld.cpp: implementation of the COwnDrawChld class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ArrEd.h"
#include "ODChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void RegisterODChild(COLORREF clBk, LPCTSTR szClassName)
{
   WNDCLASS wc;
   ZeroMemory(&wc,  sizeof(WNDCLASS));
   wc.lpfnWndProc = (WNDPROC)ODWndProc;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hCursor = ::LoadCursor(NULL,IDC_ARROW); 
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.hbrBackground = (HBRUSH) CreateSolidBrush(clBk);
   wc.lpszClassName = szClassName;
   
   VERIFY( AfxRegisterClass(&wc) ); 
}

LRESULT CALLBACK ODWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   HDC hdc;
   DRAWITEMSTRUCT dis;
   NMHDR nm;
   
   switch (message) {

   case WM_LBUTTONDOWN:
      {
         nm.hwndFrom = hWnd;
         nm.idFrom = GetDlgCtrlID(hWnd);
         nm.code = NM_CLICK;
         SendMessage(GetParent(hWnd),WM_NOTIFY,nm.idFrom,(LPARAM)&nm);
      }
      break;
   case WM_RBUTTONDOWN:
      {
         nm.hwndFrom = hWnd;
         nm.idFrom = GetDlgCtrlID(hWnd);
         nm.code = NM_RCLICK;
         SendMessage(GetParent(hWnd),WM_NOTIFY,nm.idFrom,(LPARAM)&nm);
      }
      break;
   case WM_MOUSEMOVE:
      if ( wParam == MK_LBUTTON ) {
         nm.hwndFrom = hWnd;
         nm.idFrom = GetDlgCtrlID(hWnd);
         nm.code = WM_MOUSEMOVE;//NM_CLICK;
         SendMessage(GetParent(hWnd),WM_NOTIFY,nm.idFrom,(LPARAM)&nm);
      }
      break;
   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      dis.CtlType = 0;  
      dis.itemID  = 0;
      dis.itemState = 0;
      dis.itemData = 0; 
      dis.itemAction = ODA_DRAWENTIRE;
      dis.hDC = hdc;
      dis.hwndItem = hWnd;
      dis.CtlID   = GetDlgCtrlID(hWnd);
      GetClientRect(hWnd, &dis.rcItem);

      SendMessage(GetParent(hWnd),WM_DRAWITEM,dis.CtlID,(LPARAM)&dis);

      EndPaint(hWnd, &ps);
      break;


   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

