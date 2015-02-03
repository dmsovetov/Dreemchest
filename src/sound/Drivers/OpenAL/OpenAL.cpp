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

#include    "OpenAL.h"

#include    "OpenALSource.h"
#include    "OpenALBuffer.h"

#include    "../../Decoders/SoundDecoder.h"

#define     MAX_PCM_SIZE    10024

namespace dreemchest {

namespace sound {

// ** OpenAL::OpenAL
OpenAL::OpenAL( void )
{
    float lp[] = { 0.0f, 0.0f,  0.0f };
    float lv[] = { 0.0f, 0.0f,  0.0f };
    float lo[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };

    log::verbose( "Creating OpenAL device... " );
    m_device = alcOpenDevice( NULL );
    log::verbose( "succeeded\n" );

    log::verbose( "Creating OpenAL context... " );
    m_context = alcCreateContext( m_device, NULL );
    log::verbose( "succeeded\n" );

    alcMakeContextCurrent( m_context );

    alListenerfv( AL_POSITION,    lp );
    alListenerfv( AL_VELOCITY,    lv );
    alListenerfv( AL_ORIENTATION, lo );

    log::msg( "AL_VERSION:    %s\n", alGetString( AL_VERSION ) );
    log::msg( "AL_RENDERER:   %s\n", alGetString( AL_RENDERER ) );
    log::msg( "AL_VENDOR:     %s\n", alGetString( AL_VENDOR ) );
    log::msg( "AL_EXTENSIONS: %s\n", alGetString( AL_EXTENSIONS ) );
}

OpenAL::~OpenAL( void )
{
    alcMakeContextCurrent( NULL );
    alcDestroyContext( m_context );
    alcCloseDevice( m_device );
}

// ** OpenAL::maxSources
ALuint OpenAL::maxSources( void )
{
#ifdef DC_PLATFORM_IOS
    return 32;
#endif

    return 64;
}

// ** OpenAL::soundSampleFormat
ALuint OpenAL::soundSampleFormat( SoundSampleFormat format )
{
    switch( format ) {
    case SoundSampleMono8:      return AL_FORMAT_MONO8;
    case SoundSampleMono16:     return AL_FORMAT_MONO16;
    case SoundSampleStereo8:    return AL_FORMAT_STEREO8;
    case SoundSampleStereo16:   return AL_FORMAT_STEREO16;
    }

    return 0;
}

// ** OpenAL::flushErrors
void OpenAL::flushErrors( void )
{
    while( alGetError() ) {
        // ** Do nothing
    }
}

// ** OpenAL::dumpErrors
void OpenAL::dumpErrors( const char *label )
{
    ALenum error = AL_NO_ERROR;

    do {
        error = alGetError();

        switch( error ) {
        case AL_INVALID_NAME:       log::error( "%s : invalid name\n", label );         break;
        case AL_INVALID_ENUM:       log::error( "%s : invalid enum\n", label );         break;
        case AL_INVALID_VALUE:      log::error( "%s : invalid value\n", label );        break;
        case AL_INVALID_OPERATION:  log::error( "%s : invalid operation\n", label );    break;
        case AL_OUT_OF_MEMORY:      log::error( "%s : out of memory\n", label );        break;
        }
    } while( error != AL_NO_ERROR );
}

// ** OpenAL::createSource
SoundSource* OpenAL::createSource( void )
{
    return DC_NEW OpenALSource;
}

// ** OpenAL::createBuffer
SoundBuffer* OpenAL::createBuffer( SoundDecoder* decoder, u32 chunks )
{
    DC_BREAK_IF( decoder == NULL );
    return DC_NEW OpenALBuffer( decoder, chunks, chunks == 1 ? decoder->size() : 16536 );
}

} // namespace sound
    
} // namespace dreemchest