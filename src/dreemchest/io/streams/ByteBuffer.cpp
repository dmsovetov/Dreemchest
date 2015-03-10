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

namespace io {

// ** ByteBuffer::ByteBuffer
ByteBuffer::ByteBuffer( const u8* pointer, u64 size ) : m_position( 0 )
{
    if( size == 0 ) {
        return;
    }
    
    m_buffer.resize( size );
    memcpy( &m_buffer[0], pointer, size );
    fill( 0 );
}

ByteBuffer::~ByteBuffer( void )
{

}

// ** ByteBuffer::create
ByteBufferPtr ByteBuffer::create( void )
{
    return ByteBufferPtr( new ByteBuffer( NULL, 0 ) );
}

// ** ByteBuffer::fill
void ByteBuffer::fill( u8 value )
{
    memset( &m_buffer[0], value, length() );
}

// ** ByteBuffer::bytesAvailable
u64 ByteBuffer::bytesAvailable( void ) const
{
    return length() - m_position;
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
u64 ByteBuffer::length( void ) const
{
    return ( u64 )m_buffer.size();
}

// ** ByteBuffer::trimFromLeft
void ByteBuffer::trimFromLeft( u64 size )
{
    DC_BREAK_IF( size > length() );
    m_buffer.erase( m_buffer.begin(), m_buffer.begin() + size );
    m_position = std::min( m_position, length() );
}

// ** ByteBuffer::trimFromRight
void ByteBuffer::trimFromRight( u64 size )
{
    DC_BREAK_IF( size > length() );
    m_buffer.erase( m_buffer.begin() + (length() - size), m_buffer.end() );
    m_position = std::min( m_position, length() );
}

// ** ByteBuffer::read
u64 ByteBuffer::read( void* buffer, u64 size ) const
{
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );
    DC_BREAK_IF( size > bytesAvailable() );

    memcpy( buffer, current(), size );
    return size;
}

// ** ByteBuffer::write
u64 ByteBuffer::write( const void* buffer, u64 size )
{
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );

    u64 extra = std::max( ( u64 )0, size - bytesAvailable() );

    if( extra ) {
        m_buffer.resize( length() + extra );
    }

    memcpy( &m_buffer[m_position], buffer, size );
    return size;
}

// ** ByteBuffer::position
u64 ByteBuffer::position( void ) const
{
    return m_position;
}

// ** ByteBuffer::setPosition
void ByteBuffer::setPosition( u64 offset, SeekOrigin origin )
{
    switch( origin ) {
    case SeekSet:   m_position = std::min( length() - 1, offset );
                    break;
            
    case SeekCur:   m_position = std::min( m_position + offset, length() );
                    break;
            
	case SeekEnd:   m_position = std::max( static_cast<u64>(0), length() - offset );
                    break;
	}
}

} // namespace io

DC_END_DREEMCHEST
