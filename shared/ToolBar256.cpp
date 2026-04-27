#include "stdafx.h"

#if _MSC_VER > 1200
#include "..\src\mfc\afximpl.h"
#else
#include "..\mfc\src\afximpl.h"
#endif

#include <afxpriv.h>

#include "ToolBar256.h"

#ifdef _THEMED_
#include <uxtheme.h>
#include <Tmschema.h>
#endif


#define VERSION_6  MAKELONG(0, 6) 

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////

#ifndef TBIF_BYINDEX
#define TBIF_BYINDEX            0x80000000 // this specifies that the wparam in Get/SetButtonInfo is an index, not id
#endif

#ifndef BTNS_SHOWTEXT
#define BTNS_SHOWTEXT 0x40
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 creation etc

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif



#if(1)
#define _afxComCtlVersion _afxComCtlVersion_my
#define _AfxGetComCtlVersion _AfxGetComCtlVersion_my

static int _afxComCtlVersion = -1;

static DWORD AFXAPI _AfxGetComCtlVersion()
{
   struct AFX_DLLVERSIONINFO
   {
           DWORD cbSize;
           DWORD dwMajorVersion;                   // Major version
           DWORD dwMinorVersion;                   // Minor version
           DWORD dwBuildNumber;                    // Build number
           DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
   };
   typedef HRESULT (CALLBACK* AFX_DLLGETVERSIONPROC)(AFX_DLLVERSIONINFO *);

   // return cached version if already determined...
   if (_afxComCtlVersion != -1)
      return _afxComCtlVersion;

   // otherwise determine comctl32.dll version via DllGetVersion
   HINSTANCE hInst = ::GetModuleHandleA("COMCTL32.DLL");
   ASSERT(hInst != NULL);
   AFX_DLLGETVERSIONPROC pfn;
   pfn = (AFX_DLLGETVERSIONPROC)GetProcAddress(hInst, "DllGetVersion");
   DWORD dwVersion = VERSION_WIN4;
   if (pfn != NULL)
   {
      AFX_DLLVERSIONINFO dvi;
      memset(&dvi, 0, sizeof(dvi));
      dvi.cbSize = sizeof(dvi);
      HRESULT hr = (*pfn)(&dvi);
      if (SUCCEEDED(hr))
      {
         ASSERT(dvi.dwMajorVersion <= 0xFFFF);
         ASSERT(dvi.dwMinorVersion <= 0xFFFF);
         dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
      }
   }
   _afxComCtlVersion = dwVersion;
   return dwVersion;
}
#endif

CToolBar256::CToolBar256() : 
   m_bParentUpdateUIOnly(FALSE),
   m_pStringMap(NULL),
   m_bDelayedButtonLayout(TRUE),
   m_hIL(NULL)
{
   // top and bottom borders are 1 larger than default for ease of grabbing
   m_cyTopBorder = 3;
   m_cyBottomBorder = 3;
}

CToolBar256::~CToolBar256()
{
   delete m_pStringMap;
   if ( m_hIL ) ImageList_Destroy(m_hIL);

   m_nCount = 0;
}

BOOL CToolBar256::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
   return CreateEx(pParentWnd, 0, dwStyle, 
      CRect(m_cxLeftBorder, m_cyTopBorder, m_cxRightBorder, m_cyBottomBorder), nID);
}

