// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "MatrTest.h"
#include "Matrix.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//////////////////////////////////////////////////////////////////////

CMatrix::CMatrix(int nrow, int ncol)
{
   m_nCol=ncol;
   m_nRow=nrow;
   a = new double[ ncol*nrow ];
//    pRefs = new LONG;
//    *pRefs = 1;
}

CMatrix::CMatrix(const CMatrix &M)
{  
//   Release();
   m_nCol = M.m_nCol;
   m_nRow = M.m_nRow;
   //a = M.a;
   a = new double[ m_nCol*m_nRow ];
   memcpy(a,M.a,m_nCol*m_nRow*sizeof(double));
//    pRefs = M.pRefs;
//    InterlockedIncrement(pRefs);
}
void CMatrix::Release()
{
//    if ( pRefs ) {
//       if(InterlockedDecrement(pRefs) <= 0 ) { 
         delete [] a; 
         a = NULL;
//          delete pRefs;
//          pRefs = NULL;
//       }
//    }
}
CMatrix::~CMatrix()
{
   Release();      
}

CMatrix& CMatrix::operator= ( const CMatrix &A )
{
   ASSERT(this != &A);
   Release();//ASSERT ( ( m_nCol == A.GetCol()) && ( m_nRow == A.GetRow()) );
   m_nCol = A.m_nCol;
   m_nRow = A.m_nRow;
   a = new double[A.m_nCol*A.m_nRow];
//    pRefs = new LONG;
//    *pRefs = 1;
   memcpy(a,A.a,sizeof(double)*m_nCol*m_nRow);
   return *this;
}

double* CMatrix::operator= ( double *t )
{                   // копия исходных данных!!!
   memcpy(a,t,sizeof(double)*m_nCol*m_nRow);
   return t;
}

CMatrix CMatrix::operator* ( CMatrix& A )
{
   ASSERT ( m_nCol == A.GetRow() );
   int nrow, ncol;
   nrow = m_nRow;//m_nCol;
   ncol = A.GetCol();//A.GetRow();
   CMatrix C( nrow,ncol );
   for ( int i=0;i<nrow;i++ )
   {
      for ( int j=0;j <ncol;j++ )
      {
         C.elem(i,j) = 0;
         for ( int n=0;n<m_nCol/*nrow*/;n++ ) C.elem(i,j) += elem(i,n) * A.elem(n,j);
      }
   }
   return C;
}

/* ========================================== */
CMatrix CMatrix::operator* ( double t )
{
   int row, col;
   row = GetRow();
   col = GetCol();

   CMatrix C( col, row );
   for ( int i=0;i<row;i++ )
   {
      for ( int j=0;j<col;j++ )
      {
         C.elem(i,j) = elem(i,j)*t;
      }
   }
   return C;
}

/* ========================================== */
CMatrix CMatrix::operator+ ( CMatrix& A )
{
   ASSERT ( ( m_nCol == A.GetCol()) && ( m_nRow == A.GetRow()) );
   CMatrix C(m_nCol, m_nRow);
   for ( int i=0;i<m_nRow;i++ )
      for ( int j=0;j<m_nCol;j++ ) 
         C.elem(i,j) = elem(i,j) + A.elem(i,j);

   return C;
}

/* ========================================== */
CMatrix CMatrix::operator- ( CMatrix& A )
{
   ASSERT ( ( m_nCol == A.GetCol()) && ( m_nRow == A.GetRow()) );
   
      CMatrix C(m_nCol,  m_nRow);
      for ( int i=0;i<m_nRow;i++ )
      {
         for ( int j=0;j<m_nCol;j++ ) C.elem(i,j) = elem(i,j) - A.elem(i,j);
      }
      return C;
   
}

/* ========================================== */
CMatrix CMatrix::operator~ ( )//transparent
{
   CMatrix C(m_nCol,m_nRow);

   for ( int i=0;i<m_nCol;i++ )
      for ( int j=0;j<m_nRow;j++ )
      {
         C.elem(i,j) = elem(j,i);

//          printf("elem(%d,%d) = %5.2f\n", i,j, C.elem(i,j));
      }
//    C.print();
   return C;
}

/* ========================================== */
double CMatrix::operator = ( double t )
{
   for ( int i=0;i<m_nRow;i++ )
   for ( int j=0;j<m_nCol;j++ ) elem(i,j) = t;
   return t;
}


