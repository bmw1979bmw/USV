//#include "stdafx.h"
//#include "WorldCommon.h"


void TWorldSystem::Serialize(CArchive &ar)
{
   CString t;
   // bPort - сериализуется через обмен - посылается имя для команд!
   if ( ar.IsStoring() ) {
      ar << iAlias;
      // для команд нужно имя
      if ( (iAlias & IALIAS_CMDMASK) && (iAlias & IALIAS_MASK) ) ar << sName_;
      else ar << t;
      ar << nType
         << nIconIdx
         << nIconCnt
         << fBumpDist
         << dwQuant
         << nLength
         << nClass
         << dwVersion
         << fMagMoment
         << fWidth;             
   } else {
      ar >> iAlias;
      if ( (iAlias & IALIAS_CMDMASK) && (iAlias & IALIAS_MASK) ) ar >> sName_;
      else ar >> t;
      ar >> nType
         >> nIconIdx;
      if ( ar.m_nObjectSchema >= 1 /*CUR_VERSION*/ ) {
         ar >> nIconCnt;
      } else {
         nIconCnt = IsCarrier()? 2 : 1;
      }
      ar >> fBumpDist
         >> dwQuant
         >> nLength
         >> nClass
         >> dwVersion
         >> fMagMoment
         >> fWidth;
   }
   Frm.Serialize(ar);
}
