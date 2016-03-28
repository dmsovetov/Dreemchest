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

#include "ConnectionTCP.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** ConnectionTCP::ConnectionTCP
ConnectionTCP::ConnectionTCP( TCPSocketPtr socket ) : m_socket( socket )
{
    DC_ABORT_IF( !m_socket.valid(), "invalid socket" );

    // Create the temporary read buffer for received packet
    m_packet = Io::ByteBuffer::create();

    // Subscribe for socket events
    m_socket->subscribe<TCPSocket::Data>( dcThisMethod( ConnectionTCP::handleSocketData ) );
    m_socket->subscribe<TCPSocket::Closed>( dcThisMethod( ConnectionTCP::handleSocketClosed ) );
}

// ** ConnectionTCP::~ConnectionTCP
ConnectionTCP::~ConnectionTCP( void )
{
    close();
}

// ** ConnectionTCP::socket
TCPSocketWPtr ConnectionTCP::socket( void ) const
{
    return m_socket;
}

// ** ConnectionTCP::address
const Address& ConnectionTCP::address( void ) const
{
	return socket()->address();
}

// ** ConnectionTCP::sendData
s32 ConnectionTCP::sendData( Io::ByteBufferWPtr data )
{
	DC_BREAK_IF( !m_socket.valid(), "invalid socket" );
	s32 result = m_socket->send( data->buffer(), data->length() );
    return result;
}

// ** ConnectionTCP::close
void ConnectionTCP::close( void )
{
    // Unsubscribe from socket events
	if( m_socket.valid() ) {
		m_socket->unsubscribe<TCPSocket::Data>( dcThisMethod( ConnectionTCP::handleSocketData ) );
		m_socket->unsubscribe<TCPSocket::Closed>( dcThisMethod( ConnectionTCP::handleSocketClosed ) );
	}

    // Notify all subscribers that connection is now closed
    notify<Closed>( this );

    // This connection does not have a valid socket instance - just exit
    if( !m_socket.valid() ) {
        return;
    }

    // Queue the socket for removal
    m_socket->closeLater();

    // Destroy this socket instance
    //m_socket = TCPSocketPtr();
}

// ** ConnectionTCP::handleSocketData
void ConnectionTCP::handleSocketData( const TCPSocket::Data& e )
{
    LogDebug( "socket", "%d bytes of data received from %s\n", e.data->bytesAvailable(), e.sender->address().toString() );

    // Save shortcut for a received data and socket
    Io::ByteBufferWPtr data   = e.data;
    TCPSocketWPtr      socket = e.sender;

    // Track the received amount
    trackReceivedAmount( data->bytesAvailable() );

    // Parse packets while there is data left in a TCP stream 
    while( data->hasDataLeft() ) {
        // Read single packet from a stream
        Header header = readPacket( data, m_packet );

        if( !header.type ) {
            break;
        }

        // Notify about this packet
		notifyPacketReceived( header.type, header.size, m_packet );
    }

    // Trim processed data
	LogDebug( "socket", "%d bytes from %s processed, %d bytes left in buffer\n", data->position(), socket->address().toString(), data->length() - data->position() );
	data->trimFromLeft( data->position() );
}

// ** ConnectionTCP::handleSocketClosed
void ConnectionTCP::handleSocketClosed( const TCPSocket::Closed& e )
{
    close();
}

} // namespace Network

DC_END_DREEMCHEST