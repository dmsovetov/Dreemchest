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

#include "Time.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

// -------------------------------------------------------------- TimeDelta -------------------------------------------------------------- //

// ** TimeDelta::TimeDelta
TimeDelta::TimeDelta( void ) : m_prev( 0 )
{

}

// ** TimeDelta::update
u32 TimeDelta::update( void )
{
	// Get current time
	u32 time = Platform::currentTime();

	if( m_prev == 0 ) {
		m_prev = time;
		return 0;
	}

    u32 dt = time - m_prev;
    m_prev = time;

    return dt;
}

// -------------------------------------------------------------- TimeStep --------------------------------------------------------------- //

// ** TimeStep::TimeStep
TimeStep::TimeStep( void ) : m_dt( 0 ), m_count( 0 )
{
}

// ** TimeStep::milliseconds
u32 TimeStep::milliseconds( void ) const
{
    return m_dt;
}

// ** TimeStep::seconds
f32 TimeStep::seconds( void ) const
{
    return milliseconds() * 0.001f;
}


// ** TimeStep::count
s32 TimeStep::count( void ) const
{
    return m_count;
}

// ------------------------------------------------------------ FixedTimeStep ------------------------------------------------------------ //

// ** FixedTimeStep::FixedTimeStep
FixedTimeStep::FixedTimeStep( u32 step, s32 maxSteps )
    : m_dt( step )
    , m_accumulated( 0 )
    , m_maxSteps( maxSteps )
{
}

// ** FixedTimeStep::advance
TimeStep FixedTimeStep::advance( u32 dt )
{
	// Accumulate time
	m_accumulated += dt;

	if( m_accumulated < m_dt ) {
		return TimeStep();
	}

	// Calculate the iterations count.
    TimeStep step;
    step.m_count = m_accumulated / m_dt;
    step.m_dt    = m_dt;

    // Dont perform more than maximum allowed number of steps.
    if( step.count() > m_maxSteps ) {
        step.m_count = m_maxSteps;
        step.m_dt    = m_accumulated / step.m_count;
    }

    // Decrease an accumulator.
	m_accumulated = m_accumulated - step.count() * step.milliseconds();

	return step;
}

// ** FixedTimeStep::maxSteps
s32 FixedTimeStep::maxSteps( void ) const
{
    return m_maxSteps;
}

// ** FixedTimeStep::setMaxSteps
void FixedTimeStep::setMaxSteps( s32 value )
{
    m_maxSteps = value;
}

// ** FixedTimeStep::setTimeStep
void FixedTimeStep::setTimeStep( u32 value )
{
    m_dt = value;
}

// ** FixedTimeStep::dt
u32 FixedTimeStep::dt( void ) const
{
    return m_dt;
}

// ** FixedTimeStep::accumulated
u32 FixedTimeStep::accumulated( void ) const
{
    return m_accumulated;
}

} // namespace Platform

DC_END_DREEMCHEST