BOOL CToolBar256::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
   ASSERT_VALID(pParentWnd);   // must have a parent
   ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));

   SetBorders(rcBorders);

   // save the style
   m_dwStyle = (dwStyle & CBRS_ALL);
   if (nID == AFX_IDW_TOOLBAR)
      m_dwStyle |= CBRS_HIDE_INPLACE;

   dwStyle &= ~CBRS_ALL;
   dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
   dwStyle |= dwCtrlStyle;

   // initialize common controls
   VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_BAR_REG));
   _AfxGetComCtlVersion();
   ASSERT(_afxComCtlVersion != -1);

   // create the HWND
   const RECT rect={0, 0, 0, 0}; //CRect rect; rect.SetRectEmpty();
   if (!CWnd::Create(TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
      return FALSE;

   // Note: Parent must resize itself for control bar to be resized

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar256

BOOL CToolBar256::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (!CControlBar::OnNcCreate(lpCreateStruct))
      return FALSE;

   // if the owner was set before the toolbar was created, set it now
   if (m_hWndOwner != NULL)
      DefWindowProc(TB_SETPARENT, (WPARAM)m_hWndOwner, 0);

   DefWindowProc(TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
   return TRUE;
}

void CToolBar256::SetOwner(CWnd* pOwnerWnd)
{
   ASSERT_VALID(this);
   if (m_hWnd != NULL)
   {
      ASSERT(::IsWindow(m_hWnd));
      DefWindowProc(TB_SETPARENT, (WPARAM)pOwnerWnd->GetSafeHwnd(), 0);
   }
   CControlBar::SetOwner(pOwnerWnd);
}


BOOL CToolBar256::LoadToolBar(UINT lpszResourceName, COLORREF clrTransparent)
{
   ASSERT_VALID(this);
   ASSERT(lpszResourceName != NULL);
   if ( !lpszResourceName ) return FALSE;

   // determine location of the bitmap in resource fork
   HINSTANCE hInst = AfxFindResourceHandle((LPCTSTR)lpszResourceName, RT_TOOLBAR);
   HRSRC hRsrc = ::FindResource(hInst, (LPCTSTR)lpszResourceName, RT_TOOLBAR);

   if (hRsrc == NULL) {
      hInst = AfxGetInstanceHandle();
      hRsrc = ::FindResource(hInst, (LPCTSTR)lpszResourceName, RT_TOOLBAR);
   }
   if (hRsrc == NULL)
      return FALSE;

   HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
   if (hGlobal == NULL)
      return FALSE;

   struct CToolBarData
   {
      WORD wVersion;
      WORD wWidth;
      WORD wHeight;
      WORD wItemCount;
      //WORD aItems[wItemCount]

      WORD* items()
         { return (WORD*)(this+1); }
   };

   CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
   if (pData == NULL)
      return FALSE;
   ASSERT(pData->wVersion == 1);

   BOOL bResult = SetButtons(pData->items(), pData->wItemCount);

   if ( bResult )
   {
      HBITMAP hBmp = ::LoadBitmap(hInst, (LPCTSTR)lpszResourceName);
      if ( hBmp ) {
//       COLORREF clr = RGB(192, 192, 192);
//       if you want to find first pixel automatically {                     
//          HDC hDC0 = ::GetDC(NULL);
//          if ( hDC0 ) {
//             HDC hDC = CreateCompatibleDC(hDC0);
//             if ( hDC ) {
//                HGDIOBJ ho = SelectObject(hDC, hBmp);
//                COLORREF r = GetPixel(hDC, 0, 0);
//                if ( r != CLR_INVALID ) clr = r;
//                SelectObject(hDC, ho);
//                DeleteDC(hDC);
//             }
//             ::ReleaseDC(NULL, hDC0);
//          }
//       }
         if ( m_hIL ) ImageList_Destroy(m_hIL);
         m_hIL = ImageList_Create(pData->wWidth, pData->wHeight, ILC_COLORDDB|ILC_MASK, 0, 0);
         ImageList_AddMasked(m_hIL, hBmp, clrTransparent);
         SendMessage(TB_SETIMAGELIST, 0, (LPARAM)m_hIL);
         DeleteObject(hBmp);
      }
   }

   UnlockResource(hGlobal);
   FreeResource(hGlobal);

   return bResult;
}

int CToolBar256::AddBitmap(int nNumButtons, UINT nBitmapID, COLORREF clrTransparent)
{
   ASSERT(::IsWindow(m_hWnd));
   HINSTANCE hInst = AfxFindResourceHandle((LPCTSTR)(DWORD_PTR)nBitmapID, RT_BITMAP);
   HBITMAP hBmp = ::LoadBitmap(hInst, MAKEINTRESOURCE(nBitmapID));
   if ( !hBmp ) return -1;
   int idx = ImageList_GetImageCount(m_hIL);
   ImageList_AddMasked(m_hIL, hBmp, clrTransparent);
   SendMessage(TB_SETIMAGELIST, 0, (LPARAM)m_hIL);
   DeleteObject(hBmp);
   return idx;
}


BOOL CToolBar256::SetButtons(const WORD* lpIDArray, int nIDCount)
{
   ASSERT_VALID(this);
   ASSERT(nIDCount >= 1);  // must be at least one of them
   ASSERT(lpIDArray == NULL ||
      AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));

   // delete all existing buttons
   int nCount = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   while (nCount--)
      VERIFY(DefWindowProc(TB_DELETEBUTTON, 0, 0));

   // width of separator includes 8 pixel overlap
   ASSERT(_afxComCtlVersion != -1);
   int iSepSize = ((GetStyle()&TBSTYLE_FLAT)||_afxComCtlVersion == VERSION_IE4)  
      ? 6 : 8;

   BYTE btStyle = TBSTYLE_AUTOSIZE;

   // если LIST & !FLAT - значит кнопки с надписями - не AUTOSIZE и большой сепаратор
   DWORD st = GetStyle();
   if ( /*(st&TBSTYLE_LIST) &&*/ 
      !(st&TBSTYLE_FLAT) && (m_dwStyle&(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT)) ) 
   { 
      btStyle = 0;
      iSepSize = 20;
   }

   CString s;

   TBBUTTON button; 
   memset(&button, 0, sizeof(TBBUTTON));
   button.iString = -1;
   if (lpIDArray != NULL)
   {
      // add new buttons to the common control
      int iImage = 0;
      for (int i = 0; i < nIDCount; i++)
      {
         button.fsState = TBSTATE_ENABLED;
         button.iString = -1;

         if ((button.idCommand = *lpIDArray++) == 0)
         {
            // separator
            button.fsStyle = TBSTYLE_SEP;
            button.iBitmap = iSepSize;
         }
         else
         {
            // a command button with image
            button.fsStyle = btStyle|TBSTYLE_BUTTON;//|TBSTYLE_AUTOSIZE;
            button.iBitmap = iImage++;
         }
         if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
            return FALSE;

         /*if ( bList )*/ 
         {
            // check 3rd string as text for button
            TCHAR buf[256];
            if ( AfxLoadString(button.idCommand, buf, 256 ) ) {
               if ( AfxExtractSubString(s, buf, 2) ) {
                  SetButtonStyle(i,
                     //TBSTYLE_AUTOSIZE|
                     btStyle|TBSTYLE_BUTTON|BTNS_SHOWTEXT);
                  SetButtonText(i, s);
               }
            } 
         }
      }
   }
   else
   {
      // add 'blank' buttons
      button.fsState = TBSTATE_ENABLED;
      for (int i = 0; i < nIDCount; i++)
      {
         ASSERT(button.fsStyle == TBSTYLE_BUTTON);
         if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
            return FALSE;
      }
   }
   m_nCount = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   m_bDelayedButtonLayout = TRUE;

   return TRUE;
}

// for Dialogs: set font from dialog window
void CToolBar256::SetSameFont(HWND hWndDlg)
{
   HFONT hFont = (HFONT)::SendMessage(hWndDlg, WM_GETFONT, 0, 0);
   if ( hFont ) SendMessage(WM_SETFONT, (WPARAM)hFont, 0);
}

// for Dialogs: move toolbar to dialog control coordinates
// (or use RepositionBars(AFX_IDW_TOOLBAR, AFX_IDW_TOOLBAR, 0) )
//
// use CBRS_ORIENT_HORZ (CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM) styles 
// to make horizontal toolbar
//
// use CBRS_SIZE_DYNAMIC style of horizontal 
// toolbar for resizing up to client width
// 
void CToolBar256::MoveToControl(HWND hWndDlg, UINT nID, BOOL bBottom)
{
   RECT rc;
   HWND hControl = ::GetDlgItem(hWndDlg, nID);
   if ( !hControl ) return;
   ::GetWindowRect(hControl, &rc);

   BOOL bHorz = m_dwStyle&CBRS_ORIENT_HORZ;
   BOOL bDyn = m_dwStyle&CBRS_SIZE_DYNAMIC;

   CSize sz = CalcFixedLayout(0, bHorz);

   if ( bBottom ) rc.top = rc.bottom - sz.cy;
   
   ::ScreenToClient(hWndDlg, ((LPPOINT)&rc));

   if ( bHorz && bDyn ) {
      rc.left = 0;
      RECT rcw;
      ::GetClientRect(hWndDlg, &rcw);
      sz.cx = rcw.right;
   }
   
   SetWindowPos(NULL, rc.left, rc.top, sz.cx, sz.cy,
      SWP_NOZORDER|SWP_SHOWWINDOW);

}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 attribute access

void CToolBar256::_GetButton(int nIndex, TBBUTTON* pButton) const
{
   CToolBar256* pBar = (CToolBar256*)this;
   VERIFY(pBar->DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
}

void CToolBar256::_SetButton(int nIndex, TBBUTTON* pButton)
{
   // get original button state
   TBBUTTON button;
   VERIFY(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)&button));

   // prepare for old/new button comparsion
   button.bReserved[0] = 0;
   button.bReserved[1] = 0;
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
   pButton->bReserved[0] = 0;
   pButton->bReserved[1] = 0;

   // nothing to do if they are the same
   if (memcmp(pButton, &button, sizeof(TBBUTTON)) != 0)
   {
      // don't redraw everything while setting the button
      DWORD dwStyle = GetStyle();
      ModifyStyle(WS_VISIBLE, 0);
      VERIFY(DefWindowProc(TB_DELETEBUTTON, nIndex, 0));
      VERIFY(DefWindowProc(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
      ModifyStyle(0, dwStyle & WS_VISIBLE);

      // invalidate appropriate parts
      if (((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
         ((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
      {
         // changing a separator
         Invalidate();
      }
      else
      {
         // invalidate just the button
         CRect rect;
         if (DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
            InvalidateRect(rect);
      }
   }
}

int CToolBar256::CommandToIndex(UINT nIDFind) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   CToolBar256* pBar = (CToolBar256*)this;
   return (int)pBar->DefWindowProc(TB_COMMANDTOINDEX, nIDFind, 0);
}

UINT CToolBar256::GetItemID(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex, &button);
   return button.idCommand;
}

void CToolBar256::GetItemRect(int nIndex, LPRECT lpRect) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // handle any delayed layout
   if (m_bDelayedButtonLayout)
      ((CToolBar256*)this)->Layout();

   // now it is safe to get the item rectangle
   CToolBar256* pBar = (CToolBar256*)this;
   if (!pBar->DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)lpRect))
      SetRectEmpty(lpRect);
}

void CToolBar256::Layout()
{
   ASSERT(m_bDelayedButtonLayout);

   m_bDelayedButtonLayout = FALSE;

   BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
      ((CToolBar256*)this)->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH | LM_COMMIT);
   else if (bHorz)
      ((CToolBar256*)this)->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK | LM_COMMIT);
   else
      ((CToolBar256*)this)->CalcDynamicLayout(0, LM_VERTDOCK | LM_COMMIT);
}

UINT CToolBar256::GetButtonStyle(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex, &button);
   return MAKELONG(button.fsStyle, button.fsState);
}

