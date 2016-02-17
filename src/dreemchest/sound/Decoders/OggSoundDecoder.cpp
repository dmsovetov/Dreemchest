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

#include "OggSoundDecoder.h"
#include "../SoundStream.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** OggSoundDecoder::~OggSoundDecoder
OggSoundDecoder::~OggSoundDecoder( void )
{
    close();
}

// ** OggSoundDecoder::open
bool OggSoundDecoder::open( ISoundStreamPtr stream )
{
    ov_callbacks    cb;

    cb.close_func = OggSoundDecoder::closeOgg;
    cb.read_func  = OggSoundDecoder::readOgg;
    cb.seek_func  = OggSoundDecoder::seekOgg;
    cb.tell_func  = OggSoundDecoder::tellOgg;

    m_vorbisFile    = DC_NEW OggVorbis_File;

    SoundDecoder::open( stream );

    if( ov_open_callbacks( m_stream.get(), m_vorbisFile, NULL, -1, cb ) < 0 ) {
        LogError( "OggSoundDecoder::Open : ov_open_callbacks failed\n" );
        return false;
    }

    m_vorbisComment = ov_comment( m_vorbisFile, -1 );
    m_vorbisInfo    = ov_info( m_vorbisFile, -1 );
    m_rate          = m_vorbisInfo->rate;
    m_format        = ( m_vorbisInfo->channels == 1 ) ? SoundSampleMono16 : SoundSampleStereo16;

    return true;
}

// ** OggSoundDecoder::close
void OggSoundDecoder::close( void )
{
    if( m_vorbisFile ) {
        ov_clear( m_vorbisFile );
        delete m_vorbisFile;
    }

    SoundDecoder::close();
}

// ** OggSoundDecoder::size
u32 OggSoundDecoder::size( void ) const
{
    return ov_pcm_total( m_vorbisFile, -1 ) * 4;
}

// ** OggSoundDecoder::read
u32 OggSoundDecoder::read( u8 *buffer, u32 size )
{
    s32     currentSection      = 0;
    u32    totalRet = 0, ret   = 0;

    while( totalRet < size ) {
        ret = ov_read( m_vorbisFile, ( char* )buffer + totalRet, size - totalRet, 0, 2, 1, &currentSection );

        if( ret == 0 ) { // eof
            break;
        }
        else {
            totalRet += ret;
        }
    }

    return totalRet;
}

// ** OggSoundDecoder::seek
void OggSoundDecoder::seek( u32 pos )
{
    ov_pcm_seek( m_vorbisFile, pos );
}

// ** OggSoundDecoder::readOgg
size_t OggSoundDecoder::readOgg( void *ptr, size_t size, size_t nmemb, void *source )
{
    return reinterpret_cast<ISoundStream*>( source )->read( ptr, ( u32 )(size * nmemb) );
}

// ** OggSoundDecoder::seekOgg
s32 OggSoundDecoder::seekOgg( void *source, ogg_int64_t offset, s32 origin )
{
    ISoundStream*            file = reinterpret_cast<ISoundStream*>( source );
    ISoundStream::SeekOrigin so;

    switch( origin ) {
    case SEEK_SET:  so = ISoundStream::SeekSet; break;
    case SEEK_CUR:  so = ISoundStream::SeekCur; break;
    case SEEK_END:  so = ISoundStream::SeekEnd; break;
    default:        return -1;
    }

    file->setPosition( ( u32 )offset, so );

    return 0;
}

// ** OggSoundDecoder::tellOgg
long OggSoundDecoder::tellOgg( void *source ) {
    return reinterpret_cast<ISoundStream*>( source )->position();
}

// ** OggSoundDecoder::closeOgg
s32 OggSoundDecoder::closeOgg( void *source ) {
    return 0;
}

} // namespace Sound

DC_END_DREEMCHEST