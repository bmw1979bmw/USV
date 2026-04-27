// LArray.h: interface for the LArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LARRAY_H__935E6E11_6EB7_11D5_B7AB_0050DAC48A11__INCLUDED_)
#define AFX_LARRAY_H__935E6E11_6EB7_11D5_B7AB_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TextArchive.h"

enum HydrophoneTypes
{
   Omnidir=0,
   Half_Sphere,
   Cardioid
};
enum ArrayTypes
{
   Plane=0,
   Vert_Cylindr,
   Hor_Cylindr,
   Sphere,
   Disk
};

//------------------------------------------------------------------------------

class LArray  
{
public:
	void Serialize(CTextArchive &ar);
	LArray();
	virtual ~LArray();
public:
	BOOL GetEffSize(float &EffHeight, float &EffWidth);
	float GetPattern(float ha, float va);
	void DeleteContents(void);

   int      m_AntennaType;
   int      m_HydrophoneType;
   float    m_Dh;   // Hor param
   float    m_Dv;   // Ver param
   int      m_Nh;   // # Hor elem
   int      m_Nv;   // # Ver elem
   float    m_teta0;// compensate 
   float    m_fi0;  // compensate
   float    m_dTeta;// углы по высоте\\_/если m_bQuart->антенна от 0 до m_dFi\r\n"     
   float    m_dFi;  // углы по ширине/ \\иначе от -m_dFi до m_dFi, teta аналогично\r\n"
   float    m_fl;   // Freq low
   float    m_df;   // band
   int      m_cf;   // # freq
   float    m_nf;     // Decr  степень спада спектра //  int was
   BOOL     m_bQuart;  // BOOL четверть
   int      m_nPoints;  // точек*точек

   float   *m_Patt;
   
   float    m_KK;      // коэффициент концентрации
	float	   m_Width07; // ширина диаграммы по уровню 0.7
	float	   m_Width07V; // ширина диаграммы по уровню 0.7 Вертикальная плоскость
   
   float    m_SigmaErr; // СКО ошибки возбуждения
};

#endif // !defined(AFX_LARRAY_H__935E6E11_6EB7_11D5_B7AB_0050DAC48A11__INCLUDED_)
