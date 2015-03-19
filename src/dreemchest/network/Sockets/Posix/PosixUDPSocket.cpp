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

#include "PosixUDPSocket.h"
#include "PosixNetwork.h"
#include "../../../io/streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** PosixUDPSocket::PosixUDPSocket
PosixUDPSocket::PosixUDPSocket( UDPSocketDelegate* delegate, bool broadcast ) : m_delegate( delegate )
{
    m_socket = socket( AF_INET, SOCK_DGRAM, 0 );
    m_buffer = io::ByteBuffer::create( 4096 );

	if( m_delegate == NULL ) {
		m_delegate = UDPSocketDelegatePtr( DC_NEW UDPSocketDelegate );
	}

	m_socket.setNonBlocking();

	if( broadcast ) {
		m_socket.enableBroadcast();
	}
}

PosixUDPSocket::~PosixUDPSocket( void )
{

}

// ** PosixUDPSocket::send
u32 PosixUDPSocket::send( const NetworkAddress& address, u16 port, const void* buffer, u32 size )
{
    // ** Format address
    sockaddr_in dest;

    dest.sin_addr.s_addr = address;
	dest.sin_port        = htons( port );
	dest.sin_family      = AF_INET;

    // ** Send datagram
    s32 result = sendto( m_socket, ( s8* )buffer, size, 0, ( sockaddr* )&dest, sizeof( dest ) );
    if( result == -1 ) {
        log::warn( "PosixUDPSocket::Send : failed to send data to %s:%d, %s\n", address.toString(), port, strerror( errno ) );
        DC_BREAK;
		return 0;
    }

    return result;
}

// ** PosixUDPSocket::listen
bool PosixUDPSocket::listen( u16 port )
{
    // ** Format address
    sockaddr_in addr;
    
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port        = htons( port );

    //** Bind address
    s32 result = bind( m_socket, ( sockaddr* )&addr, sizeof( addr ) );
    if( result == -1 ) {
		m_delegate->handleListenFailure( m_parent );
        return false;
    }

	m_delegate->handleListenSuccess( m_parent );
    return true;
}

// ** PosixUDPSocket::update
void PosixUDPSocket::update( void )
{
    sockaddr_in addr;
    socklen_t   addrlen = sizeof( addr );
    
    s32 size = recvfrom( m_socket, ( s8* )m_buffer->buffer(), m_buffer->length(), 0, ( sockaddr* )&addr, &addrlen );
    if( size <= 0 ) {
	#if defined( DC_PLATFORM_WINDOWS )
		if( WSAGetLastError() != WSAEWOULDBLOCK ) { DC_BREAK; }
	#else
		if( errno != EAGAIN ) { DC_BREAK; }
	#endif
        return;
    }

	NetworkAddress remoteAddress( addr.sin_addr.s_addr );
	m_delegate->handleReceivedData( m_parent, remoteAddress, m_buffer->buffer(), m_buffer->length() );
}

} // namespace net

DC_END_DREEMCHEST
