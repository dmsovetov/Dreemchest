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

#include    "SoundSource.h"
#include    "SoundBuffer.h"

DC_BEGIN_DREEMCHEST

namespace sound {

// ** SoundSource::SoundSource
SoundSource::SoundSource( void ) : m_state( Unknown ), m_isLooped( false ), m_volume( 1.0f ), m_pitch( 1.0f )
{

}

SoundSource::~SoundSource( void )
{
}

// ** SoundSource::update
void SoundSource::update( void )
{

}

// ** SoundSource::buffer
SoundBuffer* SoundSource::buffer( void ) const
{
    return m_buffer.get();
}

// ** SoundSource::setBuffer
void SoundSource::setBuffer( SoundBuffer* value )
{
    m_buffer = value;
}

// ** SoundSource::state
SoundSource::SourceState SoundSource::state( void ) const
{
    return m_state;
}

// ** SoundSource::setState
void SoundSource::setState( SourceState value )
{
    m_state = value;
}

// ** SoundSource::isLooped
bool SoundSource::isLooped( void ) const
{
    return m_isLooped;
}

// ** SoundSource::setLooped
void SoundSource::setLooped( bool value )
{
    m_isLooped = value;
}

// ** SoundSource::volume
f32 SoundSource::volume( void ) const
{
    return m_volume;
}

// ** SoundSource::setVolume
void SoundSource::setVolume( f32 value )
{
    m_volume = value;
}

// ** SoundSource::pitch
f32 SoundSource::pitch( void ) const
{
    return m_pitch;
}

// ** SoundSource::setPitch
void SoundSource::setPitch( f32 value )
{
    m_pitch = value;
}

} // namespace sound

DC_END_DREEMCHEST