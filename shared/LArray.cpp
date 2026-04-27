// LArray.cpp: implementation of the LArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LArray.h"
#include "math2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LArray::LArray()
{
   m_Patt = NULL;
}

LArray::~LArray()
{
   delete m_Patt;
}

void LArray::DeleteContents()
{
   delete m_Patt;   m_Patt = NULL; 
}

//#ifdef ZERORETURN
#define _C(i) "\r\n"
//#else
//#define _C(i) i
//#endif


void LArray::Serialize(CTextArchive &ar)
{
   if (ar.IsStoring()) {	
      // storing code
      ar << "\r\n"; // ;LArray по мощности\r\n";
      ar << 70                << _C(";Version\r\n")
         << m_AntennaType     << _C("; m_AntennaType\r\n")
         << m_HydrophoneType  << _C("; m_HydrophoneType\r\n")
         << m_Dh              << _C("; m_Dh размер по гориз. м или радиус\r\n")
         << m_Dv              << _C("; m_Dv размер по верт. м или радиус\r\n")
         << m_Nh              << _C("; m_Nh элементов по гориз.\r\n")
         << m_Nv              << _C("; m_Nv элементов по верт.\r\n")
         << m_teta0           << _C("; m_teta0 углы компенсации\r\n")
         << m_fi0             << _C("; m_fi0 углы компенсации\r\n")
         << m_dTeta           << _C("; m_dTeta углы по высоте\\_/если m_bQuart->антенна от 0 до m_dFi\r\n")
         << m_dFi             << _C("; m_dFi   углы по ширине/ \\иначе от -m_dFi до m_dFi, teta аналогично\r\n")
         << m_fl              << _C("; m_fl ниж. частота\r\n")
         << m_df              << _C("; m_df полоса\r\n")
         << m_cf              << _C("; m_cf число частот\r\n")
         << m_nf              << _C("; m_nf степень спада спектра\r\n")
         << m_bQuart          << _C("; m_bQuart признак, что четверть\r\n")
         << m_KK              << _C("; m_KK коэф. концентрации\r\n")
         << m_Width07         << _C("; m_Width07 ширина по ур. 0.7\r\n" )
         << m_Width07V        << _C("; m_Width07V вер ширина по ур. 0.7\r\n")
         << m_nPoints         << _C("; m_nPoints число точек\r\n")
         << _C("; Pattern:  [m_nPoints*m_nPoints] по строкам");
      for ( int i=0; i<m_nPoints*m_nPoints ;i++ ) {
         if (!( i%m_nPoints ) ) ar << "\r\n";
         ar << m_Patt[i];
      }
   } else {	
      // loading code
      int v;
      ar >> v;
      if ( v == 70 ) ar >> m_AntennaType;
      else m_AntennaType = v;

      ar >> m_HydrophoneType
         >> m_Dh
         >> m_Dv
         >> m_Nh
         >> m_Nv
         >> m_teta0
         >> m_fi0
         >> m_dTeta
         >> m_dFi
         >> m_fl
         >> m_df
         >> m_cf
         >> m_nf     
         >> m_bQuart
         >> m_KK
         >> m_Width07;
      if ( v == 70 ) ar >> m_Width07V;
      ar >> m_nPoints;
      ASSERT(!m_Patt);   // delete m_Patt;
      if ( m_nPoints <= 0 || m_nPoints > 2048 ) {
         AfxThrowArchiveException(CArchiveException::badIndex,
            ar.GetFile()->GetFileName());
      }
      m_Patt = new float [m_nPoints*m_nPoints];
      for ( int i=0; i<m_nPoints*m_nPoints ;i++ ) {
         ar >> m_Patt[i];
      }
   }
}

float LArray::GetPattern(float ha, float va)
{
   if ( !m_Patt ) {
      TRACE(_T("LArray::m_Patt == NULL"));
      return 0;
   }
   ASSERT(m_bQuart); // пожалуйста только четверть!!!

   _Qfabs(ha);
   if ( ha > m_dTeta ) return 0;
   _Qfabs(va);
   if ( va > m_dFi ) return 0;

   int npt = m_nPoints-1;
   int iha = (int)( ha*npt/m_dTeta ); // ближайшая точка (индекс)
   float kha = iha*m_dTeta/npt; // угол ближайшей точки
   int iva = (int)( va*npt/m_dFi );
   float kva = iva*m_dFi/npt;
   ASSERT(iha < m_nPoints && iva < m_nPoints );
   float *p = &m_Patt[ iva*m_nPoints + iha ];
   float D  = *p;

   float Dh = ( iha >= m_nPoints-1)?0:p[1];
   float Dv = ( iva >= m_nPoints-1)?0:p[m_nPoints];   // по строкам

   D -=
      ((D-Dv)*(va-kva) + (D-Dh)*(ha-kha))
      /
      2;
   return D; 
}

BOOL LArray::GetEffSize(float &EffHeight, float &EffWidth)
{
   switch ( m_AntennaType ) {
   case Plane:   // плоская
      EffHeight=m_Dv;
      EffWidth=m_Dh;
      break;
   case Vert_Cylindr:   //цилиндрическая
      EffHeight=m_Dv;
      EffWidth=(float)(2*m_Dh*fsin(m_dTeta/2*GrRd));
      break;
   case Hor_Cylindr:   //цилиндрическая
      EffHeight=(float)(2*m_Dv*fsin(m_dFi/2*GrRd));
      EffWidth=m_Dh;
      break;
   case Sphere:    // шар
      EffHeight=(float)(2*m_Dh*fsin(m_dFi/2*GrRd));
      EffWidth=(float)(2*m_Dh*fsin(m_dTeta/2*GrRd));
      break;
   default: ;
      EffHeight = EffWidth = 0;
      return FALSE;
   }
   return TRUE;
}
