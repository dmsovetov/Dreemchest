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

#ifndef __DC_Threads_H__
#define __DC_Threads_H__

#include "../Dreemchest.h"

// --------------------------------------- Module macroses --------------------------------------- //

#define DC_SCOPED_LOCK( mutex ) \
            DC_DREEMCHEST_NS Threads::ScopedLock mutex##ScopedLock( mutex )

// ---------------------------------- Module types and namespaces -------------------------------- //

DC_BEGIN_DREEMCHEST

namespace Threads {

	dcDeclarePtrs( Thread )
	dcDeclarePtrs( TaskProgress )
	dcDeclarePtrs( Mutex )
	dcDeclarePtrs( Condition )
	dcDeclarePtrs( Task )
	dcDeclarePtrs( TaskManager )
	dcDeclarePtrs( TaskProgress )
	dcDeclarePtrs( TaskQueue )
	dcDeclarePtrs( TaskThread )

    typedef cClosure<void(TaskProgressWPtr, void*)> TaskFunction;
}

DC_END_DREEMCHEST

// ------------------------------------- Export module headers ------------------------------------ //

#ifndef DC_BUILD_LIBRARY
    #include "Task/TaskManager.h"
    #include "Thread.h"
    #include "Mutex.h"
#endif

#endif    /*    !__DC_Thread_H__    */
