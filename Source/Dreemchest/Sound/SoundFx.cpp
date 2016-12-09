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

#include "SoundFx.h"

#include "SoundChannel.h"
#include "SoundGroup.h"
#include "SoundData.h"
#include "SoundEvent.h"
#include "SoundStream.h"

#include "Decoders/SoundDecoder.h"
#include "Drivers/SoundSource.h"
#include "Drivers/SoundBuffer.h"
#include "Drivers/SoundEngine.h"

#ifdef OPENAL_FOUND
    #include "Drivers/OpenAL/OpenAL.h"
#endif    /*    #ifdef OPENAL_FOUND    */

DC_BEGIN_DREEMCHEST

namespace Sound {

// ------------------------------------------------------ SoundFx ------------------------------------------------------ //

// ** SoundFx::SoundFx
SoundFx::SoundFx( SoundHal hal, IStreamOpenerPtr streamOpener )
    : m_hal( NULL )
    , m_volume( 1.0f )
    , m_pitch( 1.0f )
    , m_streamOpener( streamOpener.valid() ? streamOpener : DC_NEW StandardStreamOpener )
    , m_distanceModel( InverseDistanceAttenuation )
{
    switch( hal ) {
    case None:      m_hal = NULL;           break;
    case Default:   
                    #ifdef OPENAL_FOUND
                        m_hal = DC_NEW OpenAL;
                        if( !m_hal->initialize() ) {
                            LogError( "sfx", "%s", "failed to create OpenAL backend, fallback to default\n" );
                            m_hal = DC_NEW SoundEngine;
                        }
                    #else
                        LogError( "sfx", "%s", "the default sound HAL is OpenAL, but library compiled without OpenAL\n" );
                    #endif  /*  #ifdef OPENAL_FOUND */
                    break;
    }
}

SoundFx::~SoundFx( void )
{
    reset();
}

// ** SoundFx::create
SoundFxPtr SoundFx::create( SoundHal hal, IStreamOpenerPtr streamOpener )
{
    return SoundFxPtr( DC_NEW SoundFx( hal, streamOpener ) );
}

// ** SoundFx::createGroup
SoundGroupWPtr SoundFx::createGroup( CString identifier )
{
    NIMBLE_ABORT_IF( identifier == NULL, "invalid identifier" );

    if( findGroupByName( identifier ).valid() ) {
        LogWarning( "sfx", "failed to create group, group with a same name '%s' found\n", identifier );
        return NULL;
    }

    SoundGroup* group = DC_NEW SoundGroup( identifier );
    String64    hash( identifier );

    m_groups[hash] = group;

    return group;
}

// ** SoundFx::createSound
SoundDataWPtr SoundFx::createSound( CString identifier, CString uri, SoundGroupWPtr group )
{
    NIMBLE_ABORT_IF( identifier == NULL, "invalid identifier" );
    NIMBLE_ABORT_IF( uri == NULL, "invalid URI" );

    // Ensure we don't have a sound with a same name.
    if( findSoundByName( identifier ).valid() ) {
        LogWarning( "sfx", "failed to create sound, sound with a same name '%s' found\n", identifier );
        return NULL;
    }

    // ** Create a sound data instance
    SoundData* sound = DC_NEW SoundData( this, identifier, uri, group );
    String64   hash( identifier );

    m_sounds[hash] = sound;

    return sound;
}

// ** SoundFx::createEvent
SoundEventWPtr SoundFx::createEvent( CString identifier )
{
    NIMBLE_ABORT_IF( identifier == NULL, "invalid identifier" );

    if( findEventByName( identifier ).valid() ) {
        LogWarning( "sfx", "failed to create event, sound with a same name '%s' found\n", identifier );
        return NULL;
    }

    SoundEvent* event = DC_NEW SoundEvent( identifier );
    String64    hash( identifier );

    m_events[hash] = event;

    return event;
}

// ** SoundFx::distanceModel
DistanceModel SoundFx::distanceModel( void ) const
{
    return m_distanceModel;
}

// ** SoundFx::setDistanceModel
void SoundFx::setDistanceModel( DistanceModel value )
{
    m_distanceModel = value;
    m_hal->setDistanceModel( value );
}

// ** SoundFx::findSoundByName
SoundDataWPtr SoundFx::findSoundByName( CString identifier )
{
    Sounds::iterator i = m_sounds.find( String64( identifier ) );

    if( i != m_sounds.end() ) {
        return i->second;
    }

    return SoundDataWPtr();
}

// ** SoundFx::removeSoundByName
void SoundFx::removeSoundByName( CString identifier )
{
    NIMBLE_ABORT_IF( identifier == NULL, "invalid identifier" );
    m_sounds.erase( String64( identifier ) );
}

// ** SoundFx::renameSound
bool SoundFx::renameSound( CString identifier, CString newName )
{
    // Sound with a same name exists
    if( findSoundByName( newName ).valid() ) {
        return false;
    }

    // Get the sound
    SoundDataPtr sound = findSoundByName( identifier );

    if( sound == NULL ) {
        return false;
    }

    // Rename sound
    removeSoundByName( identifier );

    String64 hash( newName );
    sound->setIdentifier( newName );

    m_sounds[hash] = sound;

    return true;
}

// ** SoundFx::findGroupByName
SoundGroupWPtr SoundFx::findGroupByName( CString identifier )
{
    SoundGroups::iterator i = m_groups.find( String64( identifier ) );

    if( i != m_groups.end() ) {
        return i->second;
    }

    return SoundGroupWPtr();
}

// ** SoundFx::removeGroupByName
void SoundFx::removeGroupByName( CString identifier )
{
    NIMBLE_ABORT_IF( identifier == NULL, "invalid identifier" );
    m_groups.erase( String64( identifier ) );
}

// ** SoundFx::renameGroup
bool SoundFx::renameGroup( CString identifier, CString newName )
{
    // Group with a same name exists
    if( findGroupByName( newName ).valid() ) {
        return false;
    }

    // Get the sound
    SoundGroupPtr group = findGroupByName( identifier );

    if( group == NULL ) {
        return false;
    }

    // Rename group
    removeGroupByName( identifier );

    String64 hash = String64( newName );
    group->setIdentifier( newName );
    
    m_groups[hash] = group;
    
    return true;
}

// ** SoundFx::findEventByName
SoundEventWPtr SoundFx::findEventByName( CString identifier )
{
    SoundEvents::iterator i = m_events.find( String64( identifier ) );

    if( i != m_events.end() ) {
        return i->second;
    }

    return SoundEventWPtr();
}

// ** SoundFx::createSource
SoundSourcePtr SoundFx::createSource( SoundDataWPtr data )
{
    NIMBLE_ABORT_IF( !data.valid(), "invalid sound data" );
    
    if( m_hal == NULL ) {
        LogError( "sfx", "failed to create sound source for '%s', no HAL created.\n", data->identifier() );
        return NULL;
    }

    // Create sound source
    SoundSourcePtr source = m_hal->createSource();
    if( !source.valid() ) {
        LogError( "sfx", "failed to create sound source for '%s'\n", data->identifier() );
        return NULL;
    }

    // Inherit sound source properties from sound data
    source->setBuffer( createBuffer( data ) );
    source->setPitch( data->pitchForSound() );
    source->setLooped( data->isLooped() );

    return source;
}

// ** SoundFx::createBuffer
SoundBufferPtr SoundFx::createBuffer( SoundDataWPtr data )
{
    NIMBLE_ABORT_IF( !data.valid(), "invalid sound data" );

    if( !m_hal.valid() ) {
        LogError( "sfx", "failed to create sound buffer for '%s', no HAL created.\n", data->identifier() );
        return NULL;
    }

    // ** Create sound decoder
    SoundDecoderPtr decoder = createDecoder( data );
    if( !decoder.valid() && !data->pcm().valid() ) {
        LogError( "sfx", "failed to create sound decoder for '%s'\n", data->identifier() );
        return NULL;
    }

    // ** Create buffer
    switch( data->loading() ) {
    case SoundData::Decode:     if( !data->pcm().valid() ) {
                                    data->setPcm( m_hal->createBuffer( decoder, 1 ) );
                                }
                                return data->pcm();

    case SoundData::Stream:     return m_hal->createBuffer( decoder, 3 );
    case SoundData::LoadToRam:  return m_hal->createBuffer( decoder, 3 );
    }

    return NULL;
}

// ** SoundFx::createDecoder
SoundDecoderPtr SoundFx::createDecoder( SoundDataWPtr data )
{
    SoundDecoderPtr decoder;
    ISoundStreamPtr stream = m_streamOpener->open( data->uri() );

    if( !stream.valid() ) {
        LogError( "sfx", "failed to open sound stream %s\n", data->uri() );
        return SoundDecoderPtr();
    }

    switch( data->loading() ) {
    case SoundData::Decode:     if( !data->pcm().valid() ) {
                                    decoder = m_hal->createSoundDecoder( stream, data->format() );
                                } else {
                                //    stream->release();
                                }
                                break;

    case SoundData::LoadToRam:  decoder = m_hal->createSoundDecoder( stream->loadToRam(), data->format() );
                                //stream->release();
                                break;

    case SoundData::Stream:     decoder = m_hal->createSoundDecoder( stream, data->format() );
                                break;
    }

    return decoder;
}

// ** SoundFx::setListenerPosition
void SoundFx::setListenerPosition( const Vec3& value )
{
    m_hal->setPosition( value );
}

// ** SoundFx::event
SoundChannelPtr SoundFx::event( CString identifier )
{
    // Find the event by name
    SoundEventWPtr event = findEventByName( identifier );

    if( event == NULL ) {
        LogError( "sfx", "no such event '%s'\n", identifier );
        return NULL;
    }

    // ** Get the sound label from an event
    const char* label = event->requestIdentifier();

    if( label == NULL ) {
        LogDebug( "sfx", "event '%s' skipped due to probability\n", identifier );
        return NULL;
    }

    return play( label );
}

// ** SoundFx::play
SoundChannelPtr SoundFx::play( CString identifier )
{
    // Find the sound data by name
    SoundDataWPtr data = findSoundByName( identifier );

    if( data == NULL ) {
        LogError( "sfx", "no such sound '%s'\n", identifier );
        return NULL;
    }

    // Get the sound group
    SoundGroupWPtr group = data->group();

    // Ensure we have a free playback slot
    if( group.valid() && !group->requestSlot( data ) ) {
        return NULL;
    }

    // Cleanup dead channels
    cleanupChannels();

    // Create a sound source
    SoundSourcePtr source = createSource( data );

    if( !source.valid() ) {
        LogError( "sfx", "failed to start playback for '%s', no sound source created\n", identifier );
        return NULL;
    }

    // Calculate the sound fade time
    f32 fadeTime = data->fadeTime();

    if( fadeTime < 0.0f ) {
        fadeTime = group->fadeTime();
    }

    // Create channel
    SoundChannel* channel = DC_NEW SoundChannel( data, source );
    channel->setVolume( data->volumeForSound() );
    channel->resume( fadeTime );
    m_channels.push_back( channel );

    // Add sound to a group
    if( group.valid() ) {
        group->addSound( channel );
    }

    LogVerbose( "sfx", "channel for %s created (%x)\n", identifier, channel );

    return channel;
}

// ** SoundFx::playWithParameters
SoundChannelPtr SoundFx::playWithParameters( CString identifier, bool loop, f32 fade )
{
    NIMBLE_NOT_IMPLEMENTED;
    return SoundChannelPtr();
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

        SoundGroupWPtr group = createGroup( data.identifier.c_str() );
        group->setData( data );
    }

