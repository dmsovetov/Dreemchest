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

#include "TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TCPSocket::TCPSocket
TCPSocket::TCPSocket( TCPSocketDelegate* delegate, SocketDescriptor& descriptor, const NetworkAddress& address )
	: m_descriptor( descriptor ), m_address( address )
{
	m_stream   = DC_NEW TCPStream( &m_descriptor );
	m_delegate = TCPSocketDelegatePtr( delegate ? delegate : DC_NEW TCPSocketDelegate );

    if( m_descriptor.isValid() ) {
        return;
    }

    m_descriptor = ::socket( PF_INET, SOCK_STREAM, 0 );
	if( !m_descriptor.isValid() ) {
		LogError( "socket", "failed to create TCP socket, %d\n", Network::lastError() );
		return;
	}

	m_descriptor.enableAddressReuse();
}

TCPSocket::~TCPSocket( void )
{
	close();
}

// ** TCPSocket::connectTo
TCPSocketPtr TCPSocket::connectTo( const NetworkAddress& address, u16 port, TCPSocketDelegate* delegate )
{
	TCPSocketPtr socket( DC_NEW TCPSocket( delegate ) );

	if( !socket->connect( address, port ) ) {
		return TCPSocketPtr();
	}

	return socket;
}

// ** TCPSocket::isValid
bool TCPSocket::isValid( void ) const
{
	return m_descriptor.isValid();
}

// ** TCPSocket::descriptor
const SocketDescriptor& TCPSocket::descriptor( void ) const
{
	return m_descriptor;
}

// ** TCPSocket::address
const NetworkAddress& TCPSocket::address( void ) const
{
	return m_address;
}

// ** TCPSocket::setDelegate
void TCPSocket::setDelegate( const TCPSocketDelegatePtr& value )
{
	m_delegate = value;
}

// ** TCPSocket::connect
bool TCPSocket::connect( const NetworkAddress& address, u16 port )
{
    DC_ABORT_IF( !m_descriptor.isValid(), "the socket should be valid" );

    m_address  = address;

	sockaddr_in addr = Network::toSockaddr( address, port );

	// ** Connect
	s32 result = ::connect( m_descriptor, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result == SOCKET_ERROR ) {
		return false;
	}

	// ** Set non blocking mode
	m_descriptor.setNonBlocking();

	// ** Set no delay
	m_descriptor.setNoDelay();

	return true;
}

// ** TCPSocket::close
void TCPSocket::close( void )
{
	if( m_descriptor.isValid() && m_delegate.valid() ) {
		m_delegate->handleClosed( this );
	}
	
	m_descriptor.close();
}

// ** TCPSocket::send
u32 TCPSocket::send( const void* buffer, u32 size )
{
	s32 result = m_stream->write( buffer, size );
	if( result == -1 ) {
		close();
		return 0;
	}

	return ( u32 )result;
}

// ** TCPSocket::fetch
void TCPSocket::fetch( void )
{
    if( !m_descriptor.isValid() ) {
        LogError( "socket", "updating socket with an invalid descriptor.\n" );
        return;
    }
    
	s32				 result = 0;
	TCPStream*		 stream = m_stream.get();
	TCPStream::State state  = TCPStream::Idle;

	while( (state = stream->pull()) ) {
		if( state == TCPStream::Closed ) {
			close();
			break;
		}

		m_delegate->handleReceivedData( this, this, stream );

		if( !isValid() ) {
			break;
		}
	}
}

} // namespace net

DC_END_DREEMCHEST