CMatrix CMatrix::operator- ( )
{
   ASSERT(0);
   ASSERT ( m_nRow == m_nCol );
   
   CMatrix C( m_nRow, m_nRow);
   C = 0.;

   int *V = new int[m_nRow];
   int N=m_nRow,Nf=RAS_fact(m_nRow);
   int K,i,j;
   double SUM = 0, MUL,sig;

   for ( K=0; K<Nf ; K++ ) {

      RAS_set ( N, Nf, K, V ); // формируем очередную раскладку индексов

      MUL = sig = RAS_sign(K);
      for ( i=0 ;i<N;i++ ) {
         MUL *= elem(i,V[i]);
      }
      SUM += MUL;

      for ( i=0 ;i<N;i++ ) {
         MUL = sig;
         for ( j=0 ;j<N;j++ ) {
            if ( i !=j ) MUL *= elem(j,V[j]);
         }
         C.elem(i,V[i]) += MUL; // /a[i][V[i]];
      }

   }

   delete [] V;
   if ( SUM ) return C*(1./SUM);
   else
   {
      _ASSERTE(!"Обратной матрицы нет! Вернули [0]");
       TRACE(_T("Обратной матрицы нет! Вернули [0]"));
       CMatrix C(1,1);
       C = 0.;
       return C;
   }
}

//===============================================
//   создает K-ю расстановку индексов из N элементов
//   Параметры:
//   N   -   общее число индексов ( размерность матрицы )   //4
//   Nf = N!                                                //24
//   K   -   Номер расстановки 0...N!
//   V   -   массив под индексы размерностью [N]
//
void CMatrix::RAS_set ( int N, int Nf, int K, int *V ) // Ver 2.
{
   int p = Nf,q,i,n,S;
   for (i=0;i<N;i++) V[i] = -1;
   for ( n=0; n<N ;n++ ) {
      p /= (N-n);
      q = K/p;
      K %= p;
      S = 0;
      for ( i=0;i<N;i++ ) {
         if ( V[i] == -1 ) {
            if ( S==q ) {
               V[i] = n;
               break;
            }
            S++;
         }
      }
   }
}
//===============================================
//  Специфичный знак номера раскладки
int  CMatrix::RAS_sign( int K )
{
   if ( (K&3) && (K&3)!=3 ) return -1;
   return 1;
}
//===============================================
int  CMatrix::RAS_fact( int K )
{
   if ( !K ) return 1;
   for ( int i=K-1; i>0;i-- )K*=i;
   return K;
}

//===============================================
double  CMatrix::Det()
{
   int V[4];
   int N=4,Nf=24;

   int K,i;
   double SUM = 0, MUL;

   for ( K=0; K<Nf ; K++ ) {

      RAS_set ( N, Nf, K, V ); // формируем очередную раскладку индексов

      MUL=RAS_sign(K);
      for ( i=0 ;i<N;i++ ) {
         MUL *= elem(i,V[i]);
      }
      SUM += MUL;
   }
   return SUM;
}
//===============================================

void  CMatrix::print()
{
   printf("CMatrix: %Fp \n",this );
   for ( int i=0;i<m_nRow;i++ )
   {
      for ( int j=0;j<m_nCol;j++ )
      {
         printf("%5.2f,", elem(i,j));
      }
      printf("\n");
   }

} 
//===============================================

/*
*-----------------------------------------------------------------------------
*	funct:	mat_lu
*	desct:	in-place LU decomposition with partial pivoting
*	given:	!! A = square matrix (n x n) !ATTENTION! see commen
*		P = permutation vector (n x 1)
*	retrn:	number of permutation performed
*		-1 means suspected singular matrix
*	comen:	A will be overwritten to be a LU-composite matrix
*
*	note:	the LU decomposed may NOT be equal to the LU of
*		the orignal matrix a. But equal to the LU of the
*		rows interchanged matrix.
*-----------------------------------------------------------------------------
*/

int mat_lu( CMatrix &A, CMatrix &P )
{
	int	i, j, k, n;
	int	maxi, tmp;
	double	c, c1;
	int	p;

	n = A.GetCol();

	for (p=0,i=0; i<n; i++)
		{
		P(i,0) = i;
		}

	for (k=0; k<n; k++)
	{
	/*
	* --- partial pivoting ---
	*/
	for (i=k, maxi=k, c=0.0; i<n; i++)
		{
		c1 = fabs( A(P(i,0),k) );
		if (c1 > c)
			{
			c = c1;
			maxi = i;
			}
		}

	/*
	*	row exchange, update permutation vector
	*/
	if (k != maxi)
		{
		p++;
		tmp = P(k,0);
		P(k,0) = P(maxi,0);
		P(maxi,0) = tmp;
		}

	/*
	*	suspected singular matrix
	*/
	if ( A(P(k,0),k) == 0.0 )
		return (-1);

	for (i=k+1; i<n; i++)
		{
		/*
		* --- calculate m(i,j) ---
		*/
		A(P(i,0),k) = A(P(i,0),k) / A(P(k,0),k);

		/*
		* --- elimination ---
		*/
		for (j=k+1; j<n; j++)
			{
			A(P(i,0),j) -= A(P(i,0),k) * A(P(k,0),j);
			}
		}
	}

	return (p);
}

