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

#ifndef __Base_Vector_H__
#define __Base_Vector_H__

DC_BEGIN_DREEMCHEST

	//! N-dimensional vector.
	template<typename T>
	class Vector {
	public:

					//! Constructs Vector instance.
					Vector( s32 size );

		//! Returns the Nth element of a vector.
		const T&	operator [] ( s32 index ) const;

		//! Returns the Nth element of a vector.
		T&			operator [] ( s32 index );

		//! Calculates the dot product between two vectors.
		T			operator * ( const Vector& other ) const;

		//! Returns the vector size.
		u32			size( void ) const;

		//! Calculates length of a vector.
		T			length( void ) const;

		//! Normalizes a vector.
		T			normalize( void );

	private:

		Array<T>	m_data;	//!< Actual vector components.
	};

	// ** Vector::Vector
	template<typename T>
	Vector<T>::Vector( s32 size )
	{
		m_data.resize( size );
		for( u32 i = 0, n = ( u32 )m_data.size(); i < n; i++ ) {
			m_data[i] = 0;
		}
	}

	// ** Vector::operator []
	template<typename T>
	const T& Vector<T>::operator [] ( s32 index ) const
	{
		DC_BREAK_IF( index < 0 || index >= size() );
		return m_data[index];
	}

	// ** Vector::operator []
	template<typename T>
	T& Vector<T>::operator [] ( s32 index )
	{
		DC_BREAK_IF( index < 0 || index >= ( s32 )size() );
		return m_data[index];
	}

	// ** Vector::operator *
	template<typename T>
	T Vector<T>::operator * ( const Vector& other ) const
	{
		DC_BREAK_IF( size() != other.size() );

		T sum = 0;

		for( u32 i = 0, n = size(); i < n; i++ ) {
			sum += m_data[i] * other[i];
		}

		return sum;
	}

	// ** Vector::size
	template<typename T>
	u32 Vector<T>::size( void ) const
	{
		return ( u32 )m_data.size();
	}

	// ** Vector::length
	template<typename T>
	T Vector<T>::length( void ) const
	{
		T sum = 0;

		for( u32 i = 0, n = size(); i < n; i++ ) {
			sum += m_data[i] * m_data[i];
		}

		return sqrt( sum );
	}

	// ** Vector::normalize
	template<typename T>
	T Vector<T>::normalize( void )
	{
		T len = length();

		if( len == 0 ) {
			return len;
		}

		for( u32 i = 0, n = size(); i < n; i++ ) {
			m_data[i] /= len;
		}

		return len;
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Vector_H__ )  */
