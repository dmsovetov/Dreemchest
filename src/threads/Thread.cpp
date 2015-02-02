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

#include	"Thread.h"

#ifdef DC_THREADING_POSIX
    #include    "Posix/PosixThread.h"
#elif DC_THREADING_WINDOWS
    #include    "Windows/WindowsThread.h"
#else
    #error Unknown threading option defined.
#endif

namespace dreemchest {

namespace thread {

// ** Thread::Thread
Thread::Thread( void ) : m_isAlive( false ), m_userData( NULL ), m_id( 0 )
{

}

Thread::~Thread( void )
{
}

// ** Thread::create
Thread* Thread::create( void )
{
#ifdef DC_THREADING_POSIX
    return DC_NEW PosixThread;
#elif DC_THREADING_WINDOWS
    return DC_NEW WindowsThread;
#else
    #error Unknown threading option defined.
#endif
}

// ** Thread::release
void Thread::release( void )
{
    delete this;
}

// ** Thread::isAlive
inline bool Thread::isAlive( void ) const
{
    return m_isAlive;
}

// ** Thread::id
u32 Thread::id( void ) const
{
    DC_BREAK_IF( m_id == 0 );
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
u32 Thread::currentThread( void )
{
#ifdef DC_PLATFORM_WINDOWS
	return GetCurrentThreadId();
#else
    return ( u32 )pthread_self();
#endif
}

} // namespace thread

} // namespace dreemchest