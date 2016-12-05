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

#include "Stream.h"
#include "ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Io {

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
s32 Stream::readString( String& str ) const
{
    s32 result		= 0;
    s32 strResult	= 0;
    u32 length		= 0;

    str     = "";
    result += read( &length, sizeof( length ) );

    if( length == 0 ) {
        return result;
    }

    str.resize( length );
    result += read( &str[0], length );

    return result + strResult;
}

// ** Stream::writeString
s32 Stream::writeString( CString str )
{
    s32 result = 0;
    u32 length = ( u32 )strlen( str );

    result += write( &length, sizeof( length ) );
	if( length ) {
		result += write( str, length );
	}
    
    return result;
}

// ** Stream::length
s32 Stream::length( void ) const
{
    NIMBLE_NOT_IMPLEMENTED;
    return 0;
}
    
// ** Stream::position
s32 Stream::position( void ) const
{
    NIMBLE_NOT_IMPLEMENTED;
    return 0;
}
    
// ** Stream::setPosition
void Stream::setPosition( s32 offset, SeekOrigin origin )
{
    NIMBLE_NOT_IMPLEMENTED;
}
    
// ** Stream::hasDataLeft
bool Stream::hasDataLeft( void ) const
{
    return position() < length();
}
    
// ** Stream::read
s32 Stream::read( void* buffer, s32 size ) const
{
    NIMBLE_NOT_IMPLEMENTED;
    return 0;
}
    
// ** Stream::write
s32 Stream::write( const void* buffer, s32 size )
{
    NIMBLE_NOT_IMPLEMENTED;
    return 0;
}

} // namespace Io

DC_END_DREEMCHEST
