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

#ifndef __DC_Threads_Mutex_H__
#define __DC_Threads_Mutex_H__

#include "Threads.h"

DC_BEGIN_DREEMCHEST

namespace Threads {
        
    //! Mutex object.
    class dcInterface Mutex : public RefCounted {
    public:

        //! Creates a new Mutex object.
        /*!
         \param recursive Flag indicating that a recursive mutex should be created.
         \return Mutex object.
         */
        static MutexPtr     create( bool recursive = false );

        //! Tries to lock this mutex, returns true if the lock is successfull
        //! (mutex was not locked before this call), otherwise returns false.
        virtual bool        tryLock( void ) = 0;

        //! Locks this mutex.
        virtual void        lock( void )    = 0;

        //! Unlocks this mutex.
        virtual void        unlock( void )  = 0;
    };

    //! Condition object.
    class dcInterface Condition : public RefCounted {
    public:

        //! Creates a new Condition object.
        static ConditionPtr create( void );

        //! Waits for this condition to trigger.
        virtual void        wait( void )    = 0;

        //! Triggers this condition.
        virtual void        trigger( void ) = 0;
    };

    //! A helper struct to lock a mutex by a C++ scope.
    class ScopedLock {
    public:

                    ScopedLock( MutexWPtr mutex ) : m_mutex( mutex ) { m_mutex->lock(); }
                    ~ScopedLock( void ) { m_mutex->unlock(); }

    private:

        MutexWPtr    m_mutex;
    };

} // namespace Threads

DC_END_DREEMCHEST

#endif    /*    !__DC_Threads_Mutex_H__    */
