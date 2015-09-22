/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef __Base_Matrix_H__
#define __Base_Matrix_H__

DC_BEGIN_DREEMCHEST

	//! The MxN matrix class.
	template<typename T>
	class Matrix {
	public:

						//! Constructs the Matrix instance.
						Matrix( s32 columns, s32 rows );
						Matrix( const Matrix& other );
						~Matrix( void );

		//! Copies the matrix.
		const Matrix<T>&	operator = ( const Matrix& other );

		//! Returns the reference to an element of a matrix.
		T&					operator () ( s32 column, s32 row );

		//! Returns the reference to an element of a matrix.
		const T&			operator () ( s32 column, s32 row ) const;

		//! Multiplies two matrices.
		Matrix				operator * ( const Matrix& other ) const;

		//! Multiplies a matrix by vector.
		Vector<T>			operator * ( const Vector<T>& v ) const;

		//! Returns the Nth row of a matrix.
		Vector<T>			row( s32 index ) const;

		//! Returns the Nth column of a matrix.
		Vector<T>			col( s32 index ) const;

	private:

		s32					m_rows;	//!< The total number of rows.
		s32					m_cols;	//!< The total number of columns.
		T*					m_data;	//!< The actual matrix data.
	};

	// ** Matrix::Matrix
	template<typename T>
	Matrix<T>::Matrix( s32 columns, s32 rows ) : m_rows( rows ), m_cols( columns )
	{
		m_data = new T[columns * rows];

		for( s32 r = 0; r < rows; r++ ) {
			for( s32 c = 0; c < columns; c++ ) {
				m_data[r * columns + c] = c == r ? 1.0f : 0.0f;
			}
		}
	}

	// ** Matrix::Matrix
	template<typename T>
	Matrix<T>::Matrix( const Matrix& other ) : m_rows( other.m_rows ), m_cols( other.m_cols )
	{
		m_data = new T[m_rows * m_cols];
		memcpy( m_data, other.m_data, m_rows * m_cols * sizeof( T ) );
	}

	// ** Matrix::~Matrix
	template<typename T>
	Matrix<T>::~Matrix( void )
	{
		delete[]m_data;
	}

	// ** Matrix::operator =
	template<typename T>
	const Matrix<T>& Matrix<T>::operator = ( const Matrix& other )
	{
		m_rows = other.m_rows;
		m_cols = other.m_cols;
		m_data = new T[m_rows * m_cols];
		memcpy( m_data, other.m_data, m_rows * m_cols * sizeof( T ) );
		return *this;
	}

	// ** Matrix::operator()
	template<typename T>
	const T& Matrix<T>::operator()( s32 column, s32 row ) const
	{
		DC_BREAK_IF( column < 0 || column >= m_cols );
		DC_BREAK_IF( row    < 0 || row    >= m_rows );

		return m_data[row * m_cols + column];
	}

	// ** Matrix::operator()
	template<typename T>
	T& Matrix<T>::operator()( s32 column, s32 row )
	{
		DC_BREAK_IF( column < 0 || column >= m_cols );
		DC_BREAK_IF( row    < 0 || row    >= m_rows );

		return m_data[row * m_cols + column];
	}

	// ** Matrix::operator *
	template<typename T>
	Matrix<T> Matrix<T>::operator * ( const Matrix& other ) const
	{
		DC_BREAK_IF( m_cols != other.m_rows );
		Matrix C( other.m_cols, m_rows );

		for( s32 i = 0; i < m_rows; i++ ) {
			for( s32 j = 0; j < other.m_cols; j++ ) {
				T sum = 0;

				for( int k = 0; k < m_cols; k++ ) {
					sum = sum + (*this)( k, i ) * other( j, k );
				}

				C( j, i ) = sum;
			}
		}

		return C;
	}

	// ** Matrix::operator *
	template<typename T>
	Vector<T> Matrix<T>::operator * ( const Vector<T>& v ) const
	{
		DC_BREAK_IF( m_cols != v.size() );
		Vector<T> C( v.size() );

		for( s32 i = 0; i < m_rows; i++ ) {
			T sum = 0;

			for( int k = 0; k < m_cols; k++ ) {
				sum = sum + (*this)( k, i ) * v[k];
			}

			C[i] = sum;
		}

		return C;
	}

	// ** Matrix::row
	template<typename T>
	Vector<T> Matrix<T>::row( s32 index ) const
	{
		DC_BREAK_IF( index < 0 || index >= m_rows );

		Vector<T> r( m_cols );

		for( s32 i = 0; i < m_cols; i++ ) {
			r[i] = (*this)( i, index );
		}

		return r;
	}

	// ** Matrix::col
	template<typename T>
	Vector<T> Matrix<T>::col( s32 index ) const
	{
		DC_BREAK_IF( index < 0 || index >= m_cols );

		Vector<T> r( m_rows );

		for( s32 i = 0; i < m_rows; i++ ) {
			r[i] = (*this)( index, i );
		}

		return r;
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Matrix_H__ )  */
