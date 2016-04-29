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

#include "SoundEngine.h"
#include "SoundSource.h"
#include "SoundBuffer.h"
#include "Sound/SoundStream.h"

#include "../Decoders/WavSoundDecoder.h"

#ifdef DC_HAVE_VORBIS
    #include "../Decoders/OggSoundDecoder.h"
#endif

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** SoundEngine::SoundEngine
SoundEngine::SoundEngine( void )
{

}

SoundEngine::~SoundEngine( void )
{

}

// ** SoundEngine::initialize
bool SoundEngine::initialize( void )
{
    return true;
}

// ** SoundEngine::createSource
SoundSourcePtr SoundEngine::createSource( void )
{
    return SoundSourcePtr();
}

// ** SoundEngine::createBuffer
SoundBufferPtr SoundEngine::createBuffer( SoundDecoderPtr decoder, u32 chunks )
{
    return SoundBufferPtr();
}

// ** SoundEngine::setVolume
void SoundEngine::setVolume( f32 value )
{
}

// ** SoundEngine::setPitch
void SoundEngine::setPitch( f32 value )
{
}

// ** SoundEngine::setPosition
void SoundEngine::setPosition( const Vec3& value )
{
}

// ** SoundEngine::setDistanceModel
void SoundEngine::setDistanceModel( DistanceModel value )
{
}

// ** SoundEngine::createSoundDecoder
SoundDecoderPtr SoundEngine::createSoundDecoder( SoundContainerFormat format ) const
{
    switch( format ) {
    case SoundFormatUnknown:    LogWarning( "soundEngine", "unknown sound format\n" );
                                return SoundDecoderPtr();

    case SoundFormatWav:        return DC_NEW WavSoundDecoder;
    case SoundFormatMp3:
								#ifdef DC_HAVE_MP3
									return DC_NEW Mp3SoundDecoder;
								#else
									LogWarning( "soundEngine", "MP3 sound decoder is not supported\n" );
								#endif
								break;
    case SoundFormatOgg:
								#ifdef DC_HAVE_VORBIS
									return DC_NEW OggSoundDecoder;
								#else
									LogWarning( "soundEngine", "Vorbis sound decoder is not supported\n" );
								#endif
								break;
    }

    return NULL;
}

// ** SoundEngine::createSoundDecoderWithFormat
SoundDecoderPtr SoundEngine::createSoundDecoderWithFormat( ISoundStreamPtr stream, SoundContainerFormat format )
{
    DC_ABORT_IF( !stream.valid(), "invalid stream" );

    SoundDecoderPtr soundDecoder = createSoundDecoder( format );

    if( !soundDecoder.valid() ) {
        LogError( "soundEngine", "failed to open file\n" );
        return SoundDecoderPtr();
    }

    if( soundDecoder->open( stream ) ) {
        return soundDecoder;
    }

    LogError( "soundEngine", "unknown file format\n" );
    return SoundDecoderPtr();
}

// ** SoundEngine::createSoundDecoder
SoundDecoderPtr SoundEngine::createSoundDecoder( ISoundStreamPtr stream, SoundContainerFormat format )
{
    return createSoundDecoderWithFormat( stream, format );
}

} // namespace Sound

DC_END_DREEMCHEST