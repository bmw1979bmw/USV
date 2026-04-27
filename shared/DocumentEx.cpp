// DocumentEx.cpp : implementation file
//

#include "stdafx.h"
//#include "Docs.h"
#include "DocumentEx.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
void AFXAPI AfxGetRoot(LPCTSTR lpszPath, CString& strRoot);
void AFX_CDECL AfxTimeToFileTime(const CTime& time, LPFILETIME pFileTime);

/////////////////////////////////////////////////////////////////////////////
// CDocumentEx
/*
Docs(4.18.73)
+ Ура! Починил дебильный MFC,  который не создавал временный файл, 
потому что не мог найти свободное пространство больше 4Гб и размер файла тоже!
Написан классик CDocumentEx. В нем заменена функция GetFile, 
работающая с замененным классом CMirrorFileEx : public CMirrorFile, 
Open которого корректно обрабатывает свободное пространство и размер файла.
Можно пользоваться. Переписать Docs\DocumentEx.h, cpp, 
произвести свой документ от СDocumentEx (Ручками дописать Ex).
*/

class CMirrorFileEx : public CMirrorFile
{
public:
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);
};

BOOL CMirrorFileEx::Open(LPCTSTR lpszFileName, UINT nOpenFlags,
	CFileException* pError)
{
	ASSERT(lpszFileName != NULL);
	m_strMirrorName.Empty();

//	CFileStatus status;
   WIN32_FIND_DATA ffd;
	if (nOpenFlags & CFile::modeCreate) //opened for writing
	{
   	TCHAR m_szFullName[_MAX_PATH]; // absolute path name
	   if (AfxFullPath(m_szFullName, lpszFileName))
	   {
	      HANDLE hFind = FindFirstFile((LPTSTR)lpszFileName, &ffd);
         if (hFind != INVALID_HANDLE_VALUE) {
	         FindClose(hFind);
   			CString strRoot;
	   		AfxGetRoot(lpszFileName, strRoot);
			   ULARGE_INTEGER Free, t;
			   if (GetDiskFreeSpaceEx(strRoot, &Free, &t, NULL) )
			   {
               t.u.HighPart = ffd.nFileSizeHigh;
               t.u.LowPart = ffd.nFileSizeLow;
			      if (Free.QuadPart > 2 * t.QuadPart) // at least 2x free space avail
			      {
                  // из версии DevStudio8
				      // get the directory for the file
				      TCHAR szPath[_MAX_PATH];
				      LPTSTR lpszName;
				      GetFullPathName(lpszFileName, _MAX_PATH, szPath, &lpszName);
				      *lpszName = NULL;

                  char *cp = strrchr(ffd.cFileName,'.');
                  if ( cp ) cp++;
                  else cp = _T("MFC");
                  if ( !*cp ) cp = _T("MFC");
				      //let's create a temporary file name
				      GetTempFileName(szPath, cp/*_T("MFC")*/, 0,
					      m_strMirrorName.GetBuffer(_MAX_PATH+1));
				      m_strMirrorName.ReleaseBuffer();

//				      m_strMirrorName = GetTempName(lpszFileName, TRUE);
			      }
            }
         }
	   }
	}

	if (!m_strMirrorName.IsEmpty() &&
		CFile::Open(m_strMirrorName, nOpenFlags, pError))
	{
		m_strFileName = lpszFileName;
		FILETIME ftCreate, ftAccess, ftModify;
		if (::GetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify))
		{
			AfxTimeToFileTime(CTime(ffd.ftCreationTime), &ftCreate);
			SetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify);
		}

		DWORD dwLength = 0;
		PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
		if (GetFileSecurity(lpszFileName, DACL_SECURITY_INFORMATION,
			NULL, dwLength, &dwLength))
		{
			pSecurityDescriptor = (PSECURITY_DESCRIPTOR) new BYTE[dwLength];
			if (::GetFileSecurity(lpszFileName, DACL_SECURITY_INFORMATION,
				pSecurityDescriptor, dwLength, &dwLength))
			{
				SetFileSecurity(m_strMirrorName, DACL_SECURITY_INFORMATION, pSecurityDescriptor);
			}
			delete[] (BYTE*)pSecurityDescriptor;
		}
		return TRUE;
	}
	m_strMirrorName.Empty();
	return CFile::Open(lpszFileName, nOpenFlags, pError);
}

CFile* CDocumentEx::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,
	CFileException* pError)
{
	CMirrorFileEx* pFile = new CMirrorFileEx;
	ASSERT(pFile != NULL);
	if (!pFile->Open(lpszFileName, nOpenFlags, pError))
	{
		delete pFile;
		pFile = NULL;
	}
	return pFile;
}
