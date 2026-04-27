// TextArchive.h: interface for the CTextArchive class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define __TEXTARCHIVE_H__

// текстовый файл как архив ASCII
// UNICODE не поддерживается
// возврат каретки CRLF или LF
// не понимает выражения типа +++++++++++123 или ---------23.56 просто скипает
class CTextArchive : public CArchive
{    // пожалуйста не добавляйте переменные! только static!
     // при преобразовании от CArchive будет мрачный дисторшен
public:
	void SkipFloats(int nFloats);   // пропустить N чисел
	void SkipInts(int nInts);       // пропустить N чисел
   static BYTE m_cComment;
   static BYTE m_cDelimiter;
   void Delimiter(void);

   CTextArchive(CFile* pFile, UINT nMode);
   virtual ~CTextArchive();

   CTextArchive& operator <<( const char *str );
   CTextArchive& operator <<( const CString &s )
      { (*this) << (LPCTSTR) s; return *this; }
   CTextArchive& operator <<( BYTE by );
   CTextArchive& operator <<( unsigned int w );
   CTextArchive& operator <<( int i );
   CTextArchive& operator <<( LONG l );
   CTextArchive& operator <<( DWORD dw );
   CTextArchive& WriteDw( DWORD dw ); // write in hex
   CTextArchive& operator <<( float f );
   CTextArchive& operator <<( double d );

   CTextArchive& operator <<( CFile &f );
   CTextArchive& WriteName(LPCTSTR szName,BOOL bCRLF=FALSE); // в кавычках
   
//    CTextArchive& operator >>( char *str );
   CTextArchive& operator >>( CString &s );
   CTextArchive& operator >>( BYTE& by );
   CTextArchive& operator >>( unsigned int & w );
   CTextArchive& operator >>( int& i );
   CTextArchive& operator >>( LONG& l );
   CTextArchive& operator >>( DWORD& dw );
   CTextArchive& operator >>( float& f );
   CTextArchive& operator >>( double& d );

   CTextArchive& ReadName(CString &s); // в кавычках

   void ReadWord( char *str, char *delims );
   void UntilChar(char c);
   void UntilLF();
   static int IsNumber(BYTE B);
   void ReadChars( char *str, int nchars ); // читать символы в строку

// если не хочешь использовать >> (все >> используют эти):
   double ReadDouble();
   LONG ReadLong();
   DWORD ReadULong();
private:
   int ReadNext(char *buf,int nbuflen,int flags = 3); 
public:
   void __cdecl Format(LPCTSTR lpszfmt, ... ); // write in format BEWARE UNICODE!
};

