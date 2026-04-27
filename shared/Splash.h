// CG: This file was added by the Splash Screen component.

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CSplashWnd : public CWnd
{
public:
// Construction
	CSplashWnd();
	CSplashWnd(int bEnable);

// Attributes:
public:
	CBitmap m_bitmap;
   CString m_szMessage;
// Operations
public:
   void Show();
   void Hide();

// Overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Message(LPCTSTR szMessage);
	~CSplashWnd();
	BOOL Create(BOOL bEnable);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
