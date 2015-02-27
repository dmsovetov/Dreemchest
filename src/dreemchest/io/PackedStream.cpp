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

#include	"PackedStream.h"
#include    "Archive.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** PackedStream::PackedStream
PackedStream::PackedStream( Stream* file, IBufferCompressor* compressor, int fileSize, int fileOffset )
    : m_compressor( compressor ), m_file( file ), m_fileSize( fileSize ), m_fileOffset( fileOffset )
{
    m_position       = 0;
    m_bufferOffset   = 0;
    m_bytesAvailable = 0;
    m_buffer         = DC_NEW u8[16536 * 2];
}

PackedStream::~PackedStream( void )
{
    DC_RELEASE( m_file );
    DC_DELETE( m_buffer );
    DC_DELETE( m_compressor );
}

// ** PackedStream::reopen
void PackedStream::reopen( void )
{
    m_file->setPosition( m_fileOffset );
    m_position = 0;

    decompressChunk();
}

// ** PackedStream::read
long PackedStream::read( void *buffer, long size )
{
    DC_BREAK_IF( m_file == NULL );
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );

    return readFile( ( u8* )buffer, size );
}

// ** PackedStream::setPosition
void PackedStream::setPosition( long offset, eSeekOrigin origin )
{
	DC_BREAK_IF( m_file == NULL );

	long currentPos = position();
	long targetPos	= 0;

	switch( origin ) {
    case SeekCur: targetPos = currentPos  + offset;     break;
    case SeekSet: targetPos = offset;                   break;
    case SeekEnd: targetPos = length() - offset;		break;
	}

	if( targetPos == currentPos ) {
		return;
	}

	if( targetPos < currentPos ) {
		reopen();

		for( int i = 0; i < targetPos; i++ ) {
			u8 temp;
			read( &temp, 1 );
        }
	} else {
		for( int i = 0; i < (targetPos - currentPos); i++ ) {
			u8 temp;
			read( &temp, 1 );
        }
	}
}

// ** PackedStream::position
long PackedStream::position( void ) const
{
    DC_BREAK_IF( m_file == NULL );
    return m_position;
}

// ** PackedStream::hasDataLeft
bool PackedStream::hasDataLeft( void ) const
{
    return position() < length();
}

// ** PackedStream::length
long PackedStream::length( void ) const
{
    return m_fileSize;
}

// ** PackedStream::readFile
int PackedStream::readFile( u8 *buffer, int size )
{
    int bytesRead = readFromBuffer( buffer, size );
    DC_BREAK_IF( bytesRead > size );

    // ** Output buffer is full
    if( bytesRead >= size ) {
        return size;
    }

    // ** End of compressed file
    if( !hasDataLeft() ) {
        return bytesRead;
    }

    // ** Read next chunk from archive
    decompressChunk();

    return bytesRead + readFile( buffer + bytesRead, size - bytesRead );
}

// ** PackedStream::readFromBuffer
int PackedStream::readFromBuffer( u8 *buffer, int size )
{
    int bytesRead = 0;

    for( bytesRead = 0; bytesRead < size; bytesRead++ ) {
        if( m_bufferOffset >= m_bytesAvailable ) {
            break;
        }

        *(buffer++) = m_buffer[m_bufferOffset++];
    }

    m_position += bytesRead;

    return bytesRead;
}

// ** PackedStream::decompressChunk
void PackedStream::decompressChunk( void )
{
    int  compressedSize = 0;
    const int CHUNK_SIZE = 16536;
    u8 compressed[CHUNK_SIZE * 2];
    
    m_file->read( &compressedSize, sizeof( compressedSize ) );
    m_file->read( compressed, compressedSize );
    
    m_bytesAvailable = m_compressor->decompressToBuffer( compressed, compressedSize, m_buffer, CHUNK_SIZE * 2 );
    m_bufferOffset   = 0;
    
    DC_BREAK_IF( m_bytesAvailable > CHUNK_SIZE * 2 );
}

} // namespace io

DC_END_DREEMCHEST
