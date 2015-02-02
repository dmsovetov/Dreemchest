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

#ifndef        __DC_PosixMutex_H__
#define        __DC_PosixMutex_H__

#include    "Posix.h"
#include    "../Mutex.h"

namespace dreemchest {

namespace thread {
        
    // ** class PosixMutex
    class PosixMutex : public Mutex {
    public:

                            PosixMutex( bool recursive );
        virtual                ~PosixMutex( void );

        // ** Mutex
        virtual bool        tryLock( void );
        virtual void        lock( void );
        virtual void        unlock( void );

    private:

        pthread_mutex_t        m_mutex;
        pthread_mutexattr_t m_attr;
    };

    // ** class PosixCondition
    class PosixCondition : public Condition {
    public:

                            PosixCondition( void );
        virtual             ~PosixCondition( void );

        // ** Condition
        virtual void        wait( void );
        virtual void        trigger( void );

    private:

        pthread_cond_t      m_condition;
        pthread_mutex_t     m_mutex;
    };

} // namespace thread

} // namespace dreemchest

#endif    /*    !__DC_PosixMutex_H__    */