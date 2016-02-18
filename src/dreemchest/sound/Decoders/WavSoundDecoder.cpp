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

#include "WavSoundDecoder.h"
#include "../SoundStream.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** WavSoundDecoder::~WavSoundDecoder
WavSoundDecoder::~WavSoundDecoder( void )
{
}

// ** WavSoundDecoder::Open
bool WavSoundDecoder::open( ISoundStreamPtr stream )
{
    SoundDecoder::open( stream );

    // ** struct sWaveID
    struct sWaveID {
        char        fileID[4];
        u32         fileSize;
        char        fileType[4];
    };

    sWaveFileHeader    waveID;
    sWaveFileFormat    waveFormat;

    m_stream->read( &waveID, sizeof( sWaveID ) );

    if( waveID.fileID[0] != 'R' ||
        waveID.fileID[1] != 'I' ||
        waveID.fileID[2] != 'F' ||
        waveID.fileID[3] != 'F' ) {
        return false;
    }

    if( waveID.fileType[0] != 'W' ||
        waveID.fileType[1] != 'A' ||
        waveID.fileType[2] != 'V' ||
        waveID.fileType[3] != 'E' ) {
        return false;
    }

    u32 formatLength = 0;

    m_stream->setPosition( 4, ISoundStream::SeekCur ); // ** Skip 'fmt ' string
    m_stream->read( &formatLength, sizeof( u32 ) );
    m_stream->read( &waveFormat, sizeof( sWaveFileFormat ) );

    m_stream->read( &m_pcmSize, sizeof( u32 ) );
    m_pcmDataOffset = m_stream->position();

    // ** Initialize format and rate properties
    m_rate = waveFormat.nSamplesPerSec;

    if( waveFormat.nChannels > 2 ) {
        LogWarning( "wav", "unsupported sound format, file contains %d channels\n", waveFormat.nChannels );
        return false;
    }

    if( waveFormat.wBitsPerSample > 16 ) {
        LogWarning( "wav", "unsupported sound format, sound sample size is %d only 8 and 16 are supported\n", waveFormat.wBitsPerSample );
        return false;
    }

    switch( waveFormat.nChannels ) {
    case 1: m_format = ( waveFormat.wBitsPerSample == 16 ) ? SoundSampleMono16   : SoundSampleMono8;    break;
    case 2: m_format = ( waveFormat.wBitsPerSample == 16 ) ? SoundSampleStereo16 : SoundSampleStereo8;    break;
    }

    return true;
}

// ** WavSoundDecoder::size
u32 WavSoundDecoder::size( void ) const
{
    return m_pcmSize;
}

// ** WavSoundDecoder::read
u32 WavSoundDecoder::read( u8 *buffer, u32 size )
{
    return m_stream->read( buffer, size );
}

// ** WavSoundDecoder::seek
void WavSoundDecoder::seek( u32 pos )
{
    m_stream->setPosition( pos );
}

} // namespace Sound

DC_END_DREEMCHEST