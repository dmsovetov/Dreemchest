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

#include "TaskProgress.h"
#include "../Mutex.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

// ** TaskProgress::TaskProgress
TaskProgress::TaskProgress( void ) : m_isCompleted( false )
{
    m_mutex     = Mutex::create();
    m_condition = Condition::create();
}

TaskProgress::~TaskProgress( void )
{
    DC_BREAK_IF( !m_isCompleted, "destroying progress of an incompleted task" )

    {
        DC_SCOPED_LOCK( m_mutex );
        m_condition = ConditionPtr();
    }
    
    m_mutex = MutexPtr();
}

// ** TaskProgress::progress
f32 TaskProgress::progress( void ) const
{
    DC_SCOPED_LOCK( m_mutex );
    return m_progress;
}

// ** TaskProgress::setProgress
void TaskProgress::setProgress( f32 value )
{
    DC_SCOPED_LOCK( m_mutex );
    m_progress = value;
}

// ** TaskProgress::status
String TaskProgress::status( void ) const
{
    DC_SCOPED_LOCK( m_mutex );
    return m_status;
}

// ** TaskProgress::setStatus
void TaskProgress::setStatus( const char *value )
{
    DC_SCOPED_LOCK( m_mutex );
    m_status = value;
}

// ** TaskProgress::waitForCompletion
void TaskProgress::waitForCompletion( void )
{
    if( m_isCompleted ) {
        return;
    }

    m_condition->wait();
}

// ** TaskProgress::complete
void TaskProgress::complete( void )
{
    m_condition->trigger();

    DC_SCOPED_LOCK( m_mutex );
    m_isCompleted = true;
}

} // namespace Threads

DC_END_DREEMCHEST