#pragma once
// EditViewEx.h : header file
/////////////////////////////////////////////////////////////////////////////
// CEditViewEx view

// find added!

class CEditViewEx : public CEditView
{
protected:
	CEditViewEx();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEditViewEx)

// Attributes
public:
   //CFindReplaceDialog *m_pFRDlg;
   HWND m_hFRDlg;
   CString m_sFind;
   BOOL m_bDownFind;

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEditViewEx)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEditViewEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditViewEx)
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