#pragma once

// GridEdit2.h : header file
/////////////////////////////////////////////////////////////////////////////
// CGridEdit2 window
// служебное окошко для редактирования элементов списков и таблиц

#define WMY_UPDATEFROMEDIT (WM_APP+77)

class CGridEdit2 : public CEdit
{
public:
	//{{AFX_VIRTUAL(CGridEdit2)
	public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
   void SendUpdateFromEdit();
   BOOL bCancel;
   WPARAM  m_wParam;

public:
   BOOL Create( int nItem, int nSubItem, const RECT& rect, CWnd* pParentWnd, 
      DWORD dwStyle, UINT nID=55);

protected:
	//{{AFX_MSG(CGridEdit2)
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg UINT OnGetDlgCode();
   afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

