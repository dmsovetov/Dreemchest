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

DC_BEGIN_DREEMCHEST

namespace io {

// ** Stream::Stream
Stream::Stream( void )
{

}
    
Stream::~Stream( void )
{

}

// ** Stream::dispose
void Stream::dispose( void )
{

}

// ** Stream::readString
u64 Stream::readString( String& str ) const
{
    u64 result		= 0;
    u64 strResult	= 0;
    u32 length		= 0;

    result += read( &length, sizeof( length ) );
    str.resize( length );
    result += read( &str[0], length );

    return result + strResult;
}

// ** Stream::writeString
u64 Stream::writeString( const char* str )
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
u64 Stream::length( void ) const
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::position
u64 Stream::position( void ) const
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::setPosition
void Stream::setPosition( u64 offset, SeekOrigin origin )
{
    DC_BREAK;
}
    
// ** Stream::hasDataLeft
bool Stream::hasDataLeft( void ) const
{
    return position() < length();
}
    
// ** Stream::read
u64 Stream::read( void* buffer, u64 size ) const
{
    DC_BREAK;
    return 0;
}
    
// ** Stream::write
u64 Stream::write( const void* buffer, u64 size )
{
    DC_BREAK;
    return 0;
}

} // namespace io

DC_END_DREEMCHEST
