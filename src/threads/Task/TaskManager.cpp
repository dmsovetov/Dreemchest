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

#include    "TaskManager.h"
#include    "TaskQueue.h"
#include    "TaskThread.h"
#include    "TaskProgress.h"

#include    "../Thread.h"

namespace dreemchest {

namespace thread {

// ** TaskManager::TaskManager
TaskManager::TaskManager( void )
{
    // ** Create built-in task queues
    for( int i = 0; i < TotalBuiltInQueues; i++ ) {
        m_taskQueues[i] = DC_NEW TaskQueue;
    }

    m_mainThread = Thread::currentThread();

    startTaskThread( "Worker1", m_taskQueues[BackgroundQueue] );
    startTaskThread( "Worker2", m_taskQueues[BackgroundQueue] );
    startTaskThread( "Worker3", m_taskQueues[BackgroundQueue] );
    startTaskThread( "Worker4", m_taskQueues[BackgroundQueue] );

    startTaskThread( "Asset", DC_NEW TaskQueue );
}

TaskManager::~TaskManager( void )
{
    for( int i = 0; i < TotalBuiltInQueues; i++ ) {
        delete m_taskQueues[i];
    }

    for( TaskThreads::iterator i = m_taskThreads.begin(), end = m_taskThreads.end(); i != end; i++ ) {
        delete i->second;
    }
}

// ** TaskManager::totalBackgroundTasks
int TaskManager::totalBackgroundTasks( void ) const
{
    return m_taskQueues[BackgroundQueue]->size();
}

// ** TaskManager::doMainThreadTasks
void TaskManager::doMainThreadTasks( void )
{
    TaskQueue *queue = m_taskQueues[MainQueue];
    while( queue->hasTasks() ) {
        queue->doTask();
    }
}

// ** TaskManager::runMainThreadTask
dcTaskProgressWeak TaskManager::runMainThreadTask( const TaskFunction& task, void *userData, bool wait )
{
    DC_BREAK_IF( !task );

    TaskProgress *progress = DC_NEW TaskProgress;
    if( m_mainThread != Thread::currentThread() ) {
        m_taskQueues[MainQueue]->pushTask( task, userData, progress, 0 );
        if( wait ) {
            progress->waitForCompletion();
            return NULL;
        }

        return progress;
    }

    task( progress, userData );
    delete progress;
    
    return NULL;
}

// ** TaskManager::runBackgroundTask
dcTaskProgressWeak TaskManager::runBackgroundTask( const TaskFunction& task, void* userData, int priority, const char* thread )
{
    DC_BREAK_IF( !task );

    TaskProgress *progress  = DC_NEW TaskProgress;
    TaskQueue    *queue     = taskQueue( thread );
    DC_BREAK_IF( queue == NULL );
    queue->pushTask( task, userData, progress, priority );

    return progress;
}

// ** TaskManager::runTask
dcTaskProgressWeak TaskManager::runTask( const TaskFunction& task, void *userData, u32 threadId, bool wait )
{
    DC_BREAK_IF( !task );

    TaskProgress *progress = DC_NEW TaskProgress;
    TaskQueue    *queue    = taskQueue( threadId );
    DC_BREAK_IF( queue == NULL );
    queue->pushTask( task, userData, progress, 0 );

    return progress;
}

// ** TaskManager::taskQueue
TaskQueue* TaskManager::taskQueue( const char *name ) const
{
    if( name ) {
        TaskThreads::const_iterator i = m_taskThreads.find( StringHash( name ) );

        if( i != m_taskThreads.end() ) {
            return i->second->taskQueue();
        }

        DC_BREAK;
    }

    return m_taskQueues[BackgroundQueue];
}

// ** TaskManager::taskQueue
TaskQueue* TaskManager::taskQueue( u32 threadId ) const
{
    if( threadId == m_mainThread ) {
        return m_taskQueues[MainQueue];
    }
    
    for( TaskThreads::const_iterator i = m_taskThreads.begin(), end = m_taskThreads.end(); i != end; ++i ) {
        if( i->second->threadId() == threadId ) {
            return i->second->taskQueue();
        }
    }

    return NULL;
}

// ** TaskManager::startTaskThread
void TaskManager::startTaskThread( const char *name, TaskQueue *queue )
{
    DC_BREAK_IF( queue == NULL );
    
    strhash     hash        = StringHash( name );
    TaskThread *taskThread  = DC_NEW TaskThread( name, queue );
    m_taskThreads[hash]     = taskThread;
}

} // namespace thread

} // namespace dreemchest