void CToolBar256::SetButtonStyle(int nIndex, UINT nStyle)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex, &button);
   if (button.fsStyle != (BYTE)LOWORD(nStyle) || button.fsState != (BYTE)HIWORD(nStyle))
   {
      button.fsStyle = (BYTE)LOWORD(nStyle);
      button.fsState = (BYTE)HIWORD(nStyle);
      _SetButton(nIndex, &button);
      m_bDelayedButtonLayout = TRUE;
   }
}

//#define CX_OVERLAP  0


CSize CToolBar256::CalcSize(TBBUTTON* pData, int nCount)
{
   ASSERT(pData != NULL && nCount > 0);

   CSize sizeResult(0, 0);
   if ( !pData || nCount <= 0 ) return sizeResult;

   CPoint cur(0, 0);

   //IA64: Assume retval of TB_GETEXTENDEDSTYLE is still 32-bit
   //DWORD dwExtendedStyle = DWORD(DefWindowProc(TB_GETEXTENDEDSTYLE, 0, 0));

   for (int i = 0; i < nCount; i++)
   {
      if (pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      SIZE sizeButton;
      {
         CToolBar256* pBar = (CToolBar256*)this;

         TBBUTTONINFO tbi;
         tbi.cbSize = sizeof(tbi);
         tbi.dwMask = TBIF_STATE|TBIF_BYINDEX;
         
         pBar->DefWindowProc(TB_GETBUTTONINFO, i, (LPARAM)&tbi); 
         BYTE bSav = tbi.fsState;

         if ( bSav != pData[i].fsState ) {
            tbi.fsState = pData[i].fsState;
            pBar->DefWindowProc(TB_SETBUTTONINFO, i, (LPARAM)&tbi); 
         }

         {
         RECT rc;
         pBar->DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rc); 
         sizeButton.cx = rc.right-rc.left;
         sizeButton.cy = rc.bottom-rc.top;
         }

         if ( bSav != pData[i].fsState ) {
            tbi.fsState = bSav;
            pBar->DefWindowProc(TB_SETBUTTONINFO, i, (LPARAM)&tbi); 
         }
      } 

      if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand == 0) ) 
      {
         if (pData[i].fsState & TBSTATE_WRAP)
            sizeResult.cy = max(cur.y + sizeButton.cy /*+ sizeButton.cx*/, sizeResult.cy);
         else
            sizeResult.cx = max(cur.x + sizeButton.cx, sizeResult.cx);

      }
      else     
      {
         sizeResult.cx = max(cur.x + sizeButton.cx, sizeResult.cx);
         sizeResult.cy = max(cur.y + sizeButton.cy, sizeResult.cy);
      }

      cur.x += sizeButton.cx;

      if (pData[i].fsState & TBSTATE_WRAP)
      {
         cur.x = 0;
         cur.y += sizeButton.cy;
      }
   }

   return sizeResult;
}

