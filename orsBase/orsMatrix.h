/******************************************************************************
 * $Id: orsMatrix.h, v 1.0 date: 06/12/2008 江万寿 Exp$
 *
 * Project:
 * Purpose:  declarations of orsMatrix
 * Author:   江万寿, wsjws@163.com
 *
 ******************************************************************************
 * Copyright (c) 2008, 江万寿
 *
 *		未得到授权请勿拷贝
 *
 ******************************************************************************
 *
 * $Log: orsMatrix.h,v $
 *
 * Revision 1.0  06/12/2008  江万寿
 * New
 *
 */

#if !defined(AFX_MATRIX_H__E39CB199_C15A_469D_9AA1_8FC1476A9475__INCLUDED_)
#define AFX_MATRIX_H__E39CB199_C15A_469D_9AA1_8FC1476A9475__INCLUDED_

#include <assert.h>
#include <string.h>

#include "orsBase/orsTypedef.h"


class orsMatrixBase
{
protected:
	orsDataTYPE	m_dataType;
	int	m_nRows;
	int	m_nCols;

	void  *m_pBuf;
	void  **m_rows;
protected:
	// 表明orsMatrixBase 必须由派生类实例化
	orsMatrixBase(): m_dataType(ORS_DT_UnKNOWN),
		m_nRows(0), m_nCols(0), m_pBuf(NULL), m_rows(NULL) {};

	// 含义是矩阵服务只针对浮点类型。但该类型不影响矩阵模板的转置，赋值和迹的计算
	void SetDataType( int bytes )	{
		switch (bytes)
		{
		case 1:
			m_dataType = ORS_DT_BYTE;
			break;
		case 2:
			m_dataType = ORS_DT_INT16;
			break;
		case 4:
			m_dataType = ORS_DT_FLOAT32;
			break;
		case 8:
			m_dataType = ORS_DT_FLOAT64;
			break;
		default:
			break;
		}
	};

public:
	void SetDataType( orsDataTYPE dataType )	{
		m_dataType = dataType;
	}

	orsDataTYPE	GetDataType() const 	{return m_dataType;};

	void	*Buf()	{return m_pBuf;};
	const void	*Buf() const	{return m_pBuf;};

	int		Rows()	 const	{return m_nRows;};
	int		Cols()	 const	{return m_nCols;};
};


//////////////////////////////////////////////////////////////////////////
// orsMatrix矩阵模板用于进行基本的矩阵运算
//
//	数据结构：连续矩阵数据 + 每行的头指针
//
//////////////////////////////////////////////////////////////////////////
template <typename _T>
class orsMatrix : public orsMatrixBase
{
public:

	orsMatrix()
	{
		SetDataType( sizeof(_T) );
	};

	orsMatrix( int nRows, int nCols )
	{
		SetDataType( sizeof(_T) );
		Alloc( nRows, nCols );
	}

	//copy constructor
	orsMatrix( const orsMatrix<_T> &mB )	{
		SetDataType(sizeof(_T) );
		Alloc( mB.m_nRows, mB.m_nCols  );
		
		memcpy( m_pBuf, mB.m_pBuf, m_nRows*m_nCols*sizeof(_T) );
	}

	~orsMatrix()	{	DeAlloc();	}

	void DeAlloc()
	{
		if( m_rows )
			delete (_T**)m_rows;
		m_rows = NULL;
		if( m_pBuf )
			delete (_T*)m_pBuf;
		m_pBuf = NULL;
	}

	bool Alloc(int nRows, int nCols )
	{
		if( nRows == m_nRows && nCols == m_nCols )
			return true;

		DeAlloc();

		m_pBuf = new _T[nRows*nCols];
		if( NULL == m_pBuf )
			return false;

		m_rows = (void **)new _T*[nRows];

		if( NULL == m_rows )
			return false;

		_T* pBuf = (_T *)m_pBuf;
		for( int iRow = 0; iRow< nRows; ++iRow )
		{
			m_rows[iRow] = pBuf;
			pBuf += nCols;
		}

		m_nRows = nRows;
		m_nCols = nCols;

		return true;
	}


