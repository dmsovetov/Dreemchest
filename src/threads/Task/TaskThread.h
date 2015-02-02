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

#ifndef        __DC_TaskThread_H__
#define        __DC_TaskThread_H__

#include    "../Threads.h"

namespace dreemchest {

namespace thread {

    // ** class TaskThread
    class TaskThread {
    public:

                                TaskThread( const char *name, class TaskQueue *queue );
                                ~TaskThread( void );

        class TaskQueue*        taskQueue( void ) const;
        u32                     threadId( void ) const;

    private:

        void                    run( void *userData );

    private:

        String                  m_name;
        class TaskQueue*        m_tasks;
        dcThread                m_thread;
    };
    
} // namespace thread

} // namespace dreemchest

#endif    /*    !__DC_TaskQueue_H__    */