int CToolBar256::WrapToolBar(TBBUTTON* pData, int nCount, int nWidth)
{
   ASSERT(pData != NULL && nCount > 0);
   if ( !pData || nCount <= 0 ) return 1;

   int nResult = 0;
   int x = 0;
   for (int i = 0; i < nCount; i++)
   {
      pData[i].fsState &= ~TBSTATE_WRAP;

      if (pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      int dx, dxNext;
      {
         SIZE sizeButton;
         {
            RECT rc;
            CToolBar256* pBar = (CToolBar256*)this;
            pBar->DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rc); 
            sizeButton.cx = rc.right-rc.left;
         } 

         dx = sizeButton.cx; 
         dxNext = dx;
      }

      if (x + dx > nWidth)
      {
         BOOL bFound = FALSE;
         for (int j = i; j >= 0  &&  !(pData[j].fsState & TBSTATE_WRAP); j--)
         {
            // Find last separator that isn't hidden
            // a separator that has a command ID is not
            // a separator, but a custom control.
            if ((pData[j].fsStyle & TBSTYLE_SEP) &&
               (pData[j].idCommand == 0) &&
               !(pData[j].fsState & TBSTATE_HIDDEN))
            {
               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
         }
         if (!bFound)
         {
            for (int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
            {
               // Never wrap anything that is hidden,
               // or any custom controls
               if ((pData[j].fsState & TBSTATE_HIDDEN) ||
                  ((pData[j].fsStyle & TBSTYLE_SEP) &&
                  (pData[j].idCommand != 0)))
                  continue;

               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
            if (!bFound)
               x += dxNext;
         }
      }
      else
         x += dxNext;
   }
   return nResult + 1;
}

void  CToolBar256::SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert)
{
   ASSERT(pData != NULL && nCount > 0);

   if (!bVert)
   {
      int nMin, nMax, nTarget, nCurrent, nMid;

      // Wrap ToolBar as specified
      nMax = nLength;
      nTarget = WrapToolBar(pData, nCount, nMax);

      // Wrap ToolBar vertically
      nMin = 0;
      nCurrent = WrapToolBar(pData, nCount, nMin);

      if (nCurrent != nTarget)
      {
         while (nMin < nMax)
         {
            nMid = (nMin + nMax) / 2;
            nCurrent = WrapToolBar(pData, nCount, nMid);

            if (nCurrent == nTarget)
               nMax = nMid;
            else
            {
               if (nMin == nMid)
               {
                  WrapToolBar(pData, nCount, nMax);
                  break;
               }
               nMin = nMid;
            }
         }
      }
      CSize size = CalcSize(pData, nCount);
      WrapToolBar(pData, nCount, size.cx);
   }
   else
   {
      CSize sizeMax, sizeMin, sizeMid;

      // Wrap ToolBar vertically
      WrapToolBar(pData, nCount, 0);
      sizeMin = CalcSize(pData, nCount);

      // Wrap ToolBar horizontally
      WrapToolBar(pData, nCount, 32767);
      sizeMax = CalcSize(pData, nCount);

      while (sizeMin.cx < sizeMax.cx)
      {
         sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
         WrapToolBar(pData, nCount, sizeMid.cx);
         sizeMid = CalcSize(pData, nCount);

         if (nLength < sizeMid.cy)
         {
            if (sizeMin.cx == sizeMid.cx)
            {
               WrapToolBar(pData, nCount, sizeMax.cx);
               return;
            }
            sizeMin.cx = sizeMid.cx;
         }
         else if (nLength > sizeMid.cy)
         {
            if (sizeMax.cx == sizeMid.cx)
            {
               WrapToolBar(pData, nCount, sizeMin.cx);
               return;
            }
            sizeMax.cx = sizeMid.cx;
         }
         else
            return;
      }
   }
}


CSize CToolBar256::CalcLayout(DWORD dwMode, int nLength)
{
   struct _AFX_CONTROLPOS
   {
      int nIndex, nID;
      CRect rectOldPos;
   };
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));
   if (dwMode & LM_HORZDOCK)
      ASSERT(dwMode & LM_HORZ);

   int nCount;
   TBBUTTON* pData = NULL;
   CSize sizeResult(0, 0);

   //BLOCK: Load Buttons
   {
      nCount = int(DefWindowProc(TB_BUTTONCOUNT, 0, 0));
      if (nCount != 0)
      {
         int i;
         pData = new TBBUTTON[nCount];
         for (i = 0; i < nCount; i++)
            _GetButton(i, &pData[i]);
      }
   }

   if (nCount > 0)
   {
      if (!(m_dwStyle & CBRS_SIZE_FIXED))
      {
         BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

         if (bDynamic && (dwMode & LM_MRUWIDTH))
            SizeToolBar(pData, nCount, m_nMRUWidth);
         else if (bDynamic && (dwMode & LM_HORZDOCK))
            SizeToolBar(pData, nCount, 32767);
         else if (bDynamic && (dwMode & LM_VERTDOCK))
            SizeToolBar(pData, nCount, 0);
         else if (bDynamic && (nLength != -1))
         {
            CRect rect; rect.SetRectEmpty();
            CalcInsideRect(rect, (dwMode & LM_HORZ));
            BOOL bVert = (dwMode & LM_LENGTHY);
            int nLen = nLength + (bVert ? rect.Height() : rect.Width());

            SizeToolBar(pData, nCount, nLen, bVert);
         }
         else if (bDynamic && (m_dwStyle & CBRS_FLOATING))
            SizeToolBar(pData, nCount, m_nMRUWidth);
         else
            SizeToolBar(pData, nCount, (dwMode & LM_HORZ) ? 32767 : 0);
      }

      sizeResult = CalcSize(pData, nCount);

      if (dwMode & LM_COMMIT)
      {
         _AFX_CONTROLPOS* pControl = NULL;
         int nControlCount = 0;
         BOOL bIsDelayed = m_bDelayedButtonLayout;
         m_bDelayedButtonLayout = FALSE;
         int i;

         for (i = 0; i < nCount; i++)
            if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               nControlCount++;

         if (nControlCount > 0)
         {
            pControl = new _AFX_CONTROLPOS[nControlCount];
            int nControlCountAlloc = nControlCount;
            nControlCount = 0;


            for(i = 0; i < nCount && nControlCount < nControlCountAlloc; i++)
            {
               if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               {
                  pControl[nControlCount].nIndex = i;
                  pControl[nControlCount].nID = pData[i].idCommand;

                  CRect rect;
                  GetItemRect(i, &rect);
                  ClientToScreen(&rect);
                  pControl[nControlCount].rectOldPos = rect;

                  nControlCount++;
               }
            }
         }

         if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
            m_nMRUWidth = sizeResult.cx;
         for (i = 0; i < nCount; i++)
            _SetButton(i, &pData[i]);

         if (nControlCount > 0)
         {
            for (i = 0; i < nControlCount; i++)
            {
               CWnd* pWnd = GetDlgItem(pControl[i].nID);
               if (pWnd != NULL)
               {
                  CRect rect;
                  pWnd->GetWindowRect(&rect);
                  CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
                  GetItemRect(pControl[i].nIndex, &rect);
                  pt = rect.TopLeft() + pt;
                  pWnd->SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
               }
            }
            delete[] pControl;
         }
         m_bDelayedButtonLayout = bIsDelayed;
      }
      delete[] pData;
   }

   //BLOCK: Adjust Margins
   {
      CRect rect; rect.SetRectEmpty();
      CalcInsideRect(rect, (dwMode & LM_HORZ));
      sizeResult.cy -= rect.Height();
      sizeResult.cx -= rect.Width();

      CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
      sizeResult.cx = max(sizeResult.cx, size.cx);
      sizeResult.cy = max(sizeResult.cy, size.cy);
   }
   return sizeResult;
}

