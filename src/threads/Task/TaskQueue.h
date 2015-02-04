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

#ifndef        __DC_TaskQueue_H__
#define        __DC_TaskQueue_H__

#include    "../../Common.h"
#include    "TaskProgress.h"

DC_BEGIN_DREEMCHEST

namespace thread {

    // ** class TaskQueue
    //! Task queue object.
    class TaskQueue {
    friend class TaskThread;
        
        // ** struct Task
        struct Task {
            u32                     m_priority; //!< Task priority, used to sort tasks inside the priority queue.
            dcTaskProgressStrong    m_progress; //!< Task's progress object. ?? Does it thread-safe?
            TaskFunction            m_function; //!< Task's callback function.
            void*                   m_userData; //!< Task's associated user data.

                                    //! Constructs a new Task object instance.
                                    /*!
                                     \param function Task callback function.
                                     \param userData Associated task user data.
                                     \param progress Task progress object, used to notify other threads about task status.
                                     \param priority Task priority.
                                     */
                                    Task( TaskFunction function, void* userData, TaskProgress* progress, u32 priority );

            //! Comparison operator.
            bool operator < ( const Task& other ) const;

            //! Cast's Task to it's callback function.
            operator TaskFunction( void ) const;
        };

        //! Container type to store a queued tasks.
        typedef std::priority_queue<Task> TaskPriorityQueue;

    public:

                                //! Constructs a new TaskQueue instance.
                                TaskQueue( void );
                                ~TaskQueue( void );

        //! Put's this task queue to sleep until new tasks are added.
        void                    waitForTasks( void );

        //! Adds a new task to this queue.
        /*!
         \param task Actual task function.
         \param userData Associated user data.
         \param progress Task progress object.
         \param priority Task priority.
         */
        void                    pushTask( const TaskFunction& task, void* userData, TaskProgress* progress, u32 priority );

        //! Does a next task in queue.
        /*!
         This function lockes a queue mutex, then pops a next task from queue and releases the mutex.
         Now the task is processed.
         */
        void                    doTask( void );

        //! Returns true if there are any queued tasks.
        bool                    hasTasks( void ) const;

        //! Returns an amount of queued tasks.
        u32                     size( void ) const;

    private:

        //! Task queue mutex used to lock a m_tasks variable.
        dcMutex                 m_mutex;

        //! Condition to wait for tasks in sleep mode.
        dcCondition             m_condition;

        //! Actual task queue.
        TaskPriorityQueue       m_tasks;
    };
    
} // namespace thread

DC_END_DREEMCHEST

#endif    /*    !__DC_TaskQueue_H__    */