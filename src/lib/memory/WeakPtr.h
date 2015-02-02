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

#ifndef		__DC_WeakPtr_H__
#define		__DC_WeakPtr_H__

#include	"RefCounted.h"

namespace dreemchest {

	// ** class WeakPtr
	template<typename T>
	class WeakPtr {
	private:

		mutable T*          m_ptr;
		mutable WeakProxy*  m_weakProxy;

	private:

		void				setPointer( T *pointer );
		void				manageProxy( void ) const;

	public:

							WeakPtr( T *pointer = NULL );
                            WeakPtr( const WeakPtr<T>& other );
							~WeakPtr( void );

		T*					get( void ) const;
		T*					operator -> ( void );
		const T*			operator -> ( void ) const;
		T&					operator *	( void );
		const T&			operator *	( void ) const;
		WeakPtr&			operator =	( T *pointer );
		WeakPtr&			operator =	( const WeakPtr<T>& other );
		bool				operator ==	( T *pointer ) const;
		bool				operator != ( T *pointer ) const;
	};

	// ** WeakPtr::WeakPtr
	template<typename T>
	WeakPtr<T>::WeakPtr( T *pointer ) : m_ptr( NULL ), m_weakProxy( NULL ) {
		setPointer( pointer );
	}

	// ** WeakPtr::WeakPtr
	template<typename T>
	WeakPtr<T>::WeakPtr( const WeakPtr<T>& other ) : m_ptr( NULL ), m_weakProxy( NULL ) {
		setPointer( other.get() );
	}

	// ** WeakPtr::~WeakPtr
	template<typename T>
	WeakPtr<T>::~WeakPtr( void ) {
		setPointer( NULL );
	}

	// ** WeakPtr::operator ->
	template<typename T>
	T* WeakPtr<T>::operator -> ( void ) {
		manageProxy();
		return m_ptr;
	}

	template<typename T>
	const T* WeakPtr<T>::operator -> ( void ) const {
		manageProxy();
		return m_ptr;
	}

	// ** WeakPtr::operator *
	template<typename T>
	T& WeakPtr<T>::operator * ( void ) {
		manageProxy();
		DC_BREAK_IF( m_ptr == NULL );
		return *m_ptr;
	}

	// ** WeakPtr::operator =
	template<typename T>
	WeakPtr<T>& WeakPtr<T>::operator = ( T *pointer ) {
		setPointer( pointer );
		return *this;
	}

	// ** WeakPtr::operator =
	template<typename T>
	WeakPtr<T>& WeakPtr<T>::operator = ( const WeakPtr<T>& other ) {
		setPointer( other.get() );
		return *this;
	}

	// ** WeakPtr::operator ==
	template<typename T>
	bool WeakPtr<T>::operator == ( T *pointer ) const {
		manageProxy();
		return ( m_ptr == pointer );
	}

	// ** WeakPtr::operator !=
	template<typename T>
	bool WeakPtr<T>::operator != ( T *pointer ) const {
		manageProxy();
		return ( m_ptr != pointer );
	}

	// ** WeakPtr::setPointer
	template<typename T>
	void WeakPtr<T>::setPointer( T *pointer ) {
		if( m_ptr == pointer ) {
			return;
		}

		if( m_weakProxy ) {
			m_weakProxy->release();
			m_weakProxy = NULL;
		}

		if( (m_ptr = pointer) ) {
			m_weakProxy = m_ptr->weakProxy();
		}
	}

	// ** cWeakPtr::get
	template<typename T>
	T* WeakPtr<T>::get( void ) const  {
		manageProxy();
		return m_ptr; 
	}

	// ** WeakPtr::manageProxy
	template<typename T>
	void WeakPtr<T>::manageProxy( void ) const {
		if( !m_ptr ) {
			return;
		}

		DC_BREAK_IF( m_weakProxy == NULL );
		if( !m_weakProxy->isAlive() ) {
			m_weakProxy->release();
			m_weakProxy = NULL;
			m_ptr		= NULL;
		}
	}

} // namespace dreemchest

#endif	/*	!__DC_WeakPtr_H__	*/