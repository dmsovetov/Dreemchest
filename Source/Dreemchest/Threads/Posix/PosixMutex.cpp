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

#include "PosixMutex.h"
#include "PosixThread.h"

#include "../Thread.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

// ** PosixMutex::PosixMutex
PosixMutex::PosixMutex( bool recursive )
{
    u32 result = 0;
    
    result = pthread_mutexattr_init( &m_attr );
    NIMBLE_BREAK_IF( result );
    
    result = pthread_mutexattr_settype( &m_attr, recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_DEFAULT );
    NIMBLE_BREAK_IF( result );

    result = pthread_mutex_init( &m_mutex, &m_attr );
    NIMBLE_BREAK_IF( result );
}

PosixMutex::~PosixMutex( void )
{
    u32 result = pthread_mutex_destroy( &m_mutex );
    NIMBLE_BREAK_IF( result );

    result = pthread_mutexattr_destroy( &m_attr );
    NIMBLE_BREAK_IF( result );
}

// ** PosixMutex::lock
void PosixMutex::lock( void )
{
    u32 result = pthread_mutex_lock( &m_mutex );
    NIMBLE_BREAK_IF( result );
}

// ** PosixMutex::unlock
void PosixMutex::unlock( void )
{
    u32 result = pthread_mutex_unlock( &m_mutex );
    NIMBLE_BREAK_IF( result );
}

// ** PosixMutex::tryLock
bool PosixMutex::tryLock( void )
{
    return pthread_mutex_trylock( &m_mutex ) == 0;
}

// -------------------------------------------------- PosixCondition -------------------------------------------------- //

// ** PosixCondition::PosixCondition
PosixCondition::PosixCondition( void )
{
    u32 result = 0;
    
    // ** Create condition var
    result = pthread_cond_init( &m_condition, NULL );
    NIMBLE_BREAK_IF( result );

    // ** Create condition mutex
    result = pthread_mutex_init( &m_mutex, NULL );
    NIMBLE_BREAK_IF( result );
}

PosixCondition::~PosixCondition( void )
{
    u32 result = 0;

    result = pthread_cond_destroy( &m_condition );
    NIMBLE_BREAK_IF( result );

    while( pthread_mutex_trylock( &m_mutex ) == 0 ) {
		PosixThread::threadYield();
    }
    pthread_mutex_unlock( &m_mutex );
    result = pthread_mutex_destroy( &m_mutex );
    NIMBLE_BREAK_IF( result );
}

// ** PosixCondition::wait
void PosixCondition::wait( void )
{
    u32 result = 0;

    result = pthread_mutex_lock( &m_mutex );
    NIMBLE_BREAK_IF( result );
    
    result = pthread_cond_wait( &m_condition, &m_mutex );
    NIMBLE_BREAK_IF( result );

    result = pthread_mutex_unlock( &m_mutex );
    NIMBLE_BREAK_IF( result );
}

// ** PosixCondition::trigger
void PosixCondition::trigger( void )
{
    u32 result = 0;

    result = pthread_mutex_lock( &m_mutex );
    NIMBLE_BREAK_IF( result );
    
    result = pthread_cond_signal( &m_condition );
    NIMBLE_BREAK_IF( result );
    
    result = pthread_mutex_unlock( &m_mutex );
    NIMBLE_BREAK_IF( result );
}

} // namespace Threads

DC_END_DREEMCHEST
