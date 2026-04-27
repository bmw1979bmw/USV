#include "stdafx.h"
#include "strcpy_n.h"

// копирует сколько влезет символов
size_t strcpy_n(char *dst, size_t dstBufLen, const char *src)
{
   if ( !dst ) return 0;
   if ( !dstBufLen ) return 0;
   size_t nLen = src ? strlen(src) : 0;
   dstBufLen--;
   if ( nLen > dstBufLen ) nLen = dstBufLen;
   if ( nLen ) strncpy(dst, src, nLen);
   dst[nLen] = 0;
   return nLen;
}

// скопировать, если влезет. Иначе false 
BOOL can_strcpy_n(char *dst, size_t dstBufLen, const char *src)
{
   if ( !dst ) return FALSE;
   if ( !dstBufLen ) return FALSE;
   size_t nLen = src ? strlen(src) : 0;
   dstBufLen--;
   if ( nLen > dstBufLen ) return FALSE;
   if ( nLen ) strncpy(dst, src, nLen);
   dst[nLen] = 0;
   return BOOL(nLen);
}
