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

#include "Client.h"
#include "Connection.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Client::Client
Client::Client( TCPSocketPtr socket )
{
    // Create connection
	m_connection = createConnection( socket.get() );

    // Subscribe for connection events
    m_connection->subscribe<Connection::Closed>( dcThisMethod( Client::handleConnectionClosed ) );

    // Set the default ping rate
//	setPingRate( 500 );
}

Client::~Client( void )
{
    // Subscribe from connection events
    m_connection->unsubscribe<Connection::Closed>( dcThisMethod( Client::handleConnectionClosed ) );
}

// ** Client::connection
const ConnectionPtr& Client::connection( void ) const
{
	return m_connection;
}

// ** Client::connection
ConnectionPtr& Client::connection( void )
{
	return m_connection;
}

// ** Client::create
ClientPtr Client::create( const Address& address, u16 port )
{
	TCPSocketPtr clientSocket = TCPSocket::connectTo( address, port );

	if( clientSocket == NULL ) {
		return ClientPtr();
	}

	return ClientPtr( DC_NEW Client( clientSocket ) );
}

// ** Client::update
void Client::update( u32 dt )
{
	Application::update( dt );
	m_connection->socket()->recv();
}

// ** Client::eventListeners
ConnectionList Client::eventListeners( void ) const
{
	return ConnectionList();
}

// ** Client::detectServers
bool Client::detectServers( u16 port )
{
	return true;
}

// ** Client::handleConnectionClosed
void Client::handleConnectionClosed( const Connection::Closed& e )
{
    LogVerbose( "handler", "client connection closed\n" );
	notify<ConnectionClosed>();
}

} // namespace Network

DC_END_DREEMCHEST