CSize CToolBar256::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
   DWORD dwMode = bStretch ? LM_STRETCH : 0;
   dwMode |= bHorz ? LM_HORZ : 0;

   return CalcLayout(dwMode);
}

CSize CToolBar256::CalcDynamicLayout(int nLength, DWORD dwMode)
{
   if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
      ((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
   {
      return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
   }
   return CalcLayout(dwMode, nLength);
}

void CToolBar256::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex, &button);
   nID = button.idCommand;
   nStyle = MAKELONG(button.fsStyle, button.fsState);
   iImage = button.iBitmap;
}

void CToolBar256::SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage)
{
   ASSERT_VALID(this);

   TBBUTTON button;
   _GetButton(nIndex, &button);
   TBBUTTON save;
   memcpy(&save, &button, sizeof(save));
   button.idCommand = nID;
   button.iBitmap = iImage;
   button.fsStyle = (BYTE)LOWORD(nStyle);
   button.fsState = (BYTE)HIWORD(nStyle);
   if (memcmp(&save, &button, sizeof(save)) != 0)
   {
      _SetButton(nIndex, &button);
      m_bDelayedButtonLayout = TRUE;
   }
}

int CToolBar256::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // check child windows first by calling CControlBar
   int nHit = CControlBar::OnToolHitTest(point, pTI);
   if (nHit != -1)
      return nHit;

   // now hit test against CToolBar256 buttons
   CToolBar256* pBar = (CToolBar256*)this;
   int nButtons = (int)pBar->DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   for (int i = 0; i < nButtons; i++)
   {
      CRect rect;
      TBBUTTON button;
      if (pBar->DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rect))
      {
         ++rect.bottom;
         ++rect.right;
         if (rect.PtInRect(point) &&
            pBar->DefWindowProc(TB_GETBUTTON, i, (LPARAM)&button) &&
            !(button.fsStyle & TBSTYLE_SEP))
         {
            nHit = GetItemID(i);
            if (pTI != NULL && pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
            {
               pTI->hwnd = m_hWnd;
               pTI->rect = rect;
               pTI->uId = nHit;
               pTI->lpszText = LPSTR_TEXTCALLBACK;
            }
            // found matching rect, return the ID of the button
            return nHit != 0 ? nHit : -1;
         }
      }
   }
   return -1;
}

