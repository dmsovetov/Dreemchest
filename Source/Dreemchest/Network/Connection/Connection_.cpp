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

#include "Connection_.h"
#include "ConnectionMiddleware.h"

#include "../Packets/Packet.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Connection::Connection
Connection_::Connection_( void )
    : m_id( 0 )
    , m_totalBytesReceived( 0 )
    , m_totalBytesSent( 0 )
    , m_time( 0 )
    , m_timeout( 0 )
    , m_roundTripTime( 0 )
    , m_shouldClose( false )
{
}

// ** Connection::setId
void Connection_::setId( u32 value )
{
    m_id = value;
}

// ** Connection::id
u32 Connection_::id( void ) const
{
    return m_id;
}

// ** Connection::closeLater
void Connection_::closeLater( void )
{
    m_shouldClose = true;
}

// ** Connection::willBeClosed
bool Connection_::willBeClosed( void ) const
{
    return m_shouldClose;
}

// ** Connection::trackReceivedAmount
void Connection_::trackReceivedAmount( s32 value )
{
    m_totalBytesReceived += value;
}

// **  Connection::trackSentAmount
void Connection_::trackSentAmount( s32 value )
{
    m_totalBytesSent += value;
}

// ** Connection::totalBytesReceived
s32 Connection_::totalBytesReceived( void ) const
{
    return m_totalBytesReceived;
}

// ** Connection::totalBytesSent
s32 Connection_::totalBytesSent( void ) const
{
    return m_totalBytesSent;
}

// ** Connection::time
s32 Connection_::time( void ) const
{
    return m_time;
}

// ** Connection::setTime
void Connection_::setTime( s32 value )
{
    m_time = value;
}

// ** Connection::timeout
s32 Connection_::timeout( void ) const
{
    return m_timeout;
}

// ** Connection::roundTripTime
s32 Connection_::roundTripTime( void ) const
{
    return m_roundTripTime;
}

// ** Connection::setRoundTripTime
void Connection_::setRoundTripTime( s32 value )
{
    m_roundTripTime = value;
}

// ** Connection::notifyPacketReceived
void Connection_::notifyPacketReceived( PacketTypeId type, u16 size, Io::ByteBufferWPtr packet )
{
    // Reset the timeout counter
    m_timeout = 0;

    // Notify all listeners about this packet
    notify<Received>( this, type, size, packet );
}

// ** Connection::addMiddleware
void Connection_::addMiddleware( ConnectionMiddlewareUPtr instance )
{
    // Set this connection as parent
    instance->setConnection( static_cast<Connection*>( this ) );

    // Add new middleware to a list
    m_middlewares.push_back( instance );
}

// ** Connection::send
void Connection_::send( const AbstractPacket& packet )
{
    // Create the network data to write packet to
    Io::ByteBufferPtr stream = Io::ByteBuffer::create();

    // Write packet to binary stream
    u32 bytesWritten = writePacket( packet, stream );

    // Send binary data to socket
    s32 bytesSent = sendData( stream );

    // The socket was closed.
    if( bytesSent == 0 ) {
        close();
        return;
    }

    LogDebug( "packet", "%s sent to #%d (%d bytes)\n", packet.name(), id(), bytesSent );

    // Increase the sent bytes counter.
    trackSentAmount( bytesSent );

    NIMBLE_BREAK_IF( bytesWritten != bytesSent, "failed to send all data" );
}

// ** Connection::writePacket
s32 Connection_::writePacket( const AbstractPacket& packet, Io::ByteBufferWPtr stream ) const
{
    // Save current position to track the total number of bytes written
    s32 position = stream->position();

    // Write the header to a binary stream
    Header header( packet.id(), 0 );
    stream->write( &header.type, sizeof( header.type ) );
    stream->write( &header.size, sizeof( header.size ) );

    // Write packet to a binary stream
    s32 start = stream->position();
    packet.serialize( stream );
    header.size = stream->position() - start;

    // Finalize the packet formatting by fixing a packet size inside the header
    stream->setPosition( position + sizeof( header.type ) );
    stream->write( &header.size, sizeof( header.size ) );

    // Rewind back to the end of a packet
    stream->setPosition( start + header.size );

    return stream->position() - position;
}

// ** Connection::readPacket
Connection_::Header Connection_::readPacket( Io::ByteBufferWPtr stream, Io::ByteBufferWPtr packet ) const
{
    // The received data is too small to be a readable packet
    if( stream->bytesAvailable() < Header::Size ) {
        return Header();
    }

    // Save current stream position
    s32 initial = stream->position();

    // Read the packet header
    Header header;
    stream->read( &header.type, sizeof( header.type ) );
    stream->read( &header.size, sizeof( header.size ) );

    // Do we have enough data to parse the whole packet?
    if( stream->bytesAvailable() < header.size ) {
        stream->setPosition( initial );
        return Header();
    }

    // Copy data from a stream to a packet buffer
    packet->setPosition( 0, Io::SeekSet );
    if( header.size ) {
        // Copy bytes from a stream to a packet buffer
        packet->write( stream->current(), header.size );
        packet->setPosition( 0, Io::SeekSet );

        // Advance the stream cursor
        stream->setPosition( header.size, Io::SeekCur );
    }

    return header;
}

// ** Connection_::update
void Connection_::update( u32 dt )
{
    // Advance the connection time by a passed amount of milliseconds
    m_time += dt;

    // Advance the timeout counter
    m_timeout += dt;

    // Now update all connection middlewares
    for( ConnectionMiddlewares::iterator i = m_middlewares.begin(), end = m_middlewares.end(); i != end; ++i ) {
        (*i)->update( dt );
    }
}

} // namespace Network

DC_END_DREEMCHEST
