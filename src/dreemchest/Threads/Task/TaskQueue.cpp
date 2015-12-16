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

#include "TaskQueue.h"
#include "../Mutex.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

// ** TaskQueue::TaskQueue
TaskQueue::TaskQueue( void )
{
    m_mutex     = Mutex::create();
    m_condition = Condition::create();
}
    
TaskQueue::~TaskQueue( void )
{
    DC_RELEASE( m_condition );
    DC_RELEASE( m_mutex );
}

// ** TaskQueue::hasTasks
bool TaskQueue::hasTasks( void ) const
{
    DC_SCOPED_LOCK( m_mutex );
    return !m_tasks.empty();
}

// ** TaskQueue::size
u32 TaskQueue::size( void ) const
{
    DC_SCOPED_LOCK( m_mutex );
    return m_tasks.size();
}

// ** TaskQueue::pushTask
void TaskQueue::pushTask( const TaskFunction& task, void *userData, TaskProgress *progress, u32 priority )
{
    DC_SCOPED_LOCK( m_mutex );
    m_tasks.push( Task( task, userData, progress, priority ) );
    m_condition->trigger();
}

// ** TaskQueue::doTask
void TaskQueue::doTask( void )
{
    TaskFunction    function;
    TaskProgressPtr progress;
    void*           userData;

    {
        DC_SCOPED_LOCK( m_mutex );
        if( m_tasks.empty() ) {
            return;
        }

        Task task = m_tasks.top();

        function = task.m_function;
        progress = task.m_progress;
        userData = task.m_userData;
        m_tasks.pop();
    }

    DC_BREAK_IF( !function );
    function( progress.get(), userData );
    progress->complete();
}

// ** TaskQueue::waitForTasks
void TaskQueue::waitForTasks( void )
{
    m_condition->wait();
}

// ** TaskQueue::Task::Task
TaskQueue::Task::Task( TaskFunction function, void *userData, TaskProgress *progress, u32 priority )
{
    m_function = function;
    m_userData = userData;
    m_progress = progress;
    m_priority = priority;
}

// ** TaskQueue::Task::operator <
bool TaskQueue::Task::operator < ( const Task& other ) const
{
    return m_priority < other.m_priority;
}

// ** TaskQueue::Task::operator TaskFunction
TaskQueue::Task::operator TaskFunction ( void ) const
{
    return m_function;
}

} // namespace Threads

DC_END_DREEMCHEST