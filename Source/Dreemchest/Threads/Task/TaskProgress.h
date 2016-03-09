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

#ifndef __DC_Threads_TaskProgress_H__
#define __DC_Threads_TaskProgress_H__

#include "../Threads.h"

DC_BEGIN_DREEMCHEST

namespace Threads {

    //! Tracks the task progress.
    class dcInterface TaskProgress : public RefCounted {
    public:

                                TaskProgress( void );
                                ~TaskProgress( void );

        //! Returns a task progress.
        f32						progress( void ) const;

        //! Sets a task progress.
        void                    setProgress( f32 value );

        //! Returns a latest task status message.
        String                  status( void ) const;

        //! Sets a task status message.
        void                    setStatus( CString value );

        //! Wait for this task to complete.
        void                    waitForCompletion( void );

        //! Mark this task as completed.
        void                    complete( void );

    private:

        //! Task mutex.
        MutexPtr				m_mutex;

        //! Task wait/completed condition.
        ConditionPtr			m_condition;

        //! Task progress.
        f32						m_progress;

        //! A boolean flag indicating that this task is completed
        bool                    m_isCompleted;

        //! Task status message.
        String                  m_status;
    };

} // namespace Threads

DC_END_DREEMCHEST

#endif    /*    !__DC_Threads_TaskProgress_H__    */