	void Zero()
	{
		//	assert( NULL != m_pBuf );
		if( NULL != m_pBuf  )	{
			memset( m_pBuf, 0, m_nRows*m_nCols*sizeof(_T) );
		}
	}

	void Ones()//填充1
	{
		if( NULL != m_pBuf )
		{
			_T *tempBuf= (_T *)m_pBuf;
			for (int i=0;i<m_nCols*m_nRows;i++)
				tempBuf[i]=_T(1.0);		
		}
	}

	_T *operator [] (int i)
	{
		return (_T *)m_rows[i];
	}

	const _T *operator [] (int i) const
	{
		return (_T *)m_rows[i];
    }


	// assignment operator (constructor)
	orsMatrix& operator= (const orsMatrix &mB)
	{
		// trap assignment to self
		if( this == &mB )
			return *this;

		Alloc( mB.m_nRows, mB.m_nCols );

		memcpy( m_pBuf, mB.m_pBuf, m_nRows*m_nCols*sizeof(_T) );

		return *this;
	}
// 	// add by yanyuan
// 	orsMatrix Abs() const
// 	{
// 		orsMatrix mB(m_nRows, m_nCols);
// 
// 		for (int i = 0; i < m_nRows; i++)
// 		{
// 			for (int j = 0; j < m_nCols; j++)
// 			{
// 				_T val = ((_T **)m_rows)[i][j];
// 				mB[i][j] = val >= 0 ? val : -(val);
// 			}
// 		}
// 
// 		return mB;
// 
// 	}

	double Trace() const
	{
		assert( m_nCols == m_nRows );

		double trace = 0.0;
		for( int i = 0; i < m_nRows; i++ )
			trace += ((_T **)m_rows)[i][i];

		return trace;
	}

	void Transpose( orsMatrix &mB )
	{
		assert( mB.m_nRows == m_nCols && mB.Cols() == m_nRows );

		for( int i=0 ; i<m_nRows; i++)
		{
			for ( int j=0; j<m_nCols; j++)
				mB[j][i] = ((_T **)m_rows)[i][j];
		}
	}

	orsMatrix transpose() const
	{
		orsMatrix mB( m_nCols, m_nRows );

		for( int i=0 ; i<m_nRows; i++)
		{
			for ( int j=0; j<m_nCols; j++)
				mB[j][i] = ((_T **)m_rows)[i][j];
		}

		return mB;
	}

	//added by yaohuang
	//求代数余子式
	orsMatrix AlgbComplement(int row, int col)
	{
		if (row >= m_nRows ||col >= m_nCols)
			return *this;

		orsMatrix<_T> leftMatrix;
		leftMatrix.Alloc( m_nRows-1, m_nCols-1 );

		bool aft_row = false;
		bool aft_col = false;

		for (int i = 0; i < leftMatrix.m_nRows; i++)
		{
			aft_col = false;
			for (int j = 0; j < leftMatrix.m_nCols; j++)
			{
				if(i == row)
					aft_row = true;
				if(j == col)
					aft_col = true;

				if((!aft_row) && (!aft_col))
					leftMatrix[i][j] = (*this)[i][j];
				else if(aft_row && (!aft_col))
					leftMatrix[i][j] = (*this)[i+1][j];
				else if ((!aft_row) && aft_col)
					leftMatrix[i][j] = (*this)[i][j+1];
				else
					leftMatrix[i][j] = (*this)[i+1][j+1];
			}
		}
		return leftMatrix;
	}

