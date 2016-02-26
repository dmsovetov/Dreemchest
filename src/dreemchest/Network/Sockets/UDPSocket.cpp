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

#include "UDPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** UDPSocket::UDPSocket
UDPSocket::UDPSocket( bool broadcast )
{
    m_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
    m_data       = Io::ByteBuffer::create();

	m_descriptor.setNonBlocking();

	if( broadcast ) {
		m_descriptor.enableBroadcast();
	}
}

// ** UDPSocket::send
u32 UDPSocket::send( const Address& address, u16 port, const void* buffer, u32 size )
{
    // ** Format address
    sockaddr_in dest;

    dest.sin_addr.s_addr = address;
	dest.sin_port        = htons( port );
	dest.sin_family      = AF_INET;

    // ** Send datagram
    s32 result = sendto( m_descriptor, ( s8* )buffer, size, 0, ( sockaddr* )&dest, sizeof( dest ) );
    if( result == -1 ) {
        LogWarning( "socket", "sendto failed to %s:%d, %s\n", address.toString(), port, strerror( errno ) );
        DC_BREAK;
		return 0;
    }

    return result;
}

// ** UDPSocket::listen
bool UDPSocket::listen( u16 port )
{
    // ** Format address
    sockaddr_in addr;
    
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port        = htons( port );

    //** Bind address
    s32 result = bind( m_descriptor, ( sockaddr* )&addr, sizeof( addr ) );
    if( result == -1 ) {
        return false;
    }

    return true;
}

// ** UDPSocket::recv
void UDPSocket::recv( void )
{
    sockaddr_in addr;
    socklen_t   addrlen = sizeof( addr );
    
    SocketResult result = recvfrom( m_descriptor, ( s8* )m_data->buffer(), m_data->length(), 0, ( sockaddr* )&addr, &addrlen );

    if( result.wouldBlock() ) {
        return;
    }

    if( result.isError() ) {
        LogError( "socket", "recvfrom failed %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        return;
    }

	Address remoteAddress( addr.sin_addr.s_addr );
    notify<Data>( this, Address( addr.sin_addr.s_addr ), m_data );
}

// ** UDPSocket::create
UDPSocketPtr UDPSocket::create( void )
{
	return UDPSocketPtr( DC_NEW UDPSocket( false ) );
}

// ** UDPSocket::createBroadcast
UDPSocketPtr UDPSocket::createBroadcast( void )
{
	return UDPSocketPtr( DC_NEW UDPSocket( true ) );
}


} // namespace Network

DC_END_DREEMCHEST
