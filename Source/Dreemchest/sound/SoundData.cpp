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

#include "SoundData.h"
#include "SoundFx.h"
#include "SoundGroup.h"
#include "Drivers/SoundBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** SoundData::SoundData
SoundData::SoundData( SoundFxWPtr sfx, CString identifier, CString uri, SoundGroupWPtr group ) : m_soundFx( sfx ), m_uri( uri ), m_group( group ), m_pcm( NULL ), m_format( SoundFormatUnknown )
{
    m_identifier        = identifier;
    m_type              = 0;
    m_loading           = LoadToRam;
    m_fadeTime          = 0;
    m_volume            = 1.0f;
    m_volumeModifier    = Range( 1.0f, 1.0f );
    m_pitch             = 1.0f;
    m_pitchModifier     = Range( 1.0f, 1.0f );
    m_isLooped          = false;
    m_priority          = 0;
}

SoundData::~SoundData( void )
{
}

// ** SoundData::identifier
CString SoundData::identifier( void ) const
{
    return m_identifier.c_str();
}

// ** SoundData::setIdentifier
void SoundData::setIdentifier( CString value )
{
    DC_ABORT_IF( value != NULL, "invalid identifier" );
    DC_ABORT_IF( strlen( value ) == 0, "empty identifier" );
    
    m_identifier = value;
}

// ** SoundData::format
SoundContainerFormat SoundData::format( void ) const
{
    return m_format;
}

// ** SoundData::setFormat
void SoundData::setFormat( SoundContainerFormat value )
{
    m_format = value;
}

// ** SoundData::uri
CString SoundData::uri( void ) const
{
    return m_uri.c_str();
}

// ** SoundData::setUri
void SoundData::setUri( CString uri )
{
    m_uri = uri;
}

// ** SoundData::group
SoundGroupWPtr SoundData::group( void ) const
{
    return m_group;
}

// ** SoundData::setGroup
void SoundData::setGroup( SoundGroupWPtr value )
{
    m_group = value;
}

// ** SoundData::type
u32 SoundData::type( void ) const
{
    return m_type;
}

// ** SoundData::setType
void SoundData::setType( u32 value )
{
    m_type = value;
}

// ** SoundData::loading
SoundData::LoadingFlags SoundData::loading( void ) const
{
    return m_loading;
}

// ** SoundData::setLoading
void SoundData::setLoading( LoadingFlags value )
{
    m_loading = value;
}

// ** SoundData::fadeTime
f32 SoundData::fadeTime( void ) const
{
    return m_fadeTime;
}

// ** SoundData::setFadeTime
void SoundData::setFadeTime( f32 value )
{
    m_fadeTime = value;
}

// ** SoundData::volume
f32 SoundData::volume( void ) const
{
    return m_volume;
}

// ** SoundData::setVolume
void SoundData::setVolume( f32 value )
{
    m_volume = value;
}

// ** SoundData::pitch
f32 SoundData::pitch( void ) const
{
    return m_pitch;
}

// ** SoundData::setPitch
void SoundData::setPitch( f32 value )
{
    m_pitch = value;
}

// ** SoundData::isLooped
bool SoundData::isLooped( void ) const
{
    return m_isLooped;
}

// ** SoundData::setLooped
void SoundData::setLooped( bool value )
{
    m_isLooped = value;
}

// ** SoundData::priority
u32 SoundData::priority( void ) const
{
    return m_priority;
}

// ** SoundData::setPriority
void SoundData::setPriority( u32 value )
{
    m_priority = value;
}

// ** SoundData::data
SoundDataInfo SoundData::data( void ) const
{
    SoundDataInfo result;

    result.identifier       = m_identifier;
    result.uri              = m_uri;
    result.group            = m_group.valid() ? m_group->identifier() : "";
    result.type             = m_type;
    result.loading          = m_loading;
    result.fadeTime         = m_fadeTime;
    result.volume           = m_volume;
    result.volumeModifier   = m_volumeModifier;
    result.pitch            = m_pitch;
    result.pitchModifier    = m_pitchModifier;
    result.isLooped         = m_isLooped;
    result.priority         = m_priority;

    return result;
}

// ** SoundData::setData
void SoundData::setData( const SoundDataInfo& value )
{
    m_identifier        = value.identifier;
    m_uri               = value.uri;
    m_group             = m_soundFx->findGroupByName( value.group.c_str() );
    m_type              = value.type;
    m_loading           = static_cast<LoadingFlags>( value.loading );
    m_fadeTime          = value.fadeTime;
    m_volume            = value.volume;
    m_volumeModifier    = value.volumeModifier;
    m_pitch             = value.pitch;
    m_pitchModifier     = value.pitchModifier;
    m_isLooped          = value.isLooped;
    m_priority          = value.priority;
}

// ** SoundData::pcm
SoundBufferWPtr SoundData::pcm( void ) const
{
    return m_pcm;
}

// ** SoundData::setPcm
void SoundData::setPcm( SoundBufferPtr value )
{
    m_pcm = value;
}

// ** SoundData::volumeForSound
f32 SoundData::volumeForSound( void ) const
{
    return m_volume * RANDOM_SCALAR( m_volumeModifier.min, m_volumeModifier.max );
}

// ** SoundData::pitchForSound
f32 SoundData::pitchForSound( void ) const
{
    return m_pitch * RANDOM_SCALAR( m_pitchModifier.min, m_pitchModifier.max );
}

} // namespace Sound

DC_END_DREEMCHEST