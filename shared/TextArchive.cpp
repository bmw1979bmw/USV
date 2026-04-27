// TextArchive.cpp: implementation of the CTextArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BYTE CTextArchive::m_cComment   = ';';
BYTE CTextArchive::m_cDelimiter = ' ';

CTextArchive::CTextArchive(CFile* pFile, UINT nMode):
   CArchive(pFile, nMode)
{
}

CTextArchive::~CTextArchive()
{

}

CTextArchive& CTextArchive::operator <<( const char *str )
{
   Write(str, lstrlen(str));
   return *this;
}

void __cdecl CTextArchive::Format(LPCTSTR lpszfmt, ... ) 
{
   CString s;
   va_list args;
   va_start(args, lpszfmt);
   s.FormatV(lpszfmt, args);
   va_end(args);
   *this << (LPCSTR)s;    // BEWARE UNICODE !!!
}

CTextArchive& CTextArchive::WriteName( LPCTSTR szName, BOOL bCRLF )
{
   Write("\"", 1);
   Write(szName, lstrlen(szName));
   Write("\"", 1);
   if ( bCRLF ) Write("\r\n", 2);
   else   Delimiter();
   return *this;
}

CTextArchive& CTextArchive::ReadName( CString &s )
{
   s.Empty();
   s.GetBuffer(256); 
   int k=0;
   TRY {
      BYTE b;
      do {
         *(CArchive *)this >> b;
         if ( m_cComment && b == m_cComment ) UntilChar('\n');
      } while( b != '\"' );
      do {
         *(CArchive *)this >> b;
         if ( b < 32 || b == '\"' ) break;
         k++;
         s += b; 
      } while( 1 /*b != 0x0A*/ );
   } CATCH ( CArchiveException, e ) {
      if ( e->m_cause != CArchiveException::endOfFile || k == 0 ) 
         AfxThrowArchiveException(e->m_cause,
         GetFile()->GetFileName());
   } END_CATCH;
   return *this;
}

CTextArchive& CTextArchive::operator <<( CFile &f )
{
   char buf[128];
   UINT nBytes;
   while( (nBytes = f.Read(buf, 128)) != 0 ){
      Write(buf, nBytes);
   }
   return *this;
}

CTextArchive& CTextArchive::operator <<( BYTE by )
{
   return CTextArchive::operator <<((DWORD)by);
}

CTextArchive& CTextArchive::operator <<( unsigned int w )
{
   return CTextArchive::operator <<((DWORD)w);
}

CTextArchive& CTextArchive::operator <<( int i )
{
   return CTextArchive::operator <<((LONG)i);
}

CTextArchive& CTextArchive::operator <<( LONG l )
{
   char buf[16];
   sprintf(buf, "%ld", l);
   *this << buf;
   Delimiter();
   return *this;
}

CTextArchive& CTextArchive::operator <<( DWORD dw )
{
   char buf[16];
   sprintf(buf, "%lu", dw);
   *this << buf;
   Delimiter();
   return *this;
}

CTextArchive& CTextArchive::WriteDw( DWORD dw )
{
   char buf[16];
   sprintf(buf, "0x%08X", dw);
   *this << buf;
   Delimiter();
   return *this;
}

CTextArchive& CTextArchive::operator <<( float f )
{
   return CTextArchive::operator <<((double)f);
}

CTextArchive& CTextArchive::operator <<( double d )
{
   static char buf[64];
   sprintf(buf, "%g", d);
   *this << buf;
   Delimiter();
   return *this;
}

void CTextArchive::Delimiter()
{
   if ( m_cDelimiter ) {
      CArchive::operator<<(m_cDelimiter);
   }
}

CTextArchive& CTextArchive::operator >>( BYTE& by )
{
   by = (BYTE)ReadULong();
   return *this;
}

CTextArchive& CTextArchive::operator >>(  unsigned int & w )
{
   w = ReadULong();
   return *this;
}

CTextArchive& CTextArchive::operator >>( int& i )
{
   i = ReadLong();
   return *this;
}

CTextArchive& CTextArchive::operator >>( LONG& l )
{
   l = ReadLong();
   return *this;
}

CTextArchive& CTextArchive::operator >>( DWORD& dw )
{
   dw = ReadULong();
   return *this;
}

