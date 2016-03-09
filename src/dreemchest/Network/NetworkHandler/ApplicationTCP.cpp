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

#include "ApplicationTCP.h"

#include "../Sockets/TCPSocket.h"
#include "../Sockets/TCPSocketListener.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** ApplicationTCP:ApplicationTCP
ApplicationTCP::ApplicationTCP( TCPSocketPtr socket ) : m_socket( socket ), m_workaroundClientConn( false )
{
    DC_ABORT_IF( !socket.valid(), "invalid TCP socket" );
    m_socket->subscribe<TCPSocket::Connected>( dcThisMethod( ApplicationTCP::handleSocketConnected ) );
    m_socket->subscribe<TCPSocket::Closed>( dcThisMethod( ApplicationTCP::handleSocketClosed ) );
}

// ** ApplicationTCP::~ApplicationTCP
ApplicationTCP::~ApplicationTCP( void )
{
    m_socket->unsubscribe<TCPSocket::Connected>( dcThisMethod( ApplicationTCP::handleSocketConnected ) );
    m_socket->unsubscribe<TCPSocket::Closed>( dcThisMethod( ApplicationTCP::handleSocketClosed ) );
}

// ** ApplicationTCP::connect
ApplicationTCPPtr ApplicationTCP::connect( const Address& address, u16 port )
{
    // Connect socket to a remote host
    TCPSocketPtr socket = TCPSocket::connectTo( address, port );

    // Failed to connect - can't create TCP application instance
    if( !socket.valid() ) {
        return ApplicationTCPPtr();
    }

    // WORKAROUND
    ApplicationTCP* app = DC_NEW ApplicationTCP( socket );
    app->m_workaroundClientConn = true;

    return ApplicationTCPPtr( app );
}

// ** ApplicationTCP::listen
ApplicationTCPPtr ApplicationTCP::listen( u16 port )
{
    // Bind listening socket to a port
    TCPSocketPtr socket = TCPSocketListener::bindTo( port );

    // Failed to connect - can't create TCP application instance
    if( !socket.valid() ) {
        return ApplicationTCPPtr();
    }

    return ApplicationTCPPtr( DC_NEW ApplicationTCP( socket ) );
}

// ** ApplicationTCP::update
void ApplicationTCP::update( u32 dt )
{
    // Update the base application
    Application::update( dt );

    if( m_workaroundClientConn ) {
        handleSocketConnected( TCPSocket::Connected( m_socket ) );
        m_workaroundClientConn = false;
    }

    // Receive data from a socket
    m_socket->recv();
}

// ** ApplicationTCP::handleSocketConnected
void ApplicationTCP::handleSocketConnected( const TCPSocket::Connected& e )
{
    // Create a connection instance for this socket
    ConnectionWPtr connection = createConnection( e.sender );

    // Register this connection
    m_connectionBySocket[e.sender] = connection;

    // Notify listeners about a new connection
    notify<Connected>( this, connection );
}

// ** ApplicationTCP::handleSocketClosed
void ApplicationTCP::handleSocketClosed( const TCPSocket::Closed& e )
{
    // Find the connection by a socket instance
    ConnectionBySocket::iterator i = m_connectionBySocket.find( e.sender );
    DC_ABORT_IF( i == m_connectionBySocket.end(), "no connection associated with this socket instance" );

    // Notify listeners about a disconnection
    notify<Disconnected>( this, i->second );

    // Remove the connection
    removeConnection( i->second );

    // Unregister the connection
    m_connectionBySocket.erase( i );
}

} // namespace Network

DC_END_DREEMCHEST