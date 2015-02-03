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

#include    "SoundEngine.h"
#include    "../Decoders/OggSoundDecoder.h"

namespace dreemchest {

namespace sound {

// ** SoundEngine::SoundEngine
SoundEngine::SoundEngine( void )
{

}

SoundEngine::~SoundEngine( void )
{

}

// ** SoundEngine::createSource
SoundSource* SoundEngine::createSource( void )
{
    return NULL;
}

// ** SoundEngine::createBuffer
SoundBuffer* SoundEngine::createBuffer( SoundDecoder* decoder, u32 chunks )
{
    return NULL;
}
/*
// ** SoundEngine::createSoundDecoderFromUnknown
SoundDecoder* SoundEngine::createSoundDecoderFromUnknown( ISoundStream* stream )
{
    char    header[33];
    int     format;

    // ** Read header string
    stream->readString( header, 32 );
    if( strcmp( header, "DCSOUND" ) != 0 ) {
        log::error( "SoundEngine::createSoundDecoderFromUnknown : invalid file format" );
        return false;
    }

    // ** Read format ID
    stream->read( &format, sizeof( format ) );

    return createSoundDecoderWithFormat( stream, scast( SoundFormat, format ) );
}*/

// ** SoundEngine::createSoundDecoderWithFormat
SoundDecoder* SoundEngine::createSoundDecoderWithFormat( ISoundStream* stream, SoundFormat format )
{
    DC_BREAK_IF( stream == NULL );

    SoundDecoder* soundDecoder = DC_NEW OggSoundDecoder;

    if( !soundDecoder ) {
        log::error( "OpenAL::createSoundDecoder : failed to open file" );
        return NULL;
    }

    if( soundDecoder->open( stream ) ) {
        return soundDecoder;
    }

    log::error( "OpenAL::createSoundDecoder : unknown file format" );
    return NULL;
}

// ** SoundEngine::createSoundDecoder
SoundDecoder* SoundEngine::createSoundDecoder( ISoundStream* stream, SoundFormat format )
{
//    if( format == SoundFormatUnknown ) {
//        return createSoundDecoderFromUnknown( stream );
//    }

    return createSoundDecoderWithFormat( stream, format );
}

} // namespace sound

} // namespace dreemchest