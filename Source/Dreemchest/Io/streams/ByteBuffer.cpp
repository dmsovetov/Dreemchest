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

#include "ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Io {

// ** ByteBuffer::ByteBuffer
ByteBuffer::ByteBuffer( const u8* pointer, s32 size )
    : m_position( 0 )
{
    if( size == 0 )
    {
        return;
    }
    
    m_buffer.resize( size );

    if( pointer )
    {
        memcpy( &m_buffer[0], pointer, size );
    }
}

ByteBuffer::~ByteBuffer( void )
{

}

// ** ByteBuffer::create
ByteBufferPtr ByteBuffer::create( s32 size )
{
    return ByteBufferPtr( DC_NEW ByteBuffer( NULL, size ) );
}

// ** ByteBuffer::createFromData
ByteBufferPtr ByteBuffer::createFromData( const u8* pointer, s32 size )
{
    return ByteBufferPtr( DC_NEW ByteBuffer( pointer, size ) );
}

// ** ByteBuffer::createFromArray
ByteBufferPtr ByteBuffer::createFromArray( const Array<u8>& data )
{
    return ByteBufferPtr( DC_NEW ByteBuffer( &data[0], data.size() ) );
}

// ** ByteBuffer::fill
void ByteBuffer::fill( u8 value )
{
    memset( &m_buffer[0], value, length() );
}

// ** ByteBuffer::bytesAvailable
s32 ByteBuffer::bytesAvailable( void ) const
{
    return length() - m_position;
}

// ** ByteBuffer::array
const Array<u8>& ByteBuffer::array( void ) const
{
    return m_buffer;
}

// ** ByteBuffer::buffer
const u8* ByteBuffer::buffer( void ) const
{
    return &m_buffer[0];
}

// ** ByteBuffer::current
const u8* ByteBuffer::current( void ) const
{
    return &m_buffer[m_position];
}

// ** ByteBuffer::length
s32 ByteBuffer::length( void ) const
{
    return ( u64 )m_buffer.size();
}

// ** ByteBuffer::copy
ByteBufferPtr ByteBuffer::copy( void ) const
{
    return ByteBuffer::createFromArray( m_buffer );
}

// ** ByteBuffer::trimFromLeft
void ByteBuffer::trimFromLeft( s32 size )
{
    NIMBLE_ABORT_IF( size > length(), "too long chunk to trim" );
    m_buffer.erase( m_buffer.begin(), m_buffer.begin() + size );
    m_position = min2( m_position, length() );
}

// ** ByteBuffer::trimFromRight
void ByteBuffer::trimFromRight( s32 size )
{
    NIMBLE_ABORT_IF( size > length(), "too long chunk to trim" );
    m_buffer.erase( m_buffer.begin() + (length() - size), m_buffer.end() );
    m_position = min2( m_position, length() );
}

// ** ByteBuffer::read
s32 ByteBuffer::read( void* buffer, s32 size ) const
{
    NIMBLE_ABORT_IF( buffer == NULL, "invalid destination buffer" );
    NIMBLE_ABORT_IF( size <= 0, "the size should be positive" );
    NIMBLE_ABORT_IF( size > bytesAvailable(), "not enough data to read" );

    memcpy( buffer, current(), size );
    m_position += size;
    
    return size;
}

// ** ByteBuffer::write
s32 ByteBuffer::write( const void* buffer, s32 size )
{
    NIMBLE_ABORT_IF( buffer == NULL, "invalid destination buffer" );
    NIMBLE_ABORT_IF( size <= 0, "the size should be positive" );

    s32 extra = max2( 0, size - bytesAvailable() );

    if( extra )
    {
        m_buffer.resize( length() + extra );
    }

    memcpy( &m_buffer[m_position], buffer, size );
    m_position += size;

    return size;
}

// ** ByteBuffer::position
s32 ByteBuffer::position( void ) const
{
    return m_position;
}

// ** ByteBuffer::setPosition
void ByteBuffer::setPosition( s32 offset, SeekOrigin origin )
{
    switch( origin )
    {
        case SeekSet:   m_position = min2( length(), offset );
                        break;
            
        case SeekCur:   m_position = min2( m_position + offset, length() );
                        break;
            
        case SeekEnd:   m_position = max2( 0, length() - offset );
                        break;
    }
}

} // namespace Io

DC_END_DREEMCHEST
