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

#include    "SoundEvent.h"

namespace dreemchest {

namespace sound {

// ** SoundEvent::SoundEvent
SoundEvent::SoundEvent( const char* identifier ) : m_identifier( identifier ), m_lastPlayedIndex( -1 )
{

}

SoundEvent::~SoundEvent( void )
{

}

// ** SoundEvent::probability
u8 SoundEvent::probability( void ) const
{
    return m_probability;
}

// ** SoundEvent::type
SoundEvent::EventType SoundEvent::type( void ) const
{
    return m_type;
}

// ** SoundEvent::requestIdentifier
const char* SoundEvent::requestIdentifier( void ) const
{
    if( m_sounds.empty() ) {
        return NULL;
    }

    // ** Event probability
    if( rand() % 100 > m_probability ) {
        return NULL;
    }

    // ** Get a random sound for triggered event
    m_lastPlayedIndex = generateSoundIndex();

    return m_sounds[m_lastPlayedIndex].c_str();
}

// ** SoundEvent::generateSoundIndex
u32 SoundEvent::generateSoundIndex( void ) const
{
    u32 count = ( u32 )m_sounds.size();

    // ** Not enough sound for a random playback
    if( count == 1 ) {
        return 0;
    }

    // ** Event type is a playlist - return the next index
    if( m_type == Playlist ) {
        return m_lastPlayedIndex + 1 >= count ? 0 : m_lastPlayedIndex + 1;
    }

    // ** Generate a random no repetitive index
    u32 index = -1;

    do {
        index = rand() % count;
    } while( index == m_lastPlayedIndex && count > 2 && m_type == RandomNoRepeat );

    return index;
}

// ** SoundEvent::data
SoundEventInfo SoundEvent::data( void ) const
{
    SoundEventInfo info;

    info.identifier     = m_identifier;
    info.sounds         = m_sounds;
    info.probability    = m_probability;
    info.type           = m_type;

    return info;
}

// ** SoundEvent::setData
void SoundEvent::setData( const SoundEventInfo& value )
{
    m_identifier        = value.identifier;
    m_sounds            = value.sounds;
    m_probability       = value.probability;
    m_type              = static_cast<EventType>( value.type );
}

} // namespace sound
    
} // namespace dreemchest