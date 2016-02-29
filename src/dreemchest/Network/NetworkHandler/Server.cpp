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

#include "Server.h"
#include "Connection.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// --------------------------------------- Server -------------------------------------- //

// ** Server::Server
Server::Server( TCPSocketListenerPtr socketListener ) : m_socketListener( socketListener )
{
    m_socketListener->subscribe<TCPSocketListener::Accepted>( dcThisMethod( Server::handleConnectionAccepted ) );
    m_socketListener->subscribe<TCPSocketListener::Closed>( dcThisMethod( Server::handleConnectionClosed ) );
}

// ** Server::create
ServerPtr Server::create( u16 port )
{
	TCPSocketListenerPtr socketListener = TCPSocketListener::bindTo( port );

	if( socketListener == NULL ) {
		return ServerPtr();
	}

	return ServerPtr( DC_NEW Server( socketListener ) );
}

// ** Server::update
void Server::update( u32 dt )
{
	Application::update( dt );
	m_socketListener->recv();
}

#if DEV_DEPRECATED_PACKETS
// ** Server::handleDetectServersPacket
bool Server::handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet )
{
	return true;
}
#endif

// ** Server::handleConnectionAccepted
void Server::handleConnectionAccepted( const TCPSocketListener::Accepted& e )
{
	LogVerbose( "handler", "connection accepted from %s\n", e.socket->address().toString() );
	ConnectionPtr connection = createConnection( e.socket );

	notify<ClientConnected>( connection );
}

// ** Server::handleConnectionClosed
void Server::handleConnectionClosed( const TCPSocketListener::Closed& e )
{
	ConnectionPtr connection = findConnectionBySocket( e.socket );

	LogVerbose( "handler", "connection closed %s\n", e.socket->address().toString() );
	removeConnection( e.socket );

	notify<ClientDisconnected>( connection );
}

// ** Server::eventListeners
ConnectionList Server::eventListeners( void ) const
{
    const TCPSocketList& sockets = m_socketListener->connections();
    
    ConnectionList connections;
    for( TCPSocketList::const_iterator i = sockets.begin(), end = sockets.end(); i != end; ++i ) {
		ConnectionPtr connection = findConnectionBySocket( const_cast<TCPSocket*>( i->get() ) );
		if( connection == ConnectionPtr() ) {
			LogDebug( "handler", "Server::eventListeners : null\n" );
			continue;
		}

        connections.push_back( connection );
    }
    
	return connections;
}

} // namespace Network

DC_END_DREEMCHEST
