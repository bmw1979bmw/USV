#pragma once

// объекты синхронизации

class CWMutex {
   HANDLE hHandle;

public:   
   operator HANDLE(){ return hHandle; }
   CWMutex() : hHandle(0) { }
   ~CWMutex() { if ( hHandle ) VERIFY(CloseHandle(hHandle)); }
   void Close() { if ( hHandle) { VERIFY(CloseHandle(hHandle)); hHandle=0; } }
   BOOL Open( DWORD dwDesiredAccess, LPCTSTR lpName )
   {
      ASSERT(hHandle==0);
      hHandle = OpenMutex(dwDesiredAccess,0,lpName);
      return (BOOL)hHandle;
   }
   BOOL Release(){ return ReleaseMutex(hHandle); }
   BOOL CreateNew ( BOOL bInitialOwner, LPCTSTR lpName )
   {
      ASSERT(hHandle==0);
      hHandle = CreateMutex(0, bInitialOwner, lpName );
      if ( !hHandle ) return FALSE;
      if ( GetLastError() == ERROR_ALREADY_EXISTS ) {
         CloseHandle(hHandle);
         hHandle=0;
         return FALSE;
      }
      return TRUE;
   }
};

class CWSemaphore {
public:
   HANDLE hHandle;
   operator HANDLE(){ return hHandle; }
   CWSemaphore() : hHandle(0) {  }
   ~CWSemaphore() { if ( hHandle ) VERIFY(CloseHandle(hHandle)); }
   void Close() { if ( hHandle) { VERIFY(CloseHandle(hHandle)); hHandle=0; } }
   BOOL Open( DWORD dwDesiredAccess, LPCTSTR lpName )
   {
      ASSERT(hHandle==0);
      hHandle = OpenSemaphore(dwDesiredAccess,0,lpName);
      return (BOOL)hHandle;
   }
   BOOL Release(LONG lCount=1, LONG *pPrev=NULL ) 
      { return ReleaseSemaphore(hHandle, lCount, pPrev); } 

   BOOL CreateNew(LONG nInitialCount,LONG nMaximumCount,LPCTSTR lpName)
   {
      ASSERT(hHandle==0);
      hHandle = CreateSemaphore(0,nInitialCount,nMaximumCount,lpName);
      if ( !hHandle ) return FALSE;
      if ( GetLastError() == ERROR_ALREADY_EXISTS ) {
         CloseHandle(hHandle);
         hHandle=0;
         return FALSE;
      }
      return TRUE;
   }
};

class CWEvent {
public:
   HANDLE hHandle;
   
   operator HANDLE(){ return hHandle; }
   operator const HANDLE*() const { return &hHandle; }

   CWEvent() : hHandle(0) { }
   ~CWEvent() { if ( hHandle ) VERIFY(CloseHandle(hHandle)); }
   void Close() { if ( hHandle) { VERIFY(CloseHandle(hHandle)); hHandle=0; } }
   
   void Attach(HANDLE h){ ASSERT(!hHandle); hHandle = h; }
   void Detach(){ hHandle = 0; }

   BOOL Open( DWORD dwDesiredAccess, LPCTSTR lpName )
   {
      ASSERT(hHandle==0);
      hHandle = OpenEvent(dwDesiredAccess,0,lpName);
      return (BOOL)hHandle;
   }
   BOOL Set()  { return SetEvent(hHandle); }
   BOOL Reset(){ return ResetEvent(hHandle); }
   BOOL Pulse(){ return PulseEvent(hHandle); }
   BOOL CreateNew(BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName)
   {
      ASSERT(hHandle==0);
      hHandle = CreateEvent(0,bManualReset,bInitialState,lpName);
      if ( !hHandle ) return FALSE;
      if ( GetLastError() == ERROR_ALREADY_EXISTS ) {
         CloseHandle(hHandle);
         hHandle=0;
         return FALSE;
      }
      return TRUE;
   }
   
};

