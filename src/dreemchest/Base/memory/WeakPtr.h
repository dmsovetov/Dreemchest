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

#ifndef        __Base_WeakPtr_H__
#define        __Base_WeakPtr_H__

#include    "StrongPtr.h"

DC_BEGIN_DREEMCHEST

    // ** class WeakPtr
    template<typename T>
    class WeakPtr {
    private:

        mutable T*          m_ptr;
        mutable WeakProxy*  m_weakProxy;

    private:

        void                setPointer( T *pointer );
        void                manageProxy( void ) const;

    public:

                            WeakPtr( T *pointer = NULL );
                            WeakPtr( const WeakPtr<T>& other );
							WeakPtr( const StrongPtr<T>& strong );

							template <class R>
							WeakPtr( const WeakPtr<R>& other ) : m_ptr( NULL ), m_weakProxy( NULL ) {
								setPointer( other.get() );
							}
							template <class R>
							WeakPtr( const StrongPtr<R>& other ) : m_ptr( NULL ), m_weakProxy( NULL ) {
								setPointer( other.get() );
							}

                            ~WeakPtr( void );

        T*                  get( void ) const;
		StrongPtr<T>		lock( void ) const;
		bool				valid( void ) const;
        T*                  operator -> ( void );
        const T*            operator -> ( void ) const;
        T&                  operator *  ( void );
        const T&            operator *  ( void ) const;
        WeakPtr&            operator =  ( T *pointer );
        WeakPtr&            operator =  ( const WeakPtr<T>& other );
        bool                operator == ( T *pointer ) const;
		bool				operator == ( const WeakPtr<T>& other ) const;
        bool                operator != ( T *pointer ) const;
		bool				operator != ( const WeakPtr<T>& other ) const;
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

	// ** WeakPtr::WeakPtr
	template<typename T>
	WeakPtr<T>::WeakPtr( const StrongPtr<T>& strong ) : m_ptr( NULL ), m_weakProxy( NULL ) {
		setPointer( strong.get() );
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

    // ** WeakPtr::operator ==
    template<typename T>
    bool WeakPtr<T>::operator == ( const WeakPtr<T>& other ) const {
        manageProxy();
        return ( m_ptr == other.get() );
    }

    // ** WeakPtr::operator !=
    template<typename T>
    bool WeakPtr<T>::operator != ( T *pointer ) const {
        manageProxy();
        return ( m_ptr != pointer );
    }

    // ** WeakPtr::operator !=
    template<typename T>
    bool WeakPtr<T>::operator != ( const WeakPtr<T>& other ) const {
        manageProxy();
        return ( m_ptr != other.get() );
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

    // ** WeakPtr::get
    template<typename T>
    T* WeakPtr<T>::get( void ) const  {
        manageProxy();
        return m_ptr; 
    }

    // ** WeakPtr::lock
    template<typename T>
    StrongPtr<T> WeakPtr<T>::lock( void ) const  {
        manageProxy();
        return StrongPtr<T>( m_ptr ); 
    }

	// ** WeakPtr::valid
    template<typename T>
    bool WeakPtr<T>::valid( void ) const {
		manageProxy();
		return m_ptr != NULL;
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
            m_ptr        = NULL;
        }
    }

DC_END_DREEMCHEST

#endif    /*    !__Base_WeakPtr_H__    */
