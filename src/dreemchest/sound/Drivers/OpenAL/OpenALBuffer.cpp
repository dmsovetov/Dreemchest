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

#include "OpenALBuffer.h"
#include "OpenALSource.h"
#include "OpenAL.h"

#include "../../Decoders/SoundDecoder.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** OpenALBuffer::OpenALBuffer
OpenALBuffer::OpenALBuffer( SoundDecoderPtr data, u32 chunks, u32 pcmSize ) : SoundBuffer( data, chunks )
{
    // ** Resize buffer id array
    m_buffers.resize( m_chunks );

    // ** Initialize variables
    m_pcmSize = pcmSize;
    m_pcm     = DC_NEW u8[pcmSize];
    m_format  = OpenAL::soundSampleFormat( data->format() );

    // ** Generate buffers
    alGenBuffers( m_chunks, &m_buffers[0] );
    for( u32 i = 0; i < m_chunks; i++ ) {
        DC_BREAK_IF( !alIsBuffer( m_buffers[i] ) );
    }

    // ** Fill buffers with data
    m_decoder->seek( 0 );
    for( u32 i = 0; i < m_chunks; i++ ) {
        readSoundDecoder( m_buffers[i], m_pcmSize );
    }

    // ** If the buffer is not streamed - cleanup data right now
    if( chunks == 1 ) {
        DC_DELETE_ARRAY( m_pcm );
        m_decoder = SoundDecoderPtr();
    }

    OpenAL::dumpErrors( "OpenALBuffer::OpenALBuffer" );
}

OpenALBuffer::~OpenALBuffer( void )
{
    DC_DELETE_ARRAY( m_pcm );
    alDeleteBuffers( m_buffers.size(), &m_buffers[0] );

    OpenAL::dumpErrors( "OpenALBuffer::~OpenALBuffer" );
}

// ** OpenALBuffer::attachToSource
void OpenALBuffer::attachToSource( SoundSource* target )
{
    DC_BREAK_IF( target == NULL );

    ALuint source = static_cast<OpenALSource*>( target )->m_id;

    alSourceQueueBuffers( source, m_buffers.size(), &m_buffers[0] );
    OpenAL::dumpErrors( "OpenALBuffer::attachToSource" );
}

// ** OpenALBuffer::detachFromSource
void OpenALBuffer::detachFromSource( SoundSource* target )
{
    DC_BREAK_IF( target == NULL );

    OpenAL::flushErrors();

    ALint  queued, state;
    ALuint bufferId;
    ALuint source = static_cast<OpenALSource*>( target )->m_id;

    alGetSourcei( source, AL_SOURCE_STATE, &state );
    DC_BREAK_IF( state != AL_STOPPED );

    alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );
    while( queued-- ) {
        alSourceUnqueueBuffers( source, 1, &bufferId );
        OpenAL::dumpErrors( "OpenALBuffer::detachFromSource - alSourceUnqueueBuffers" );
    }
}

// ** OpenALBuffer::readSoundDecoder
bool OpenALBuffer::readSoundDecoder( ALuint target, u32 size )
{
    DC_BREAK_IF( size <= 0 );

    size = m_decoder->read( m_pcm, size );
    if( size <= 0 ) {
        return false;
    }

    alBufferData( target, m_format, ( void* )m_pcm, size, m_decoder->rate() );

    return true;
}

// ** OpenALBuffer::updateStream
bool OpenALBuffer::updateStream( SoundSource* target, bool isLooped )
{
    DC_BREAK_IF( target == NULL );

    if( chunks() <= 1 ) {
        return false;
    }

    OpenAL::flushErrors();

    ALuint source = static_cast<OpenALSource*>( target )->m_id;

    // ** Ensure the streamed sound is not looped
    ALint state;
    alGetSourcei( source, AL_LOOPING, &state );
    DC_BREAK_IF( state == 1 );

    // ** Get total processed buffers
    ALint processed = 0;
    alGetSourcei( source, AL_BUFFERS_PROCESSED, &processed );

    // ** Fill the processe buffers with data
    while( processed-- ) {
        ALuint buffer = 0;

        // ** Remove buffer from a queue
        alSourceUnqueueBuffers( source, 1, &buffer );
        if( buffer == 0 ) {
            OpenAL::dumpErrors( "OpenALBuffer::updateStream - alSourceUnqueueBuffers" );
            continue;
        }

        // ** Fill a buffer with data and put it to a queue
        if( readSoundDecoder( buffer, m_pcmSize ) ) {
            alSourceQueueBuffers( source, 1, &buffer );
            OpenAL::dumpErrors( "OpenALBuffer::updateStream - alSourceQueueBuffers" );
            continue;
        }

        // ** No more data - rewind
        m_decoder->seek( 0 );

        // ** Handle the looping
        if( !isLooped ) {
            return true;
        }

        // ** Fill buffer with data
        if( readSoundDecoder( buffer, m_pcmSize ) ) {
            alSourceQueueBuffers( source, 1, &buffer );
            OpenAL::dumpErrors( "OpenALBuffer::updateStream - alSourceQueueBuffers" );
            continue;
        }
        
        break;
    }

    return false;
}

} // namespace Sound

DC_END_DREEMCHEST