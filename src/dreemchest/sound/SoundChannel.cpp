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
#include "SoundData.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** SoundChannel::SoundChannel
SoundChannel::SoundChannel( SoundDataWPtr data, SoundSourcePtr source ) : m_source( source ), m_volumeFader( NULL ), m_sound( data ), m_volume( 1.0f )
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

    m_volumeFader = DC_NEW Fader( m_volume, 0.0f, fade, dcThisMethod( SoundChannel::onFadeOut ) );
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

    m_volumeFader = DC_NEW Fader( 0.0f, m_volume, fade, dcThisMethod( SoundChannel::onFadeIn ) );
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

    m_volumeFader = DC_NEW Fader( m_volume, 0.0f, fade, dcThisMethod( SoundChannel::onStopped ) );
}

// ** SoundChannel::update
bool SoundChannel::update( f32 dt )
{
    m_source->update();

    // Update volume fader
    if( m_volumeFader.valid() ) {
		m_source->setVolume( m_volumeFader->update( dt ) );
	}

    return m_source->state() == SoundSource::Stopped;
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
    m_source->setState( SoundSource::Paused );
    m_source->setVolume( 0.0f );
    m_volumeFader = FaderPtr();
}

// ** SoundChannel::onStopped
void SoundChannel::onStopped( FaderWPtr fader )
{
    m_source->setState( SoundSource::Stopped );
    m_volumeFader = FaderPtr();
}

} // namespace Sound

DC_END_DREEMCHEST