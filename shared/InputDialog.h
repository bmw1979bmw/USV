#pragma once

// InputDialog.h : header file
/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog

class CInputDialog : public CDialog
{
// Construction
public:
   CInputDialog(LPCTSTR szName, int *n, CWnd* pParent=NULL);
   CInputDialog(LPCTSTR szName, DWORD *dw, CWnd* pParent=NULL);
   CInputDialog(LPCTSTR szName, float *fl, CWnd* pParent=NULL);
   CInputDialog(LPCTSTR szName, TCHAR *s, int nStrlen, CWnd* pParent=NULL);
   CInputDialog(LPCTSTR szName, CString *s, int nStrlen, CWnd* pParent=NULL);

// Dialog Data
	//{{AFX_DATA(CInputDialog)
	enum { IDD = IDD_INPUTDIALOG };
	CString	sInput;
	float	   fInput;
   DWORD 	dwInput;
	CString	Name;
	//}}AFX_DATA

   int      nStrLen;
   char     cType;

   CString *sRes;
   TCHAR   *tRes;
	float	  *fRes;
   DWORD   *dwRes;
	
	//{{AFX_VIRTUAL(CInputDialog)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

