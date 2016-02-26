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

#include "ClientHandler.h"
#include "Connection.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** ClientHandler::ClientHandler
ClientHandler::ClientHandler( TCPSocketPtr socket )
{
    // Create connection
	m_connection = createConnection( socket.get() );

    // Subscribe for socket events
    socket->subscribe<TCPSocket::Closed>( dcThisMethod( ClientHandler::handleSocketClosed ) );
    socket->subscribe<TCPSocket::Data>( dcThisMethod( ClientHandler::handleSocketData ) );

    // Set the default ping rate
	setPingRate( 500 );
}

ClientHandler::~ClientHandler( void )
{
    // Get the socket from a connection
    TCPSocketWPtr socket = m_connection->socket();

    // Unsubscibe from socket events
    socket->unsubscribe<TCPSocket::Closed>( dcThisMethod( ClientHandler::handleSocketClosed ) );
    socket->unsubscribe<TCPSocket::Data>( dcThisMethod( ClientHandler::handleSocketData ) );
}

// ** ClientHandler::connection
const ConnectionPtr& ClientHandler::connection( void ) const
{
	return m_connection;
}

// ** ClientHandler::connection
ConnectionPtr& ClientHandler::connection( void )
{
	return m_connection;
}

// ** ClientHandler::create
ClientHandlerPtr ClientHandler::create( const Address& address, u16 port )
{
	TCPSocketPtr clientSocket = TCPSocket::connectTo( address, port );

	if( clientSocket == NULL ) {
		return ClientHandlerPtr();
	}

	return ClientHandlerPtr( DC_NEW ClientHandler( clientSocket ) );
}

// ** ClientHandler::update
void ClientHandler::update( u32 dt )
{
	NetworkHandler::update( dt );
	m_connection->socket()->recv();
}

// ** ClientHandler::eventListeners
ConnectionList ClientHandler::eventListeners( void ) const
{
	return ConnectionList();
}

// ** ClientHandler::detectServers
bool ClientHandler::detectServers( u16 port )
{
	return true;
}

// ** ClientHandler::handleSocketClosed
void ClientHandler::handleSocketClosed( const TCPSocket::Closed& e )
{
    LogVerbose( "socket", "client connection closed\n" );
	notify<ConnectionClosed>();
}

// ** ClientHandler::handleData
void ClientHandler::handleSocketData( const TCPSocket::Data& e )
{
    processReceivedData( e.sender, e.data );
}

} // namespace Network

DC_END_DREEMCHEST
