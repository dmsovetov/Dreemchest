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

#include "ServerHandler.h"
#include "Connection.h"

DC_BEGIN_DREEMCHEST

namespace net {

// --------------------------------------- ServerHandler -------------------------------------- //

// ** ServerHandler::ServerHandler
ServerHandler::ServerHandler( const TCPSocketListenerPtr& socketListener ) : m_socketListener( socketListener )
{
}

// ** ServerHandler::create
ServerHandlerPtr ServerHandler::create( u16 port )
{
	ServerSocketDelegate* serverDelegate = DC_NEW ServerSocketDelegate;
	TCPSocketListenerPtr  socketListener = TCPSocketListener::bindTo( port, serverDelegate );

	if( socketListener == NULL ) {
		return ServerHandlerPtr();
	}

	ServerHandler* serverHandler    = DC_NEW ServerHandler( socketListener );
	serverDelegate->m_serverHandler = serverHandler;

	return ServerHandlerPtr( serverHandler );
}

// ** ServerHandler::update
void ServerHandler::update( u32 dt )
{
	NetworkHandler::update( dt );
	m_socketListener->fetch();
}

// ** ServerHandler::handleDetectServersPacket
bool ServerHandler::handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet )
{
	return true;
}

// ** ServerHandler::processClientConnection
void ServerHandler::processClientConnection( TCPSocket* socket )
{
	LogVerbose( "handler", "connection accepted from %s\n", socket->address().toString() );
	ConnectionPtr connection = createConnection( socket );

	m_eventEmitter.notify<ClientConnected>( connection );
}

// ** ServerHandler::processClientDisconnection
void ServerHandler::processClientDisconnection( TCPSocket* socket )
{
	ConnectionPtr connection = findConnectionBySocket( socket );

	LogVerbose( "handler", "connection closed %s\n", socket->address().toString() );
	removeConnection( socket );

	m_eventEmitter.notify<ClientDisconnected>( connection );
}

// ** ServerHandler::eventListeners
ConnectionList ServerHandler::eventListeners( void ) const
{
    const TCPSocketList& sockets = m_socketListener->connections();
    
    ConnectionList connections;
    for( TCPSocketList::const_iterator i = sockets.begin(), end = sockets.end(); i != end; ++i ) {
		ConnectionPtr connection = findConnectionBySocket( const_cast<TCPSocket*>( i->get() ) );
		if( connection == ConnectionPtr() ) {
			LogDebug( "handler", "ServerHandler::eventListeners : null\n" );
			continue;
		}

        connections.push_back( connection );
    }
    
	return connections;
}

// ---------------------------------------- ServerSocketDelegate ------------------------------------//

// ** ServerSocketDelegate::handleReceivedData
void ServerSocketDelegate::handleReceivedData( TCPSocketListener* sender, TCPSocket* socket, TCPStream* stream )
{
	m_serverHandler->processReceivedData( socket, stream );
}

// ** ServerSocketDelegate::handleConnectionAccepted
void ServerSocketDelegate::handleConnectionAccepted( TCPSocketListener* sender, TCPSocket* socket )
{
	m_serverHandler->processClientConnection( socket );
}

// ** ServerSocketDelegate::handleConnectionClosed
void ServerSocketDelegate::handleConnectionClosed( TCPSocketListener* sender, TCPSocket* socket )
{
	m_serverHandler->processClientDisconnection( socket );
}

} // namespace net

DC_END_DREEMCHEST