	//首行分解法递归求行列式
	_T det()
	{
		if(!isSquare())
			return _T(1.111111);//返回无效数字?
		if(m_nRows == 0)
			return _T(0);
		else if(m_nRows == 1)
			return (*this)[0][0];
		else if(m_nRows == 2)
			return ((*this)[0][0]*(*this)[1][1] - (*this)[0][1]*(*this)[1][0]);
		else
		{
			_T sum = 0;
			int coeffient = 1;
			for (int i = 0; i < m_nCols; i++)
			{
				orsMatrix leftMatrix = (*this).AlgbComplement(0, i);
				sum = sum + coeffient*(*this)[0][i]*leftMatrix.det();
				coeffient = -coeffient;
			}
			return sum;
		}
	}

	//是否为方阵
	bool isSquare()
	{
		return (m_nCols==m_nRows);
	}

	// 元素乘，与Matlab点乘相同
	orsMatrix<_T> elementMultiple(const orsMatrix<_T> &mB)
	{
		const orsMatrix<_T> &mA = *this;

		orsMatrix<_T> mC( mA.Rows(), mA.Cols() );

		assert( mA.Cols() == mB.Rows() && mA.Rows() == mC.Rows() && mB.Cols() == mC.Cols() );
		
		for (int row = 0; row < mC.Rows(); row++)
		{
			for (int col = 0; col < mC.Cols(); col++)
			{
				mC[row][col] = mA[row][col] * mB[row][col];
			}
		}
		
		return mC;
	};
	
	
	// 元素除，与Matlab点除相同
	orsMatrix<_T> elementDivide(const orsMatrix<_T> &mB)
	{
		const orsMatrix<_T> &mA = *this;
		
		orsMatrix<_T> mC( mA.Rows(), mA.Cols() );

		assert( mA.Cols() == mB.Rows() && mA.Rows() == mC.Rows() && mB.Cols() == mC.Cols() );
		
		for (int row = 0; row < mC.Rows(); row++)
		{
			for (int col = 0; col < mC.Cols(); col++)
			{
				if( mB[row][col] == 0 ) 
					mC[row][col] = mA[row][col]/0.0000001;
				else 
					mC[row][col] = mA[row][col]/mB[row][col];
			}
		}
		
		return mC;
	};
};

template <typename _T>
orsMatrix<_T> operator + (const orsMatrix<_T> &mA, const orsMatrix<_T> &mB)
{
	assert( mA.Rows() == mB.Rows() || mA.Cols() == mB.Cols() );

	orsMatrix<_T> mC;
	
	mC.Alloc( mA.Rows(), mA.Cols() );

	for (int row = 0; row < mC.Rows(); row++)
	{
		for (int col = 0; col < mC.Cols(); col++)
		{
			mC[row][col] = mA[row][col] + mB[row][col];
		}
	}
	return mC;
};


template <typename _T>
orsMatrix<_T> operator - (const orsMatrix<_T> &mA, const orsMatrix<_T> &mB)
{
	assert( mA.Rows() == mB.Rows() || mA.Cols() == mB.Cols() );

	orsMatrix<_T> mC;
	
	mC.Alloc( mA.Rows(), mA.Cols() );
	
	for (int row = 0; row < mC.Rows(); row++)
	{
		for (int col = 0; col < mC.Cols(); col++)
		{
			mC[row][col] = mA[row][col] - mB[row][col];
		}
	}

	return mC;
};



template <typename _T>
orsMatrix<_T> operator *(const orsMatrix<_T> &mA, const orsMatrix<_T> &mB)
{
	assert( mA.Cols() == mB.Rows() );
	
	orsMatrix<_T> mC;
	
	mC.Alloc( mA.Rows(), mB.Cols() );
	
	for (int row = 0; row < mC.Rows(); row++)
	{
		for (int col = 0; col < mC.Cols(); col++)
		{
			mC[row][col] = 0;
			for( int k=0; k < mA.Cols(); k++ )
				mC[row][col] += mA[row][k]* mB[k][col];
		}
	}
	
	return mC;
};




typedef orsMatrix<double> orsMatrixD;

typedef orsMatrix<float> orsMatrixF;

