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

#ifndef		__DC_Win32Thread_H__
#define		__DC_Win32Thread_H__

#include	"../Thread.h"
#include	"../Mutex.h"

namespace dreemchest {

namespace thread {

	// ** class Win32Thread
	class Win32Thread : public Thread {
	public:

						Win32Thread( void );
		virtual			~Win32Thread( void );

		// ** Thread
		virtual void	start( const ThreadCallback& callback, void *userData );

	private:

		static void		threadProc( void *data );

	private:

		HANDLE			m_handle;
	};

	// ** class Win32Mutex
	class Win32Mutex : public Mutex {
	public:

							Win32Mutex( void );
		virtual				~Win32Mutex( void );

		// ** Mutex
		virtual bool		tryLock( void );
		virtual void		lock( void );
		virtual void		unlock( void );

	private:

		CRITICAL_SECTION	m_criticalSection;
	};

	// ** class Win32Condition
	class Win32Condition : public Condition {
	public:

							Win32Condition( void );
		virtual             ~Win32Condition( void );

		// ** Condition
        virtual void        wait( void );
        virtual void        trigger( void );

	private:

		CRITICAL_SECTION	m_criticalSection;
		CONDITION_VARIABLE	m_condition;
	};

} // namespace thread

} // namespace dreemchest

#endif		/*	!__DC_Win32Thread_H__	*/