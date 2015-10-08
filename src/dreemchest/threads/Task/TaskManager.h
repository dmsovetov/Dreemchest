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

#ifndef __DC_TaskManager_H__
#define __DC_TaskManager_H__

#include "TaskProgress.h"

DC_BEGIN_DREEMCHEST

namespace thread {

    // ** class TaskManager
    class dcInterface TaskManager {

        typedef Hash<class TaskThread*>  TaskThreads;

        // ** enum eBuiltInQueue
        enum eBuiltInQueue {
            MainQueue,
            BackgroundQueue,
            TotalBuiltInQueues
        };
        
    public:

                                //! Task manager constructor.
                                TaskManager( void );
                                //! Task manager destructor.
                                ~TaskManager( void );

        //! Starts a new background task with a given priority at a given worker.
        /*!
         \param task Task callback.
         \param userData User data associated with a task.
         \param priority Task priority.
         \param thread Worker name.
         \return TaskProgress object.
         */
        TaskProgressWPtr		runBackgroundTask( const TaskFunction& task, void* userData = NULL, u32 priority = 0, const char* thread = NULL );

        //! Adds a new task to process on main thread.
        /*!
         \param task Task callback.
         \param userData User data associated with a task.
         \param wait A flag indicating that we should wait for task completion.
         \return TaskProgress object.
         */
        TaskProgressWPtr		runMainThreadTask( const TaskFunction& task, void* userData = NULL, bool wait = true );

        //! Starts a new task with a given priority at a given worker.
        /*!
         \param task Task callback.
         \param userData User data associated with a task.
         \param threadId Thread identifier.
         \param wait A flag indicating that we should wait for task completion.
         \return TaskProgress object.
         */
        TaskProgressWPtr		runTask( const TaskFunction& task, void *userData = NULL, u32 threadId = 0, bool wait = true );

        //! Processes all queued main thread tasks.
        void                    doMainThreadTasks( void );

        //! Rerturns an amount of queued background tasks.
        u32                     totalBackgroundTasks( void ) const;

    private:

        //! Starts a new thread with a given name and task queue.
        /*!
         \param name Thread name.
         \param queue Task queue object.
         */
        void                    startTaskThread( const char* name, class TaskQueue* queue );

        //! Returns a pointer to TaskQueue object with a given name.
        /*!
         \param name Task queue name.
         \returns TaskQueue object if found, otherwise NULL.
         */
        class TaskQueue*        taskQueue( const char *name ) const;

        //! Returns a pointer to TaskQueue object with a given thread id.
        /*!
         \param name Thread id.
         \returns TaskQueue object if found, otherwise NULL.
         */
        class TaskQueue*        taskQueue( u32 threadId ) const;

    private:

        //! Array of built-in task queues.
        class TaskQueue*        m_taskQueues[TotalBuiltInQueues];

        //! Started task threads.
        TaskThreads             m_taskThreads;

        //! Main thread id.
        u64                     m_mainThread;
    };
    
} // namespace thread

DC_END_DREEMCHEST

#endif    /*    !__DC_TaskManager_H__    */