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
#include "../Sockets/TCPStream.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** NetworkHandler::NetworkHandler
NetworkHandler::NetworkHandler( void )
{
    DC_BREAK_IF( TypeInfo<NetworkHandler>::name() != String( "NetworkHandler" ) );
    
	registerPacketHandler<packets::Time>			  ( dcThisMethod( NetworkHandler::handleTimePacket ) );
	registerPacketHandler<packets::Event>             ( dcThisMethod( NetworkHandler::handleEventPacket ) );
	registerPacketHandler<packets::DetectServers>	  ( dcThisMethod( NetworkHandler::handleDetectServersPacket ) );
	registerPacketHandler<packets::RemoteCall>        ( dcThisMethod( NetworkHandler::handleRemoteCallPacket ) );
	registerPacketHandler<packets::RemoteCallResponse>( dcThisMethod( NetworkHandler::handleRemoteCallResponsePacket ) );

	m_broadcastListener = UDPSocket::createBroadcast( DC_NEW UDPSocketNetworkDelegate( this ) );
}

// ** NetworkHandler::findConnectionBySocket
ConnectionPtr NetworkHandler::findConnectionBySocket( TCPSocket* socket ) const
{
	ConnectionBySocket::const_iterator i = m_connections.find( socket );
	return i != m_connections.end() ? i->second : ConnectionPtr();
}

// ** NetworkHandler::createConnection
ConnectionPtr NetworkHandler::createConnection( TCPSocket* socket )
{
	ConnectionPtr connection( DC_NEW Connection( this, socket ) );
	m_connections[socket] = connection;
	return connection;
}

// ** NetworkHandler::removeConnection
void NetworkHandler::removeConnection( TCPSocket* socket )
{
	m_connections.erase( socket );
}

// ** NetworkHandler::listenForBroadcasts
void NetworkHandler::listenForBroadcasts( u16 port )
{
	m_broadcastListener->listen( port );
}

// ** NetworkHandler::currentTime
UnixTime NetworkHandler::currentTime( void ) const
{
	return UnixTime::current();
}

// ** NetworkHandler::processReceivedData
void NetworkHandler::processReceivedData( TCPSocket* socket, TCPStream* stream )
{
	using namespace io;

	log::verbose( "%d bytes of data received from %s\n", stream->bytesAvailable(), socket->address().toString() );

	// ** Find a connection by socket
	ConnectionPtr connection = findConnectionBySocket( socket );
	DC_BREAK_IF( connection == NULL );

    ByteBufferPtr source( stream );

	Serializables packets = BinarySerializer::read( source );

	for( Serializables::iterator i = packets.begin(), end = packets.end(); i != end; ++i ) {
		NetworkPacket* packet = i->get();

		log::verbose( "Packet %s(%d) received from %s\n", packet->typeName(), packet->typeId(), socket->address().toString() );

		PacketHandlers::iterator j = m_packetHandlers.find( packet->typeId() );
		if( j == m_packetHandlers.end() ) {
			log::warn( "NetworkHandler::processReceivedData : unhandled packet of type %s received from %s\n", packet->typeName(), socket->address().toString() );
			continue;
		}

		log::verbose( "Packet [%s] received from %s\n", packet->typeName(), socket->address().toString() );
		if( !j->second->handle( connection, packet ) ) {
			log::warn( "NetworkHandler::processReceivedData : invalid packet of type %s received from %s\n", packet->typeName(), socket->address().toString() );
		}
	}

	log::verbose( "%d bytes from %s processed, %d bytes left in buffer\n", stream->position(), socket->address().toString(), stream->length() - stream->position() );
	
	stream->trimFromLeft( stream->position() );
}

// ** NetworkHandler::eventListeners
ConnectionList NetworkHandler::eventListeners( void ) const
{
	return ConnectionList();
}

// ** NetworkHandler::handleTimePacket
bool NetworkHandler::handleTimePacket( ConnectionPtr& connection, packets::Time& packet )
{
	connection->send<packets::Time>( packet.timestamp, packet.roundTripTime );
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
	DC_BREAK_IF( i == m_eventHandlers.end() )

	if( i == m_eventHandlers.end() ) {
		log::warn( "NetworkHandler::handleEventPacket : unknown event %d received\n", packet.eventId );
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
		log::warn( "NetworkHandler::handleRemoteCallPacket : trying to invoke unknown remote procedure %d\n", packet.method );
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
void NetworkHandler::update( void )
{
//	if( m_broadcastListener != NULL ) {
//		m_broadcastListener->update();
//	}
}

} // namespace net

DC_END_DREEMCHEST