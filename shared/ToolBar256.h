#pragma once
#ifndef __ToolBar256_H__
#define __ToolBar256_H__

// v.1.0.2

//! Класс окна "Панель инструментов".
//! Функции соответствуют MFC-классу CToolBar
class CToolBar256 : public CControlBar
{
/// @cond 
   DECLARE_DYNAMIC(CToolBar256)

// Construction
public:
   CToolBar256();

//typedef UINT UINT;

   BOOL Create(CWnd* pParentWnd,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
      UINT nID = AFX_IDW_TOOLBAR);
#if _MFC_VER >= 0x0600
   BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
      CRect rcBorders = CRect(0, 0, 0, 0),
      UINT nID = AFX_IDW_TOOLBAR);
#endif

   BOOL LoadToolBar(UINT lpszResourceName, COLORREF clrTransparent = RGB(192, 192, 192));
   BOOL LoadToolBar(LPCTSTR lpszResourceName, COLORREF clrTransparent = RGB(192, 192, 192))
   { return LoadToolBar((UINT)lpszResourceName, clrTransparent); }

   int AddBitmap(int nNumButtons, UINT nBitmapID, COLORREF clrTransparent = RGB(192, 192, 192));
   BOOL SetButtons(const WORD* lpIDArray, int nIDCount);
   // lpIDArray can be NULL to allocate empty buttons

   void SetSameFont(HWND hWndDlg);
   // for Dialogs: move to dialog control coordinates
   void MoveToControl(HWND hWndDlg, UINT nID, BOOL bBottom);

// Attributes
public:
   // standard control bar things
   int CommandToIndex(UINT nIDFind) const;
   UINT GetItemID(int nIndex) const;
   virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
   UINT GetButtonStyle(int nIndex) const;
   void SetButtonStyle(int nIndex, UINT nStyle);

   // for changing button info
   void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
   void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);
   BOOL SetButtonText(int nIndex, LPCTSTR lpszText);
   CString GetButtonText(int nIndex) const;
   void GetButtonText(int nIndex, CString& rString) const;

   // for direct access to the underlying common control
   CToolBarCtrl& GetToolBarCtrl() const { return *(CToolBarCtrl*)this; }

   DWORD SetExtededStyle(DWORD dwExStyle) {
      return (DWORD)SendMessage(TB_SETEXTENDEDSTYLE, 0, dwExStyle);
   }


// Implementation
public:
   virtual ~CToolBar256();
   virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
   virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
   virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
   virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
   void SetOwner(CWnd* pOwnerWnd);
   virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);

#ifdef _DEBUG
   void AssertValid() const;
#endif

   // OnUpdateCmdUi will check only parent/owner
   void SetParentUpdateUIOnly(BOOL bSet) { m_bParentUpdateUIOnly = bSet; }

protected:
   BOOL m_bDelayedButtonLayout; // used to manage when button layout should be done

   BOOL m_bParentUpdateUIOnly; // OnUpdateCmdUi will check only parent/owner

   HIMAGELIST m_hIL;

   CMapStringToPtr* m_pStringMap;  // used as CMapStringToUInt

   // implementation helpers
   void _GetButton(int nIndex, TBBUTTON* pButton) const;
   void _SetButton(int nIndex, TBBUTTON* pButton);
   CSize CalcLayout(DWORD nMode, int nLength = -1);
   CSize CalcSize(TBBUTTON* pData, int nCount);
   int WrapToolBar(TBBUTTON* pData, int nCount, int nWidth);
   void SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert = FALSE);
#if _MFC_VER >= 0x0600
   void Layout(); // called for for delayed button layout
#endif

   //{{AFX_MSG(CToolBar256)
   afx_msg UINT OnNcHitTest(CPoint);
   afx_msg void OnNcPaint();
   afx_msg void OnPaint();
   afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
   afx_msg void OnWindowPosChanging(LPWINDOWPOS);
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   class CChild : public CWnd
   {
      //{{AFX_VIRTUAL(CChild)
      protected:
      virtual void PostNcDestroy() 
      {
            CWnd::PostNcDestroy();
            delete this;
      }
      //}}AFX_VIRTUAL
   };
public:

   BOOL CreateStandard(CWnd* pParentWnd, UINT nId, DWORD dwCbrs=0);

   BOOL CreateListed(CWnd* pParentWnd, UINT nIdResource,
      DWORD dwCbrs=0, UINT nID = AFX_IDW_TOOLBAR);

   CChild* CreateEditPane(UINT nId, int width=60, DWORD style=ES_LEFT)
   { return CreateToolPane(nId, _T("EDIT"), NULL, width, style); }

   CChild* CreateButtonPane(UINT nId, int width=60, 
      DWORD style=BS_PUSHLIKE|BS_CHECKBOX) /* style for non-border button */
   { return CreateToolPane(nId, _T("BUTTON"), NULL, width, style, WS_EX_STATICEDGE); }

   CChild* CreateStaticPane(UINT nId, int width=60, DWORD style=SS_CENTER)
   { return CreateToolPane(nId, _T("STATIC"), NULL, width, style); }

   CChild* CreateStaticPane(UINT nId, LPCTSTR pszWindowName, int width=60, DWORD style=SS_CENTER)
   { return CreateToolPane(nId, _T("STATIC"), pszWindowName, width, SS_CENTER|style, 0); }

   CChild* CreateToolPane( 
      UINT nId, 
      LPTSTR classname, 
      LPCTSTR pszWindowName=NULL,
      int width=60, 
      DWORD style=0,
      DWORD exstyle=WS_EX_CLIENTEDGE);
   BOOL CreateToolPaneEx( 
      CWnd *pWnd,
      UINT nId, 
      LPTSTR classname, 
      LPCTSTR pszWindowName=NULL,
      int width=60, 
      DWORD style=0,
      DWORD exstyle=WS_EX_CLIENTEDGE);

   void SetSeparatorsSize(int nSize);
/// @endcond 
};



#endif