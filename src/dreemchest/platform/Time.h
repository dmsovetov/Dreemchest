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

#ifndef __DC_Platform_Time_H__
#define __DC_Platform_Time_H__

#include "Platform.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

	//! Returns current time in milliseconds.
	extern u32 currentTime( void );

	//! Fixed time step timer.
	class FixedTimeStep {
	public:

				//! Constructs FixedTimeStep instance.
				FixedTimeStep( u32 step );

		//! Returns the total number of steps passed since last call.
		s32		stepCount( void );

		//! Returns the time step as integer value in milliseconds.
		u32		milliseconds( void ) const;

		//! Returns the time step as floating point value in seconds.
		f32		seconds( void ) const;

	private:

		u32		m_dt;			//!< The requested timer step.
		u32		m_prev;			//!< The previous time value.
		u32		m_accumulated;	//!< The accumulated value.
	};

} // namespace Platform

DC_END_DREEMCHEST

#endif /*   !__DC_Platform_Time_H__   */