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
#include "NetworkApplication.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Connection::Connection
Connection::Connection( Application* application, const TCPSocketPtr& socket )
    : ConnectionTCP( socket ), m_application( application ), m_nextRemoteCallId( 1 )
{
    memset( &m_traffic, 0, sizeof( m_traffic ) );
}

// ** Connection::traffic
const Connection::Traffic& Connection::traffic( void ) const
{
    return m_traffic;
}

// ** Connection::application
Application* Connection::application( void ) const
{
    return m_application;
}

// ** Connection::handleResponse
void Connection::handleResponse( const Packets::RemoteCallResponse& packet )
{
    // Find pending remote call
    PendingRemoteCalls::iterator i = m_pendingRemoteCalls.find( packet.id );

    if( i == m_pendingRemoteCalls.end() ) {
        LogWarning( "rpc", "received response with an invalid request id %d\n", packet.id );
        return;
    }

    // Run a callback
    i->second.m_handler->handle( this, packet );
    m_pendingRemoteCalls.erase( i );
}

// ** Connection::update
void Connection::update( u32 dt )
{
    ConnectionTCP::update( dt );

    if( time() - m_traffic.m_lastUpdateTimestamp >= 1000 ) {
        m_traffic.m_sentBps        = (totalBytesSent()     - m_traffic.m_lastSentBytes)      * 8;
        m_traffic.m_receivedBps = (totalBytesReceived() - m_traffic.m_lastReceivedBytes) * 8;
        m_traffic.m_lastUpdateTimestamp = time();
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
