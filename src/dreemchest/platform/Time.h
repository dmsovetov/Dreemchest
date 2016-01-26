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

    //! Time step data issued by fixed timer.
    class TimeStep {
    friend class FixedTimeStep;
    public:

                    //! Constructs empty TimeStep instance.
                    TimeStep( void );

        //! Returns the time step duration in millseconds.
        u32         milliseconds( void ) const;

        //! Returns the time step duration in seconds.
        f32         seconds( void ) const;

        //! Returns the total number of iterations for this time step.
        s32         count( void ) const;

    private:

        u32         m_dt;     //!< Time step time in milliseconds.
        s32         m_count;  //!< The total number of iteration to be repeated for this time step.
    };

    //! Calculates the time passed since last update.
    class TimeDelta {
    public:

                    //! Constructs TimeDelta instance.
                    TimeDelta( void );

        //! Calculates time passed since last update.
        /*!
         \return Milliseconds passed since last update.
        */
        u32         update( void );

    private:

        u32         m_prev; //!< Last recorded time.
    };

	//! Fixed time step timer.
	class FixedTimeStep {
	public:

				    //! Constructs FixedTimeStep instance.
				    FixedTimeStep( u32 step, s32 maxSteps );

        //! Advances the fixed time step timer by a specified amount of milliseconds.
        /*!
         \param dt Time in milliseconds passed since last advance.
         \return The time step instance.
        */
        TimeStep    advance( u32 dt );

        //! Returns the maximum number of steps that can be performed.
        s32         maxSteps( void ) const;

	private:

		u32		    m_dt;			//!< The requested timer step.
		u32		    m_accumulated;	//!< The accumulated value.
        s32         m_maxSteps;     //!< The maxium number of steps to be performed.
	};

} // namespace Platform

DC_END_DREEMCHEST

#endif /*   !__DC_Platform_Time_H__   */