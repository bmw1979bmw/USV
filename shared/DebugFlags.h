#pragma once

#include "Developer.h"

#define DEBUGFLAG_00       0x0001
#define DEBUGFLAG_01       0x0002
#define DEBUGFLAG_02       0x0004
#define DEBUGFLAG_03       0x0008
#define DEBUGFLAG_04       0x0010
#define DEBUGFLAG_05       0x0020
#define DEBUGFLAG_06       0x0040
#define DEBUGFLAG_07       0x0080
#define DEBUGFLAG_08       0x0100
#define DEBUGFLAG_09       0x0200
#define DEBUGFLAG_10       0x0400
#define DEBUGFLAG_11       0x0800
#define DEBUGFLAG_XTEST    0x1000   
#define DEBUGFLAG_YTEST    0x2000   
#define DEBUGFLAG_ZTEST    0x4000
#define DEBUGFLAG_NOUPDATE 0x8000   
#define DEBUGFLAG_16       0x10000
#define DEBUGFLAG_17       0x20000
#define DEBUGFLAG_18       0x40000
#define DEBUGFLAG_19       0x80000
#define DEBUGFLAG_20       0x100000
#define DEBUGFLAG_21       0x200000
#define DEBUGFLAG_22       0x400000
#define DEBUGFLAG_23       0x800000
#define DEBUGFLAG_24       0x1000000
#define DEBUGFLAG_25       0x2000000
#define DEBUGFLAG_26       0x4000000
#define DEBUGFLAG_27       0x8000000
#define DEBUGFLAG_28       0x10000000
#define DEBUGFLAG_29       0x20000000
#define DEBUGFLAG_30       0x40000000
#define DEBUGFLAG_31       0x80000000

#define FLAG_XTEST        (g_dwDebugFlags & DEBUGFLAG_XTEST)
#define FLAG_YTEST        (g_dwDebugFlags & DEBUGFLAG_YTEST)
#define FLAG_ZTEST        (g_dwDebugFlags & DEBUGFLAG_ZTEST)
#define FLAG_NOUPDATE     (g_dwDebugFlags.DontUpdate())
#define FLAG_DONTUPDATE   (g_dwDebugFlags.DontUpdate())

/////////////////////////////////////////////////////////////////////
class CDebugFlags 
{
   DWORD m_dwDebugFlags;
   CDebugFlags() : m_dwDebugFlags(0) {
   }
//    ~CDebugFlags(){
//    }
public:
   static CDebugFlags& Instance() {
      static CDebugFlags s_dwDebugFlags;
      return s_dwDebugFlags;
   }
   BOOL DontUpdate() const
   {
       return (m_dwDebugFlags & DEBUGFLAG_NOUPDATE) || !g_Developer;
   }

   DWORD operator & (DWORD dwFlag) const { return m_dwDebugFlags & dwFlag; }
//   void operator &= (DWORD dwFlag) { m_dwDebugFlags &= dwFlag; }
   void operator ^= (DWORD dwFlag) { m_dwDebugFlags ^= dwFlag; }
   void operator |= (DWORD dwFlag) { m_dwDebugFlags |= dwFlag; }

   void  Clear() { m_dwDebugFlags = 0; }
   DWORD Get(DWORD dwFlag)  const { return m_dwDebugFlags & dwFlag; }
   void  Set(DWORD dwFlag)   { m_dwDebugFlags |= dwFlag; }
   void  Reset(DWORD dwFlag) { m_dwDebugFlags &= ~dwFlag; }

   void Load() { m_dwDebugFlags = (DWORD)AfxGetApp()->GetProfileInt("", "DebugFlags", 0); }
   void Save() const { AfxGetApp()->WriteProfileInt("", "DebugFlags", m_dwDebugFlags); }
};

#define g_dwDebugFlags (CDebugFlags::Instance())

//extern DWORD g_dwDebugFlags;



/////////////////////////////////////////////////////////////////////

