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

#include "SocketDescriptor.h"

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
#endif  /*  #ifndef INVALID_SOCKET  */

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
#endif  /*  #ifndef SOCKET_ERROR  */

DC_BEGIN_DREEMCHEST

namespace Network {

// ** SocketDescriptor::Invalid
SocketDescriptor SocketDescriptor::Invalid = INVALID_SOCKET;

// ** SocketDescriptor::ActiveCount
u32 SocketDescriptor::ActiveCount = 0;

// ** SocketDescriptor::SocketDescriptor
SocketDescriptor::SocketDescriptor( s32 socket ) : m_socket( socket )
{
    ActiveCount++;
    LogDebug( "socket", "%d descriptors now active\n", ActiveCount );
}

// ** SocketDescriptor::SocketDescriptor
SocketDescriptor::SocketDescriptor( const SocketDescriptor& other )
{
    ActiveCount++;

    m_socket = other.m_socket;
    other.m_socket = INVALID_SOCKET;
}

SocketDescriptor::~SocketDescriptor( void )
{
    close();
    ActiveCount--;
    LogDebug( "socket", "%d descriptors now active\n", ActiveCount );;
}

// ** SocketDescriptor::operator bool
SocketDescriptor::operator s32( void ) const
{
    return m_socket;
}

// ** SocketDescriptor::operator =
const SocketDescriptor& SocketDescriptor::operator = ( const SocketDescriptor& other )
{
    m_socket = other.m_socket;
    other.m_socket = INVALID_SOCKET;
    return *this;
}

// ** SocketDescriptor::operator =
const SocketDescriptor& SocketDescriptor::operator = ( s32 socket )
{
    close();
    m_socket = socket;
    return *this;
}

// ** SocketDescriptor::operator ==
bool SocketDescriptor::operator == ( const SocketDescriptor& other ) const
{
    return m_socket == other.m_socket;
}

// ** SocketDescriptor::isValid
bool SocketDescriptor::isValid( void ) const
{
    return m_socket != INVALID_SOCKET;
}

// ** SocketDescriptor::error
s32 SocketDescriptor::error( void ) const
{
    s32 err;
#if defined( DC_PLATFORM_WINDOWS )
    s32       size = sizeof( err );
#else
    socklen_t size = sizeof( err );
#endif
    
    getsockopt( m_socket, SOL_SOCKET, SO_ERROR, ( s8* )&err, &size );
    return err;
}

// ** SocketDescriptor::accept
SocketDescriptor SocketDescriptor::accept( Address& remoteAddress ) const
{
    sockaddr_in addr;
#if defined( DC_PLATFORM_WINDOWS )
    s32       size = sizeof( addr );
#else
    socklen_t size = sizeof( addr );
#endif
    SocketDescriptor socket = ::accept( m_socket, ( sockaddr* )&addr, &size );

    if( socket.isValid() ) {
        remoteAddress = addr.sin_addr.s_addr;
    }

    return socket;
}

// ** SocketDescriptor::close
void SocketDescriptor::close( void )
{
    if( m_socket == INVALID_SOCKET ) {
        return;
    }

#if defined( DC_PLATFORM_WINDOWS )
    closesocket( m_socket );
#else
    ::close( m_socket );
#endif

    m_socket = INVALID_SOCKET;
}

// ** SocketDescriptor::setNonBlocking
bool SocketDescriptor::setNonBlocking( void )
{
#if defined( DC_PLATFORM_WINDOWS )
    u_long noBlock = 1;
    SocketResult result = ioctlsocket( m_socket, FIONBIO, &noBlock );
#else
    SocketResult result = fcntl( m_socket, F_SETFL, O_NONBLOCK );
#endif

    if( result.isError() ) {
        LogError( "socket", "failed to switch socket to a non-blocking mode %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        NIMBLE_BREAK
    }

    return result != SOCKET_ERROR;
}

// ** SocketDescriptor::setNoDelay
bool SocketDescriptor::setNoDelay( void )
{
#if defined( DC_PLATFORM_WINDOWS )
    s8 one = 1;
#else
    s32 one = 1;
#endif

    SocketResult result = setsockopt( m_socket, IPPROTO_TCP, TCP_NODELAY, &one, sizeof( one ) );

    if( result.isError() ) {
        LogError( "socket", "failed to enable the TCP no delay option %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        NIMBLE_BREAK
    }

    return result != SOCKET_ERROR;
}

// ** SocketDescriptor::enableBroadcast
bool SocketDescriptor::enableBroadcast( void )
{
#if defined( DC_PLATFORM_WINDOWS )
    s8 one = 1;
#else
    s32 one = 1;
#endif

    SocketResult result = setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof( one ) );

    if( result.isError() ) {
        LogError( "socket", "failed to switch socket to a broadcast mode %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        NIMBLE_BREAK
    }

    return result != SOCKET_ERROR;
}

// ** SocketDescriptor::enableAddressReuse
bool SocketDescriptor::enableAddressReuse( void )
{
#if defined( DC_PLATFORM_WINDOWS )
    s8 one = 1;
#else
    s32 one = 1;
#endif

    SocketResult result = setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );

    if( result.isError() ) {
        LogError( "socket", "failed to enable address reuse on socket %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        NIMBLE_BREAK
    }

    return result != SOCKET_ERROR;
}

} // namespace Network

DC_END_DREEMCHEST
