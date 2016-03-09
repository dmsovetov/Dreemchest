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

#include "../Connection/ConnectionMiddleware.h"

#include "../Sockets/UDPSocket.h"
#include "../Sockets/TCPSocket.h"

#include "../Packets/Ping.h"
#include "../Packets/RemoteCall.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Application::Application
Application::Application( void )
    : m_nextConnectionId( 1 )
{
    DC_ABORT_IF( TypeInfo<Application>::name() != String( "Application" ), "the type info return an invalid name" );
    
    addPacketHandler< PacketHandlerCallback<Packets::Event> >( dcThisMethod( Application::handleEventPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::Ping> >( dcThisMethod( Application::handlePingPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::RemoteCall> >( dcThisMethod( Application::handleRemoteCallPacket ) );
    addPacketHandler< PacketHandlerCallback<Packets::RemoteCallResponse> >( dcThisMethod( Application::handleRemoteCallResponsePacket ) );
}

// ** Application::createConnection
ConnectionPtr Application::createConnection( TCPSocketWPtr socket )
{
	// Create the connection instance and add it to an active connections set
	ConnectionPtr connection( DC_NEW Connection( this, socket ) );
	m_connections.insert( connection );

    // Subscribe for connection events.
    connection->subscribe<Connection::Received>( dcThisMethod( Application::handlePacketReceived ) );
	connection->subscribe<Connection::Closed>( dcThisMethod( Application::handleConnectionClosed ) );

    // Assigned the connection id
    connection->setId( m_nextConnectionId++ );

    LogVerbose( "connection", "connection #%d accepted from %s (%d active connections)\n", connection->id(), connection->address().toString(), m_connections.size() );

	return connection;
}

// ** Application::removeConnection
void Application::removeConnection( ConnectionWPtr connection )
{
    LogVerbose( "connection", "connection #%d closed (%d active connections)\n", connection->id(), m_connections.size() - 1 );

    // Unsubscribe from a connection events
    connection->unsubscribe<Connection::Received>( dcThisMethod( Application::handlePacketReceived ) );
	connection->unsubscribe<Connection::Closed>( dcThisMethod( Application::handleConnectionClosed ) );

    // Remove from a connections container
	m_connections.erase( connection );
}

// ** Application::eventListeners
ConnectionList Application::eventListeners( void ) const
{
	return ConnectionList();
}

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

// ** Application::handlePacketReceived
void Application::handlePacketReceived( const Connection::Received& e )
{
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
}

// ** Application::handleConnectionClosed
void Application::handleConnectionClosed( const Connection::Closed& e )
{
	// Remove this connection from list
	removeConnection( static_cast<Connection*>( e.sender.get() ) );
}

// ** Application::update
void Application::update( u32 dt )
{
	// Update all connections
	ConnectionSet connections = m_connections;

	for( ConnectionSet::iterator i = connections.begin(); i != connections.end(); ++i ) {
		// Get the connection instance from an iterator
		ConnectionPtr connection = *i;

		// Update the connection instance
		connection->update( dt );

		// Close this connection if it was queued for closing
		if( connection->willBeClosed() ) {
			connection->close();
		}
	}
}

} // namespace Network

DC_END_DREEMCHEST