#pragma once


// version 3 ! ( shl 6)

__inline DWORD CodeHex(const char *s)
{
   LARGE_INTEGER t;
   t.HighPart = 0;
   t.LowPart  = 0x1D9D0C3;
   char *mem = new char[strlen(s)+1]; 

   strcpy(mem,s);
   char *c = mem;
   CharUpper(c);
   while ( *c ) {
      //      unsigned char b = *c;
      //t.QuadPart <<= 1;
      t.QuadPart = (t.QuadPart << 6) + t.QuadPart;
      unsigned char d = (unsigned char)*c;
      t.QuadPart += (UINT)d*d;
      if ( t.HighPart ) {
         t.LowPart += t.HighPart;
         t.HighPart = 0;
      }
      c++;
   }
   delete [] mem;
   return t.LowPart;
}

// version 2 ! (quadrate)

__inline DWORD CodeHex2(const char *s)
{
   LARGE_INTEGER t;
   t.HighPart = 0;
   t.LowPart  = 0x1D9D0C3;
   char *mem = new char[strlen(s)+1]; 
   strcpy(mem,s);
   char *c = mem;
   CharUpper(c);
   while ( *c ) {
//      unsigned char b = *c;
      t.QuadPart <<= 1;
      unsigned char d = (unsigned char)*c;
      t.QuadPart += d*d;
      if ( t.HighPart ) {
         t.LowPart += t.HighPart;
         t.HighPart = 0;
      }
      c++;
   }
   delete [] mem;
   return t.LowPart;
}
