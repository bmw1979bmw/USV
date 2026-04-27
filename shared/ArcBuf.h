#pragma once 

#pragma warning(disable: 4355)
class CArcBuf : 
   public CMemFile, 
   public CArchive  
{
public:
   using CArchive::Write;
   using CArchive::Read;
   DWORD GetLen() { CArchive::Flush(); return (DWORD)GetLength(); }
   void * GetBuf() { CArchive::Flush(); return m_lpBuffer; }
   // для последовательного сохранения в один и тот же буфер - очистка буфера 
   CArcBuf &Empty(){ 
      ASSERT(IsStoring());   
      CArchive::Flush(); // CMemFile::SeekToBegin(); 
      SetLength(0); 
      CArchive::Flush(); // именно так!
      return *this;
   }
   // store
   CArcBuf(UINT nGrowBytes = 1024):
      CMemFile(nGrowBytes),
      CArchive((CMemFile *)this,CArchive::store,0x10000) 
   {} 
   // load
   CArcBuf(void* lpBuffer,UINT nBufferSize):
      CMemFile((BYTE*)lpBuffer, nBufferSize),
      CArchive((CMemFile *)this,CArchive::load,0x10000) 
   {} 
            
};
#pragma warning(default:4355)
