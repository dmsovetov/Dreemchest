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
ClientHandler::ClientHandler( const TCPSocketPtr& socket ) : m_connection( Connection::create( this, socket ) )
{

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
void ClientHandler::update( void )
{
	NetworkHandler::update();
	m_connection->socket()->update();
}

// ** ClientHandler::eventListeners
TCPSocketList ClientHandler::eventListeners( void ) const
{
	return TCPSocketList();
}

// -----------------------------------------ClientSocketDelegate --------------------------------------- //

// ** ClientSocketDelegate::handleClosed
void ClientSocketDelegate::handleClosed( TCPSocket* sender )
{

}

// ** ClientSocketDelegate::handleReceivedData
void ClientSocketDelegate::handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream )
{
	DC_BREAK_IF( m_clientHandler->connection()->socket()->descriptor() != socket->descriptor() );
	m_clientHandler->processReceivedData( m_clientHandler->connection(), stream );
}

/*
// ** NetworkClientHandler::GetServerTime
int NetworkClientHandler::GetServerTime( void ) const
{
    return m_serverTime;
}

// ** NetworkClientHandler::Connect
bool NetworkClientHandler::Connect( const NetworkAddress& address, u16 port )
{
    if( IsConnected() ) {
        DC_BREAK;
        return false;
    }

    // ** Connect sockets
    bool tcpConnected = m_netTCP->Connect( address, port );

    if( tcpConnected ) {
        ProcessConnection( address );
        return true;
    }
    
    ProcessFailure();
    
    return false;
}

// ** NetworkClientHandler::ProcessConnection
void NetworkClientHandler::ProcessConnection( const NetworkAddress& address )
{
    NetworkHandler::ProcessConnection( address );

    m_serverTime = -1;
    m_timeSync.SyncTo( &m_serverTime, m_ctx->iTime, this, 10, -1 );
}

// ** NetworkClientHandler::ProcessReceivedPacket
void NetworkClientHandler::ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, int connection )
{
    switch( packetId ) {
    default:    NetworkHandler::ProcessReceivedPacket( packetId, packet, address, connection );
                break;
    }
}

// ** NetworkClientHandler::Update
bool NetworkClientHandler::Update( int dt )
{
    if( !NetworkHandler::Update( dt ) ) {
        return false;
    }

    if( !m_timeSync.IsReady() ) {
        return true;
    }

    m_serverTime += dt;
    return true;
}
*/

} // namespace net

DC_END_DREEMCHEST
