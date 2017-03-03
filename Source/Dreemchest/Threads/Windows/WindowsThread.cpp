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

#include "WindowsThread.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

// ** WindowsThread::WindowsThread
WindowsThread::WindowsThread( void ) : m_handle( NULL )
{
}

WindowsThread::~WindowsThread( void )
{
    TerminateThread( m_handle, 0 );

    if( m_handle ) {
        CloseHandle( m_handle );
    }
}

// ** WindowsThread::start
void WindowsThread::start( const ThreadCallback& callback, void *userData )
{
    Thread::start( callback, userData );
    m_handle = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )threadProc, this, 0, NULL );
    SetThreadPriority( m_handle, THREAD_PRIORITY_NORMAL );
}

// ** WindowsThread::wait
void WindowsThread::wait( void ) const
{
    WaitForSingleObject( m_handle, INFINITE );
}

// ** WindowsThread::threadProc
void WindowsThread::threadProc( void *data )
{
    WindowsThread* thread = reinterpret_cast<WindowsThread*>( data );
    thread->m_callback( thread->m_userData );

    Sleep( 0 );
}

// ----------------------------- WindowsMutex ----------------------------- //

// ** WindowsMutex::WindowsMutex
WindowsMutex::WindowsMutex( void )
{
    InitializeCriticalSection( &m_criticalSection );
}

WindowsMutex::~WindowsMutex( void )
{
    DeleteCriticalSection( &m_criticalSection );
}

// ** WindowsMutex::lock
void WindowsMutex::lock( void )
{
    EnterCriticalSection( &m_criticalSection );
}

// ** WindowsMutex::unlock
void WindowsMutex::unlock( void )
{
    LeaveCriticalSection( &m_criticalSection );
}

// ** WindowsMutex::tryLock
bool WindowsMutex::tryLock( void )
{
    return TryEnterCriticalSection( &m_criticalSection ) == TRUE;
}

// ----------------------------- WindowsCondition ----------------------------- //

// ** WindowsCondition::WindowsCondition
WindowsCondition::WindowsCondition( void )
{
    InitializeCriticalSection( &m_criticalSection );
    InitializeConditionVariable( &m_condition );
}

WindowsCondition::~WindowsCondition( void )
{
    DeleteCriticalSection( &m_criticalSection );
}

// ** WindowsCondition::wait
void WindowsCondition::wait( void )
{
    EnterCriticalSection( &m_criticalSection );
    SleepConditionVariableCS( &m_condition, &m_criticalSection, INFINITE );
    LeaveCriticalSection( &m_criticalSection );
}

// ** WindowsCondition::trigger
void WindowsCondition::trigger( void )
{
    EnterCriticalSection( &m_criticalSection );
    WakeConditionVariable( &m_condition );
    LeaveCriticalSection( &m_criticalSection );
}

} // namespace Threads

DC_END_DREEMCHEST