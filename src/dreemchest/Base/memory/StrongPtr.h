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

#ifndef __Base_StrongPtr_H__
#define __Base_StrongPtr_H__

#include    "RefCounted.h"

DC_BEGIN_DREEMCHEST

    // ** class StrongPtr
    template<typename T>
    class StrongPtr {
    public:

        typedef StrongPtr<T> ThisType;

                        StrongPtr( const ThisType& other );
                        StrongPtr( T *pointer = NULL );
                        ~StrongPtr( void );

                        template <class R>
                        StrongPtr( const StrongPtr<R>& other ) : m_ptr( other.get() ) {
                            if( m_ptr ) {
                                static_cast<RefCounted*>( m_ptr )->retain();
                            }
                        }

        T*              get( void ) const;
		bool			valid( void ) const;
        T*              operator -> ( void );
        const T*        operator -> ( void ) const;
        T&              operator *  ( void );
        const T&        operator *  ( void ) const;
        ThisType&       operator =  ( T* pointer );
        ThisType&       operator =  ( const ThisType& other );
        bool            operator == ( const ThisType& other ) const;
        bool            operator != ( const ThisType& other ) const;
        bool            operator <  ( const ThisType& other ) const;

    private:

        void            setPointer( RefCounted *pointer );

    private:

        RefCounted*     m_ptr;
    };

    // ** StrongPtr::StrongPtr
    template<typename T>
    StrongPtr<T>::StrongPtr( T* pointer ) : m_ptr( pointer ) {
        if( m_ptr ) {
            static_cast<RefCounted*>( m_ptr )->retain();
        }
    }

    // ** StrongPtr::StrongPtr
    template<typename T>
    StrongPtr<T>::StrongPtr( const ThisType& other ) : m_ptr( other.m_ptr ) {
        if( m_ptr ) {
            static_cast<RefCounted*>( m_ptr )->retain();
        }
    }

    // ** StrongPtr::~StrongPtr
    template<typename T>
    StrongPtr<T>::~StrongPtr( void ) {
        if( m_ptr ) {
            m_ptr->release();
            m_ptr = NULL;
        }
    }

    // ** StrongPtr::get
    template<typename T>
    T* StrongPtr<T>::get( void ) const  {
        return ( T* )m_ptr;
    }

    // ** StrongPtr::valid
    template<typename T>
    bool StrongPtr<T>::valid( void ) const  {
        return m_ptr != NULL;
    }

    // ** StrongPtr::operator ->
    template<typename T>
    T* StrongPtr<T>::operator -> ( void ) {
        DC_BREAK_IF( m_ptr == NULL );
        return ( T* )m_ptr;
    }

    template<typename T>
    const T* StrongPtr<T>::operator -> ( void ) const {
        DC_BREAK_IF( m_ptr == NULL );
        return ( T* )m_ptr;
    }

    // ** StrongPtr::operator *
    template<typename T>
    T& StrongPtr<T>::operator * ( void ) {
        DC_BREAK_IF( m_ptr == NULL );
        return *m_ptr;
    }

    // ** StrongPtr::operator =
    template<typename T>
    typename StrongPtr<T>::ThisType& StrongPtr<T>::operator = ( const ThisType& other ) {
        setPointer( other.m_ptr );
        return *this;
    }

    template<typename T>
    typename StrongPtr<T>::ThisType& StrongPtr<T>::operator = ( T *pointer ) {
        setPointer( pointer );
        return *this;
    }

    // ** StrongPtr::operator ==
    template<typename T>
    bool StrongPtr<T>::operator == ( const ThisType& other ) const {
        return ( m_ptr == other.m_ptr );
    }

    // ** StrongPtr::operator !=
    template<typename T>
    bool StrongPtr<T>::operator != ( const ThisType& other ) const {
        return ( m_ptr != other.m_ptr );
    }

    // ** StrongPtr::operator <
    template<typename T>
    bool StrongPtr<T>::operator < ( const ThisType& other ) const {
        return ( m_ptr < other.m_ptr );
    }

    // ** StrongPtr::setPointer
    template<typename T>
    void StrongPtr<T>::setPointer( RefCounted *pointer ) {
        if( m_ptr ) {
            m_ptr->release();
        }

        if( (m_ptr = pointer) ) {
            m_ptr->retain();
        }
    }

DC_END_DREEMCHEST

#endif    /*    !__Base_StrongPtr_H__    */
