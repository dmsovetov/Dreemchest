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

#include	"WindowsThread.h"

namespace dreemchest {

namespace thread {

// ** Win32Thread::Win32Thread
Win32Thread::Win32Thread( void ) : m_handle( NULL )
{
}

Win32Thread::~Win32Thread( void )
{
	TerminateThread( m_handle, 0 );

	if( m_handle ) {
		CloseHandle( m_handle );
	}
}

// ** Win32Thread::start
void Win32Thread::start( const ThreadCallback& callback, void *userData )
{
	Thread::start( callback, userData );
	m_handle = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )threadProc, this, 0, NULL );
	SetThreadPriority( m_handle, THREAD_PRIORITY_NORMAL );
}

// ** Win32Thread::threadProc
void Win32Thread::threadProc( void *data )
{
	Win32Thread *thread = reinterpret_cast<Win32Thread*>( data );
	thread->m_callback( thread->m_userData );

	Sleep( 0 );
}

// ----------------------------- Win32Mutex ----------------------------- //

// ** Win32Mutex::Win32Mutex
Win32Mutex::Win32Mutex( void )
{
	InitializeCriticalSection( &m_criticalSection );
}

Win32Mutex::~Win32Mutex( void )
{
	DeleteCriticalSection( &m_criticalSection );
}

// ** Win32Mutex::lock
void Win32Mutex::lock( void )
{
	EnterCriticalSection( &m_criticalSection );
}

// ** Win32Mutex::unlock
void Win32Mutex::unlock( void )
{
	LeaveCriticalSection( &m_criticalSection );
}

// ** Win32Mutex::tryLock
bool Win32Mutex::tryLock( void )
{
	return TryEnterCriticalSection( &m_criticalSection ) == TRUE;
}

// ----------------------------- Win32Condition ----------------------------- //

// ** Win32Condition::Win32Condition
Win32Condition::Win32Condition( void )
{
	InitializeCriticalSection( &m_criticalSection );
	InitializeConditionVariable( &m_condition );
}

Win32Condition::~Win32Condition( void )
{
	DeleteCriticalSection( &m_criticalSection );
}

// ** Win32Condition::wait
void Win32Condition::wait( void )
{
	EnterCriticalSection( &m_criticalSection );
	SleepConditionVariableCS( &m_condition, &m_criticalSection, INFINITE );
	LeaveCriticalSection( &m_criticalSection );
}

// ** Win32Condition::trigger
void Win32Condition::trigger( void )
{
	EnterCriticalSection( &m_criticalSection );
	WakeConditionVariable( &m_condition );
	LeaveCriticalSection( &m_criticalSection );
}

} // namespace thread

} // namespace dreemchest