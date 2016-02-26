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

#include "NetworkHandler.h"
#include "Connection.h"
#include "../Sockets/UDPSocket.h"
#include "../Sockets/TCPSocket.h"

#define DEBUG_TTL_DISABLED	(1)

DC_BEGIN_DREEMCHEST

namespace net {

// ** NetworkHandler::NetworkHandler
NetworkHandler::NetworkHandler( void ) : m_pingSendRate( 0 ), m_pingTimeLeft( 0 ), m_keepAliveTime( 0 )
{
    DC_ABORT_IF( TypeInfo<NetworkHandler>::name() != String( "NetworkHandler" ), "the type info return an invalid name" );
    
	registerPacketHandler<packets::Ping>			  ( dcThisMethod( NetworkHandler::handlePingPacket ) );
	registerPacketHandler<packets::KeepAlive>		  ( dcThisMethod( NetworkHandler::handleKeepAlivePacket ) );
	registerPacketHandler<packets::Event>             ( dcThisMethod( NetworkHandler::handleEventPacket ) );
	registerPacketHandler<packets::DetectServers>	  ( dcThisMethod( NetworkHandler::handleDetectServersPacket ) );
	registerPacketHandler<packets::RemoteCall>        ( dcThisMethod( NetworkHandler::handleRemoteCallPacket ) );
	registerPacketHandler<packets::RemoteCallResponse>( dcThisMethod( NetworkHandler::handleRemoteCallResponsePacket ) );

	m_broadcastListener = UDPSocket::createBroadcast();

	setKeepAliveTime( 5 );
}

// ** NetworkHandler::setPingRate
void NetworkHandler::setPingRate( u32 value )
{
	m_pingSendRate = value;
}

// ** NetworkHandler::pingRate
u32 NetworkHandler::pingRate( void ) const
{
	return m_pingSendRate;
}

// ** NetworkHandler::setKeepAliveTime
void NetworkHandler::setKeepAliveTime( s32 value )
{
	m_keepAliveTime = value * 1000;
	m_keepAliveSendRate = static_cast<u32>( m_keepAliveTime * 0.8f );
}

// ** NetworkHandler::keepAliveTime
s32 NetworkHandler::keepAliveTime( void ) const
{
	return m_keepAliveTime / 1000;
}

// ** NetworkHandler::findConnectionBySocket
ConnectionPtr NetworkHandler::findConnectionBySocket( TCPSocketWPtr socket ) const
{
	ConnectionBySocket::const_iterator i = m_connections.find( socket );
	return i != m_connections.end() ? i->second : ConnectionPtr();
}

// ** NetworkHandler::createConnection
ConnectionPtr NetworkHandler::createConnection( TCPSocketWPtr socket )
{
	ConnectionPtr connection( DC_NEW Connection( this, socket ) );
	connection->setTimeToLive( m_keepAliveTime );
	m_connections[socket] = connection;
	return connection;
}

// ** NetworkHandler::removeConnection
void NetworkHandler::removeConnection( TCPSocketWPtr socket )
{
	m_connections.erase( socket );
}

// ** NetworkHandler::listenForBroadcasts
void NetworkHandler::listenForBroadcasts( u16 port )
{
	m_broadcastListener->listen( port );
}

// ** NetworkHandler::processReceivedData
void NetworkHandler::processReceivedData( TCPSocketWPtr socket, SocketDataWPtr data )
{
	using namespace Io;

	LogDebug( "socket", "%d bytes of data received from %s\n", data->bytesAvailable(), socket->address().toString() );

	// ** Find a connection by socket
	ConnectionPtr connection = findConnectionBySocket( socket );
	DC_ABORT_IF( !connection.valid(), "the socked does not have an associated connection" );

	connection->m_totalBytesReceived += data->bytesAvailable();

    ByteBufferPtr source( data );

	Serializables packets = BinarySerializer::read( source );

	for( Serializables::iterator i = packets.begin(), end = packets.end(); i != end; ++i ) {
		NetworkPacket* packet = i->get();

		LogDebug( "packet", "%s received from %s\n", packet->typeName(), socket->address().toString() );

		PacketHandlers::iterator j = m_packetHandlers.find( packet->typeId() );
		if( j == m_packetHandlers.end() ) {
			LogWarning( "packet", "unhandled packet of type %s received from %s\n", packet->typeName(), socket->address().toString() );
			continue;
		}

		if( !j->second->handle( connection, packet ) ) {
			LogWarning( "packet", "malformed packet of type %s received from %s\n", packet->typeName(), socket->address().toString() );
		}
	}

	LogDebug( "socket", "%d bytes from %s processed, %d bytes left in buffer\n", data->position(), socket->address().toString(), data->length() - data->position() );
	
	data->trimFromLeft( data->position() );
}

// ** NetworkHandler::eventListeners
ConnectionList NetworkHandler::eventListeners( void ) const
{
	return ConnectionList();
}

// ** NetworkHandler::handlePingPacket
bool NetworkHandler::handlePingPacket( ConnectionPtr& connection, packets::Ping& packet )
{
	if( packet.iterations ) {
		connection->send<packets::Ping>( packet.iterations - 1, packet.timestamp, connection->time() );
	} else {
		u32 rtt  = connection->time() - packet.timestamp;
		u32 time = packet.time + rtt / 2;

		if( abs( ( s64 )time - connection->time() ) > 50 ) {
			LogWarning( "connection", "%dms time error detected\n", time - connection->time() );
			connection->setTime( time );
		}

		
		connection->setRoundTripTime( rtt );
	}
	
	return true;
}

// ** NetworkHandler::handleKeepAlivePacket
bool NetworkHandler::handleKeepAlivePacket( ConnectionPtr& connection, packets::KeepAlive& packet )
{
	connection->setTimeToLive( m_keepAliveTime );
	return true;
}

// ** NetworkHandler::handleDetectServersPacket
bool NetworkHandler::handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet )
{
	return true;
}