BOOL CToolBar256::SetButtonText(int nIndex, LPCTSTR lpszText)
{
   // attempt to lookup string index in map
   INT_PTR nString = -1;
   void* p;
   if (m_pStringMap != NULL && m_pStringMap->Lookup(lpszText, p))
      nString = (INT_PTR)p;

   // add new string if not already in map
   if (nString == -1)
   {
      // initialize map if necessary
      if (m_pStringMap == NULL)
         m_pStringMap = new CMapStringToPtr;

      // add new string to toolbar list
      CString strTemp(lpszText, lstrlen(lpszText)+1);
      nString = (INT_PTR)DefWindowProc(TB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strTemp);
      if (nString == -1)
         return FALSE;

      // cache string away in string map
      m_pStringMap->SetAt(lpszText, (void*)nString);
      ASSERT(m_pStringMap->Lookup(lpszText, p));
   }

   // change the toolbar button description
   TBBUTTON button;
   _GetButton(nIndex, &button);
   button.iString = nString;
   _SetButton(nIndex, &button);

   return TRUE;
}

CString CToolBar256::GetButtonText(int nIndex) const
{
   CString strResult;
   GetButtonText(nIndex, strResult);
   return strResult;
}

void CToolBar256::GetButtonText(int nIndex, CString& rString) const
{
   if (m_pStringMap != NULL)
   {
      // get button information (need button.iString)
      TBBUTTON button;
      _GetButton(nIndex, &button);

      if ( button.iString >=0  ) {
         // look in map for matching iString
         POSITION pos = m_pStringMap->GetStartPosition();
         CString str; void* p;
         while (pos)
         {
            m_pStringMap->GetNextAssoc(pos, str, p);
            if ((INT_PTR)p == button.iString)
            {
               rString = str;
               return;
            }
         }
      }
   }
   rString.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 message handlers

BEGIN_MESSAGE_MAP(CToolBar256, CControlBar)
   //{{AFX_MSG_MAP(CToolBar256)
   ON_WM_NCHITTEST()
   ON_WM_NCPAINT()
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
   ON_WM_NCCALCSIZE()
   ON_WM_WINDOWPOSCHANGING()
   ON_WM_NCCREATE()
//    ON_MESSAGE(WM_SETTINGCHANGE, OnSettingChange)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CToolBar256::OnEraseBkgnd(CDC*)
{                   
   return (BOOL)Default();
}

UINT CToolBar256::OnNcHitTest(CPoint)
{
   return HTCLIENT;
}

void CToolBar256::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS* lpncsp)
{
   // calculate border space (will add to top/bottom, subtract from right/bottom)
   CRect rect; rect.SetRectEmpty();
   BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   CControlBar::CalcInsideRect(rect, bHorz);
   ASSERT(_afxComCtlVersion != -1);
   ASSERT(_afxComCtlVersion >= VERSION_IE4 || rect.top >= 2);

   // adjust non-client area for border space
   lpncsp->rgrc[0].left += rect.left;
   lpncsp->rgrc[0].top += rect.top;
   // previous versions of COMCTL32.DLL had a built-in 2 pixel border
   if (_afxComCtlVersion < VERSION_IE4)
      lpncsp->rgrc[0].top -= 2;
   else if (_afxComCtlVersion < VERSION_6 && !(GetStyle() & TBSTYLE_FLAT))
      lpncsp->rgrc[0].top -= 2;
   lpncsp->rgrc[0].right += rect.right;
   lpncsp->rgrc[0].bottom += rect.bottom;
}

