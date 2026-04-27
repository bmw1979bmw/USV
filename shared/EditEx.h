#pragma once
// EditEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditEx window

// find added!

class CEditEx : public CEdit
{
// Construction
public:
	CEditEx();

// Attributes
public:
   CFindReplaceDialog *m_pFRDlg;
   CString m_sFind;
   BOOL m_bDownFind;

// Operations
public:

	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
   afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//EOF