//////////////////////////////////////////////////////////////////////////

class orsVectorBase
{
protected:
	int	m_nRows;

	orsDataTYPE	m_dataType;

	void *m_pBuf;

protected:
	orsVectorBase(): m_nRows(0), m_dataType(ORS_DT_UnKNOWN), m_pBuf(NULL)	{};
	virtual ~orsVectorBase()	{	if( m_pBuf )	delete (char*)m_pBuf;	};

	void SetDataType( int bytes )	{
		switch (bytes)
		{
		case 1:
			m_dataType = ORS_DT_BYTE;
			break;
		case 2:
			m_dataType = ORS_DT_INT16;
			break;
		case 4:
			m_dataType = ORS_DT_FLOAT32;
			break;
		case 8:
			m_dataType = ORS_DT_FLOAT64;
			break;
		default:
			break;
		}
	};

public:
	void SetDataType( orsDataTYPE dataType )	{
		m_dataType = dataType;
	}

	orsDataTYPE GetDataType()	 const	{return m_dataType;};

	int		Rows()	 const	{return m_nRows;};

	void	*Buf()	 {return m_pBuf;};
	const void	*Buf()	 const		{return m_pBuf;};

};


template <typename _T>
class orsVector : public orsVectorBase
{
	
public:
	orsVector() {
		SetDataType(sizeof(_T) );
	};

	orsVector( int nRows )	{
		SetDataType(sizeof(_T) );
		Alloc( nRows );
	}

	orsVector( _T a, _T b, _T c )	{
		SetDataType(sizeof(_T) );
		Alloc( 3 );
		
		_T *buf = (_T *)Buf();
		
		buf[0] = a;	buf[1] = b;	buf[2] = c;
	}

	orsVector( _T a, _T b, _T c, _T d )	{
		SetDataType(sizeof(_T) );
		Alloc( 4 );
		
		_T *buf = (_T *)Buf();
		
		buf[0] = a;	buf[1] = b;	buf[2] = c;	buf[3] = d;
	}

	//copy constructor
	orsVector( const orsVector<_T> &vec )	{
		SetDataType(sizeof(_T) );
		Alloc( vec.m_nRows  );

		memcpy( m_pBuf, vec.m_pBuf, m_nRows*sizeof(_T) );
	}

	~orsVector()	{	DeAlloc();	}

	void DeAlloc()
	{
		if( m_pBuf )
			delete (_T*)m_pBuf;
		m_pBuf = NULL;
	}
	bool Alloc(int nRows )
	{
		if( nRows == m_nRows )
			return true;

		DeAlloc();

		m_pBuf = new _T[nRows];
		if( NULL == m_pBuf )
			return false;
		m_nRows = nRows;

		return true;
	}


	void Zero()
	{
		if( NULL != m_pBuf )
			memset( m_pBuf, 0, m_nRows*sizeof(_T));
	}

	void Ones()//填充1
	{
		if (NULL!=m_pBuf)
		{
			_T *tempBuf= (_T *)m_pBuf;
			for (int i=0;i<m_nRows;i++)
				tempBuf[i]=_T(1.0);	
		}
	}

	_T & operator [] (int i)
	{
		assert(i < m_nRows);
		return ((_T *)m_pBuf)[i];
	}

	const _T& operator [] (int i) const
	{
		assert(i < m_nRows);
		return ((_T *)m_pBuf)[i];
    }

	// assignment operator (constructor)
	orsVector<_T>& operator= (const orsVector<_T> &vec)
	{
		// trap assignment to self
		if( this == &vec )
			return *this;

		Alloc( vec.m_nRows  );

		memcpy( m_pBuf, vec.m_pBuf, m_nRows*sizeof(_T) );

		return *this;
	}

