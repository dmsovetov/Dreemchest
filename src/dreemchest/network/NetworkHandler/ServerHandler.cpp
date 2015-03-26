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
void ServerHandler::update( void )
{
	NetworkHandler::update();
	m_socketListener->update();
}

// ** ServerHandler::handleDetectServersPacket
bool ServerHandler::handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet )
{
	return true;
}

// ** ServerHandler::processClientConnection
void ServerHandler::processClientConnection( TCPSocket* socket )
{
	log::verbose( "Client %s connected to server\n", socket->address().toString() );

	ConnectionPtr connection = createConnection( socket );
	connection->send<packets::Time>();
}

// ** ServerHandler::processClientDisconnection
void ServerHandler::processClientDisconnection( TCPSocket* socket )
{
	log::verbose( "Client %s dicconnected from server\n", socket->address().toString() );
	removeConnection( socket );
}

// ** ServerHandler::eventListeners
ConnectionList ServerHandler::eventListeners( void ) const
{
    const TCPSocketList& sockets = m_socketListener->connections();
    
    ConnectionList connections;
    for( TCPSocketList::const_iterator i = sockets.begin(), end = sockets.end(); i != end; ++i ) {
		ConnectionPtr connection = findConnectionBySocket( const_cast<TCPSocket*>( i->get() ) );
		if( connection == ConnectionPtr() ) {
			log::msg( "ServerHandler::eventListeners : null\n" );
			continue;
		}

        connections.push_back( connection );
    }
    
	return connections;
}

// ** ServerHandler::handleTimePacket
bool ServerHandler::handleTimePacket( ConnectionPtr& connection, packets::Time& packet )
{
	s32 roundTripTime = UnixTime() - packet.timestamp;
	
	if( abs( packet.roundTripTime - roundTripTime ) > 1 ) {
		packet.roundTripTime = roundTripTime;
		return NetworkHandler::handleTimePacket( connection, packet );
	}

	return true;
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

/*
// ** NetworkServerHandler::NetworkServerHandler
NetworkServerHandler::NetworkServerHandler( void ) : m_time( 0 )
{

}

NetworkServerHandler::~NetworkServerHandler( void )
{

}

// ** NetworkServerHandler::Start
bool NetworkServerHandler::Start( u16 port )
{
    if( IsConnected() ) {
        DC_BREAK
        return false;
    }

    // ** Connect sockets
    bool tcpConnected = m_netTCP->Connect( NULL, port );

    // ** Check connection
    if( !tcpConnected ) {
        ProcessFailure();
        return false;
    }

    // ** Attach listeners
    m_netTCP->AttachListener( TCPSocketEvent::ConnectionAccepted,   dcThisMethod( NetworkServerHandler::OnConnectionAccepted  ) );
    m_netTCP->AttachListener( TCPSocketEvent::ConnectionClosed,     dcThisMethod( NetworkServerHandler::OnConnectionClosed    ) );

    // ** Process successfull connection
    ProcessConnection( NetworkAddress::Localhost );

    return true;
}

// ** NetworkServerHandler::Update
bool NetworkServerHandler::Update( int dt )
{
    if( !NetworkHandler::Update( dt ) ) {
        return false;
    }

    m_time += dt;
    return true;
}

// ** NetworkServerHandler::ProcessAcceptedConnection
void NetworkServerHandler::ProcessAcceptedConnection( const NetworkAddress& address, int connection )
{
	DC_BREAK
//    DispatchEvent( NetworkHandlerEvent::ConnectionAccepted, NetworkHandlerEvent::OnConnectionAccepted( connection, address ) );
}

// ** NetworkServerHandler::ProcessClosedConnection
void NetworkServerHandler::ProcessClosedConnection( int connection )
{
	DC_BREAK
//    DispatchEvent( NetworkHandlerEvent::ConnectionClosed, NetworkHandlerEvent::OnConnectionClosed( connection ) );
}

// ** NetworkServerHandler::ProcessReceivedPacket
void NetworkServerHandler::ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, int connection )
{
    switch( packetId ) {
    case sTimeSyncPacket::PacketId: {
                                        DC_ENSURE_TYPE( packet, sTimeSyncPacket );
                                        sTimeSyncPacket sync = *( sTimeSyncPacket* )packet;

                                        // ** Send pong packet
                                        sync.localTime = m_time;
                                        SendPacket( sTimeSyncPacket::PacketId, &sync, connection );
                                    }
                                    break;

    default:    NetworkHandler::ProcessReceivedPacket( packetId, packet, address, connection );
                break;
    }
}

// ** NetworkServerHandler::OnConnectionAccepted
void NetworkServerHandler::OnConnectionAccepted( const dcEvent e )
{
    dcTCPSocketEvent se = ( dcTCPSocketEvent )e;
    
    ProcessAcceptedConnection( se->m_address, se->m_connection );
}

// ** NetworkServerHandler::OnConnectionClosed
void NetworkServerHandler::OnConnectionClosed( const dcEvent e )
{
    ProcessClosedConnection( static_cast<dcTCPSocketEvent>( e )->m_connection );
}*/

} // namespace net

DC_END_DREEMCHEST
