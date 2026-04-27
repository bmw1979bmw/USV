#pragma once
#include "FMSync.h"

class CFMExchange;
class CStandApp : public CWinApp
{
   DECLARE_DYNAMIC(CStandApp)

public:
   CStandApp();

   UINT m_nDebugExit;  // в World'e теряется WM_QUIT

   CFMExchange *m_pxchg;
   //{{AFX_VIRTUAL(CStandApp)
public:
   virtual int Run();
   virtual BOOL PumpMessage();     // low level message pump
   //}}AFX_VIRTUAL
   
   //{{AFX_MSG(CStandApp)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


