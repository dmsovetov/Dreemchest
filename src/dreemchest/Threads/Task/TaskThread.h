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

#ifndef __DC_Threads_TaskThread_H__
#define __DC_Threads_TaskThread_H__

#include "../Threads.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

    //! TaskThread object with associated task queue, used for background task processing.
    class TaskThread : public RefCounted {
    public:


                                //! Constructs a new TaskThread object.
                                /*!
                                 \param name Task thread name.
                                 \param queue Task thread queue.
                                 */
                                TaskThread( const String& name, TaskQueueWPtr queue );

        //! Returns a pointer to thread task queue.
        TaskQueueWPtr			taskQueue( void ) const;
        
        //! Returns a thread identifier.
        u32                     threadId( void ) const;

    private:

        //! Thread callback function. Task queue is processed here.
        void                    run( void* userData );

    private:

        //! Task thread name.
        String                  m_name;

        //! Task queue.
        TaskQueueWPtr			m_tasks;

        //! Thread object.
        ThreadPtr				m_thread;
    };
    
} // namespace Threads

DC_END_DREEMCHEST

#endif    /*    !__DC_Threads_TaskQueue_H__    */