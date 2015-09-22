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

#ifndef __Base_ArrayAllocator_H__
#define __Base_ArrayAllocator_H__

DC_BEGIN_DREEMCHEST

    //! Allocates objects of type T from an array.
    template<typename T>
    class ArrayAllocator {
	public:

					//! Constructs ArrayAllocator instance
					ArrayAllocator( u32 size )
						: m_size( size ), m_count( 0 ) { reset(); }

		//! Clears an array of allocated objects.
		void		reset( void );

		//! Allocates the new object.
		T*			allocate( void );

		//! Returns the total number of allocated objects.
		u32			allocatedCount( void ) const;

		//! Returns the maximum capacity.
		u32			size( void ) const;

	private:

		Array<T>	m_array;	//!< An array of allocated objects.
		u32			m_size;		//!< The maximum number of objects that can be allocated.
		u32			m_count;	//!< The total number of allocated objects.
    };

	// ** ArrayAllocator::reset
	template<typename T>
	void ArrayAllocator<T>::reset( void )
	{
		m_count = 0;
		m_array.resize( m_size );
	}

	// ** ArrayAllocator::allocate
	template<typename T>
	T* ArrayAllocator<T>::allocate( void )
	{
		if( m_count >= m_size ) {
			return NULL;
		}

		T* allocated = &m_array[m_count++];
		DC_NEW( allocated ) T;

		return allocated;
	}

	// ** ArrayAllocator::allocatedCount
	template<typename T>
	u32 ArrayAllocator<T>::allocatedCount( void ) const
	{
		return m_count;
	}

	// ** ArrayAllocator::size
	template<typename T>
	u32 ArrayAllocator<T>::size( void ) const
	{
		return m_size;
	}

DC_END_DREEMCHEST

#endif    /*    !__Base_ArrayAllocator_H__    */
