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

#include "NetworkApplication.h"
#include "Connection.h"
#include "../Sockets/UDPSocket.h"
#include "../Sockets/TCPSocket.h"

#include "../Packets/Ping.h"
#include "../Packets/RemoteCall.h"

#define DEBUG_TTL_DISABLED	(1)

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Application::Application
Application::Application( void ) : m_pingSendRate( 0 ), m_pingTimeLeft( 0 ), m_keepAliveTime( 0 )
{
    DC_ABORT_IF( TypeInfo<Application>::name() != String( "Application" ), "the type info return an invalid name" );
    
#if DEV_DEPRECATED_PACKETS
	registerPacketHandler<packets::Ping>			  ( dcThisMethod( Application::handlePingPacket ) );
	registerPacketHandler<packets::KeepAlive>		  ( dcThisMethod( Application::handleKeepAlivePacket ) );
	registerPacketHandler<packets::Event>             ( dcThisMethod( Application::handleEventPacket ) );
	registerPacketHandler<packets::DetectServers>	  ( dcThisMethod( Application::handleDetectServersPacket ) );
	registerPacketHandler<packets::RemoteCall>        ( dcThisMethod( Application::handleRemoteCallPacket ) );
	registerPacketHandler<packets::RemoteCallResponse>( dcThisMethod( Application::handleRemoteCallResponsePacket ) );
#else
    addPacketHandler< PacketHandlerCallback<Packets::Event> >( dcThisMethod( Application::handleEventPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::Ping> >( dcThisMethod( Application::handlePingPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::RemoteCall> >( dcThisMethod( Application::handleRemoteCallPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::RemoteCallResponse> >( dcThisMethod( Application::handleRemoteCallResponsePacket ) );
#endif  /*  DEV_DEPRECATED_PACKETS  */

	setKeepAliveTime( 5 );
}

// ** Application::setPingRate
void Application::setPingRate( u32 value )
{
	m_pingSendRate = value;
}

// ** Application::pingRate
u32 Application::pingRate( void ) const
{
	return m_pingSendRate;
}

// ** Application::setKeepAliveTime
void Application::setKeepAliveTime( s32 value )
{
	m_keepAliveTime = value * 1000;
	m_keepAliveSendRate = static_cast<u32>( m_keepAliveTime * 0.8f );
}

// ** Application::keepAliveTime
s32 Application::keepAliveTime( void ) const
{
	return m_keepAliveTime / 1000;
}

// ** Application::findConnectionBySocket
ConnectionPtr Application::findConnectionBySocket( TCPSocketWPtr socket ) const
{
	ConnectionBySocket::const_iterator i = m_connections.find( socket );
	return i != m_connections.end() ? i->second : ConnectionPtr();
}

// ** Application::createConnection
ConnectionPtr Application::createConnection( TCPSocketWPtr socket )
{
	ConnectionPtr connection( DC_NEW Connection( this, socket ) );
	connection->setTimeToLive( m_keepAliveTime );
	m_connections[socket] = connection;

    // Subscribe for connection events.
    connection->subscribe<Connection::Received>( dcThisMethod( Application::handlePacketReceived ) );

	return connection;
}

// ** Application::removeConnection
void Application::removeConnection( TCPSocketWPtr socket )
{
    // Find a connection by socket
    ConnectionPtr connection = findConnectionBySocket( socket );
    DC_ABORT_IF( !connection.valid(), "no connection associated with this socket instance" );

    // Unsubscribe from a connection events
    connection->unsubscribe<Connection::Received>( dcThisMethod( Application::handlePacketReceived ) );

    // Remove from a connections container
	m_connections.erase( socket );
}

// ** Application::eventListeners
ConnectionList Application::eventListeners( void ) const
{
	return ConnectionList();
}

#if DEV_DEPRECATED_PACKETS

// ** Application::handlePingPacket
bool Application::handlePingPacket( ConnectionPtr& connection, packets::Ping& packet )
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

// ** Application::handleKeepAlivePacket
bool Application::handleKeepAlivePacket( ConnectionPtr& connection, packets::KeepAlive& packet )
{
	connection->setTimeToLive( m_keepAliveTime );
	return true;
}

// ** Application::handleDetectServersPacket
bool Application::handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet )
{
	return true;
}

// ** Application::handleEventPacket
bool Application::handleEventPacket( ConnectionPtr& connection, packets::Event& packet )
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

// ** Application::handleRemoteCallPacket
bool Application::handleRemoteCallPacket( ConnectionPtr& connection, packets::RemoteCall& packet )
{
	if( i == m_remoteCallHandlers.end() ) {
	// ** Find a remote call handler
	RemoteCallHandlers::iterator i = m_remoteCallHandlers.find( packet.method );

		LogWarning( "rpc", "trying to invoke unknown remote procedure %d\n", packet.method );
		return false;
	}

	// ** Invoke a method
	return i->second->handle( connection, packet );
}

// ** Application::handleRemoteCallResponsePacket
bool Application::handleRemoteCallResponsePacket( ConnectionPtr& connection, packets::RemoteCallResponse& packet )
{
	return connection->handleResponse( packet );
}

#else

// ** Application::handlePingPacket
void Application::handlePingPacket( ConnectionWPtr connection, const Packets::Ping& ping )
{
	if( ping.iterations ) {
		connection->send<Packets::Ping>( ping.iterations - 1, ping.timestamp, connection->time() );
	} else {
		u32 rtt  = connection->time() - ping.timestamp;
		u32 time = ping.time + rtt / 2;

		if( abs( ( s64 )time - connection->time() ) > 50 ) {
			LogWarning( "connection", "%dms time error detected\n", time - connection->time() );
			connection->setTime( time );
		}

		
		connection->setRoundTripTime( rtt );
	}
}

// ** Application::handleRemoteCallPacket
void Application::handleRemoteCallPacket( ConnectionWPtr connection, const Packets::RemoteCall& packet )
{
	// Find a remote call handler
	RemoteCallHandlers::iterator i = m_remoteCallHandlers.find( packet.method );

	if( i == m_remoteCallHandlers.end() ) {
		LogWarning( "rpc", "trying to invoke unknown remote procedure %d\n", packet.method );
		return;
	}

	// Invoke a method
	i->second->handle( connection, packet );
}

// ** Application::handleRemoteCallResponsePacket
void Application::handleRemoteCallResponsePacket( ConnectionWPtr connection, const Packets::RemoteCallResponse& packet )
{
    connection->handleResponse( packet );
}

// ** Application::handleEventPacket
void Application::handleEventPacket( ConnectionWPtr connection, const Packets::Event& packet )
{
	// Find an event handler from this event id.
	EventHandlers::iterator i = m_eventHandlers.find( packet.eventId );

	if( i == m_eventHandlers.end() ) {
		LogWarning( "rpc", "unknown event %d received\n", packet.eventId );
		return;
	}

	// Handle this event
	i->second->handle( connection, packet );
}

#endif  /*  DEV_DEPRECATED_PACKETS  */

// ** Application::handlePacketReceived
void Application::handlePacketReceived( const Connection::Received& e )
{
#if DEV_DEPRECATED_PACKETS
    // Get the packet and connection from an event
    PacketUPtr    packet     = e.packet;
    ConnectionPtr connection = static_cast<Connection*>( e.sender.get() );

    // Find corresponding packet handler
	PacketHandlers::iterator j = m_packetHandlers.find( packet->typeId() );

    // No handler for this type of packet
	if( j == m_packetHandlers.end() ) {
		LogWarning( "packet", "unhandled packet of type %s received from %s\n", packet->typeName(), connection->address().toString() );
		return;
	}

    // Handle the packet
	if( !j->second->handle( connection, packet.get() ) ) {
		LogWarning( "packet", "malformed packet of type %s received from %s\n", packet->typeName(), connection->address().toString() );
	}
#else
    // Type cast the connection instance
    ConnectionWPtr connection = static_cast<Connection*>( e.sender.get() );

    // Create instance of a network packet
	PacketUPtr packet = m_packetFactory.construct( e.type );

    // The packet type is unknown - skip it
	if( packet == NULL ) {
        LogDebug( "packet", "packet of unknown type %d received, %d bytes skipped\n", e.type, e.packet->bytesAvailable() );
		return;
	}

    // Get the packet stream
    Io::ByteBufferWPtr stream = e.packet;

	// Read the packet data from a stream
    s32 position = stream->position();
	packet->deserialize( stream );
	s32 bytesRead = stream->position() - position;
    DC_BREAK_IF( bytesRead != e.size, "packet size mismatch" );

	// Find all handlers that are eligible to process this type of packet
    PacketHandlers::iterator i = m_packetHandlers.find( e.type );
    if( i == m_packetHandlers.end() ) {
        return;
    }

    for( PacketHandlerList::iterator j = i->second.begin(), end = i->second.end(); j != end; ++j ) {
        (*j)->process( connection, *packet );
    }
#endif  /*  DEV_DEPRECATED_PACKETS  */
}

// ** Application::update
void Application::update( u32 dt )
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
			i->second->send<Packets::Ping>( 1, i->second->time() );
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

} // namespace Network

DC_END_DREEMCHEST