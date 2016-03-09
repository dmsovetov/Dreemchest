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

#include "Thread.h"

#ifdef DC_THREADS_POSIX
    #include "Posix/PosixThread.h"
#elif DC_THREADS_WINDOWS
    #include "Windows/WindowsThread.h"
#else
    #error Unknown threading option defined.
#endif

#ifdef DC_PLATFORM_ANDROID
	#include <unistd.h>
#endif

DC_BEGIN_DREEMCHEST

namespace Threads {

// ** Thread::Thread
Thread::Thread( void ) : m_isAlive( false ), m_userData( NULL ), m_id( 0 )
{

}

Thread::~Thread( void )
{
}

// ** Thread::create
ThreadPtr Thread::create( void )
{
#ifdef DC_THREADS_POSIX
    return ThreadPtr( DC_NEW PosixThread );
#elif DC_THREADS_WINDOWS
    return ThreadPtr( DC_NEW WindowsThread );
#else
    #error Unknown threading option defined.
#endif

	return ThreadPtr();
}

// ** Thread::isAlive
inline bool Thread::isAlive( void ) const
{
    return m_isAlive;
}

// ** Thread::id
u32 Thread::id( void ) const
{
    DC_BREAK_IF( m_id == 0, "thread id is expected to be a non-zero value" )
    return m_id;
}

// ** Thread::start
void Thread::start( const ThreadCallback& callback, void *userData )
{
    m_callback = callback;
    m_userData = userData;
    m_isAlive  = true;
}

// ** Thread::end
void Thread::end( void )
{
    m_isAlive = false;
}

// ** Thread::yield
void Thread::yield( void )
{
    sleep( 1 );
}

// ** Thread::wait
void Thread::wait( void ) const
{
}

// ** Thread::sleep
void Thread::sleep( u32 ms )
{
#ifdef DC_PLATFORM_WINDOWS
    ::Sleep( ms );
#else
    usleep( ms * 1000 );
#endif
}

// ** Thread::currentThread
u64 Thread::currentThread( void )
{
#ifdef DC_PLATFORM_WINDOWS
    return GetCurrentThreadId();
#else
    return ( u64 )pthread_self();
#endif
}

} // namespace Threads

DC_END_DREEMCHEST