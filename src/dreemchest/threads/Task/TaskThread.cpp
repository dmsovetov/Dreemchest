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

#include    "TaskThread.h"
#include    "TaskQueue.h"
#include    "../Thread.h"

DC_BEGIN_DREEMCHEST

namespace thread {

// ** TaskThread::TaskThread
TaskThread::TaskThread( const char *name, TaskQueue *queue )
{
    m_name   = name;
    m_thread = Thread::create();
    m_tasks  = queue;

    m_thread->start( dcThisMethod( TaskThread::run ), NULL );
}

TaskThread::~TaskThread( void )
{

}

// ** TaskThread::taskQueue
TaskQueue* TaskThread::taskQueue( void ) const
{
    return m_tasks;
}

// ** TaskThread::threadId
u32 TaskThread::threadId( void ) const
{
    return m_thread->id();
}

// ** TaskThread::run
void TaskThread::run( void *userData )
{
    while( true ) {
        if( !m_tasks->hasTasks() ) {
            m_tasks->waitForTasks();
        }

        m_tasks->doTask();
    }
}

} // namespace thread

DC_END_DREEMCHEST