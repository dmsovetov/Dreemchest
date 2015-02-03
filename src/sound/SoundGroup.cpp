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

#include    "SoundGroup.h"
#include    "SoundChannel.h"
#include    "SoundData.h"

namespace dreemchest {

namespace sound {

// ** SoundGroup::SoundGroup
SoundGroup::SoundGroup( const char *identifier )
{
    m_identifier = identifier;
    m_slotCount  = 4;
    m_behaviour  = ReplaceOldest;
    m_threshold  = 0;
    m_fadeTime   = 0;
}

SoundGroup::~SoundGroup( void )
{

}

// ** SoundGroup::identifier
const char* SoundGroup::identifier( void ) const
{
    return m_identifier.c_str();
}

// ** SoundGroup::setIdentifier
void SoundGroup::setIdentifier( const char *value )
{
    m_identifier = value;
}

// ** SoundGroup::slotCount
u32 SoundGroup::slotCount( void ) const
{
    return m_slotCount;
}

// ** SoundGroup::setSlotCount
void SoundGroup::setSlotCount( u32 value )
{
    m_slotCount = value;
}

// ** SoundGroup::activeSlotCount
u32 SoundGroup::activeSlotCount( void ) const
{
    return ( u32 )m_slots.size();
}

// ** SoundGroup::behaviour
SoundGroup::Behaviour SoundGroup::behaviour( void ) const
{
    return m_behaviour;
}

// ** SoundGroup::setBehaviour
void SoundGroup::setBehaviour( Behaviour value )
{
    m_behaviour = value;
}

// ** SoundGroup::threshold
u32 SoundGroup::threshold( void ) const
{
    return m_threshold;
}

// ** SoundGroup::setThreshold
void SoundGroup::setThreshold( u32 value )
{
    m_threshold = value;
}

// ** SoundGroup::fadeTime
f32 SoundGroup::fadeTime( void ) const
{
    return m_fadeTime;
}

// ** SoundGroup::setFadeTime
void SoundGroup::setFadeTime( f32 value )
{
    m_fadeTime = value;
}

// ** SoundGroup::data
SoundGroupInfo SoundGroup::data( void ) const
{
    SoundGroupInfo result;

    result.identifier = m_identifier;
    result.behaviour  = m_behaviour;
    result.slotCount  = m_slotCount;
    result.threshold  = m_threshold;
    result.fadeTime   = m_fadeTime;

    return result;
}

// ** SoundGroup::setData
void SoundGroup::setData( const SoundGroupInfo& value )
{
    m_identifier = value.identifier;
    m_behaviour  = static_cast<Behaviour>( value.behaviour );
    m_slotCount  = value.slotCount;
    m_threshold  = value.threshold;
    m_fadeTime   = value.fadeTime;
}

// ** SoundGroup::addSound
void SoundGroup::addSound( const dcSoundChannelWeak& channel )
{
    DC_BREAK_IF( activeSlotCount() + 1 > m_slotCount );
    m_slots.push_back( channel );
}

// ** SoundGroup::hasFreeSlots
bool SoundGroup::hasFreeSlots( void ) const
{
    return activeSlotCount() <= m_slotCount - 1;
}

// ** SoundGroup::update
void SoundGroup::update( void )
{
    for( int i = 0; i < activeSlotCount(); i++ ) {
        if( m_slots[i] == NULL ) {
            m_slots.erase( m_slots.begin() + i );
            i--;
        }
    }
}

// ** SoundGroup::requestSlot
bool SoundGroup::requestSlot( const SoundData *data )
{
    // ** Force group update if it's full, probably we have some stopped channels that waste slots
    if( !hasFreeSlots() ) {
        update();
    }

    // ** The group is not full - return true
    if( hasFreeSlots() ) {
        return true;
    }

    // ** The group is full - search for a free slot
    s32 index = -1;

    switch( m_behaviour ) {
    case Skip:                  index = -1;                                     break;
    case ReplaceOldest:         index =  0;                                     break;
    case ReplaceLowest:         index = findLowestSlot( data->priority() );     break;
    case ReplaceLowestOldest:   index = findLowestOldest( data->priority() );   break;
    }

    // ** No free slot found - return false
    if( index == -1 ) {
        return false;
    }

    // ** Stop a freed slot an remove it from a list
    m_slots[index]->stop( m_fadeTime );
    m_slots.erase( m_slots.begin() + index );

    return true;
}

// ** SoundGroup::findLowestSlot
s32 SoundGroup::findLowestSlot( u32 priority ) const
{
    s32 index = -1;

    for( int i = 0, n = activeSlotCount(); i < n; i++ ) {
        u32 slotPriority = m_slots[i]->sound()->priority();

        if( slotPriority < priority ) {
            index    = i;
            priority = slotPriority;
        }
    }
    
    return index;
}

// ** SoundGroup::findLowestOldest
s32 SoundGroup::findLowestOldest( u32 priority ) const
{
    s32 index = findLowestSlot( priority );
    if( index != -1 ) {
        return index;
    }
    
    for( s32 i = 0, n = activeSlotCount(); i < n; i++ ) {
        if( m_slots[i]->sound()->priority() == priority ) {
            return i;
        }
    }
    
    return -1;
}

} // namespace sound

} // namespace dreemchest