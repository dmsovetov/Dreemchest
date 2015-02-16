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

#ifndef __DC_Utils_StrongPtr_H__
#define __DC_Utils_StrongPtr_H__

#include    "RefCounted.h"

DC_BEGIN_DREEMCHEST

    // ** class StrongPtr
    template<typename T>
    class StrongPtr {
    public:

                        StrongPtr( T *pointer = NULL );
                        StrongPtr( const StrongPtr<T>& other );
                        ~StrongPtr( void );

        T*              get( void ) const;
        T*              operator -> ( void );
        const T*        operator -> ( void ) const;
        T&              operator *  ( void );
        const T&        operator *  ( void ) const;
        StrongPtr&      operator =  ( T *pointer );
        StrongPtr&      operator =  ( StrongPtr<T>& other );
        bool            operator == ( const StrongPtr<T>& other ) const;
        bool            operator != ( const StrongPtr<T>& other ) const;

    private:

        void            setPointer( RefCounted *pointer );

    private:

        RefCounted*     m_ptr;
    };

    // ** StrongPtr::StrongPtr
    template<typename T>
    StrongPtr<T>::StrongPtr( T *pointer ) : m_ptr( pointer ) {
        if( m_ptr ) {
            static_cast<RefCounted*>( m_ptr )->retain();
        }
    }

    // ** StrongPtr::StrongPtr
    template<typename T>
    StrongPtr<T>::StrongPtr( const StrongPtr<T>& other ) : m_ptr( other.m_ptr ) {
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
    StrongPtr<T>& StrongPtr<T>::operator = ( StrongPtr<T>& other ) {
        setPointer( other.m_ptr );
        return *this;
    }

    template<typename T>
    StrongPtr<T>& StrongPtr<T>::operator = ( T *pointer ) {
        setPointer( pointer );
        return *this;
    }

    // ** StrongPtr::operator ==
    template<typename T>
    bool StrongPtr<T>::operator == ( const StrongPtr<T>& other ) const {
        return ( m_ptr == other.m_ptr );
    }

    // ** StrongPtr::operator !=
    template<typename T>
    bool StrongPtr<T>::operator != ( const StrongPtr<T>& other ) const {
        return ( m_ptr != other.m_ptr );
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

#endif    /*    !__DC_Utils_StrongPtr_H__    */