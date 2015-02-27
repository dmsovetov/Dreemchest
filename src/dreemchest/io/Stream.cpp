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

#include	"Stream.h"
#include	"ByteBuffer.h"
#include    "MemoryStream.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** Stream::Stream
Stream::Stream( void )
{

}
    
Stream::~Stream( void )
{

}

// ** Stream::release
void Stream::release( void )
{
    delete this;
}

// ** Stream::writeWithFormat
long Stream::writeWithFormat( const char *format, ... )
{
/*    DC_BREAK_IF( format == NULL );
    
    va_list		ap;
    char		formated[MAX_FORMAT_BUFFER_SIZE];

    va_start( ap, format );
    vsnprintf( formated, MAX_FORMAT_BUFFER_SIZE, format, ap );
    va_end( ap );

    return write( formated, ( long )strlen( formated ) + 1 );*/
	return 0;
}

// ** Stream::writeIdentation
long Stream::writeIdentation( int size )
{
	long result = 0;

	for( int i = 0; i < size; i++ ) {
		result += writeWithFormat( "\t" );
	}

	return result;
}

// ** Stream::toByteBuffer
ByteBuffer* Stream::toByteBuffer( bool asText )
{
    int         len    = length();
    ByteBuffer *buffer = DC_NEW ByteBuffer( len + (asText ? 1 : 0) );

    if( len ) {
        read( buffer->data(), len );
    }

	if( asText ) {
		buffer->data()[len] = 0;
	}

	return buffer;
}

// ** Stream::toMemoryStream
MemoryStream* Stream::toMemoryStream( void )
{
    ByteBuffer*   buffer = toByteBuffer();
    MemoryStream* result = DC_NEW MemoryStream( buffer->data(), buffer->size(), true );
    buffer->release();

    return result;
}

// ** Stream::readString
long Stream::readString( char *dst, int bufferSize )
{
    long result		= 0;
    long strResult	= 0;
    u32 length		= 0;

    dst[0] = 0;
    result += read( &length, sizeof( length ) );

    while( strResult < ( int )length && strResult < bufferSize ) {
        int bytesRead = read( dst, std::min( ( int )length, bufferSize ) ); dst[bytesRead] = '\0';
        length		-= bytesRead;
        strResult	+= bytesRead;
    }
    
    return result + strResult;
}

// ** Stream::writeString
long Stream::writeString( const char *str )
{
    long result = 0;
    u32 length = ( u32 )strlen( str );

    result += write( &length, sizeof( length ) );
	if( length ) {
		result += write( str, length );
	}
    
    return result;
}

// ** Stream::length
long Stream::length( void ) const
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::length
long Stream::position( void ) const
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::setPosition
void Stream::setPosition( long offset, eSeekOrigin origin )
{
    DC_BREAK;
}
    
// ** Stream::hasDataLeft
bool Stream::hasDataLeft( void ) const
{
    DC_BREAK;
    return false;
}
    
// ** Stream::read
long Stream::read( void *buffer, long size )
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::write
long Stream::write( const void *buffer, long size )
{
    DC_BREAK;
    return 0;
}

} // namespace io

DC_END_DREEMCHEST
