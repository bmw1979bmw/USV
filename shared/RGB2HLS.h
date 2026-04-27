// Favorites.h: interface for the Favorites class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAVORITES_H__0C9E76E4_B6AA_11D3_9C38_0060970CEBA8__INCLUDED_)
#define AFX_FAVORITES_H__0C9E76E4_B6AA_11D3_9C38_0060970CEBA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


DWORD RGBtoHLS(DWORD lRGBColor);
DWORD HLStoRGB(DWORD lHLSColor);
#define GetHValue(i)  GetRValue(i)
#define GetLValue(i)  GetGValue(i)
#define GetSValue(i)  GetBValue(i)

#endif // !defined(AFX_FAVORITES_H__0C9E76E4_B6AA_11D3_9C38_0060970CEBA8__INCLUDED_)
