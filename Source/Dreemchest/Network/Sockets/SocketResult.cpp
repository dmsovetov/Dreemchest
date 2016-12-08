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

#include "SocketResult.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** SocketResult::SocketResult
SocketResult::SocketResult( s32 value ) : m_value( value )
{
}

// ** SocketResult::operator s32
SocketResult::operator s32( void ) const
{
    return m_value;
}

// ** SocketResult::isError
bool SocketResult::isError( void ) const
{
    return m_value == -1;
}

// ** SocketResult::wouldBlock
bool SocketResult::wouldBlock( void ) const
{
    // Get the error code
    s32 code = errorCode();

#ifdef DC_PLATFORM_WINDOWS
    if( code == WSAEWOULDBLOCK ) {
#else
    if( code == EAGAIN ) {
#endif  /*  DC_PLATFORM_WINDOWS */
        return true;
    }

    return false;
}

// ** SocketResult::errorCode
s32 SocketResult::errorCode( void ) const
{
#ifdef DC_PLATFORM_WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

// ** SocketResult::errorMessage
String SocketResult::errorMessage( void ) const
{
#ifdef DC_PLATFORM_WINDOWS
    LPSTR err = NULL;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, WSAGetLastError(), 0, ( LPSTR )&err, 0, 0 );
    String msg = err;
    LocalFree( err );
    return msg;
#else
    return strerror( errno );
#endif
}

} // namespace Network

DC_END_DREEMCHEST
