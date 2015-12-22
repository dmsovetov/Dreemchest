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

// ** FixedTimeStep::FixedTimeStep
FixedTimeStep::FixedTimeStep( u32 step ) : m_dt( step ), m_prev( 0 ), m_accumulated( 0 )
{
}

// ** FixedTimeStep::stepCount
s32 FixedTimeStep::stepCount( void )
{
	// Get current time
	u32 time = Platform::currentTime();

	if( m_prev == 0 ) {
		m_prev = time;
		return 0;
	}

	// Accumulate time
	m_accumulated += (time - m_prev);
	m_prev = time;

	if( m_accumulated < m_dt ) {
		return 0;
	}

	// Calculate the iterations count.
	s32 count = m_accumulated / m_dt;
	//m_accumulated = m_accumulated - count * m_dt;
	m_accumulated = 0;

	return count;
}

// ** FixedTimeStep::milliseconds
u32 FixedTimeStep::milliseconds( void ) const
{
	return m_dt;
}

// ** FixedTimeStep::seconds
f32 FixedTimeStep::seconds( void ) const
{
	return m_dt * 0.001f;
}

} // namespace Platform

DC_END_DREEMCHEST