CTextArchive& CTextArchive::operator >>( float& f )
{
   f = (float)ReadDouble();
   return *this;
}

CTextArchive& CTextArchive::operator >>( double& d )
{
   d = ReadDouble();
   return *this;
}

double CTextArchive::ReadDouble()
{
   char buf[128];
   double d=0;
   char *endptr;
   while( ReadNext(buf, 128, 1) ) {;
      d = strtod(buf, &endptr);
      if ( buf != endptr ) break;
   }
   return d;
}
LONG CTextArchive::ReadLong()
{
   char buf[128];
   LONG l=0;
   char *endptr;
   while( ReadNext(buf, 128, 3) ) {;
      l = strtol(buf, &endptr, 0);
      if ( buf != endptr ) break;
   }
   return l;
}

DWORD CTextArchive::ReadULong()
{
   char buf[128];
   DWORD d=0;
   char *endptr;
   while( ReadNext(buf, 128, 3) ) {;
      d = strtoul(buf, &endptr, 0);
      if ( buf != endptr ) break;
   }
   return d;
}

int CTextArchive::IsNumber(BYTE B)
{
   if ( B >= '0' && B <= '9' ) return 3;
   if ( B == '-' || B == '+' ) return 3;
   if ( B == '.' ) return 2;
   if ( B == 'e' || B == 'E' ) return 1;     // e не может быть первой!
   return 0;
}

int CTextArchive::ReadNext(char *buf, int nbuflen, int flags /* =3 */)
{
   nbuflen--;
   int delims = flags;
   int k=0;
   BYTE b;
   if ( delims == 1 ) delims++; // e не может быть первой!
   TRY {
   do {
      k = 0;
      do {
         *(CArchive *)this >> b;
         if ( m_cComment && b == m_cComment ) UntilChar('\n');
      } while( IsNumber(b) < delims );  // skip delimiters
      
      while( IsNumber(b) >= flags && k < nbuflen ) {
         buf[k++] = b;
         *(CArchive *)this >> b;
         if ( m_cComment && b == m_cComment ) UntilChar('\n');
      }
   } while( k == 1 && (*buf == 'e' || *buf == '.' ) && k < nbuflen );
      
   } CATCH ( CArchiveException, e ) {
      if ( e->m_cause != CArchiveException::endOfFile || k == 0 ) 
         AfxThrowArchiveException(e->m_cause,
            GetFile()->GetFileName());
   } END_CATCH;
   buf[k]=0;
   return k;
}

CTextArchive& CTextArchive::operator >>( CString &s )
{
   s.Empty();
   s.GetBuffer(256); 
   int k=0;
   TRY {
      BYTE b;
      do {
         *(CArchive *)this >> b;
         k++;
         if ( b >= 32 || b == '\t' ) {
            s += b; 
         }
      } while( b != 0x0A );  // skip delimiters
   } CATCH ( CArchiveException, e ) {
      if ( e->m_cause != CArchiveException::endOfFile || k == 0 ) 
         AfxThrowArchiveException(e->m_cause,
         GetFile()->GetFileName());
   } END_CATCH;
   return *this;
}

void CTextArchive::UntilChar( char c )
{ // взять все символы и указанный
   BYTE b;
   do {
      *(CArchive *)this >> b;
   } while( b != c );
}

void CTextArchive::UntilLF()
{ // взять все символы до конца строки LF
   UntilChar( 0x0A );
}

void CTextArchive::ReadWord( char *str, char *delims )
{                  // пожалуйста эксепшены сами
   BYTE b;
   do {
      *(CArchive *)this >> b;
   } while( strchr(delims, b) != NULL );  // skip delimiters

   while( strchr(delims, b) == NULL ) {
      *str++ = b;
      *(CArchive *)this >> b;
   }
   *str = 0;
}

void CTextArchive::ReadChars( char *str, int nchars ) // читать символы в строку
{
   for ( int i=0; i<nchars ;i++ ) {
      *(CArchive *)this >> *str++;
   }
   *str = 0;
}

void CTextArchive::SkipInts(int nInts)
{
   for ( int i=0; i<nInts ;i++ ) {
      ReadLong();
   }
}

void CTextArchive::SkipFloats(int nFloats)
{
   for ( int i=0; i<nFloats ;i++ ) {
      ReadDouble();
   }
}
