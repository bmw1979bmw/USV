// WsgFile.h: interface for the CWsgFile class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define WSG_DICT_NUM 0x80000001

// WSS GROUP FILE
class CWsgFile  
{
public:
	BOOL RepairBoundaryErrors(CDWordArray &arr,BOOL bViewOnly);
	BOOL CorrectHeaderMinMax();
	BOOL CheckSequence(CDWordArray &arr);
	DWORD SeekToNum(DWORD num);
   
   void DeleteContents();
   void ClearTblArray();
	void Duplicate(CFile &f);
	BOOL LoadFullList(CDWordArray &arr);
	BOOL FindNum(DWORD nNum, DWORD &dwOff, DWORD *pdwSize=NULL);
	BOOL ExtractNum(DWORD nNum, LPCTSTR szExt, LPCTSTR szDir=NULL);
	BOOL ExtractAll(LPCTSTR szExt);
	CFile & GetFile();
	BOOL RenameOversizeAndReopenOriginal();//CheckForRenameSmart();
	BOOL CloseAndRenameOversize(BOOL bAdditionalCondition);
	void EnumEnd();
	BOOL EnumNext(DWORD &nNum, DWORD &dwOff, DWORD &dwSize);
	void EnumBegin();
	BOOL AppendFile(DWORD nNum, void *pData, UINT nDataLen);
	BOOL AppendFile(DWORD nNum, CFile &srcfile);
	BOOL AppendFileCRC(DWORD nNum, CFile &srcfile, ULONGLONG *pnWritten);
   BOOL LoadFileCRC(DWORD dwCode, CMemFile &memFile);
	BOOL AppendFileBegin(DWORD nNum);
	BOOL CloseAndRenameSmart(CString *ps=NULL);
	BOOL TruncateLast(DWORD &dwResultNum);
// 	BOOL Create();
   void InitNewFile(DWORD nTableItems);
	BOOL Open(LPCTSTR szFileName,
      BOOL bEnableCreate,
      BOOL bReadOnly,DWORD nTableItems = 2000/*,BOOL bCreateOnly*/);
   CWsgFile();
	virtual ~CWsgFile();
   void Close();

   UINT m_GrpSizeMb;
   BOOL m_bCreated; // только что создан
   BOOL Opened() const { return (UINT)file.m_hFile != (UINT)CFile::hFileNull; }

   int   m_nDigits;
   DWORD &nMin;
   DWORD &nMax;
   DWORD &rnTableItems;
protected:
	void WriteHead();
	void AppendNum(DWORD nNum,DWORD off=0/*0-if filelength*/);
	void UpdateMinMax();
	BOOL RemoveLastTable();

   TCHAR title[_MAX_PATH];
   DWORD head[8];
	CFile file;
   // enum
   DWORD *m_table;
   DWORD m_tblen;
   UINT m_nIndex;
   CString m_sOriginalName;

   // для FindNum
   CArray<DWORD*,DWORD*> m_TblArray;
   BOOL m_bTblArrayInvalid; // был AppendNum - m_TblArray надо перегрузить
};