    // ** Deserialize sounds
    for( u32 i = 0, n = ( u32 )value.sounds.size(); i < n; i++ ) {
        const SoundDataInfo& data  = value.sounds[i];
        SoundGroupWPtr       group = findGroupByName( data.group.c_str() );

        SoundDataWPtr sound = createSound( data.identifier.c_str(), data.uri.c_str(), group );
        sound->setData( data );
    }

    // ** Deserialize events
    for( u32 i = 0, n = ( u32 )value.events.size(); i < n; i++ ) {
        const SoundEventInfo& data = value.events[i];
        SoundEventWPtr event = createEvent( data.identifier.c_str() );
        event->setData( data );
    }
}

// ** SoundFx::load
bool SoundFx::load( const char *identifier )
{
    NIMBLE_NOT_IMPLEMENTED;
    return false;
/*
    // ** Parse JSON
    UPtr<io::Stream> file = m_ctx->iDiskFileSystem->openFile( identifier );
    if( file == NULL ) {
        return false;
    }

    UPtr<io::ByteBuffer> text = file->toByteBuffer( true );

    JSON json = JSON::Read( ( const char* )text->data() );
    sound::sSoundFxData data;
    data.read( json );

    setData( data );

    u32 size = 0;
    LogDebug( "Loading sounds...\n" );
    for( Sounds::iterator i = m_sounds.begin(), end = m_sounds.end(); i != end; ++i ) {
        if( i->second->loading() == SoundData::Decode ) {
            LogDebug( "Decoding sound '%s'...\n", i->second->identifier() );
            SoundBuffer* buffer = createBuffer( i->second.get() );
            size += buffer->size();
        }
    }
    LogDebug( "%2.2fmb of PCM data loaded\n", f32( size ) / 1024 / 1024 );
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
    return m_volume;
}

// ** SoundFx::setVolume
void SoundFx::setVolume( f32 value )
{
    NIMBLE_BREAK_IF( value < 0.0f, "volume could not have a negative value" );
    m_volume = value;
    m_hal->setVolume( volume() );
}

// ** SoundFx::pitch
f32 SoundFx::pitch( void ) const
{
    return m_pitch;
}

// ** SoundFx::setPitch
void SoundFx::setPitch( f32 value )
{
    m_pitch = value;
    m_hal->setPitch( pitch() );
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

} // namespace Sound

DC_END_DREEMCHEST
