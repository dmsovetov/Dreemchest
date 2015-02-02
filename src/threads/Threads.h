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

#ifndef		__DC_Threads_H__
#define		__DC_Threads_H__

#include    "../Common.h"

namespace dreemchest {

    namespace thread {
        class Thread;
        class Mutex;
        class Condition;
        class Task;
        class TaskManager;
        class TaskProgress;

        typedef cClosure<void(TaskProgress*, void*)> TaskFunction;
    }

    typedef thread::Thread*                 dcThread;
    typedef thread::Mutex*                  dcMutex;
    typedef thread::Condition*              dcCondition;
    typedef thread::Task*                   dcTask;
    typedef thread::TaskProgress*           dcTaskProgress;
    typedef thread::TaskManager*            dcTaskManager;

    DC_DECLARE_PTRS( thread::TaskProgress, TaskProgress )

} // namespace dreemchest

#ifndef DC_BUILD_LIBRARY
    #include "Task/TaskManager.h"
    #include "Thread.h"
#endif

#endif	/*	!__DC_Thread_H__	*/
