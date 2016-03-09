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

#include "SoundBuffer.h"
#include "SoundSource.h"

#include "../Decoders/SoundDecoder.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ** SoundBuffer::SoundBuffer
SoundBuffer::SoundBuffer( SoundDecoderPtr decoder, u32 chunks ) : m_decoder( decoder ), m_size( 0 ), m_chunks( chunks )
{
    if( decoder.valid() ) {
		m_size = decoder->size();
	}
}

SoundBuffer::~SoundBuffer( void )
{

}

// ** SoundBuffer::chunks
u32 SoundBuffer::chunks( void ) const
{
    return m_chunks;
}

// ** SoundBuffer::decoder
SoundDecoderWPtr SoundBuffer::decoder( void ) const
{
    return m_decoder;
}

// ** SoundBuffer::size
u64 SoundBuffer::size( void ) const
{
    return m_size;
}

// ** SoundBuffer::attachToSource
void SoundBuffer::attachToSource( SoundSourceWPtr target )
{

}

// ** SoundBuffer::detachFromSource
void SoundBuffer::detachFromSource( SoundSourceWPtr target )
{

}

// ** SoundBuffer::updateStream
bool SoundBuffer::updateStream( SoundSourceWPtr target, bool isLooped )
{
    return false;
}

} // namespace Sound

DC_END_DREEMCHEST