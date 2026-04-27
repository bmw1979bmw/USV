#pragma once

// InfoPopupWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoPopupWnd window

class CInfoPopupWnd : public CWnd
{
// Construction
public:
   CInfoPopupWnd(CWnd *pParent, CPoint pt, LPCTSTR szMessage,CInfoPopupWnd **ppBackPtr = NULL);

   //{{AFX_VIRTUAL(CInfoPopupWnd)
   protected:
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   void UpdateText();
   void Move(CPoint pt);
   virtual ~CInfoPopupWnd();

   // Generated message map functions
protected:
   //{{AFX_MSG(CInfoPopupWnd)
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnSetText(WPARAM wParam, LPARAM lParam);
   afx_msg void OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   CInfoPopupWnd **m_ppBackPtr;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

