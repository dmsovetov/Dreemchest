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

#include "PosixTCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** PosixTCPSocket::PosixTCPSocket
PosixTCPSocket::PosixTCPSocket( TCPSocketDelegate* delegate, SocketDescriptor& socket, const NetworkAddress& address )
	: m_socket( socket ), m_address( address )
{
	m_parent   = NULL;
	m_stream   = DC_NEW TCPStream( &m_socket );
	m_delegate = TCPSocketDelegatePtr( delegate ? delegate : DC_NEW TCPSocketDelegate );

    if( m_socket.isValid() ) {
        return;
    }

    m_socket = ::socket( PF_INET, SOCK_STREAM, 0 );
	if( !m_socket.isValid() ) {
		log::error( "PosixTCPSocket::PosixTCPSocket : failed to create socket, %d\n", PosixNetwork::lastError() );
		return;
	}

	m_socket.enableAddressReuse();
}

PosixTCPSocket::~PosixTCPSocket( void )
{
	close();
}

// ** PosixTCPSocket::isValid
bool PosixTCPSocket::isValid( void ) const
{
	return m_socket.isValid();
}

// ** PosixTCPSocket::descriptor
const SocketDescriptor& PosixTCPSocket::descriptor( void ) const
{
	return m_socket;
}

// ** PosixTCPSocket::address
const NetworkAddress& PosixTCPSocket::address( void ) const
{
	return m_address;
}

// ** PosixTCPSocket::connectTo
bool PosixTCPSocket::connectTo( const NetworkAddress& address, u16 port )
{
	if( !m_socket.isValid() ) {
        DC_BREAK
        return false;
    }
    
    m_address  = address;

	sockaddr_in addr = PosixNetwork::toSockaddr( address, port );

	// ** Connect
	s32 result = connect( m_socket, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result == SOCKET_ERROR ) {
		return false;
	}

	// ** Set non blocking mode
	m_socket.setNonBlocking();

	return true;
}

// ** PosixTCPSocket::close
void PosixTCPSocket::close( void )
{
	m_delegate->handleClosed( m_parent );
	m_socket.close();
}

// ** PosixTCPSocket::sendTo
u32 PosixTCPSocket::sendTo( const void* buffer, u32 size )
{
	return ( u32 )m_stream->write( buffer, size );
}

// ** PosixTCPSocket::update
void PosixTCPSocket::update( void )
{
	s32				 result = 0;
	TCPStream*		 stream = m_stream.get();
	TCPStream::State state  = TCPStream::Idle;

	while( (state = stream->pull()) ) {
		if( state == TCPStream::Closed ) {
			close();
			break;
		}

		m_delegate->handleReceivedData( m_parent, m_parent, stream );
		stream->flush();

		if( !isValid() ) {
			break;
		}
	}
}

} // namespace net

DC_END_DREEMCHEST
