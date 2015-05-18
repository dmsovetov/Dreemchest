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

#include "PosixTCPSocketListener.h"
#include "PosixTCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ----------------------------------PosixTCPSocketListener::TCPClientDelegate ------------------------------ //

// ** PosixTCPSocketListener::TCPClientDelegate::TCPClientDelegate
PosixTCPSocketListener::TCPClientDelegate::TCPClientDelegate( PosixTCPSocketListener* listener ) : m_listener( listener )
{
}

// ** PosixTCPSocketListener::TCPClientDelegate::handleReceivedData
void PosixTCPSocketListener::TCPClientDelegate::handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream )
{
	if( m_listener != NULL ) m_listener->m_delegate->handleReceivedData( m_listener->m_parent, socket, stream );
}
    
// ** PosixTCPSocketListener::TCPClientDelegate::handleClosed
void PosixTCPSocketListener::TCPClientDelegate::handleClosed( TCPSocket* sender )
{
    if( m_listener != NULL ) m_listener->m_delegate->handleConnectionClosed( m_listener->m_parent, sender );
}

// ------------------------------------------PosixTCPSocketListener ---------------------------------------- //

// ** PosixTCPSocketListener::PosixTCPSocketListener
PosixTCPSocketListener::PosixTCPSocketListener( TCPSocketListenerDelegate* delegate ) : m_port( 0 )
{
	m_delegate = TCPSocketListenerDelegatePtr( delegate ? delegate : DC_NEW TCPSocketListenerDelegate );
}

// ** PosixTCPSocketListener::setupFDSet
s32 PosixTCPSocketListener::setupFDSets( fd_set& read, fd_set& write,  fd_set& except, SocketDescriptor& listener )
{
    // ** Maximum socket descriptor.
    s32 result = 0;
    
    FD_ZERO( &read );
    FD_ZERO( &write );
    FD_ZERO( &except );

    // ** Add the listener socket
	FD_SET( listener, &read );
	FD_SET( listener, &except );
    
    result = max2( result, ( s32 )listener );

    // ** Add client connections
	for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
		const SocketDescriptor& socket = (*i)->descriptor();

		FD_SET( socket, &read );
		FD_SET( socket, &write );
		FD_SET( socket, &except );
        
        result = max2( result, ( s32 )socket );
	}
    
    return result + 1;
}

// ** PosixTCPSocketListener::update
void PosixTCPSocketListener::update( void )
{
	// ** Setup FD sets
	fd_set read, write, except;
	s32 nfds = setupFDSets( read, write, except, m_socket );
    
    // ** Setup the timeout structure.
    timeval waitTime;
    waitTime.tv_sec  = 0;
    waitTime.tv_usec = 0;

	// ** Do a select
	if( select( nfds, &read, &write, &except, &waitTime ) <= 0 ) {
		log::error( "PosixTCPSocketListener::update : select failed, %s\n", PosixNetwork::lastErrorMessage().c_str() );
		return;
	}

	// ** Process listener socket
	if( FD_ISSET( m_socket, &read ) ) {
		TCPSocketPtr accepted = acceptConnection();
        m_clientSockets.push_back( accepted );

		m_delegate->handleConnectionAccepted( m_parent, accepted.get() );
	}
	else if( FD_ISSET( m_socket, &except ) ) {
		log::error( "PosixTCPSocketListener::update : error on listening socket: %d\n", m_socket.error() );
		return;
	}

	// ** Process client sockets
	for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
		TCPSocketPtr&			socket	   = *i;
		const SocketDescriptor& descriptor = socket->descriptor();
		bool					hasError   = false;

		// ** Check the erro on this socket.
		if( FD_ISSET( descriptor, &except ) ) {
			FD_CLR( descriptor, &except );
			hasError = true;
		}
		else {
			if( FD_ISSET( descriptor, &read ) ) {
				FD_CLR( descriptor, &read );
				socket->update();
			}
			if( FD_ISSET( descriptor, &write ) ) {
				FD_CLR( descriptor, &write );
			//	log::verbose( "PosixTCPSocketListener::update : writable socket handle %d\n", ( s32 )descriptor );
			}
		}

		if( hasError ) {
			s32 error = descriptor.error();
			if( error != 0 ) {
				log::error( "PosixTCPSocketListener::update : socket error %\n", error );
			}
			socket->close();
		}
	}

    // ** Remove closed connections
    for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ) {
		if( !(*i)->isValid() ) {
            i = m_clientSockets.erase( i );
        } else {
            ++i;
        }
    }
}

// ** PosixTCPSocketListener::close
void PosixTCPSocketListener::close( void )
{
	m_socket.close();
	m_clientSockets.clear();
    m_port = 0;
}
    
// ** PosixTCPSocketListener::port
u16 PosixTCPSocketListener::port( void ) const
{
    return m_port;
}

// ** PosixTCPSocketListener::connections
const TCPSocketList& PosixTCPSocketListener::connections( void ) const
{
	return m_clientSockets;
}

// ** PosixTCPSocketListener::acceptConnection
TCPSocketPtr PosixTCPSocketListener::acceptConnection( void )
{
	NetworkAddress	 address;
	SocketDescriptor socket = m_socket.accept( address );

	if( !socket.isValid() ) {
		return TCPSocketPtr();
	}

	socket.setNonBlocking();

	return TCPSocketPtr( DC_NEW TCPSocket( DC_NEW PosixTCPSocket( DC_NEW TCPClientDelegate( this ), socket, address ) ) );
}

// ** PosixTCPSocketListener::bindTo
bool PosixTCPSocketListener::bindTo( u16 port )
{
    m_socket = socket( PF_INET, SOCK_STREAM, 0 );

	if( !m_socket.isValid() ) {
		log::error( "PosixTCPSocketListener::bindTo : failed to create socket, %d\n", PosixNetwork::lastError() );
		return false;
	}

	sockaddr_in addr = PosixNetwork::toSockaddr( NetworkAddress::Null, port );
	s32 result = bind( m_socket, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result == SOCKET_ERROR ) {
		log::error( "PosixTCPSocketListener::bindTo : bind failed, %d\n", PosixNetwork::lastError() );
		return false;
	}

	// ** Set non blocking mode
	m_socket.setNonBlocking();

	// ** Listen for connections
    result = listen( m_socket, 16 );
   
	if( result == SOCKET_ERROR ) {
		log::error( "PosixTCPSocketListener::bindTo : listen failed, %d\n", PosixNetwork::lastError() );
		return false;
	}
    
    m_port = port;

    return true;
}

} // namespace net

DC_END_DREEMCHEST