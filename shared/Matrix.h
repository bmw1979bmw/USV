// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__DAEF748B_7E5C_11D8_BB62_0050DAC48A11__INCLUDED_)
#define AFX_MATRIX_H__DAEF748B_7E5C_11D8_BB62_0050DAC48A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////
////////////////////////////////////////////////////
///////////  ROW , COL  !!!/////////////////////////
// теперь работает обращение матриц
////////////////////////////////////////////////////
////////////////////////////////////////////////////


class CMatrix
{
   
   int m_nCol;
   int m_nRow;
   double *a;
//    LONG   *pRefs; // число обращений

public:
   CMatrix(){ a = NULL; }//{ pRefs = NULL; }
	CMatrix(int nrow, int ncol);
   CMatrix(const CMatrix &M);
   void Release();
	~CMatrix(void);
   CMatrix& operator= ( const CMatrix& A );
   int GetCol(void) const { return m_nCol; }
   int GetRow(void) const { return m_nRow; }
   double& elem(int row, int col) { return (*this)(row,col);/*a[row*m_nCol+col];*/ };
   double& operator ()(int row, int col) { 
      ASSERT(row < m_nRow && col < m_nCol);
      return a[row*m_nCol+col]; 
   }
   double* operator= ( double *t );
   CMatrix operator* ( CMatrix& );

   //old
//   class Matrix
public:

   double  operator= ( double t );


   CMatrix operator+ ( CMatrix& );
   CMatrix operator- ( CMatrix& );
   CMatrix operator* ( double );
   CMatrix operator- ( void );
   CMatrix operator~ ( void );  //T
   CMatrix T(){ return ~ *this; }
   CMatrix NOT(){ return - *this;}
   CMatrix operator! ( void );// { return - *this; }

   void   print( void );
   double Det();

private:
   void RAS_set ( int N, int Nf, int K, int *V );
   int  RAS_sign( int K );
   int  RAS_fact( int K );

};


#endif // !defined(AFX_MATRIX_H__DAEF748B_7E5C_11D8_BB62_0050DAC48A11__INCLUDED_)
