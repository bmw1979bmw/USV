// MPD.cpp : implementation of the CMPD class
//

#include "stdafx.h"
#include "MPD.h"
#include "FuncSv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Класс для работы с обнаруженными П и Д
BOOL CMPD::Add(const TPD & item)
{
	if (m_Count == m_maxCount)
	{
		TPD* newArray = new TPD[m_maxCount + DELTA];
		for (int i = 0; i < m_Count; i++) 
		{
			newArray[i] = m_MPD[i]; 
		}
		delete[] m_MPD;
		m_MPD = newArray;
		m_maxCount += DELTA;
	}
	m_MPD[m_Count] = item;
	m_Count++;
	return TRUE;
}

void CMPD::EditD( int i, float DD )
{
	m_MPD[i].D = DD;
}

void CMPD::Empty()
{
	m_Count = 0;                
}

CMPD::CMPD(int count)
{
	m_maxCount = count;
	m_Count = 0;
	if (m_maxCount)
	{
		m_MPD = new TPD[m_maxCount];
	} 
	else 
	{
		m_MPD = NULL;
	}
}

CMPD::~CMPD()
{
	if (m_maxCount)
	{
		delete[] m_MPD;
	}
}

TPD &CMPD::operator[](int idx)
{
//	ASSERT(idx >= 0 && idx < m_maxCount );
	return m_MPD[idx];
}

