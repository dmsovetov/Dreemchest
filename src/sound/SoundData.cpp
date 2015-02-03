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

#include    "SoundData.h"
#include    "SoundFx.h"
#include    "SoundGroup.h"
#include    "Drivers/SoundBuffer.h"

namespace dreemchest {

namespace sound {

// ** SoundData::SoundData
SoundData::SoundData( SoundFx* sfx, const char* identifier, const char* uri, const SoundGroup* group ) : m_soundFx( sfx ), m_uri( uri ), m_group( group ), m_pcm( NULL )
{
    m_identifier        = identifier;
    m_type              = 0;
    m_loading           = LoadToRam;
    m_fadeTime          = 0;
    m_volume            = 1.0f;
    m_volumeModifier    = Vec2( 1.0f, 1.0f );
    m_pitch             = 1.0f;
    m_pitchModifier     = Vec2( 1.0f, 1.0f );
    m_isLooped          = false;
    m_priority          = 0;
}

SoundData::~SoundData( void )
{
}

// ** SoundData::identifier
const char* SoundData::identifier( void ) const
{
    return m_identifier.c_str();
}

// ** SoundData::setIdentifier
void SoundData::setIdentifier( const char *value )
{
    DC_BREAK_IF( value == NULL );
    DC_BREAK_IF( value && (strlen( value ) == 0) );
    
    m_identifier = value;
}

// ** SoundData::uri
const char* SoundData::uri( void ) const
{
    return m_uri.c_str();
}

// ** SoundData::setUri
void SoundData::setUri( const char* uri )
{
    m_uri = uri;
}

// ** SoundData::group
const SoundGroup* SoundData::group( void ) const
{
    return m_group;
}

// ** SoundData::setGroup
void SoundData::setGroup( const SoundGroup *value )
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
    result.group            = m_group ? m_group->identifier() : "";
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
SoundBuffer* SoundData::pcm( void ) const
{
    return m_pcm.get();
}

// ** SoundData::setPcm
void SoundData::setPcm( SoundBuffer *value )
{
    m_pcm = value;
}

// ** SoundData::volumeForSound
f32 SoundData::volumeForSound( void ) const
{
    return m_volume * RANDOM_SCALAR( m_volumeModifier.x, m_volumeModifier.y );
}

// ** SoundData::pitchForSound
f32 SoundData::pitchForSound( void ) const
{
    return m_pitch * RANDOM_SCALAR( m_pitchModifier.x, m_pitchModifier.y );
}

} // namespace sound

} // namespace dreemchest