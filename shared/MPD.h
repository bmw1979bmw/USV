// MPD.h: interface for the CMAcl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPD_H__CCC2C644_F977_11D3_9C7C_0060970CEBA8__INCLUDED_)
#define AFX_MPD_H__CCC2C644_F977_11D3_9C7C_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stru_rgab.h"

class CMPD   // Класс для работы с обнаруженными П Д
{
private:
	enum {DELTA = 1};
	TPD* m_MPD;
	int m_Count;
	int m_maxCount;
public:
	BOOL Add(const TPD & item);
	void EditD( int i, float DD );
	void Empty();
	int GetCount() {return m_Count;}
	CMPD(int count=0);
	~CMPD();
	TPD &CMPD::operator[](int idx);
};

#endif // !defined(AFX_MPD_H__CCC2C644_F977_11D3_9C7C_0060970CEBA8__INCLUDED_)