	// 归一化
	void Unit()
	{
		double s = 0;
		_T *t = (_T *)m_pBuf;

		int i;
		for ( i =0; i< m_nRows; i++)
		{
			s += *t * *t;	t++;
		}
		
		if (s > 0) {
			s = sqrt(s);

			t = (_T*)m_pBuf;
			for (i = 0; i < m_nRows; i++)
			{
				*t /= s;	t++;
			}
		}
	}

	orsVector unit()
	{
		orsVector vA = *this;

		vA.Unit();

		return vA;
	}

	// 点积
	double dotProduct( const orsVector<_T> &vB )
	{
		assert( m_nRows == vB.Rows() );
		assert( m_dataType == vB.m_dataType );
		
		const _T *a=(const _T *)m_pBuf;
		const _T *b=(const _T *)(vB.m_pBuf);
		
		double c = 0;		
		for (int row = 0; row < m_nRows; row++)
		{
			c += *a * *b;

			a++;	b++;
		}
		
		return c;
	}

	// 叉积，仅限于三维向量
	orsVector<_T> cross( const orsVector<_T> &vec ) const
	{
		assert( 3 == m_nRows && 3 == vec.Rows() );
		assert( m_dataType == vec.m_dataType );

		const _T *a= (const _T *)m_pBuf;
		const _T *b= (const _T *)(vec.m_pBuf);

		orsVector<_T> c(3); 

		c[0] =   a[1]*b[2] - b[1]*a[2];
		c[1] = -(a[0]*b[2] - b[0]*a[2] );
		c[2] =   a[0]*b[1] - b[0]*a[1];
		
		return c;
	}

	void CopyData( _T *buf )
	{
		memcpy( buf, m_pBuf, m_nRows*sizeof(_T) );
	}

	_T	*Buf()	{return (_T *)m_pBuf;};


	// 元素乘，与Matlab点乘相同
	orsVector<_T> elementMultiple( const orsVector<_T> &vB)
	{
		assert( Rows() != vB.Rows() );
		
		orsVector<_T> &vA = *this;
		orsVector<_T> vC( Rows() ); 
		
		for (int row = 0; row < vC.Rows(); row++)
			vC[row] = vA[row]*vB[row];
		
		return vC;
	};
	
	// 元素除，与Matlab点除相同
	orsVector<_T> elementDivide( const orsVector<_T> &vB)
	{
		assert( Rows() != vB.Rows() );
		
		orsVector<_T> &vA = *this;
		orsVector<_T> vC( Rows() ); 
		
		for (int row = 0; row < vC.Rows(); row++)
			vC[row] = vA[row]/vB[row];
		
	};

};


template <typename _T>
orsVector<_T> operator+ (const orsVector<_T> &vB1, const orsVector<_T> &vB2)
{
	orsVector<_T> vRes(vB1);
	if (vB1.Rows() != vB2.Rows())
		return vRes;

	for (int row = 0; row < vRes.Rows(); row++)
	   vRes[row] = vRes[row] + vB2[row];

	return vRes;
};


template <typename _T>
orsVector<_T> operator- (const orsVector<_T> &vB1, const orsVector<_T> &vB2)
{
	orsVector<_T> vRes(vB1);
	if (vB1.Rows() != vB2.Rows())
		return vRes;

	for (int row = 0; row < vRes.Rows(); row++)
		vRes[row] = vRes[row] - vB2[row];

	return vRes;
};



template <typename _T>
orsVector<_T> operator*(const orsMatrix<_T> &mA, const orsVector<_T> &vB)
{
	assert( mA.Cols() == vB.Rows() );

	orsVector<_T> vC( mA.Rows() );
	
	for (int row = 0; row < mA.Rows(); row++)
	{
		vC[row] = 0;
		for (int col = 0; col < mA.Cols(); col++)
		{
			vC[row] += mA[row][col] * vB[col];
		}
	}

	return vC;
};



typedef orsVector<double> orsVectorD;
typedef orsVector<float> orsVectorF;

#endif // !defined(AFX_MATRIX_H__E39CB199_C15A_469D_9AA1_8FC1476A9475__INCLUDED_)
