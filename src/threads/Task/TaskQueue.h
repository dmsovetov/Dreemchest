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

#ifndef		__DC_TaskQueue_H__
#define		__DC_TaskQueue_H__

#include	"../../Common.h"
#include    "TaskProgress.h"

namespace dreemchest {

namespace thread {

    // ** class TaskQueue
    //! Task queue object.
    class TaskQueue {
    friend class TaskThread;
        
        // ** struct sTask
        struct sTask {
            int                     m_priority;
            dcTaskProgressStrong    m_progress; // ?? Does it thread-safe?
            TaskFunction            m_function;
            void*                   m_userData;
            
                                    sTask( TaskFunction function, void *userData, TaskProgress *progress, int priority );
            
            bool operator < ( const sTask& other ) const;
            operator TaskFunction( void ) const;
        };

        typedef std::priority_queue<sTask> TaskPriorityQueue;

    public:

                                TaskQueue( void );
                                ~TaskQueue( void );

        void                    waitForTasks( void );
        void                    pushTask( const TaskFunction& task, void *userData, TaskProgress *progress, int priority );
        void                    doTask( void );
        bool                    hasTasks( void ) const;
        int                     size( void ) const;

    private:

        dcMutex                 m_mutex;
        dcCondition             m_condition;
        TaskPriorityQueue       m_tasks;
    };
    
} // namespace thread

} // namespace dreemchest

#endif	/*	!__DC_TaskQueue_H__	*/