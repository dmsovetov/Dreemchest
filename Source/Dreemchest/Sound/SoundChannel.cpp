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

#include "SoundChannel.h"
#include "Fader.h"
#include "Decoders/SoundDecoder.h"
#include "Drivers/SoundSource.h"
#include "Drivers/SoundBuffer.h"
#include "SoundData.h"
#include "SoundGroup.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** SoundChannel::SoundChannel
SoundChannel::SoundChannel( SoundDataWPtr data, SoundSourcePtr source )
    : m_source( source )
    , m_volumeFader( NULL )
    , m_sound( data )
    , m_volume( 1.0f )
{

}

SoundChannel::~SoundChannel( void )
{

}

// ** SoundChannel::volume
f32 SoundChannel::volume( void ) const
{
    return m_volume;
}

// ** SoundChannel::setVolume
void SoundChannel::setVolume( f32 value )
{
    m_volume = value;
}

// ** SoundChannel::position
const Vec3& SoundChannel::position( void ) const
{
    return m_source->position();
}

// ** SoundChannel::setPosition
void SoundChannel::setPosition( const Vec3& value )
{
    // Get the sound source buffer
    SoundBufferWPtr buffer = m_source->buffer();

    if( !buffer.valid() ) {
        LogError( "channel", "channel sound source does not have a sound buffer\n" );
        return;
    }

    // Only mono sounds can be positioned
    SoundSampleFormat format = buffer->format();

    if( format != SoundSampleMono8 && format != SoundSampleMono16 ) {
        LogWarning( "channel", "stereo sound '%s' could not be positioned in 3D\n", m_sound->identifier() );
    }

    m_source->setPosition( value );
}

// ** SoundChannel::isPlaying
bool SoundChannel::isPlaying( void ) const
{
    return m_source->state() == SoundSource::Playing;
}

// ** SoundChannel::isStopped
bool SoundChannel::isStopped( void ) const
{
    return m_source->state() == SoundSource::Stopped;
}

// ** SoundChannel::sound
SoundDataWPtr SoundChannel::sound( void ) const
{
    return m_sound;
}

// ** SoundChannel::pause
void SoundChannel::pause( f32 fade )
{
    if( m_volumeFader.valid() ) {
        DC_BREAK;
        return;
    }

    if( fade > 0.0f ) {
        m_volumeFader = DC_NEW Fader( m_volume, 0.0f, fade, dcThisMethod( SoundChannel::onFadeOut ) );
    } else {
        stopPlayback( true );
    }
}

// ** SoundChannel::resume
void SoundChannel::resume( f32 fade )
{
    if( m_volumeFader.valid() ) {
        DC_BREAK;
        return;
    }

    m_source->setState( SoundSource::Playing );
    m_source->setVolume( 0.0f );

    if( fade > 0.0f ) {
        m_volumeFader = DC_NEW Fader( 0.0f, m_volume, fade, dcThisMethod( SoundChannel::onFadeIn ) );
    } else {
        m_source->setVolume( m_volume );
    }
}

// ** SoundChannel::stop
void SoundChannel::stop( f32 fade )
{
    if( fade == 0.0f ) {
        onStopped( m_volumeFader );
        return;
    }

    if( m_volumeFader.valid() ) {
        return;
    }

    if( fade > 0.0f ) {
        m_volumeFader = DC_NEW Fader( m_volume, 0.0f, fade, dcThisMethod( SoundChannel::onStopped ) );
    } else {
        stopPlayback( false );
    }
}

// ** SoundChannel::update
bool SoundChannel::update( f32 dt )
{
    m_source->update();

    // Update volume fader and calculate the volume fade factor
	f32 fade = 1.0f;

    if( m_volumeFader.valid() ) {
		fade = m_volumeFader->update( dt );
	}

	// Calculate the final volume base on fade factor, group volume & channel volume
	f32 volume = m_volume * m_sound->group()->volume() * fade;

	// Set the sond source volume
	m_source->setVolume( volume );

    // Update source distance attenuation properties
    m_source->setMaximumDistance( m_sound->maximumDistance() );
    m_source->setRolloffFactor( m_sound->rolloffFactor() );
    m_source->setReferenceDistance( m_sound->referenceDistance() );
    m_source->setRelative( m_sound->isRelative() );

    return m_source->state() == SoundSource::Stopped;
}

// ** SoundChannel::stopPlayback
void SoundChannel::stopPlayback( bool pause )
{
    m_source->setState( pause ? SoundSource::Paused : SoundSource::Stopped );
    m_source->setVolume( 0.0f );
}

// ** SoundChannel::onFadeIn
void SoundChannel::onFadeIn( FaderWPtr fader )
{
    m_source->setVolume( fader->value() );
    m_volumeFader = FaderPtr();
}

// ** SoundChannel::onFadeOut
void SoundChannel::onFadeOut( FaderWPtr fader )
{
    stopPlayback( true );
    m_volumeFader = FaderPtr();
}

// ** SoundChannel::onStopped
void SoundChannel::onStopped( FaderWPtr fader )
{
    stopPlayback( false );
    m_volumeFader = FaderPtr();
}

} // namespace Sound

DC_END_DREEMCHEST