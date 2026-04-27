// MainView.h : interface of the CMainView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINVIEW_H__366DCE9D_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
#define AFX_MAINVIEW_H__366DCE9D_1140_11D5_B73F_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMainView : public CFormView
{
protected: // create from serialization only
   CMainView();
   DECLARE_DYNCREATE(CMainView)

public:
   //{{AFX_DATA(CMainView)
	enum { IDD = IDD_USV_FORM };
 	float	m_A;
	float	m_A_ALT;
	float	m_ALT;
	float	m_KURS;
	float	m_SPEED;
	int		m_SU_KURS;
	int		m_SU_SPEED;
	BOOL		m_ZIRC;
	int		m_POWER_CMD;

	//}}AFX_DATA

   HACCEL  m_hAccelTable;
// Attributes
public:
      UINT m_nUprCode;

   CMainDoc* GetDocument();

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMainView)
	public:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CMainView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
   //{{AFX_MSG(CMainView)
   afx_msg void OnKeysDown(UINT nID);
	afx_msg void OnTransmitt();
	afx_msg void OnBtnOk();
	afx_msg void OnUprKurs();
	afx_msg void OnUprSpeed();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MainView.cpp
inline CMainDoc* CMainView::GetDocument()
   { return (CMainDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINVIEW_H__366DCE9D_1140_11D5_B73F_0050DAC48A11__INCLUDED_)
