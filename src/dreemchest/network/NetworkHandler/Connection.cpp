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

#include "Connection.h"
#include "NetworkHandler.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** Connection::Connection
Connection::Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket ) : m_networkHandler( networkHandler ), m_socket( socket ), m_nextRemoteCallId( 1 )
{

}

// ** Connection::networkHandler
NetworkHandler* Connection::networkHandler( void ) const
{
	return m_networkHandler;
}

// ** Connection::address
const NetworkAddress& Connection::address( void ) const
{
	return m_socket->address();
}

// ** Connection::socket
const TCPSocketPtr& Connection::socket( void ) const
{
	return m_socket;
}

// ** Connection::socket
TCPSocketPtr& Connection::socket( void )
{
	return m_socket;
}

// ** Connection::send
void Connection::send( NetworkPacket* packet )
{
	io::ByteBufferPtr buffer = io::ByteBuffer::create();

	// ** Set packet timestamp
	packet->timestamp = UnixTime();

	// ** Write packet to binary stream
	u32 bytesWritten = m_networkHandler->m_packetParser.writeToStream( packet, buffer );

	// ** Send binary data to socket
	s32 bytesSent = m_socket->sendTo( buffer->buffer(), buffer->length() );
	DC_BREAK_IF( bytesWritten != bytesSent );
}

// ** Connection::handleResponse
bool Connection::handleResponse( const packets::RemoteCallResponse* packet )
{
	// ** Find pending remote call
	PendingRemoteCalls::iterator i = m_pendingRemoteCalls.find( packet->id );

	if( i == m_pendingRemoteCalls.end() ) {
		log::warn( "Connection::handleResponse : invalid request id %d\n", packet->id );
		return false;
	}

	// ** Run a callback
	bool result = i->second.m_handler->handle( ConnectionPtr( this ), packet );	//!! Potential bug :(
	m_pendingRemoteCalls.erase( i );

	return result;
}

// ** Connection::update
void Connection::update( void )
{
	if( m_pendingRemoteCalls.empty() ) {
		return;
	}

	UnixTime currentTime;

	for( PendingRemoteCalls::iterator i = m_pendingRemoteCalls.begin(); i != m_pendingRemoteCalls.end(); ) {
		if( (currentTime - i->second.m_timestamp) > 60 ) {
			log::warn( "Connection::update : remote procedure call '%s' timed out\n", i->second.m_name.c_str() );
			i = m_pendingRemoteCalls.erase( i );
		} else {
			++i;
		}
	}
}

} // namespace net

DC_END_DREEMCHEST