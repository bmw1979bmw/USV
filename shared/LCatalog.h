// LCatalog.h: interface for the LCatalog class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma pack(push,Lcatalog)
#pragma pack(1)

class LCatalog  
{
public:
   //LCatalog() { offset = 0xFFFFFFFF; addr = NULL; };
   virtual ~LCatalog(){ /*if ( addr ) */delete addr; };
///////////////
   DWORD offset;
   DWORD nRecLen;
   void *addr; 
};

#pragma pack(pop,Lcatalog)