void CToolBar256::OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle)
{
   // a dynamically resizeable toolbar can not have the CBRS_FLOAT_MULTI
   ASSERT(!((dwNewStyle & CBRS_SIZE_DYNAMIC) &&
         (m_dwDockStyle & CBRS_FLOAT_MULTI)));

   // a toolbar can not be both dynamic and fixed in size
   ASSERT (!((dwNewStyle & CBRS_SIZE_FIXED) &&
      (dwNewStyle & CBRS_SIZE_DYNAMIC)));

   // CBRS_SIZE_DYNAMIC can not be disabled once it has been enabled
   ASSERT (((dwOldStyle & CBRS_SIZE_DYNAMIC) == 0) ||
      ((dwNewStyle & CBRS_SIZE_DYNAMIC) != 0));

   if (m_hWnd != NULL &&
      ((dwOldStyle & CBRS_BORDER_ANY) != (dwNewStyle & CBRS_BORDER_ANY)))
   {
      // recalc non-client area when border styles change
      SetWindowPos(NULL, 0, 0, 0, 0,
         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
   }
   m_bDelayedButtonLayout = TRUE;
}

void CToolBar256::OnNcPaint()
{
   // Get window DC that is clipped to the non-client area.
   CWindowDC dc(this);
   CRect rectClient;
   GetClientRect(rectClient);
   CRect rectWindow;
   GetWindowRect(rectWindow);
   ScreenToClient(rectWindow);
   rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
   dc.ExcludeClipRect(rectClient);

   COLORREF 
#ifdef _THEMED_
   clr = 0;
   HTHEME hTheme = OpenThemeData(m_hWnd, L"Toolbar");
   if ( hTheme ) {
      GetThemeColor(hTheme, TP_BUTTON, TS_NORMAL, TMT_FILLCOLORHINT, &clr);
      CloseThemeData(hTheme);
   } 
   if ( !clr ) 
#endif      
   clr = ::GetSysColor(COLOR_BTNFACE);

   // Draw the borders in the non-client area.
   rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
   if ( (GetStyle() & TBSTYLE_FLAT) && (m_dwStyle & CBRS_BORDER_ANY) ) {
      dc.FillSolidRect( &rectWindow, clr); // Fill in the background.
   }
   DrawBorders(&dc, rectWindow);

   // Erase the parts that are not drawn.
   dc.IntersectClipRect(rectWindow);
   SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

   // Draw the gripper in the non-client area.
   DrawGripper(&dc, rectWindow);
}

void CToolBar256::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
   // not necessary to invalidate the borders
   DWORD dwStyle = m_dwStyle;
   m_dwStyle &= ~(CBRS_BORDER_ANY);
   CControlBar::OnWindowPosChanging(lpWndPos);
   m_dwStyle = dwStyle;
    
   // If we can resize while floating
   if (dwStyle & CBRS_SIZE_DYNAMIC)
   {
      // And we are resizing
      if (lpWndPos->flags & SWP_NOSIZE)
         return;

      // Then redraw the buttons
      Invalidate();
   }
}

void CToolBar256::OnPaint()
{
   if (m_bDelayedButtonLayout)
      Layout();

   Default();
}


/////////////////////////////////////////////////////////////////////////////


CToolBar256::CChild* CToolBar256::CreateToolPane(UINT nId, LPTSTR classname, 
                                       LPCTSTR pszWindowName, 
                                       int width, DWORD style, 
                                       DWORD exstyle)
{
   CChild *e =  new CChild; 
   if ( CreateToolPaneEx(e, nId, classname, 
                                  pszWindowName, width, 
                                  style, exstyle)
       ) 
   {
      return e;
   }

   delete e;
   return NULL;
}

BOOL CToolBar256::CreateToolPaneEx(CWnd *pWnd, UINT nId, LPTSTR classname, 
                                  LPCTSTR pszWindowName, int width, 
                                  DWORD style, DWORD exstyle)
{
   RECT rc;
   int idx = CommandToIndex( nId );
   if ( idx < 0 ) return FALSE;
   GetItemRect(idx, &rc);
   SetButtonInfo(idx, nId, TBBS_SEPARATOR, width+2);
   CWnd *e = pWnd; 
   rc.left++;
   rc.right = rc.left + width;
   if ( 
      !e->CreateEx(exstyle,
      classname, pszWindowName,
      WS_CHILD|WS_VISIBLE|style, 
      rc,
      this,
      nId)) 
   {
      //delete e;
      return NULL;
   }
   HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
   if (hFont == NULL) hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

   e->SendMessage(WM_SETFONT, (WPARAM)hFont);
   return TRUE;
}

