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

// ** ConnectionTCP::sendDataToSocket
s32 ConnectionTCP::sendDataToSocket( SocketDataPtr data )
{
	DC_ABORT_IF( !m_socket.valid(), "invalid socket" );
	s32 result = m_socket->send( data->buffer(), data->length() );
    return result;
}

// ** ConnectionTCP::close
void ConnectionTCP::close( void )
{
    // Notify all subscribers that connection is now closed
    notify<Closed>( this );

    // Unsubscribe from socket events
    m_socket->unsubscribe<TCPSocket::Data>( dcThisMethod( ConnectionTCP::handleSocketData ) );
    m_socket->unsubscribe<TCPSocket::Closed>( dcThisMethod( ConnectionTCP::handleSocketClosed ) );

    // Destroy this socket instance
    m_socket = TCPSocketPtr();
}

// ** ConnectionTCP::handleSocketData
void ConnectionTCP::handleSocketData( const TCPSocket::Data& e )
{
    LogDebug( "socket", "%d bytes of data received from %s\n", e.data->bytesAvailable(), e.sender->address().toString() );

    // Save shortcut for a received data and socket
    SocketDataWPtr data   = e.data;
    TCPSocketWPtr  socket = e.sender;

    // Track the received amount
    trackReceivedAmount( data->bytesAvailable() );

    Io::ByteBufferPtr source( data );
	Io::Serializables packets = Io::BinarySerializer::read( source );

	for( Io::Serializables::iterator i = packets.begin(), end = packets.end(); i != end; ++i ) {
		NetworkPacket* packet = i->get();

		LogDebug( "packet", "%s received from %s\n", packet->typeName(), socket->address().toString() );
        notify<Received>( this, packet );
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