/*
*-----------------------------------------------------------------------------
*	funct:	mat_backsubs1
*	desct:	back substitution
*	given:	A = square matrix A (LU composite)
*		!! B = column matrix B (attention!, see comen)
*		!! X = place to put the result of X
*		P = Permutation vector (after calling mat_lu)
*		xcol = column of x to put the result
*	retrn:	column matrix X (of AX = B)
*	comen:	B will be overwritten
*-----------------------------------------------------------------------------
*/
CMatrix mat_backsubs1( CMatrix &A, CMatrix &B, CMatrix &X, CMatrix &P, int xcol )
{
	int	i, j, k, n;
	double	sum;

	n = A.GetCol();

	for (k=0; k<n; k++)
		{
		for (i=k+1; i<n; i++)
			B(P(i,0),0) -= A(P(i,0),k) * B(P(k,0),0);
		}

	X(n-1,xcol) = B(P(n-1,0),0) / A(P(n-1,0),n-1);
	for (k=n-2; k>=0; k--)
		{
		sum = 0.0;
		for (j=k+1; j<n; j++)
			{
			sum += A(P(k,0),j) * X(j,xcol);
			}
		X(k,xcol) = (B(P(k,0),0) - sum) / A(P(k,0),k);
		}

	return (X);
}

/*
*-----------------------------------------------------------------------------
*	funct:	mat_inv
*	desct:	find inverse of a matrix
*	given:	a = square matrix a
*	retrn:	square matrix Inverse(A)
*		NULL = fails, singular matrix, or malloc() fails
*-----------------------------------------------------------------------------
*/
CMatrix CMatrix::operator! ( )
{

	int	i,  n;
// 	double	temp;

	n = m_nCol;
	CMatrix A(*this);
	CMatrix B( n, 1 );
	CMatrix C( n, n);
	CMatrix P( n, 1 );

	/*
	*	- LU-decomposition -
	*	also check for singular matrix
	*/
	if (mat_lu(A, P) == -1)
		{
         _ASSERTE(!"Обратной матрицы нет! Вернули [0]");
          TRACE(_T("Обратной матрицы нет! Вернули [0]"));
          CMatrix C(1,1);
          C = 0.;
          return C;
		}

	for (i=0; i<n; i++)
		{
		B = 0.0;
		B(i,0) = 1.0;
		mat_backsubs1( A, B, C, P, i );
		}


	return C;
}




#if(0)
void main()
{
   CMatrix U,B(3,1),A(3,2),AT;
   B(0,0) = 0.04969;
   B(1,0) = 0.0465566;
   B(2,0) = 0.0444;

   A(0,0) = 1;
   A(1,0) = 1 ;
   A(2,0) = 1;
   A(0,1) = 14.15;
   A(1,1) = 14.6;
   A(2,1) = 15.05;

   AT = ~A;
   U = (-(AT*A))*AT*B;

   U.print();

    double t[] = {7.,2.,3.,4.,
                 1.,1.,1.,5.,
                 1.,1.,1.,6.,
                 3.4,55,12,1
                 };
//    double t[] = {6.,3.,0.,3.,
//             4.,4.,2.,1.,
//             0.,4.,4.,2.,
//             7.,7.,8.,5.
//                 };  */
    CMatrix M (4,4);
    M = t;
    M.print();

    CMatrix N(4,4),K(4,4),Q;
    Q = M;
    N = !M;
    N.print();
    K = Q*N;
    K.print();


// 
//    CMatrix N(3,4);
//    printf(">> M:\n");
//            M.print();
// 
// 
//    N = M;
//    printf(">> N:\n" );
//            N.print();
// 
//    printf(">> M[0] %5.2f \n",  M.elem(0,0));
// 
//    N = (~M);
//    printf(">> ~M:\n" );
//            N.print();
// /*
//    N = (!M);
//    printf(">> !M:\n" );
//            N.print();
// 
// */
//    printf(">> N[0] %5.2f \n",  N.elem(0,0));
// 
}
#endif
