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

#include    "PosixThread.h"

#ifdef DC_PLATFORM_WINDOWS
    #pragma comment( lib, "../../../../lib/pthreadVC1.lib" )
#endif


DC_BEGIN_DREEMCHEST

namespace thread {

// ** PosixThread::PosixThread
PosixThread::PosixThread( void )
{
}

PosixThread::~PosixThread( void )
{
#ifdef DC_PLATFORM_ANDROID
    printf( "PosixThread::~PosixThread : not implemented\n" );
#else
    pthread_cancel( m_thread );
#endif
}

// ** PosixThread::start
void PosixThread::start( const ThreadCallback& callback, void *userData )
{
    Thread::start( callback, userData );
    
    u32 error = pthread_create( &m_thread, NULL, threadProc, this );
    DC_BREAK_IF( error );
}

// ** PosixThread::threadProc
void* PosixThread::threadProc( void *data )
{
    PosixThread *thread = reinterpret_cast<PosixThread*>( data );
    thread->m_id        = currentThread();
    thread->m_callback( thread->m_userData );

    sched_yield();

    return NULL;
}

// ** PosixThread::yield
void PosixThread::yield( void )
{
	threadYield();
}

// ** PosixThread::threadYield
void PosixThread::threadYield( void )
{
#if defined( DC_PLATFORM_ANDROID )
    sched_yield();
#elif defined( DC_PLATFORM_LINUX )
	pthread_yield();
#else
    pthread_yield_np();
#endif
}

} // namespace thread

DC_END_DREEMCHEST