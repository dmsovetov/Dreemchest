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

#ifndef		__DC_WindowsThread_H__
#define		__DC_WindowsThread_H__

#include	"../Thread.h"
#include	"../Mutex.h"

#include	<windows.h>

DC_BEGIN_DREEMCHEST

namespace thread {

	// ** class WindowsThread
	class WindowsThread : public Thread {
	public:

						WindowsThread( void );
		virtual			~WindowsThread( void );

		// ** Thread
		virtual void	start( const ThreadCallback& callback, void *userData );
		virtual void	wait( void ) const;

	private:

		static void		threadProc( void *data );

	private:

		HANDLE			m_handle;
	};

	// ** class WindowsMutex
	class WindowsMutex : public Mutex {
	public:

							WindowsMutex( void );
		virtual				~WindowsMutex( void );

		// ** Mutex
		virtual bool		tryLock( void );
		virtual void		lock( void );
		virtual void		unlock( void );

	private:

		CRITICAL_SECTION	m_criticalSection;
	};

	// ** class WindowsCondition
	class WindowsCondition : public Condition {
	public:

							WindowsCondition( void );
		virtual             ~WindowsCondition( void );

		// ** Condition
        virtual void        wait( void );
        virtual void        trigger( void );

	private:

		CRITICAL_SECTION	m_criticalSection;
		CONDITION_VARIABLE	m_condition;
	};

} // namespace thread

DC_END_DREEMCHEST

#endif		/*	!__DC_Win32Thread_H__	*/