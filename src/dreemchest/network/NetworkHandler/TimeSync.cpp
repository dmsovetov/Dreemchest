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

#include	"TimeSync.h"
#include    "NetworkHandler.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TimeSync::TimeSync
TimeSync::TimeSync( void )
{
	DC_BREAK

    m_isReady   = false;
    m_isRunning = false;
}

// ** TimeSync::TimeSync
TimeSync::~TimeSync( void )
{
    DC_BREAK_IF( m_isRunning );
}

// ** TimeSync::IsReady
bool TimeSync::IsReady( void ) const
{
    return m_isReady;
}
/*
// ** TimeSync::SyncTo
void TimeSync::SyncTo( int *remoteTime, dcTime time, NetworkHandler *local, int iterations, int connection )
{
    m_remoteTime = remoteTime;
    m_localTime  = time;
    m_local      = local;
    m_iterations = iterations;
    m_isReady    = false;
    m_isRunning  = true;
	DC_BREAK
//    m_local->AttachListener( NetworkHandlerEvent::PacketReceived, dcThisMethod( TimeSync::OnPacketReceived ) );

    SendPing( time->GetMilliseconds() );
}
*/
// ** TimeSync::SendPing
void TimeSync::SendPing( int timestamp )
{
//    sTimeSyncPacket ping;
//    ping.timestamp = timestamp;
//    ping.localTime = 0;
//    m_local->SendPacket( sTimeSyncPacket::PacketId, &ping );
}
/*
// ** TimeSync::OnPacketReceived
void TimeSync::OnPacketReceived( const dcEvent e )
{
    dcNetworkHandlerEvent ne = ( dcNetworkHandlerEvent )e;
    if( ne->m_packetId != sTimeSyncPacket::PacketId ) {
        return;
    }

    sTimeSyncPacket *sync       = ( sTimeSyncPacket* )ne->m_packet;
    int             localTime   = m_localTime->GetMilliseconds( true );
    int             latency     = localTime - sync->timestamp;

    m_iterations--;
    if( latency < m_bestTime.m_latency ) {
        m_bestTime.m_latency = latency;
        m_bestTime.m_local   = localTime;
        m_bestTime.m_remote  = sync->localTime + latency / 2;
    }

    if( m_iterations <= 0 ) {
        OnTimeSynchronized( localTime );
    } else {
        SendPing( localTime );
    }
}
*/
// ** TimeSync::OnTimeSynchronized
void TimeSync::OnTimeSynchronized( int localTime )
{
 /*   m_local->DetachListener( NetworkHandlerEvent::PacketReceived, dcThisMethod( TimeSync::OnPacketReceived ) );
    
    m_isReady     = true;
    m_isRunning   = false;
    *m_remoteTime = m_bestTime.m_remote + (localTime - m_bestTime.m_local);*/
}

} // namespace net

DC_END_DREEMCHEST
