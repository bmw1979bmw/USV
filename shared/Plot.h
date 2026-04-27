#pragma once

inline void Plot_Mark(CWorldClient &xworld,
                      DWORD nID, 
                      float X, float Y, float Z, float K, float V, float L, float D, 
                      DWORD dwNoises, DWORD dwAcFlags,
                      BOOL  bMove,
                      int   nType)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      int   nType;
   } st = { 
      nID,
      X, Y, Z, K, V, L, D,
      dwNoises, dwAcFlags,
      bMove,
      nType
   }; 

   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'MARK', &st, sizeof st);
}

inline void Plot_Ellipse(CWorldClient &xworld,
                         DWORD nID, 
                         float X, float Y, float Z, float K, float V, float L, float D, 
                         DWORD dwNoises, DWORD dwAcFlags,
                         BOOL  bMove,
                         float Angle, float CX, float CY, float VX, float VY)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      float Angle; float CX; float CY; float VX; float VY;
   } st = { 
      nID,
         X, Y, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
     Angle, CX, CY, VX, VY
   }; 

   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'ELL0', &st, sizeof st);
}


inline void Plot_Rect(CWorldClient &xworld,
                      DWORD nID, 
                      float X, float Y, float Z, float K, float V, float L, float D, 
                      DWORD dwNoises, DWORD dwAcFlags,
                      BOOL  bMove,
                      float Angle, float CX, float CY, float VX, float VY)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      float Angle; float CX; float CY; float VX; float VY;
   } st = { 
      nID,
         X, Y, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
         Angle, CX, CY, VX, VY
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'RECT', &st, sizeof st);
}



inline void Plot_Circle(CWorldClient &xworld,
                        DWORD nID, 
                        float X, float Y, float Z, float K, float V, float L, float D, 
                        DWORD dwNoises, DWORD dwAcFlags,
                        BOOL  bMove,
                        int   nRadius)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      int   nRadius;
   } st = { 
      nID,
         X, Y, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
         nRadius
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'CIR0', &st, sizeof st);
}


inline void Plot_Pie(CWorldClient &xworld,
                     DWORD nID, 
                     float X, float Y, float Z, float K, float V, float L, float D, 
                     DWORD dwNoises, DWORD dwAcFlags,
                     BOOL  bMove,
                     float Course, float Sector, float Dist1, float Dist2
                     )
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      float Course; float Sector; float Dist1; float Dist2;
   } st = { 
      nID,
         X, Y, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
         Course, Sector, Dist1, Dist2
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'PIE0', &st, sizeof st);
}


inline void Plot_Poly(CWorldClient &xworld,
                      DWORD nID, 
                      float Z, float K, float V, float L, float D, 
                      DWORD dwNoises, DWORD dwAcFlags,
                      BOOL  bMove,
                      int   nPoints,
                      float *XY /*[nPoints*2]*/)
{
   struct TSendPoly {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      int   nPoints;
      float XY[1];
   };

   UINT nBytesSize = sizeof(TSendPoly) + nPoints*2*sizeof(float) - sizeof(float);
   TSendPoly *pdata = (TSendPoly *) new BYTE[nBytesSize];
   pdata->nID = nID;
   pdata->X = 0; // not used
   pdata->Y = 0; // not used
   pdata->Z = Z;
   pdata->K = K;
   pdata->V = V;
   pdata->L = L;
   pdata->D = D;
   pdata->dwNoises = dwNoises;
   pdata->dwAcFlags = dwAcFlags;
   pdata->bMove = bMove;
   pdata->nPoints = nPoints;
   memcpy(pdata->XY, XY, nPoints*2*sizeof(float));
            
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'POLY', pdata, nBytesSize);
   delete (BYTE*) pdata;
}


inline void Plot_PolyOLD(CWorldClient &xworld,
                      DWORD nID, 
                      float X0, float Y0, float Z, float K, float V, float L, float D, 
                      DWORD dwNoises, DWORD dwAcFlags,
                      BOOL  bMove,
                      int   nPoints,
                      float X1, float Y1, float X2, float Y2, float X3=0, float Y3=0, 
                      float X4=0, float Y4=0, float X5=0, float Y5=0, float X6=0, float Y6=0)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      int   nPoints;
         float X1; float Y1; float X2; float Y2; float X3; float Y3; 
         float X4; float Y4; float X5; float Y5; float X6; float Y6;
   } st = { 
      nID,
         X0, Y0, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
         nPoints,
         X1, Y1, X2, Y2, X3, Y3, 
         X4, Y4, X5, Y5, X6, Y6
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'POLY', &st, sizeof st);
}



inline void Plot_Color(CWorldClient &xworld,
                       DWORD nID, COLORREF color)
{
   struct {
      DWORD nID; COLORREF color;
   } st = { 
      nID, color
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'CLR0', &st, sizeof st);
}

inline void Plot_PenWidth(CWorldClient &xworld,
                       DWORD nID, DWORD width)
{
   struct {
      DWORD nID; DWORD width;
   } st = { 
      nID, width
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'PEN0', &st, sizeof st);
}


inline void Plot_Brush(CWorldClient &xworld,
                       DWORD nID, COLORREF color, DWORD nType)
{
   struct {
      DWORD nID; COLORREF color; DWORD nType;
   } st = { 
      nID, color, nType
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'BRU0', &st, sizeof st);
}


inline void Plot_FParam(CWorldClient &xworld,
                        DWORD nID, int iParam, float value)
{
   struct {
      DWORD nID; int iParam; float value;
   } st = { 
      nID, iParam, value
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'PARF', &st, sizeof st);
}


inline void Plot_IParam(CWorldClient &xworld,
                        DWORD nID, int iParam, int value)
{
   struct {
      DWORD nID; int iParam; int value;
   } st = { 
      nID, iParam, value
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'PARI', &st, sizeof st);
}



inline void Plot_DParam(CWorldClient &xworld,
                        DWORD nID, int iParam, DWORD value)
{
   struct {
      DWORD nID; int iParam; DWORD value;
   } st = { 
      nID, iParam, value
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'PARD', &st, sizeof st);
}

inline void Plot_Band(CWorldClient &xworld,
                      DWORD nID, 
                      float X, float Y, float Z, float K, float V, float L, float D, 
                      DWORD dwNoises, DWORD dwAcFlags,
                      BOOL  bMove,
                      float Angle, float W, float D1)
{
   struct {
      DWORD nID;
      float X; float Y; float Z; float K; float V; float L; float D; 
      DWORD dwNoises; DWORD dwAcFlags;
      BOOL  bMove;
      float Angle; float W; float D1;
   } st = { 
      nID,
         X, Y, Z, K, V, L, D,
         dwNoises, dwAcFlags,
         bMove,
         Angle, W, D1
   }; 
   
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'BAND', &st, sizeof st);
}



inline void Plot_Delete(CWorldClient &xworld,
                        DWORD nID)
{
   xworld.SendCommand(IALIAS_PLOTTER, (DWORD)-1, 'DEL0', &nID, sizeof nID);
}

