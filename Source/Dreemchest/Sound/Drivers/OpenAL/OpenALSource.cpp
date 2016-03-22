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

#include "OpenALSource.h"
#include "OpenALBuffer.h"
#include "OpenAL.h"

#include "../../Decoders/SoundDecoder.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** OpenALSource::s_aliveCount
u32 OpenALSource::s_aliveCount = 0;

// ** OpenALSource::OpenALSource
OpenALSource::OpenALSource( void )
{
    s_aliveCount++;
    DC_BREAK_IF( s_aliveCount > OpenAL::maxSources(), "too much sound sources created" );

    alGenSources( 1, &m_id );
    alSourcei( m_id, AL_LOOPING, AL_FALSE );
    DC_BREAK_IF( !alIsSource( m_id ), "the generated id expected to be a sound source" );

    OpenAL::dumpErrors( "OpenALSource::OpenALSource" );
}

OpenALSource::~OpenALSource( void )
{
    alSourcei( m_id, AL_BUFFER, 0 );

    setState( Stopped );
    setBuffer( NULL );

    alDeleteSources( 1, &m_id );

    OpenAL::dumpErrors( "OpenALSource::~OpenALSource" );

    s_aliveCount--;
}

// ** OpenALSource::setBuffer
void OpenALSource::setBuffer( SoundBufferPtr value )
{
    if( m_buffer != NULL ) {
        setState( Stopped );
        m_buffer->detachFromSource( this );
    }

    SoundSource::setBuffer( value );

    if( m_buffer != NULL ) {
        m_buffer->attachToSource( this );
    }
}

// ** OpenALSource::state
SoundSource::SourceState OpenALSource::state( void ) const
{
    ALint state;
    alGetSourcei( m_id, AL_SOURCE_STATE, &state );

    switch( state ) {
    case AL_STOPPED: return Stopped;
    case AL_PAUSED:  return Paused;
    case AL_PLAYING: return Playing;
    }

    return Unknown;
}

// ** OpenALSource::setState
void OpenALSource::setState( SourceState value )
{
    SoundSource::setState( value );

    switch( value ) {
    case Stopped: alSourceStop( m_id );  break;
    case Paused:  alSourcePause( m_id ); break;
    case Playing: alSourcePlay( m_id );  break;
    default:      break;
    }

    OpenAL::dumpErrors( "OpenALSource::setState" );
    if( state() != value ) {
        LogWarning( "openal", "failed to change source state\n" );
    }
}

// ** OpenALSource::setVolume
void OpenALSource::setVolume( f32 value )
{
    SoundSource::setVolume( value );
    alSourcef( m_id, AL_GAIN, m_volume );

    OpenAL::dumpErrors( "OpenALSource::setVolume" );
}

// ** OpenALSource::setPitch
void OpenALSource::setPitch( f32 value )
{
    SoundSource::setPitch( value );
    alSourcef( m_id, AL_PITCH, m_pitch );

    OpenAL::dumpErrors( "OpenALSource::setPitch" );
}

// ** OpenALSource::setPosition
void OpenALSource::setPosition( const Vec3& value )
{
    SoundSource::setPosition( value );

    ALfloat v[3] = { value.x, value.y, value.z };
    alSourcefv( m_id, AL_POSITION, v );
}

// ** OpenALSource::setRelative
void OpenALSource::setRelative( bool value )
{
    alSourcei( m_id, AL_SOURCE_RELATIVE, value ? AL_TRUE : AL_FALSE );
}

// ** OpenALSource::setReferenceDistance
void OpenALSource::setReferenceDistance( f32 value )
{
    alSourcef( m_id, AL_REFERENCE_DISTANCE, value );
}

// ** OpenALSource::setMaximumDistance
void OpenALSource::setMaximumDistance( f32 value )
{
    alSourcef( m_id, AL_MAX_DISTANCE, value );
}

// ** OpenALSource::setRolloffFactor
void OpenALSource::setRolloffFactor( f32 value )
{
    alSourcef( m_id, AL_ROLLOFF_FACTOR, value );
}

// ** OpenALSource::update
void OpenALSource::update( void )
{
	DC_ABORT_IF( !m_buffer.valid(), "invalid buffer" );

    // ** Ensure we have a valid source state for a streamed sources
    if( m_state != state() && m_buffer->chunks() > 1 ) {
        setState( m_state );
    }
    
    // ** Don't update stopped/paused sources
    if( state() != Playing ) {
        return;
    }
    
    // ** Update stream
    bool completed = m_buffer->updateStream( this, isLooped() );
    
    if( completed ) {
        setState( Stopped );
    }
}

} // namespace Sound

DC_END_DREEMCHEST