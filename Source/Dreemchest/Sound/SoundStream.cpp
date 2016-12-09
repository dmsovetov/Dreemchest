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

#include "SoundStream.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

// ---------------------------------------- StandardStreamOpener ---------------------------------------- //

// ** StandardStreamOpener::open
ISoundStreamPtr StandardStreamOpener::open( CString uri )
{
    return StandardSoundStream::open( uri );
}

// ------------------------------------------ StandardSoundStream ------------------------------------------- //

// ** StandardSoundStream::StandardSoundStream
StandardSoundStream::StandardSoundStream( Io::StreamPtr stream ) : m_stream( stream )
{

}

// ** StandardSoundStream::open
ISoundStreamPtr StandardSoundStream::open( CString uri )
{
    Io::DiskFileSystem diskFileSystem;
    Io::StreamPtr       stream = diskFileSystem.openFile( uri );
    return stream.valid() ? DC_NEW StandardSoundStream( stream ) : NULL;
}

// ** StandardSoundStream::length
u32 StandardSoundStream::length( void ) const
{
    return m_stream->length();
}

// ** StandardSoundStream::loadToRam
ISoundStreamPtr StandardSoundStream::loadToRam( void ) const
{
    UPtr<u8> data = DC_NEW u8[m_stream->length()];
    m_stream->read( data.get(), m_stream->length() );
    return DC_NEW MemorySoundStream( Io::ByteBuffer::createFromData( data.get(), m_stream->length() ) );
}

// ** StandardSoundStream::read
u32 StandardSoundStream::read( void* buffer, u32 size )
{
    return m_stream->read( buffer, size );
}

// ** StandardSoundStream::setPosition
void StandardSoundStream::setPosition( u32 offset, SeekOrigin origin )
{
    switch( origin ) {
    case SeekSet: m_stream->setPosition( offset, Io::SeekSet ); break;
    case SeekCur: m_stream->setPosition( offset, Io::SeekCur ); break;
    case SeekEnd: m_stream->setPosition( offset, Io::SeekEnd ); break;
    }
}

// ** StandardSoundStream::position
u32 StandardSoundStream::position( void ) const
{
    return m_stream->position();
}

// ---------------------------------------- MemorySoundStream ----------------------------------------- //

// ** MemorySoundStream::MemorySoundStream
MemorySoundStream::MemorySoundStream( Io::ByteBufferPtr data ) : m_data( data )
{

}

MemorySoundStream::~MemorySoundStream( void )
{

}

// ** MemorySoundStream::length
u32 MemorySoundStream::length( void ) const
{
    return m_data->length();
}

// ** MemorySoundStream::loadToRam
ISoundStreamPtr MemorySoundStream::loadToRam( void ) const
{
    return ISoundStreamPtr( DC_NEW MemorySoundStream( m_data->copy() ) );
}

// ** MemorySoundStream::read
u32 MemorySoundStream::read( void* buffer, u32 size )
{
    if( m_data->bytesAvailable() < size ) {
        return 0;
    }

    return m_data->read( buffer, size );
}

// ** MemorySoundStream::setPosition
void MemorySoundStream::setPosition( u32 offset, SeekOrigin origin )
{
    switch( origin ) {
    case SeekSet: m_data->setPosition( offset, Io::SeekSet ); break;
    case SeekCur: m_data->setPosition( offset, Io::SeekCur ); break;
    case SeekEnd: m_data->setPosition( offset, Io::SeekEnd ); break;
    }
}

// ** MemorySoundStream::position
u32 MemorySoundStream::position( void ) const
{
    return m_data->position();
}

} // namespace Sound

DC_END_DREEMCHEST
