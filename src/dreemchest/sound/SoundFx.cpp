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

#include    "SoundFx.h"

#include    "SoundChannel.h"
#include    "SoundGroup.h"
#include    "SoundData.h"
#include    "SoundEvent.h"
#include    "SoundStream.h"

#include    "Decoders/SoundDecoder.h"
#include    "Drivers/SoundSource.h"
#include    "Drivers/SoundBuffer.h"
#include    "Drivers/OpenAL/OpenAL.h"

DC_BEGIN_DREEMCHEST

namespace sound {

IMPLEMENT_LOGGER( log )

// ------------------------------------------------------ SoundFx ------------------------------------------------------ //

// ** SoundFx::SoundFx
SoundFx::SoundFx( SoundHal hal, IStreamOpener* streamOpener ) : m_hal( NULL ), m_streamOpener( streamOpener ? streamOpener : DC_NEW StandardStreamOpener )
{
    switch( hal ) {
    case None:      m_hal = NULL;           break;
    case Default:   
                    #ifdef HAVE_OPENAL
                        m_hal = DC_NEW OpenAL;
                    #else
                        log::error( "SoundFx::SoundFx : the default sound HAL is OpenAL, but library compiled without OpenAL\n" );
                    #endif
                    break;
    }
}

SoundFx::~SoundFx( void )
{
    reset();
    delete m_streamOpener;
    delete m_hal;
}

// ** SoundFx::createGroup
SoundGroup* SoundFx::createGroup( const char *identifier )
{
    DC_BREAK_IF( identifier == NULL );

    if( findGroupByName( identifier ) ) {
        log::warn( "SoundFx::createGroup : failed to create group, group with a same name '%s' found\n", identifier );
        return NULL;
    }

    SoundGroup* group = DC_NEW SoundGroup( identifier );
    strhash     hash  = StringHash( identifier );

    m_groups[hash] = group;

    return group;
}

// ** SoundFx::createSound
SoundData* SoundFx::createSound( const char *identifier, const char* uri, const SoundGroup* group )
{
    DC_BREAK_IF( identifier == NULL );
    DC_BREAK_IF( uri == NULL );

    // ** Ensure we don't have a sound with a same name.
    if( findSoundByName( identifier ) ) {
        log::warn( "SoundFx::createSound : failed to create sound, sound with a same name '%s' found\n", identifier );
        return NULL;
    }

    // ** Create a sound data instance
    SoundData* sound = DC_NEW SoundData( this, identifier, uri, group );
    strhash    hash  = StringHash( identifier );

    m_sounds[hash] = sound;

    return sound;
}

// ** SoundFx::createEvent
SoundEvent* SoundFx::createEvent( const char* identifier )
{
    DC_BREAK_IF( identifier == NULL )

    if( findEventByName( identifier ) ) {
        log::warn( "SoundFx::createEvent : failed to create event, sound with a same name '%s' found\n", identifier );
        return NULL;
    }

    SoundEvent* event = DC_NEW SoundEvent( identifier );
    strhash     hash  = StringHash( identifier );

    m_events[hash] = event;

    return event;
}

// ** SoundFx::findSoundByName
SoundData* SoundFx::findSoundByName( const char* identifier )
{
    Sounds::iterator i = m_sounds.find( StringHash( identifier ) );

    if( i != m_sounds.end() ) {
        return i->second.get();
    }

    return NULL;
}

// ** SoundFx::removeSoundByName
void SoundFx::removeSoundByName( const char* identifier )
{
    DC_BREAK_IF( identifier == NULL );
    m_sounds.erase( StringHash( identifier ) );
}

// ** SoundFx::renameSound
bool SoundFx::renameSound( const char* identifier, const char* newName )
{
    // ** Sound with a same name exists
    if( findSoundByName( newName ) ) {
        return false;
    }

    // ** Get the sound
    dcSoundDataStrong sound = findSoundByName( identifier );

    if( sound == NULL ) {
        return false;
    }

    // ** Rename sound
    removeSoundByName( identifier );

    strhash hash = StringHash( newName );
    sound->setIdentifier( newName );

    m_sounds[hash] = sound;

    return true;
}

// ** SoundFx::findGroupByName
SoundGroup* SoundFx::findGroupByName( const char* identifier )
{
    SoundGroups::iterator i = m_groups.find( StringHash( identifier ) );

    if( i != m_groups.end() ) {
        return i->second.get();
    }

    return NULL;
}

// ** SoundFx::removeGroupByName
void SoundFx::removeGroupByName( const char* identifier )
{
    DC_BREAK_IF( identifier == NULL );
    m_groups.erase( StringHash( identifier ) );
}

// ** SoundFx::renameGroup
bool SoundFx::renameGroup( const char* identifier, const char* newName )
{
    // ** Group with a same name exists
    if( findGroupByName( newName ) ) {
        return false;
    }

    // ** Get the sound
    dcSoundGroupStrong group = findGroupByName( identifier );

    if( group == NULL ) {
        return false;
    }

    // ** Rename group
    removeGroupByName( identifier );

    strhash hash = StringHash( newName );
    group->setIdentifier( newName );
    
    m_groups[hash] = group;
    
    return true;
}

// ** SoundFx::findEventByName
SoundEvent* SoundFx::findEventByName( const char *identifier )
{
    SoundEvents::iterator i = m_events.find( StringHash( identifier ) );

    if( i != m_events.end() ) {
        return i->second;
    }

    return NULL;
}

// ** SoundFx::createSource
SoundSource* SoundFx::createSource( SoundData *data ) const
{
    DC_BREAK_IF( data == NULL );
    
    if( m_hal == NULL ) {
        log::error( "SoundFx::createSource : failed to create sound source for '%s', no HAL created.\n", data->identifier() );
        return NULL;
    }

    // ** Create sound source
    SoundSource* source = m_hal->createSource();
    if( !source ) {
        log::error( "SoundFx::createSource : failed to create sound source for '%s'\n", data->identifier() );
        return NULL;
    }

    // ** Setup source
    source->setBuffer( createBuffer( data ) );
    source->setLooped( data->isLooped() );
    source->setPitch( data->pitchForSound() );

    return source;
}

// ** SoundFx::createBuffer
SoundBuffer* SoundFx::createBuffer( SoundData *data ) const
{
    DC_BREAK_IF( data == NULL );

    if( !m_hal ) {
        log::error( "SoundFx::createBuffer : failed to create sound buffer for '%s', no HAL created.\n", data->identifier() );
        return NULL;
    }

    // ** Create sound decoder
    SoundDecoder* decoder = createDecoder( data );
    if( !decoder && !data->pcm() ) {
        log::error( "SoundFx::createBuffer : failed to create sound decoder for '%s'\n", data->identifier() );
        return NULL;
    }

    // ** Create buffer
    switch( data->loading() ) {
    case SoundData::Decode:     if( !data->pcm() ) {
                                    data->setPcm( m_hal->createBuffer( decoder, 1 ) );
                                }
                                return data->pcm();

    case SoundData::Stream:     return m_hal->createBuffer( decoder, 3 );
    case SoundData::LoadToRam:  return m_hal->createBuffer( decoder, 3 );
    }

    return NULL;
}

// ** SoundFx::createDecoder
SoundDecoder* SoundFx::createDecoder( const SoundData *data ) const
{
    SoundDecoder* decoder   = NULL;
    ISoundStream* stream    = m_streamOpener->open( data->uri() );

    if( !stream ) {
        log::error( "SoundFx::createDecoder : failed to open sound stream %s\n", data->uri() );
        return NULL;
    }

    switch( data->loading() ) {
    case SoundData::Decode:     if( !data->pcm() ) {
                                    decoder = m_hal->createSoundDecoder( stream, SoundFormatOgg );
                                } else {
                                    stream->release();
                                }
                                break;

    case SoundData::LoadToRam:  decoder = m_hal->createSoundDecoder( stream->loadToRam(), SoundFormatOgg );
                                stream->release();
                                break;

    case SoundData::Stream:     decoder = m_hal->createSoundDecoder( stream, SoundFormatOgg );
                                break;
    }

    return decoder;
}

// ** SoundFx::event
dcSoundChannelStrong SoundFx::event( const char *identifier )
{
    // ** Get the event by name
    SoundEvent* event = findEventByName( identifier );

    if( event == NULL ) {
        log::error( "SoundFx::event : no such event '%s'\n", identifier );
        return NULL;
    }

    // ** Get the sound label from an event
    const char* label = event->requestIdentifier();

    if( label == NULL ) {
        log::msg( "Event '%s' skipped due to probability\n", identifier );
        return NULL;
    }

    return play( label );
}

// ** SoundFx::play
dcSoundChannelStrong SoundFx::play( const char *identifier )
{
    // ** Get the sound data by name
    SoundData* data = findSoundByName( identifier );

    if( data == NULL ) {
        log::error( "SoundFx::play : no such sound '%s'\n", identifier );
        return NULL;
    }

    // ** Get the sound group
    SoundGroup* group = const_cast<SoundGroup*>( data->group() );

    // ** Ensure we have a free playback slot
    if( group && !group->requestSlot( data ) ) {
        return NULL;
    }

    // ** Cleanup dead channels
    cleanupChannels();

    // ** Create a sound source
    SoundSource* source = createSource( data );

    if( !source ) {
        log::error( "SoundFx::play : failed to start playback for '%s', no sound source created\n", identifier );
        return NULL;
    }

    // ** Create channel
    SoundChannel* channel = DC_NEW SoundChannel( data, source );
    channel->setVolume( data->volumeForSound() );
    channel->resume( data->fadeTime() );
    m_channels.push_back( channel );

    // ** Add sound to a group
    if( group ) {
        group->addSound( channel );
    }

    log::verbose( "Channel for %s created (%x)\n", identifier, channel );

    return channel;
}

// ** SoundFx::playWithParameters
dcSoundChannelStrong SoundFx::playWithParameters( const char *identifier, bool loop, f32 fade )
{
    DC_NOT_IMPLEMENTED;
    return NULL;
}

// ** SoundFx::data
SoundFxInfo SoundFx::data( void ) const
{
    SoundFxInfo result;

    // ** Serialize sounds
    for( Sounds::const_iterator i = m_sounds.begin(), end = m_sounds.end(); i != end; ++i ) {
        result.sounds.push_back( i->second->data() );
    }

    // ** Serialize groups
    for( SoundGroups::const_iterator i = m_groups.begin(), end = m_groups.end(); i != end; ++i ) {
        result.groups.push_back( i->second->data() );
    }

    return result;
}

// ** SoundFx::setData
void SoundFx::setData( const SoundFxInfo& value )
{
    m_sounds.clear();
    m_groups.clear();

    // ** Deserialize groups
    for( u32 i = 0, n = ( u32 )value.groups.size(); i < n; i++ ) {
        const SoundGroupInfo& data = value.groups[i];

        SoundGroup* group = createGroup( data.identifier.c_str() );
        group->setData( data );
    }

    // ** Deserialize sounds
    for( u32 i = 0, n = ( u32 )value.sounds.size(); i < n; i++ ) {
        const SoundDataInfo& data  = value.sounds[i];
        SoundGroup*          group = findGroupByName( data.group.c_str() );

        SoundData* sound = createSound( data.identifier.c_str(), data.uri.c_str(), group );
        sound->setData( data );
    }

    // ** Deserialize events
    for( u32 i = 0, n = ( u32 )value.events.size(); i < n; i++ ) {
        const SoundEventInfo& data = value.events[i];
        SoundEvent* event = createEvent( data.identifier.c_str() );
        event->setData( data );
    }
}

// ** SoundFx::load
bool SoundFx::load( const char *identifier )
{
    DC_NOT_IMPLEMENTED;
    return false;
/*
    // ** Parse JSON
    AutoPtr<io::Stream> file = m_ctx->iDiskFileSystem->openFile( identifier );
    if( file == NULL ) {
        return false;
    }

    AutoPtr<io::ByteBuffer> text = file->toByteBuffer( true );

    JSON json = JSON::Read( ( const char* )text->data() );
    sound::sSoundFxData data;
    data.read( json );

    setData( data );

    u32 size = 0;
    log::verbose( "Loading sounds...\n" );
    for( Sounds::iterator i = m_sounds.begin(), end = m_sounds.end(); i != end; ++i ) {
        if( i->second->loading() == SoundData::Decode ) {
            log::verbose( "Decoding sound '%s'...\n", i->second->identifier() );
            SoundBuffer* buffer = createBuffer( i->second.get() );
            size += buffer->size();
        }
    }
    log::verbose( "%2.2fmb of PCM data loaded\n", f32( size ) / 1024 / 1024 );
*/
    return true;
}

// ** SoundFx::reset
void SoundFx::reset( void )
{
    m_channels.clear();
    m_events.clear();
    m_sounds.clear();
    m_groups.clear();
}

// ** SoundFx::volume
f32 SoundFx::volume( void ) const
{
    log::warn( "SoundFx::volume : not implemented\n" );
    return 0.0f;
}

// ** SoundFx::setVolume
void SoundFx::setVolume( f32 value )
{
    log::warn( "SoundFx::setVolume : not implemented\n" );
}

// ** SoundFx::pitch
f32 SoundFx::pitch( void ) const
{
    log::warn( "SoundFx::pitch : not implemented\n" );
    return 0.0f;
}

// ** SoundFx::setPitch
void SoundFx::setPitch( f32 value )
{
    log::warn( "SoundFx::setPitch : not implemented\n" ); 
}

// ** SoundGroups& SoundFx::groups
const SoundGroups& SoundFx::groups( void ) const
{
    return m_groups;
}

// ** Sounds& SoundFx::sounds
const Sounds& SoundFx::sounds( void ) const
{
    return m_sounds;
}

// ** SoundFx::update
void SoundFx::update( f32 dt )
{
    // ** Update channels
    for( u32 i = 0; i < ( u32 )m_channels.size(); i++ ) {
        m_channels[i]->update( dt );
    }

    // ** Cleanup stopped channels
    cleanupChannels();

    // ** Update sound groups
    for( SoundGroups::iterator i = m_groups.begin(), end = m_groups.end(); i != end; ++i ) {
        i->second->update();
    }
}

// ** SoundFx::cleanupChannels
void SoundFx::cleanupChannels( void )
{
    for( u32 i = 0; i < ( u32 )m_channels.size(); i++ ) {
        if( !m_channels[i]->isStopped() ) {
            continue;
        }

        // ** Remove channel from a list
        m_channels.erase( m_channels.begin() + i );
        i--;
    }
}

} // namespace sound

DC_END_DREEMCHEST