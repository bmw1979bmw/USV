// Favorites.cpp: implementation of the Favorites class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RGB2HLS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//===========================================
#define  HLSMAX   240   /* H,L, and S vary over 0-HLSMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HLSMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HLSMAX must each fit in a byte. */

/* Hue is undefined if Saturation is 0 (grey-scale) */
/* This value determines where the Hue scrollbar is */
/* initially set for achromatic colors */
#define UNDEFINED (HLSMAX*2/3)

//===========================================
DWORD  RGBtoHLS(DWORD lRGBColor)
{
   WORD R,G,B;          /* input RGB values */
   WORD H,L,S;          /* input RGB values */
   WORD cMax,cMin;      /* max and min RGB values */
   WORD  Rdelta,Gdelta,Bdelta; /* intermediate value: % of spread from max */

   /* get R, G, and B out of DWORD */
   R = GetRValue(lRGBColor);
   G = GetGValue(lRGBColor);
   B = GetBValue(lRGBColor);

   /* calculate lightness */
   cMax = max( max(R,G), B);
   cMin = min( min(R,G), B);
   L = ( ((cMax+cMin)*HLSMAX) + RGBMAX )/(2*RGBMAX);

   if (cMax == cMin) {           /* r=g=b --> achromatic case */
      S = 0;                     /* saturation */
      H = UNDEFINED;             /* hue */
   }
   else {                        /* chromatic case */
      /* saturation */
      if (L <= (HLSMAX/2))
         S = ( ((cMax-cMin)*HLSMAX) + ((cMax+cMin)/2) ) / (cMax+cMin);
      else
         S = ( ((cMax-cMin)*HLSMAX) + ((2*RGBMAX-cMax-cMin)/2) )
            / (2*RGBMAX-cMax-cMin);

      /* hue */
      Rdelta = ( ((cMax-R)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Gdelta = ( ((cMax-G)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Bdelta = ( ((cMax-B)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);

      if (R == cMax)
         H = Bdelta - Gdelta;
      else if (G == cMax)
         H = (HLSMAX/3) + Rdelta - Bdelta;
      else /* B == cMax */
         H = ((2*HLSMAX)/3) + Gdelta - Rdelta;

      if (H < 0)
         H += HLSMAX;
      if (H > HLSMAX)
         H -= HLSMAX;
   }
   return RGB(H,L,S);
}

//===========================================
/* utility routine for HLStoRGB */
//===========================================
WORD HueToRGB(WORD n1,WORD n2,WORD hue)
{

   /* range check: note values passed add/subtract thirds of range */
   if (hue < 0)
      hue += HLSMAX;

   if (hue > HLSMAX)
      hue -= HLSMAX;

   /* return r,g, or b value from this tridrant */
   if (hue < (HLSMAX/6))
      return ( n1 + (((n2-n1)*hue+(HLSMAX/12))/(HLSMAX/6)) );
   if (hue < (HLSMAX/2))
      return ( n2 );
   if (hue < ((HLSMAX*2)/3))
      return ( n1 + (((n2-n1)*(((HLSMAX*2)/3)-hue)+(HLSMAX/12))/(HLSMAX/6)));
   else
      return ( n1 );
}
//===========================================
DWORD HLStoRGB(DWORD lHLSColor)
{
   WORD R,G,B;                /* RGB component values */
   WORD  Magic1,Magic2;       /* calculated magic numbers (really!) */

   WORD H,L,S;
   H = GetRValue(lHLSColor);
   L = GetGValue(lHLSColor);
   S = GetBValue(lHLSColor);

   if (S == 0) {            /* achromatic case */
      R=G=B=(L*RGBMAX)/HLSMAX;
      if (H != UNDEFINED) {
         return -1;
         /* ERROR */
      }
   }
   else  {                    /* chromatic case */
      /* set up magic numbers */
      if (L <= (HLSMAX/2))
         Magic2 = (L*(HLSMAX + S) + (HLSMAX/2))/HLSMAX;
      else
         Magic2 = L + S - ((L*S) + (HLSMAX/2))/HLSMAX;
      Magic1 = 2*L-Magic2;

      /* get RGB, change units from HLSMAX to RGBMAX */
      R = (HueToRGB(Magic1,Magic2,H+(HLSMAX/3))*RGBMAX + (HLSMAX/2))/HLSMAX;
      G = (HueToRGB(Magic1,Magic2,H)*RGBMAX + (HLSMAX/2)) / HLSMAX;
      B = (HueToRGB(Magic1,Magic2,H-(HLSMAX/3))*RGBMAX + (HLSMAX/2))/HLSMAX;
   }

   return RGB(R,G,B);
}
//===========================================
