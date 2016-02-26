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

#include "TCPSocketListener.h"
#include "TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TCPSocketListener::TCPSocketListener
TCPSocketListener::TCPSocketListener( void ) : m_port( 0 )
{

}

// ** TCPSocketListener::setupFDSet
s32 TCPSocketListener::setupFDSets( fd_set& read, fd_set& write,  fd_set& except, SocketDescriptor& listener )
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

		if( !socket.isValid() ) {
			continue;
		}

		FD_SET( socket, &read );
		FD_SET( socket, &write );
		FD_SET( socket, &except );
        
        result = max2( result, ( s32 )socket );
	}
    
    return result + 1;
}

// ** TCPSocketListener::fetch
void TCPSocketListener::fetch( void )
{
    // ** Remove closed connections
	removeClosedConnections();

	// ** Setup FD sets
	fd_set read, write, except;
	s32 nfds = setupFDSets( read, write, except, m_descriptor );
    
    // ** Setup the timeout structure.
    timeval waitTime;
    waitTime.tv_sec  = 0;
    waitTime.tv_usec = 0;

	// ** Do a select
	if( select( nfds, &read, &write, &except, &waitTime ) <= 0 ) {
		if( Network::lastError() ) {
			LogError( "socket", "select failed, %s\n", Network::lastErrorMessage().c_str() );
		}
		return;
	}

	// ** Process listener socket
	if( FD_ISSET( m_descriptor, &read ) ) {
		TCPSocketPtr accepted = acceptConnection();
        m_clientSockets.push_back( accepted );

        // Emit the event
        notify<Accepted>( this, accepted );
	}
	else if( FD_ISSET( m_descriptor, &except ) ) {
		LogError( "socket", "error on listening socket: %d\n", m_descriptor.error() );
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
				socket->fetch();
			}
			if( FD_ISSET( descriptor, &write ) ) {
				FD_CLR( descriptor, &write );
			//	LogDebug( "socket", "writable socket handle %d\n", ( s32 )descriptor );
			}
		}

		if( hasError ) {
			s32 error = descriptor.error();
			if( error != 0 ) {
				LogError( "socket", "update socket error %\n", error );
			}
			socket->close();
		}
	}
}

// ** TCPSocketListener::removeClosedConnections
void TCPSocketListener::removeClosedConnections( void )
{
    for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ) {
		if( !(*i)->isValid() ) {
            i = m_clientSockets.erase( i );
        } else {
            ++i;
        }
    }
}

// ** TCPSocketListener::close
void TCPSocketListener::close( void )
{
	m_descriptor.close();
	m_clientSockets.clear();
    m_port = 0;
}
    
// ** TCPSocketListener::port
u16 TCPSocketListener::port( void ) const
{
    return m_port;
}

// ** TCPSocketListener::connections
const TCPSocketList& TCPSocketListener::connections( void ) const
{
	return m_clientSockets;
}

// ** TCPSocketListener::acceptConnection
TCPSocketPtr TCPSocketListener::acceptConnection( void )
{
	NetworkAddress	 address;
	SocketDescriptor descriptor = m_descriptor.accept( address );

	if( !descriptor.isValid() ) {
		return TCPSocketPtr();
	}

	descriptor.setNonBlocking();
    descriptor.setNoDelay();

    // Create socket instance and subscribe for events
    TCPSocketPtr socket( DC_NEW TCPSocket( descriptor, address ) );
    socket->subscribe<TCPSocket::Data>( dcThisMethod( TCPSocketListener::handleSocketData ) );
    socket->subscribe<TCPSocket::Closed>( dcThisMethod( TCPSocketListener::handleSocketClosed ) );

	return socket;
}

// ** TCPSocketListener::bindTo
TCPSocketListenerPtr TCPSocketListener::bindTo( u16 port )
{
	TCPSocketListenerPtr listener( DC_NEW TCPSocketListener );

	if( !listener->bind( port ) ) {
		return TCPSocketListenerPtr();
	}

	return listener;
}

// ** TCPSocketListener::bind
bool TCPSocketListener::bind( u16 port )
{
    m_descriptor = socket( PF_INET, SOCK_STREAM, 0 );

	if( !m_descriptor.isValid() ) {
		LogError( "socket", "failed to create socket, %d\n%s\n", Network::lastError(), Network::lastErrorMessage().c_str() );
		return false;
	}

	sockaddr_in addr = Network::toSockaddr( NetworkAddress::Null, port );
	s32 result = ::bind( m_descriptor, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result == SOCKET_ERROR ) {
		LogError( "socket", "bind failed, %d\n", Network::lastError() );
		return false;
	}

	// ** Set non blocking mode
	m_descriptor.setNonBlocking();

	// ** Set no delay
	m_descriptor.setNoDelay();

	// ** Listen for connections
    result = listen( m_descriptor, 16 );
   
	if( result == SOCKET_ERROR ) {
		LogError( "socket", "listen failed, %d\n", Network::lastError() );
		return false;
	}
    
    m_port = port;

    return true;
}

// ** TCPSocketListener::handleSocketClosed
void TCPSocketListener::handleSocketClosed( const TCPSocket::Closed& e )
{
    LogVerbose( "socket", "remote socket connection closed (remote address %s)", e.sender->address().toString() );
    notify<Closed>( this, e.sender );
}

// ** TCPSocketListener::handleSocketData
void TCPSocketListener::handleSocketData( const TCPSocket::Data& e )
{
    notify<Data>( this, e.sender, e.stream );
}

} // namespace net

DC_END_DREEMCHEST