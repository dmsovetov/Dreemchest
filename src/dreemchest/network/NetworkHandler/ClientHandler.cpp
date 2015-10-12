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

namespace net {

// -----------------------------------------ClientHandler --------------------------------------- //

// ** ClientHandler::ClientHandler
ClientHandler::ClientHandler( const TCPSocketPtr& socket )
{
	m_connection = createConnection( socket.get() );
	setPingRate( 500 );
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
ClientHandlerPtr ClientHandler::create( const NetworkAddress& address, u16 port )
{
	ClientSocketDelegate* clientDelegate = DC_NEW ClientSocketDelegate;
	TCPSocketPtr	      clientSocket   = TCPSocket::connectTo( address, port, clientDelegate );

	if( clientSocket == NULL ) {
		return ClientHandlerPtr();
	}

	ClientHandler* clientHandler    = DC_NEW ClientHandler( clientSocket );
	clientDelegate->m_clientHandler = clientHandler;

	return ClientHandlerPtr( clientHandler );
}

// ** ClientHandler::update
void ClientHandler::update( u32 dt )
{
	NetworkHandler::update( dt );
	m_connection->socket()->update();
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

// ** ClientHandler::processConnectionClosed
void ClientHandler::processConnectionClosed( TCPSocket* socket )
{
	m_eventEmitter.emit<ConnectionClosed>();
}

// ----------------------------------------- ClientSocketDelegate --------------------------------------- //

// ** ClientSocketDelegate::ClientSocketDelegate
ClientSocketDelegate::ClientSocketDelegate( ClientHandlerWPtr clientHandler ) : m_clientHandler( clientHandler )
{

}

// ** ClientSocketDelegate::handleClosed
void ClientSocketDelegate::handleClosed( TCPSocket* sender )
{
	if( !m_clientHandler.valid() ) {
		return;
	}

	log::verbose( "ClientSocketDelegate::handleClosed : connection closed\n" );
	m_clientHandler->processConnectionClosed( sender );
}

// ** ClientSocketDelegate::handleReceivedData
void ClientSocketDelegate::handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream )
{
	if( !m_clientHandler.valid() ) {
		return;
	}

	m_clientHandler->processReceivedData( sender, stream );
}

} // namespace net

DC_END_DREEMCHEST
