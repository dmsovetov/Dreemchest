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

#ifndef		__DC_Utils_AutoPtr_H__
#define		__DC_Utils_AutoPtr_H__

#include "../../Namespace.h"

DC_BEGIN_DREEMCHEST

	template<typename T>
	class AutoPtr {

		ClassDisableHeapAlloc()

	protected:

		T*                  m_pointer;

	public:

                            AutoPtr( T *pointer = NULL );
                            ~AutoPtr( void );

		T*                  get( void ) const;
		T*                  drop( void );

		T*                  operator -> ( void );
		const T*            operator -> ( void ) const;
		T&                  operator *	( void );
		const T&            operator *	( void ) const;
		AutoPtr&            operator =	( T *pointer );
		bool                operator ==	( const AutoPtr<T>& other ) const;
		bool                operator != ( const AutoPtr<T>& other ) const;
        const AutoPtr<T>&   operator =  ( const AutoPtr<T>& other );
	};

	// ** AutoPtr::AutoPtr
	template<typename T>
	AutoPtr<T>::AutoPtr( T *pointer ) {
		m_pointer = pointer;
	}

	template<typename T>
	AutoPtr<T>::~AutoPtr( void ) {
		if( m_pointer ) {
			delete m_pointer;
			m_pointer = NULL;
		}
	}

	// ** AutoPtr::operator ->
	template<typename T>
	T* AutoPtr<T>::operator -> ( void ) {
		DC_BREAK_IF( m_pointer == NULL );
		return m_pointer;
	}

	template<typename T>
	const T* AutoPtr<T>::operator -> ( void ) const {
		DC_BREAK_IF( m_pointer == NULL );
		return m_pointer;
	}

	// ** AutoPtr::operator *
	template<typename T>
	T& AutoPtr<T>::operator * ( void ) {
		DC_BREAK_IF( m_pointer == NULL );
		return *m_pointer;
	}

	template<typename T>
	AutoPtr<T>& AutoPtr<T>::operator = ( T *pointer ) {
		if( m_pointer ) {
			delete m_pointer;
		}

		m_pointer = pointer;
		return *this;
	}


    // ** AutoPtr::operator =
	template<typename T>
	const AutoPtr<T>& AutoPtr<T>::operator = ( const AutoPtr<T>& other ) {
		if( m_pointer ) {
			delete m_pointer;
		}

		m_pointer = other.m_pointer;
        const_cast<AutoPtr<T>&>( other ).m_pointer = NULL;

		return *this;
	}

	// ** AutoPtr::operator ==
	template<typename T>
	bool AutoPtr<T>::operator == ( const AutoPtr<T>& other ) const {
		return ( m_pointer == other.m_pointer );
	}

	// ** AutoPtr::operator !=
	template<typename T>
	bool AutoPtr<T>::operator != ( const AutoPtr<T>& other ) const {
		return ( m_pointer != other.m_pointer );
	}

	// ** AutoPtr::get
	template<typename T>
	T* AutoPtr<T>::get( void ) const {
		return m_pointer;
	}

	// ** AutoPtr::drop
	template<typename T>
	T* AutoPtr<T>::drop( void ) {
		T * tmp		= m_pointer;
		m_pointer	= NULL;
		return tmp;
	}

DC_END_DREEMCHEST

#endif		/*	!__DC_Utils_AutoPtr_H__	*/