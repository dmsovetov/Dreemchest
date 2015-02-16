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

#include    "Fader.h"

DC_BEGIN_DREEMCHEST

namespace sound {

// ** Fader::Fader
Fader::Fader( f32 start, f32 end, f32 duration, FaderCallback callback ) : m_start( start ), m_end( end ), m_duration( duration ), m_time( 0.0f ), m_callback( callback )
{
    DC_BREAK_IF( duration < 0 || duration > 60000 )
}

// ** Fader::value
f32 Fader::value( void ) const
{
    f32 scalar = std::min( 1.0f, m_time / m_duration );
    return m_start * (1.0f - scalar) + m_end * scalar;
}

// ** Fader::update
f32 Fader::update( f32 dt )
{
    m_time += dt * 1000.0f;
    f32 result = value();

    if( m_time >= m_duration ) {
        m_callback( this );
    }
    
    return result;
}

} // namespace sound

DC_END_DREEMCHEST