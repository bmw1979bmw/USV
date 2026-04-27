#if !defined(AFX_LISTBOXEX_H__F9E91B54_8525_11D3_9BFF_0060970CEBA8__INCLUDED_)
#define AFX_LISTBOXEX_H__F9E91B54_8525_11D3_9BFF_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ListBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx window

class CListBoxEx : public CListBox
{
// Construction
public:
   CListBoxEx():CListBox(){}

// Attributes
public:

// Operations
public:

// Overrides
   //{{AFX_VIRTUAL(CListBoxEx)
   //}}AFX_VIRTUAL

// Implementation
public:
   CListBoxEx & operator <<(UINT id);
   int AddString( UINT id);
   int AddString(LPCTSTR s){return CListBox::AddString(s);}
   int operator <<= (int n) { return SetCurSel(n);}   
   void DeleteAllStrings(void);

   // Generated message map functions
protected:
   //{{AFX_MSG(CListBoxEx)
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__F9E91B54_8525_11D3_9BFF_0060970CEBA8__INCLUDED_)