BOOL CToolBar256::CreateStandard(CWnd * pParentWnd, UINT nId, DWORD dwCbrs)
{
   if ( !CreateEx(pParentWnd, TBSTYLE_FLAT,
       WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
       CRect(2, 2, 2, 2)) ||
      !LoadToolBar(nId)) return FALSE;

   SetBarStyle(GetBarStyle() |
      CBRS_TOOLTIPS | CBRS_FLYBY | dwCbrs | //CBRS_SIZE_DYNAMIC|
      CBRS_GRIPPER| CBRS_BORDER_ANY | CBRS_BORDER_3D);
   EnableDocking(CBRS_ALIGN_ANY);
   return TRUE;
}

BOOL CToolBar256::CreateListed(CWnd * pParentWnd, UINT nIdResource, DWORD dwCbrs, UINT nID)
{
   if ( !CreateEx(pParentWnd, TBSTYLE_FLAT|TBSTYLE_LIST,
       WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
       CRect(2, 2, 2, 2), nID) ||
      !LoadToolBar(nIdResource)) return FALSE;

   SetBarStyle(GetBarStyle() |
      CBRS_TOOLTIPS | CBRS_FLYBY | dwCbrs | //CBRS_SIZE_DYNAMIC|
      CBRS_GRIPPER| CBRS_BORDER_ANY | CBRS_BORDER_3D);
   EnableDocking(CBRS_ALIGN_ANY);
   return TRUE;
}

void CToolBar256::SetSeparatorsSize(int nSize)
{
   int n = GetToolBarCtrl().GetButtonCount();
   UINT ID, nStyle;
   int nImage;
   for ( int i=0; i<n ;i++ ) {
      GetButtonInfo(i, ID, nStyle, nImage);
      if ( nStyle == TBBS_SEPARATOR ) {
         SetButtonInfo(i, (UINT)-1, TBBS_SEPARATOR, nSize);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 idle update through CToolCmdUI class

class CToolCmdUI256 : public CCmdUI        // class private to this file !
{
public: // re-implementations only
   virtual void Enable(BOOL bOn);
   virtual void SetCheck(int nCheck);
   virtual void SetText(LPCTSTR lpszText);
};

void CToolCmdUI256::Enable(BOOL bOn)
{
   m_bEnableChanged = TRUE;
   CToolBar256* pToolBar = (CToolBar256*)m_pOther;
   ASSERT(pToolBar != NULL);
   if ( !pToolBar ) return;

   ASSERT_KINDOF(CToolBar256, pToolBar);
   ASSERT(m_nIndex < m_nIndexMax);

   UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;
   if (!bOn)
   {
      nNewStyle |= TBBS_DISABLED;
      // WINBUG: If a button is currently pressed and then is disabled
      // COMCTL32.DLL does not unpress the button, even after the mouse
      // button goes up!  We work around this bug by forcing TBBS_PRESSED
      // off when a button is disabled.
      nNewStyle &= ~TBBS_PRESSED;
   }
   ASSERT(!(nNewStyle & TBBS_SEPARATOR));
   pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}

void CToolCmdUI256::SetCheck(int nCheck)
{
   ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
   CToolBar256* pToolBar = (CToolBar256*)m_pOther;
   ASSERT(pToolBar != NULL);
   if ( !pToolBar ) return;

   ASSERT_KINDOF(CToolBar256, pToolBar);
   ASSERT(m_nIndex < m_nIndexMax);

   UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) &
            ~(TBBS_CHECKED | TBBS_INDETERMINATE);
   if (nCheck == 1)
      nNewStyle |= TBBS_CHECKED;
   else if (nCheck == 2)
      nNewStyle |= TBBS_INDETERMINATE;
   ASSERT(!(nNewStyle & TBBS_SEPARATOR));
   pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}

void CToolCmdUI256::SetText(LPCTSTR)
{
   // ignore it
}

void CToolBar256::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   CToolCmdUI256 state;
   state.m_pOther = this;

   if ( m_bParentUpdateUIOnly ) {
      CFrameWnd* pOwner = (CFrameWnd*)GetOwner();
      if ( pOwner ) pTarget = pOwner;
   }

   state.m_nIndexMax = (UINT)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
   {
      // get buttons state
      TBBUTTON button;
      _GetButton(state.m_nIndex, &button);
      state.m_nID = button.idCommand;

      // ignore separators
      if (!(button.fsStyle & TBSTYLE_SEP))
      {
         // allow reflections
         if (CWnd::OnCmdMsg(0, 
            MAKELONG(CN_UPDATE_COMMAND_UI&0xff, WM_COMMAND+WM_REFLECT_BASE), 
            &state, NULL))
            continue;

         // allow the toolbar itself to have update handlers
         if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
            continue;

         // allow the owner to process the update
         state.DoUpdate(pTarget, bDisableIfNoHndler);
      }
   }

   // update the dialog controls added to the toolbar
   UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

#ifdef _DEBUG
void CToolBar256::AssertValid() const
{
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC(CToolBar256, CControlBar)

/////////////////////////////////////////////////////////////////////////////









