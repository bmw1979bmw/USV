#pragma once
// GridCtrl2.h : header file
//
#ifdef GVCF_INDEX
#error Do not mix GridCtrl & GridCtrl2 !!!
#endif

#include "GridEdit2.h"

#define GVS_FIXROW      0        // obsolete
#define GVS_FIXCOL      0x0002
#define GVS_AUTOCOLSIZE 0        // obsolete
#define GVS_SELALWAYS   0x0008   //   LVS_SHOWSELALWAYS
#define GVS_SHAREDBUF   0x0010

#define GVS_ALL         0x001F

#define GRID_INDEX      0x00
#define GRID_INDEX1     0x01 
#define GRID_INDEX0     GRID_INDEX
#define GRID_DWORD      0x02
#define GRID_DWHEX      0x03
#define GRID_INT        0x04
#define GRID_CHECK      0x05 /* BOOL */
#define GRID_FLOAT      0x06
#define GRID_DOUBLE     0x07
#define GRID_SZARRAY    0x08
#define GRID_TYPEMASK    0x0F

#define GRID_FLOAT3     0x16   /* %.3f */  
#define GRID_FLOAT6     0x26   /* %f */    
#define GRID_FLOAT8     0x36   /* %.8f */  

#define GRID_DOUBLE3    0x17  /* %.3f */
#define GRID_DOUBLE6    0x27  /* %f */

#define GRID_EXTYPEMASK 0x3F

#define GRID_READONLY   0x100
#define GRID_READONLY1  0x200 /* read only все кроме первой */

#define GRID_IDVAL      0x400 /* ids == value */
#define GRID_SELALWAYS  0x800

#define GRID_AUTOSIZE   0x1000
#define GRID_INITSIZE   0x2000
/*************************************************/
/******* for compatiblity ************************/
#define GVCF_INDEX     GRID_INDEX     
#define GVCF_INDEX1    GRID_INDEX1    
#define GVCF_INDEX0    GRID_INDEX0    
#define GVCF_INT       GRID_INT       
#define GVCF_DWORD     GRID_DWORD     
#define GVCF_DWHEX     GRID_DWHEX     
#define GVCF_CHECK     GRID_CHECK     
#define GVCF_FLOAT     GRID_FLOAT     
#define GVCF_DOUBLE    GRID_DOUBLE    
#define GVCF_SZARRAY   GRID_SZARRAY   
#define GVCF_FLOAT3    GRID_FLOAT3    
#define GVCF_FLOAT6    GRID_FLOAT6    
#define GVCF_FLOAT8    GRID_FLOAT8    
#define GVCF_DOUBLE3   GRID_DOUBLE3   
#define GVCF_DOUBLE6   GRID_DOUBLE6   
#define GVCF_READONLY  GRID_READONLY  
#define GVCF_READONLY1 GRID_READONLY1 
#define GVCF_IDVAL     GRID_IDVAL     
#define GVCF_SELALWAYS GRID_SELALWAYS 
#define GVCF_AUTOSIZE  GRID_AUTOSIZE  
#define GVCF_INITSIZE  GRID_INITSIZE  
/*************************************************/

#define GVSS_HORZ     1
#define GVSS_VERT     2
#define GVSS_BOTH     3

struct TColumnInfo {
   BYTE signature;  // for debug
   DWORD flags;
   void *pData;
   int   nDataStep;
   UINT  nDTFormat;
   CString sToolTip;
};


/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx window

class CHeaderCtrlEx : public CHeaderCtrl
{
// Construction
public:
// Attributes
public:
// Operations
public:
   CToolTipCtrl m_ToolTip;
// Overrides
   //{{AFX_VIRTUAL(CHeaderCtrlEx)
   public:
   virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
   protected:
   virtual void PreSubclassWindow();
   virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
   //}}AFX_VIRTUAL
// Implementation
public:
   void UpdateToolTips(void);
   // Generated message map functions
protected:
   //{{AFX_MSG(CHeaderCtrlEx)
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnDestroy();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl2 window
class CGridCtrl2 : public CListCtrl
{
// Construction
public:
   CGridCtrl2();
// Attributes
public:

protected:
   CHeaderCtrlEx m_Head;
   DWORD m_dwFlags;
   CGridEdit2 m_Edit;
   int m_nSelCol;
   CImageList m_ChkList;
   int  m_nItemHeight; // optional

   BOOL m_bNotify;
// Operations
public:
   int GetColumnCount() { return m_Head.GetItemCount(); }
// Overrides
   //{{AFX_VIRTUAL(CGridCtrl2)
   public:
   virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
   protected:
   virtual void PreSubclassWindow();
   //}}AFX_VIRTUAL

// Implementation
protected:
   BOOL FormatData(int nrow,CString &s,TColumnInfo *ci);
   char * GetCellAddr(int nrow, TColumnInfo *ci);
public:
   TColumnInfo *GetColumnInfo(int ncol);
   BOOL GetCellRect(int nItem, int nSubItem, CRect &rc);
   virtual BOOL OnReadOnlyEdit() { return TRUE; }
   virtual BOOL EditCurSel(UINT nChar=0);
   void EnsureVisibleSubItem(int nSubItem);
   void InitSize();
   virtual ~CGridCtrl2();
   BOOL Init(int /*ncols*/, int nrows, DWORD dwFlags=0);
   BOOL InitColumn(int idx,LPCTSTR szName, int cx, DWORD dwflags, 
      void *data=NULL,
      UINT nFormat = DT_LEFT,
      int nDataStep = 4
      );
   BOOL InitColumn(int idx,UINT IdName, int cx, DWORD dwflags, 
      void *data=NULL,
      UINT nFormat = DT_LEFT,
      int nDataStep = 4
      );

   virtual void DrawItem(LPDRAWITEMSTRUCT pDIS);
   virtual void MeasureItem(LPMEASUREITEMSTRUCT pMIS);

public:
	void DeleteAllColumns();
   void ResetAutosizeFlags();
   void SetItemHeight(int cy); // 0-default, -1 - ==HeaderHeight
   void InsertFromClipboard(void);
   int GetFocusedItem() { return GetNextItem(-1,LVNI_FOCUSED|LVNI_SELECTED);}
   BOOL SetSizeToFit(DWORD dwFlags);
   void SetCurSel(int ncol, int nrow, BOOL bNotify=FALSE);
   CPoint GetCurSel();
   BOOL ChangeColumn(int idx, int nlen, void *pData, BOOL bRedraw=TRUE,int datastep=0);
   BOOL ChangeColumns(BOOL bRedraw=TRUE, void * pDummy=NULL);
   void SetColDataBuf(int ncol, void *buf, BOOL fRepaint=TRUE);
   void SetColumnStyle(int idx, DWORD dwRemove, DWORD dwAdd, BOOL bRedraw=TRUE);
   void SetColumnName(int idx, LPCTSTR szName, BOOL bRedraw=TRUE);
   CString GetColumnName(int idx);
   void MouseHitTest(CPoint point);

   // Generated message map functions
protected:
   //{{AFX_MSG(CGridCtrl2)
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg UINT OnGetDlgCode();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg LONG OnUpdateFromEdit( UINT wParam, LONG lParam);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