// ** NetworkHandler::handleEventPacket
bool NetworkHandler::handleEventPacket( ConnectionPtr& connection, packets::Event& packet )
{
	// ** Find an event handler from this event id.
	EventHandlers::iterator i = m_eventHandlers.find( packet.eventId );

	if( i == m_eventHandlers.end() ) {
		LogWarning( "rpc", "unknown event %d received\n", packet.eventId );
		return false;
	}

	// ** Handle this event
	return i->second->handle( connection, packet );
}

// ** NetworkHandler::handleRemoteCallPacket
bool NetworkHandler::handleRemoteCallPacket( ConnectionPtr& connection, packets::RemoteCall& packet )
{
	// ** Find a remote call handler
	RemoteCallHandlers::iterator i = m_remoteCallHandlers.find( packet.method );

	if( i == m_remoteCallHandlers.end() ) {
		LogWarning( "rpc", "trying to invoke unknown remote procedure %d\n", packet.method );
		return false;
	}

	// ** Invoke a method
	return i->second->handle( connection, packet );
}

// ** NetworkHandler::handleRemoteCallResponsePacket
bool NetworkHandler::handleRemoteCallResponsePacket( ConnectionPtr& connection, packets::RemoteCallResponse& packet )
{
	return connection->handleResponse( packet );
}

// ** NetworkHandler::update
void NetworkHandler::update( u32 dt )
{
	bool sendPing = false;

	if( m_pingSendRate ) {
		m_pingTimeLeft -= dt;

		if( m_pingTimeLeft <= 0 ) {
			m_pingTimeLeft += m_pingSendRate;
			sendPing = true;
		}
	}

	// Update all connections
	ConnectionBySocket connections = m_connections;

	for( ConnectionBySocket::iterator i = connections.begin(); i != connections.end(); ++i ) {
	#if !DEBUG_TTL_DISABLED
		if( m_keepAliveTime && i->second->timeToLive() <= 0 ) {
			i->second->socket()->close();
			continue;
		}
	#endif

		if( sendPing ) {
			i->second->send<packets::Ping>( 1, i->second->time() );
		}

	#if !DEBUG_TTL_DISABLED
		if( (i->second->time() - i->second->keepAliveTimestamp()) > m_keepAliveSendRate ) {
			i->second->send<packets::KeepAlive>();
			i->second->setKeepAliveTimestamp( i->second->time() );
		}
	#endif

		i->second->update( dt );
	}
//	if( m_broadcastListener != NULL ) {
//		m_broadcastListener->update();
//	}
}

} // namespace net

DC_END_DREEMCHEST