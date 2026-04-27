#ifndef _DOCUMENTEX_H_
#define _DOCUMENTEX_H_

#pragma once
// DocumentEx.h : header file

class CDocumentEx : public CDocument
{
public:
	virtual CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError);
};

//{{AFX_INSERT_LOCATION}}


#endif // !defined _DOCUMENTEX_H_
