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

#include "ConnectionMiddleware.h"
#include "../NetworkHandler/Connection.h"
#include "../Packets/Ping.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ---------------------------------------------- ConnectionMiddleware ---------------------------------------------- //

// ** ConnectionMiddleware::setConnection
void ConnectionMiddleware::setConnection( Connection* value )
{
	m_connection = value;
}

// --------------------------------------------- PacketSendingMiddleware -------------------------------------------- //

// ** PacketSendingMiddleware::PacketSendingMiddleware
PacketSendingMiddleware::PacketSendingMiddleware( u32 interval )
	: m_interval( interval )
	, m_accumulator( 0 )
{
}

// ** PacketSendingMiddleware::accumulateTime
bool PacketSendingMiddleware::accumulateTime( u32 dt )
{
	// Increase the accumulated time
	m_accumulator += dt;

	// Do we have enough time accumulated?
	s32 count = m_accumulator / m_interval;

	if( count == 0 ) {
		return false;
	}

	// Reset the accumulator
	m_accumulator -= count * m_interval;

	return true;
}

// -------------------------------------------------- PingInterval -------------------------------------------------- //

// ** PingInterval::PingInterval
PingInterval::PingInterval( u32 interval, s32 iterations )
	: PacketSendingMiddleware( interval )
	, m_iterations( iterations )
{
}

// ** PingInterval::update
void PingInterval::update( u32 dt )
{
	// Update the time accumulator
	if( accumulateTime( dt ) ) {
		// Send the Ping packet
		m_connection->send<Packets::Ping>( m_iterations, m_connection->time() );
	}
}

// ------------------------------------------------ KeepAliveInterval ----------------------------------------------- //

// ** KeepAliveInterval::KeepAliveInterval
KeepAliveInterval::KeepAliveInterval( u32 interval )
	: PacketSendingMiddleware( interval )
{
}

// ** KeepAliveInterval::update
void KeepAliveInterval::update( u32 dt )
{
	// Update the time accumulator
	if( accumulateTime( dt ) ) {
		// Send the KeepAlive packet
		m_connection->send<Packets::KeepAlive>();
		LogDebug( "connection", "keep alive packet sent\n" );
	}
}

// ------------------------------------------------- CloseOnTimeout ------------------------------------------------ //

CloseOnTimeout::CloseOnTimeout( s32 interval )
	: m_interval( interval )
{
}

// ** CloseOnTimeout::update
void CloseOnTimeout::update( u32 dt )
{
	// Close the connection if the specified amount of time passed and no packets received
	if( m_connection->timeout() >= m_interval ) {
		// Queue this connection for closing
		m_connection->closeLater();
		LogVerbose( "connection", "closed on timeout\n" );
	}
}

} // namespace Network

DC_END_DREEMCHEST