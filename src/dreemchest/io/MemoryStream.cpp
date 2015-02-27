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

#include	"MemoryStream.h"
#include	"ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** MemoryStream::MemoryStream
MemoryStream::MemoryStream( u8 *pointer, int size, bool own ) : m_buffer( pointer ), m_maxSize( size ), m_isOwner( own ), m_size( 0 ), m_offset( 0 )
{
    if( !m_isOwner ) {
        return;
    }
    
    m_buffer = new u8[size];

    if( pointer ) {
        memcpy( m_buffer, pointer, size );
    } else {
        clear();
    }
}

MemoryStream::~MemoryStream( void )
{
	if( m_isOwner ) delete[]m_buffer;
}

// ** MemoryStream::clear
void MemoryStream::clear( void )
{
    m_size      = 0;
    m_offset    = 0;
    memset( m_buffer, 0, m_maxSize );
}

// ** MemoryStream::maxSize
int MemoryStream::maxSize( void ) const
{
    return m_maxSize;
}

// ** MemoryStream::bytesAvailable
int MemoryStream::bytesAvailable( void ) const
{
    return m_size - m_offset;
}

// ** MemoryStream::buffer
const u8* MemoryStream::buffer( void ) const
{
    return m_buffer;
}

// ** MemoryStream::length
long MemoryStream::length( void ) const
{
    return m_size == 0 ? m_maxSize : m_size;
}

// ** MemoryStream::trimFromLeft
void MemoryStream::trimFromLeft( int size )
{
    if( size == 0 ) {
        return;
    }
    
    DC_BREAK_IF( size > m_size );
    
    int bytesToCopy = m_size - size;
    for( int i = 0; i < bytesToCopy; i++ ) {
        m_buffer[i] = m_buffer[size + i];
    }

    m_size -= size;
    m_offset = std::min( m_size, m_offset );
    DC_BREAK_IF( m_size < 0 );
}

// ** MemoryStream::trimFromRight
void MemoryStream::trimFromRight( int size )
{
    DC_NOT_IMPLEMENTED
}

// ** MemoryStream::read
long MemoryStream::read( void *buffer, long size )
{
    DC_BREAK_IF( m_buffer == NULL );
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );

    long sizeToCopy = std::min( size, m_maxSize - m_offset );

    if( sizeToCopy ) {
        memcpy( buffer, m_buffer + m_offset, sizeToCopy );
        m_offset += sizeToCopy;
    }

    return sizeToCopy;
}

// ** MemoryStream::write
long MemoryStream::write( const void *buffer, long size )
{
	if( m_offset + size > m_maxSize ) {
        DC_BREAK
	//	Warning( "MemoryStream::Write : buffer overflow, maximum size %d, bytes written %d\n", m_maxSize, m_offset + size );
		return 0;
	}

    DC_BREAK_IF( m_buffer == NULL );
    DC_BREAK_IF( buffer == 0 );
    DC_BREAK_IF( size <= 0 );

    long sizeToCopy = std::min( size, m_maxSize - m_offset );

    if( sizeToCopy ) {
        memcpy( m_buffer + m_offset, buffer, sizeToCopy );
        m_offset += sizeToCopy;

        if( m_offset > m_size ) m_size = m_offset;
    }

    return sizeToCopy;
}

// ** MemoryStream::hasDataLeft
bool MemoryStream::hasDataLeft( void ) const
{
    return m_offset < m_size;
}

// ** MemoryStream::position
long MemoryStream::position( void ) const
{
    DC_BREAK_IF( m_buffer == NULL );
    return m_offset;
}

// ** MemoryStream::setPosition
void MemoryStream::setPosition( long offset, eSeekOrigin origin )
{
    DC_BREAK_IF( m_buffer == NULL );

    switch( origin ) {
    case SeekSet:   m_offset = std::min( m_maxSize - 1, offset );
                    break;
            
    case SeekCur:   m_offset = std::min( m_offset + offset, m_maxSize );
                    break;
            
	case SeekEnd:   m_offset = std::max( 0L, m_maxSize - offset );
                    break;
	}
}

} // namespace io

DC_END_DREEMCHEST
