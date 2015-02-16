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

#include    "Mp3SoundDecoder.h"
#include    "../OpenAL/OpenALSoundEngine.h"
#include    "../../IFileSystem.h"
#include    "../../File.h"

DC_BEGIN_DREEMCHEST

namespace sound {

// ** Mp3SoundDecoder::open
bool Mp3SoundDecoder::open( const char *fileName )
{
    SoundDecoder::Open( fileName );

    mad_synth_init( &mp3Synth  );
    mad_stream_init( &mp3Stream );
    mad_frame_init( &mp3Frame  );

    bufferPos = 0;

    if( !ReadFrame() ) {
        return false;
    }

    numChannels        = ( mp3Frame.header.mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2 );
    format            = ( numChannels == 1 ) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    rate            = mp3Frame.header.samplerate;
    bitRate            = mp3Frame.header.bitrate;
    resumeData        = NULL;
    resumeSize        = 0;
//    bitsPerSample    = 16;

    u32 size = stream->Length() * 8 / bitRate * numChannels * rate * 2;

    return true;
}

// ** Mp3SoundDecoder::close
void Mp3SoundDecoder::close( void )
{
    mad_synth_finish( &mp3Synth  );
    mad_stream_finish( &mp3Stream );
    mad_frame_finish( &mp3Frame  );

    stream->Release();
}

inline u32 resample ( mad_fixed_t sample )
{
    sample += 1 << (MAD_F_FRACBITS - 16);

    if ( sample >= MAD_F_ONE )
        sample = MAD_F_ONE - 1;
    else
    if ( sample < -MAD_F_ONE )
        sample = -MAD_F_ONE;

    return (u32)(sample >> (MAD_F_FRACBITS  - 15));
}

// ** Mp3SoundDecoder::Read
u32 Mp3SoundDecoder::Read( u8 *buffer, u32 size )
{
    u32 bytesRead = 0;

    while( bytesRead < size ) {
        if( resumeData ) {
            ResumeFrame( buffer );
            bytesRead += resumeSize * numChannels * 2;
            resumeSize = 0;
        }

        mad_synth_frame( &mp3Synth, &mp3Frame );

        mad_pcm     *pcm   = &mp3Synth.pcm;
        mad_fixed_t *left  = pcm->samples[0];
        mad_fixed_t *right = pcm->samples[1];
        u16      *data  = ( u16* )(buffer + bytesRead);

        size_t len      = pcm->length;
        size_t nBytes = len * numChannels * 2;

        if( bytesRead + nBytes >= size ) {
            u32 d = size - bytesRead;
            len = d / (numChannels * 2);

            resumeData = ( u16* )(buffer + bytesRead + d);
            resumeSize = pcm->length - len;
        }

        for( size_t i = 0; i < len; i++ ) {
            *data++ = resample( *left++ );

            if( numChannels == 2 ) {
                *data++ = resample( *right++ );
            }
        }

        if( resumeSize ) {
            resumeData    = data;
            resumeLeft    = left;
            resumeRight = right;
        }

        bytesRead += len * numChannels * 2;

        if( ReadFrame() == 0 ) {
            return bytesRead;
        }
    }

    return bytesRead;
}

// ** Mp3SoundDecoder::ResumeFrame
void Mp3SoundDecoder::ResumeFrame( u8 *buffer )
{
    u16 *data = ( u16* )buffer;

    for( size_t i = 0; i < resumeSize; i++ ) {
        *data++ = resample( *resumeLeft++ );

        if( numChannels == 2 ) {
            *data++ = resample( *resumeRight++ );
        }
    }

    resumeData = NULL;
}

// ** Mp3SoundDecoder::Seek
void Mp3SoundDecoder::Seek( u32 pos )
{
    stream->Seek( pos, SO_SET );
    ReadFrame();
}

// ** Mp3SoundDecoder::ReadFrame
u32 Mp3SoundDecoder::ReadFrame( void )
{
    while( true ) {
        u32 ret = stream->Read( buffer + bufferPos, MP3_BUFFER_SIZE - bufferPos, 1 );

        if( ret <= 0 ) {
            break;
        }

        bufferPos += ret;

        mad_stream_buffer( &mp3Stream, buffer, bufferPos );

        ret = mad_frame_decode( &mp3Frame, &mp3Stream );

        if( ret == -1 ) {
            ret = mad_stream_sync( &mp3Stream );
            ret = mad_frame_decode( &mp3Frame, &mp3Stream );
        }

        if( mp3Stream.next_frame != NULL ) {
            u32    length = buffer + bufferPos - ( u8* )mp3Stream.next_frame;

            // copy next frame data to start of buffer
            memmove( buffer, mp3Stream.next_frame, length );
            bufferPos = length;
        }

        if( ret == 0 ) {
            return 1;
        }
        if( mp3Stream.error == MAD_ERROR_BUFLEN ) {
            continue;
        }
    }

    return 0;
}

} // namespace sound

DC_END_DREEMCHEST