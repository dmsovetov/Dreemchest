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

#ifndef __DC_Threads_Thread_H__
#define __DC_Threads_Thread_H__

#include "Threads.h"

#ifdef DC_PLATFORM_WINDOWS
    #undef Yield
#endif

DC_BEGIN_DREEMCHEST

namespace Threads {

    //! Thread function callback.
    typedef cClosure<void(void*)> ThreadCallback;

    //! Thread object.
    class dcInterface Thread : public RefCounted {
    public:

        virtual             ~Thread( void );

        //! Returns true is this thread is alive.
        bool                isAlive( void ) const;

        //! Returns a thread id.
        u32                 id( void ) const;

        //! Creates a new Thread object.
        static ThreadPtr	create( void );

        //! Puts this thread to sleep for a given amount of time.
        /*!
         \param ms An amount of time to sleep in milliseconds.
         */
        static void         sleep( u32 ms );

        //! Returns a current thread id.
        static u64          currentThread( void );

        //! Starts a thread with a given callback & user data.
        /*!
         \param callback Thread callback function.
         \param userData Associated thread user data.
         */
        virtual void        start( const ThreadCallback& callback, void *userData );

        //! Stops this thread.
        virtual void        end( void );

        //! Yields a processor time to other threads.
        virtual void        yield( void );

		//! Suspends execution of the calling thread until this one terminates.
		virtual void		wait( void ) const;

	protected:

							//! Constructs Thread instance.
							Thread( void );

    protected:

        //! A boolean flag indicating that thread is alive.
        bool                m_isAlive;

        //! A callback function.
        ThreadCallback      m_callback;

        //! An associated user data.
        void*               m_userData;

        //! Thread id.
        u32                 m_id;
    };

} // namespace Threads

DC_END_DREEMCHEST

#endif    /*    !__DC_Threads_Thread_H__    */