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

#include <sound/Sound.h>
#include <threads/Threads.h>

DC_USE_DREEMCHEST
using namespace sound;

SoundDataInfo createSound( const char* identifier, const char* uri, const char* group, bool looped = false, SoundData::LoadingFlags loading = SoundData::Decode )
{
    SoundDataInfo snd;

    snd.fadeTime        = 1.0f;
    snd.isLooped        = looped;
    snd.volume          = 1.0f;
    snd.volumeModifier  = Vec2( 1.0f, 1.0f );
    snd.uri             = uri;
    snd.identifier      = identifier;
    snd.group           = group;
    snd.type            = 0;
    snd.loading         = loading;
    snd.pitch           = 1.0f;
    snd.pitchModifier   = Vec2( 1.0f, 1.0f );
    snd.priority        = 0;

    return snd;
}

SoundGroupInfo createGroup( const char* identifier, SoundGroup::Behaviour behaviour, int slots, float fadeTime = 0.0f )
{
    SoundGroupInfo group;

    group.identifier = identifier;
    group.behaviour  = behaviour;
    group.slotCount  = slots;
    group.threshold  = 0;
    group.fadeTime   = fadeTime;

    return group;
}

SoundEventInfo createEvent( const char* identifier, SoundEvent::EventType type, const char** sounds, int count )
{
    SoundEventInfo ev;

    ev.identifier   = identifier;
    ev.type         = type;
    ev.probability  = 100;

    for( int i = 0; i < count; i++ ) {
        ev.sounds.push_back( sounds[i] );
    }

    return ev;
}

void wait( u32 amount )
{
#ifdef DC_THREADS
    thread::Thread::sleep( 50 );
#else
    log::warn( "Error: no Thread::sleep method, built with no threading support\n" );
#endif
}

int testSound( int argc, char **argv )
{
    sound::log::setStandardHandler();
    
    dcSoundFx sfx = new sound::SoundFx;

    // ** Initialize sound data
    sound::SoundFxInfo info;

    info.sounds.push_back( createSound( "star1", "sound/star1.ogg", "sfx" ) );
    info.sounds.push_back( createSound( "star2", "sound/star2.ogg", "sfx" ) );
    info.sounds.push_back( createSound( "star3", "sound/star3.ogg", "sfx" ) );
    info.sounds.push_back( createSound( "music", "sound/music.ogg", "bgm", false, SoundData::Stream ) );

    info.groups.push_back( createGroup( "sfx", SoundGroup::ReplaceOldest, 3 ) );
    info.groups.push_back( createGroup( "bgm", SoundGroup::ReplaceOldest, 1, 1000.0f ) );

    const char* sounds[] = { "star1", "star2", "star3" };
    info.events.push_back( createEvent( "star", SoundEvent::Random, sounds, 3 ) );

    // ** Load data
    sfx->setData( info );

    // -------------------------------- Music -------------------------------- //
    printf( "Start music playback" );

    dcSoundChannelStrong musicChannel = sfx->play( "music" );

    while( musicChannel->isPlaying() ) {
        sfx->update( 0.01f );
        wait( 50 );
    }

    printf( "Music playback ended\n" );

    // -------------------------------- SFX -------------------------------- //
    printf( "Now playing sound FX\n" );

    for( int i = 0; i < 3; i++ ) {
        dcSoundChannelStrong sfxChannel = sfx->play( sounds[i] );

        while( sfxChannel->isPlaying() ) {
            sfx->update( 0.01f );
            wait( 1 );
        }
    }

    // -------------------------------- Events -------------------------------- //
    printf( "Now playing sound events.\n" );

    for( int i = 0; i < 10; i++ ) {
        dcSoundChannelStrong eventChannel = sfx->event( "star" );

        while( eventChannel->isPlaying() ) {
            sfx->update( 0.01f );
            wait( 1 );
        }
    }

    return 0;
}