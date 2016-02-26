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

#include "TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** TCPSocket::TCPSocket
TCPSocket::TCPSocket( SocketDescriptor& descriptor, const Address& address )
	: m_descriptor( descriptor ), m_address( address )
{
	m_data = Io::ByteBuffer::create();

    if( m_descriptor.isValid() ) {
        return;
    }

    // Request the socket descriptor
    SocketResult result = socket( PF_INET, SOCK_STREAM, 0 );

    if( result.isError() ) {
        LogError( "socket", "failed to create TCP socket %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        return;
    }

    m_descriptor = result;
	m_descriptor.enableAddressReuse();
}

TCPSocket::~TCPSocket( void )
{
	close();
}

// ** TCPSocket::connectTo
TCPSocketPtr TCPSocket::connectTo( const Address& address, u16 port )
{
	TCPSocketPtr socket( DC_NEW TCPSocket );

	if( !socket->connect( address, port ) ) {
		return TCPSocketPtr();
	}

	return socket;
}

// ** TCPSocket::isValid
bool TCPSocket::isValid( void ) const
{
	return m_descriptor.isValid();
}

// ** TCPSocket::descriptor
const SocketDescriptor& TCPSocket::descriptor( void ) const
{
	return m_descriptor;
}

// ** TCPSocket::address
const Address& TCPSocket::address( void ) const
{
	return m_address;
}

// ** TCPSocket::connect
bool TCPSocket::connect( const Address& address, u16 port )
{
    DC_ABORT_IF( !m_descriptor.isValid(), "the socket should be valid" );

    m_address = address;

	sockaddr_in addr = Network::toSockaddr( address, port );

	// Connect to a remote host
	SocketResult result = ::connect( m_descriptor, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result.isError() ) {
        LogError( "socket", "connect failed %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
		return false;
	}

	// Set non blocking mode
	m_descriptor.setNonBlocking();

	// Set no delay
	m_descriptor.setNoDelay();

	return true;
}

// ** TCPSocket::close
void TCPSocket::close( void )
{
	if( m_descriptor.isValid() ) {
        notify<Closed>( this );
	}
	
	m_descriptor.close();
}

// ** TCPSocket::send
u32 TCPSocket::send( const void* buffer, u32 size )
{
    DC_ABORT_IF( !m_descriptor.isValid(), "invalid socket descriptor" );

    s32       bytesSent = 0;
    const s8* data      = reinterpret_cast<const s8*>( buffer );

    // While we have data left - send it to a remote host
    while( bytesSent < size ) {
        // Send data
        SocketResult result = ::send( m_descriptor, data + bytesSent, size - bytesSent, 0 );

        // Everything is ok - proceed
        if( !result.isError() ) {
            bytesSent += result;
            continue;
        }

        // Would block received - just wait
        if( result.wouldBlock() ) {
            continue;
        }

        // Something went wrong - write a log message and close socket
		LogError( "socket", "send failed %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
        close();

        return 0;
    }

    return bytesSent;
}

// ** TCPSocket::recv
void TCPSocket::recv( void )
{
    DC_ABORT_IF( !m_descriptor.isValid(), "invalid socket descriptor" );

    // Rewind to the end of a buffer
	m_data->setPosition( 0, Io::SeekEnd );

    // Start receiving bytes from TCP stream
	while( true ) {
        // Read the data from a socket
        s8           chunk[1];
        SocketResult result = ::recv( m_descriptor, chunk, sizeof( chunk ), 0 );

        // Peer has performed an orderly shutdown
		if( result == 0 ) {
			close();
            return;
		}

        // Would block returned - just wait
        if( result.wouldBlock() ) {
            break;
        }

        // An error occured
        if( result.isError() ) {
            LogError( "socket", "recv returned an error %d, %s\n", result.errorCode(), result.errorMessage().c_str() );
            close();
            break;
        }

        // Write received data to a buffer
		m_data->write( chunk, result );
	}

    // Rewind back to a first byte
	m_data->setPosition( 0, Io::SeekSet );

    // If there is data inside buffer - notify subscribers
    if( m_data->bytesAvailable() ) {
        notify<Data>( this, m_data );
    }
}

} // namespace Network

DC_END_DREEMCHEST
