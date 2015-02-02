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

#ifndef        __DC_RefCounter_H__
#define        __DC_RefCounter_H__

#include    "../../Common.h"

namespace dreemchest {

    // --------------------------------- WeakProxy --------------------------------- //

    // ** class WeakProxy
    //! A reference counted object proxy for WeakPtr object.
    class WeakProxy {
    public:

                        WeakProxy( void );

        //! Retains this weak proxy.
        int                retain( void );

        //! Releases this weak proxy.
        int                release( void );

        //! Marks this proxy as dead or alive.
        void            setAlive( bool alive );

        //! Returns true if a target object is still alive.
        bool            isAlive( void ) const;

    private:

                        WeakProxy( const WeakProxy& other ) {}
        void operator = ( const WeakProxy& other ) {}

    private:

        //! Weak proxy reference counter.
        int                m_references;
        
        //! Dead or alive flag.
        bool            m_isAlive;
    };

    // ** WeakProxy::cWeakProxy
    inline WeakProxy::WeakProxy( void )
    {
        m_references = 0;
        m_isAlive     = true;
    }

    // ** WeakProxy::retain
    inline int WeakProxy::retain( void ) {
        DC_BREAK_IF( !m_isAlive );
        m_references++;
        return m_references;
    }

    // ** WeakProxy::release
    inline int WeakProxy::release( void ) {
        DC_BREAK_IF( m_references == 0 );
        if( --m_references == 0 ) {
            delete this;
        }

        return m_references;
    }

    // ** WeakProxy::setAlive
    inline void WeakProxy::setAlive( bool alive ) {
        m_isAlive = alive;
    }

    // ** WeakProxy::isAlive
    inline bool WeakProxy::isAlive( void ) const {
        return m_isAlive;
    }

    // --------------------------------- RefCounted --------------------------------- //

    // ** class RefCounted
    //! An intrusive reference counting object.
    class dcInterface RefCounted {
    public:

                            RefCounted( void );
        virtual                ~RefCounted( void );

        //! Returns a WeakProxy object.
        WeakProxy*            weakProxy( void ) const;

        //! Disposes this object.
        virtual void        dispose( void ) const;

        //! Retains this object (adds a reference).
        virtual int            retain( void ) const;

        //! Releases this object (drops a reference), disposes an object if there are no more references left.
        virtual int            release( void ) const;

        //! Returns an amount of references pointing to this object.
        virtual int            totalReferences( void ) const;

    private:

        //! Reference counter.
        mutable int            m_references;

        //! Weak proxy object.
        mutable WeakProxy*  m_weakProxy;
    };

    // ** RefCounted::RefCounted
    inline RefCounted::RefCounted( void ) {
        m_references = 0;
        m_weakProxy     = NULL;
    }

    // ** RefCounted::~RefCounted
    inline RefCounted::~RefCounted( void )
    {
        DC_BREAK_IF( m_references != 0 );
        if( m_weakProxy ) {
            m_weakProxy->setAlive( false );
            m_weakProxy->release();
            m_weakProxy = NULL;
        }
    }

    // ** RefCounted::dispose
    inline void RefCounted::dispose( void ) const {
        delete this;
    }

    // ** RefCounted::retain
    inline int RefCounted::retain( void ) const {
        m_references++;
        return m_references;
    }

    // ** RefCounted::release
    inline int RefCounted::release( void ) const {
        DC_BREAK_IF( m_references == 0 );

        int left = --m_references;
        if( left == 0 ) {
            dispose();
        }
        return left;
    }

    // ** RefCounted::totalReferences
    inline int RefCounted::totalReferences( void ) const {
        return m_references;
    }

    // ** RefCounted::weakProxy
    inline WeakProxy* RefCounted::weakProxy( void ) const {
        if( !m_weakProxy ) {
            m_weakProxy = new WeakProxy;
            m_weakProxy->retain();
        }

        m_weakProxy->retain();
        return m_weakProxy;
    }

} // namespace dreemchest

#endif    /*    !__DC_RefCounter_H__    */