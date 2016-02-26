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

namespace Network {

// ** Connection::Connection
Connection::Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket )
	: ConnectionTCP( socket ), m_networkHandler( networkHandler ), m_nextRemoteCallId( 1 ), m_time( 0 ), m_roundTripTime( 0 ), m_timeToLive( 0 ), m_keepAliveTimestamp( 0 )
{
	memset( &m_traffic, 0, sizeof( m_traffic ) );
}

// ** Connection::traffic
const Connection::Traffic& Connection::traffic( void ) const
{
	return m_traffic;
}

// ** Connection::time
u32 Connection::time( void ) const
{
	return m_time;
}

// ** Connection::setTime
void Connection::setTime( u32 value )
{
	m_time = value;
}

// ** Connection::timeToLive
s32 Connection::timeToLive( void ) const
{
	return m_timeToLive;
}

// ** Connection::setTimeToLive
void Connection::setTimeToLive( s32 value )
{
	m_timeToLive = value;
}

// ** Connection::keepAliveTimestamp
u32 Connection::keepAliveTimestamp( void ) const
{
	return m_keepAliveTimestamp;
}

// ** Connection::setKeepAliveTimestamp
void Connection::setKeepAliveTimestamp( u32 value )
{
	m_keepAliveTimestamp = value;
}

// ** Connection::roundTripTime
u32 Connection::roundTripTime( void ) const
{
	return m_roundTripTime;
}

// ** Connection::setRoundTripTime
void Connection::setRoundTripTime( u32 value )
{
	m_roundTripTime = value;
}

// ** Connection::networkHandler
NetworkHandler* Connection::networkHandler( void ) const
{
	return m_networkHandler;
}

// ** Connection::handleResponse
bool Connection::handleResponse( const packets::RemoteCallResponse& packet )
{
	// ** Find pending remote call
	PendingRemoteCalls::iterator i = m_pendingRemoteCalls.find( packet.id );

	if( i == m_pendingRemoteCalls.end() ) {
		LogWarning( "rpc", "received response with an invalid request id %d\n", packet.id );
		return false;
	}

	// ** Run a callback
    ConnectionPtr connection( this );	//!! Potential bug :(
	bool result = i->second.m_handler->handle( connection, packet );
	m_pendingRemoteCalls.erase( i );

	return result;
}

// ** Connection::update
void Connection::update( u32 dt )
{
	m_time += dt;
	m_timeToLive -= dt;

	if( m_time - m_traffic.m_lastUpdateTimestamp >= 1000 ) {
		m_traffic.m_sentBps		= (totalBytesSent()	 - m_traffic.m_lastSentBytes)	  * 8;
		m_traffic.m_receivedBps = (totalBytesReceived() - m_traffic.m_lastReceivedBytes) * 8;
		m_traffic.m_lastUpdateTimestamp = m_time;
		m_traffic.m_lastSentBytes = totalBytesSent();
		m_traffic.m_lastReceivedBytes = totalBytesReceived();
	}

	if( m_pendingRemoteCalls.empty() ) {
		return;
	}

	for( PendingRemoteCalls::iterator i = m_pendingRemoteCalls.begin(); i != m_pendingRemoteCalls.end(); ) {
		if( i->second.m_timeLeft < 0 ) {
			LogWarning( "rpc", "'%s' timed out\n", i->second.m_name.c_str() );
			i = m_pendingRemoteCalls.erase( i );
		} else {
			++i;
		}
	}
}

} // namespace Network

DC_END_DREEMCHEST