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

#include    "SoundDecoder.h"
#include    "../SoundStream.h"

namespace dreemchest {

namespace sound {

// ** SoundDecoder::SoundDecoder
SoundDecoder::SoundDecoder( void )
    : m_stream( NULL )
{
}

SoundDecoder::~SoundDecoder( void )
{
    close();
}

// ** SoundDecoder::open
bool SoundDecoder::open( ISoundStream* stream )
{
    if( !stream ) {
        return false;
    }

    m_stream = stream;
    return m_stream != NULL;
}

// ** SoundDecoder::close
void SoundDecoder::close( void )
{
    DC_RELEASE( m_stream );
}

// ** SoundDecoder::read
long SoundDecoder::read( u8 *buffer, int size )
{
    return 0;
}

// ** SoundDecoder::seek
void SoundDecoder::seek( int pos )
{

}

// ** SoundDecoder::rate
u32 SoundDecoder::rate( void ) const
{
    return m_rate;
}

// ** SoundDecoder::format
SoundSampleFormat SoundDecoder::format( void ) const
{
    return m_format;
}

// ** SoundDecoder::size
int SoundDecoder::size( void ) const
{
    return 0;
}

} // namespace sound

} // namespace dreemchest