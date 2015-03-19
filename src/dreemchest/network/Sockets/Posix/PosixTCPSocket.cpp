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

#include "PosixTCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** PosixTCPSocket::PosixTCPSocket
PosixTCPSocket::PosixTCPSocket( TCPSocketDelegate* delegate, SocketDescriptor& socket, const NetworkAddress& address )
	: m_delegate( delegate ), m_socket( socket ), m_isServer( false ), m_stream( NULL ), m_address( address )
{
	m_parent = NULL;
	m_stream = DC_NEW TCPStream( &m_socket );

	if( m_delegate == NULL ) {
		m_delegate = new TCPSocketDelegate;
	}

    if( m_socket.isValid() ) {
        return;
    }

    m_socket = ::socket( PF_INET, SOCK_STREAM, 0 );
	if( !m_socket.isValid() ) {
		log::error( "PosixTCPSocket::PosixTCPSocket : failed to create socket, %d\n", PosixNetwork::lastError() );
		return;
	}

	m_socket.enableAddressReuse();
}

PosixTCPSocket::~PosixTCPSocket( void )
{
	close();
}

// ** PosixTCPSocket::isValid
bool PosixTCPSocket::isValid( void ) const
{
	return m_socket.isValid();
}

// ** PosixTCPSocket::isServer
bool PosixTCPSocket::isServer( void ) const
{
	return m_isServer;
}

// ** PosixTCPSocket::stream
const TCPStream* PosixTCPSocket::stream( void ) const
{
	return m_stream.get();
}

// ** PosixTCPSocket::stream
TCPStream* PosixTCPSocket::stream( void )
{
	return m_stream.get();
}

// ** PosixTCPSocket::descriptor
const SocketDescriptor& PosixTCPSocket::descriptor( void ) const
{
	return m_socket;
}

// ** PosixTCPSocket::descriptor
SocketDescriptor& PosixTCPSocket::descriptor( void )
{
	return m_socket;
}

// ** PosixTCPSocket::address
const NetworkAddress& PosixTCPSocket::address( void ) const
{
	return m_address;
}

// ** PosixTCPSocket::connectTo
bool PosixTCPSocket::connectTo( const NetworkAddress& address, u16 port )
{
	if( !m_socket.isValid() ) {
        DC_BREAK
        return false;
    }
    
    m_isServer = (address == 0);
    m_address  = address;

	sockaddr_in addr   = toSockaddr( address, port );
    int         result = -1;

	// ** Connect
	if( !m_isServer ) {
		result = connect( m_socket, ( const sockaddr* )&addr, sizeof( addr ) );
	} else {
		result = bind( m_socket, ( const sockaddr* )&addr, sizeof( addr ) );
	}

    if( result != -1 ) {
		m_delegate->handleConnected( m_parent, m_address );
    } else {
		m_delegate->handleConnectionFailed( m_parent );
        return false;
    }

	// ** Set non blocking mode
	m_socket.setNonBlocking();

    if( m_isServer ) {
        listenConnections();
    }

	return true;
}

// ** PosixTCPSocket::close
void PosixTCPSocket::close( void )
{
	m_delegate->handleClosed( m_parent );

	m_socket.close();

	for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
		i->m_closed = true;
		i->m_socket = TCPSocketPtr();
	}
}

// ** PosixTCPSocket::toSockaddr
sockaddr_in PosixTCPSocket::toSockaddr( const NetworkAddress& address, u16 port ) const
{
    sockaddr_in addr;

    addr.sin_addr.s_addr = address ? ( u32 )address : INADDR_ANY;
	addr.sin_port        = htons( ( u16 )port );
	addr.sin_family      = AF_INET;

    return addr;
}

// ** PosixTCPSocket::sendTo
u32 PosixTCPSocket::sendTo( const void* buffer, u32 size )
{
	return m_stream->write( buffer, size );
/*
    DC_BREAK_IF( !m_socket.isValid() );

    u32       bytesSent = 0;
    const u8* data      = ( u8* )buffer;

    while( bytesSent < size ) {
        s32 result = send( m_socket, ( CString )(data + bytesSent), size - bytesSent, 0 );

        if( result == -1 ) {
		#ifdef DC_PLATFORM_WINDOWS
			if( WSAGetLastError() == WSAEWOULDBLOCK ) continue;
		#else
			if( errno == EAGAIN ) continue;
			log::error( "PosixTCPSocket::sendTo : %s\n", strerror( errno ) );
		#endif

			DC_BREAK
            return -1;
        }

        bytesSent += result;
    }

    return bytesSent;*/
}

// ** PosixTCPSocket::listenConnections
bool PosixTCPSocket::listenConnections( void )
{
    s32 result = listen( m_socket, 16 );
    DC_BREAK_IF( result < 0 );

    return result ? true : false;
}

// ** PosixTCPSocket::acceptConnection
PosixTCPSocket::ClientConnection PosixTCPSocket::acceptConnection( void )
{
    sockaddr_in addr;
#if defined( DC_PLATFORM_WINDOWS )
    s32         size    = sizeof( addr );
#else
    socklen_t   size    = sizeof( addr );
#endif
    SocketDescriptor socket  = accept( m_socket, ( sockaddr* )&addr, &size );

	if( !socket.isValid() ) {
        DC_BREAK;
        return ClientConnection();
    }

	ClientConnection connection;
	connection.m_buffer = io::ByteBuffer::create(400);
	connection.m_socket = TCPSocketPtr( DC_NEW TCPSocket( DC_NEW PosixTCPSocket( NULL, socket, addr.sin_addr.s_addr ) ) );
	connection.m_closed = false;

	return connection;
}

// ** PosixTCPSocket::closeConnection
void PosixTCPSocket::closeConnection( ClientConnection& connection )
{
    ClientSocketsList::iterator i = std::find( m_clientSockets.begin(), m_clientSockets.end(), connection );
    DC_BREAK_IF( i == m_clientSockets.end() );
	connection.m_closed = true;
}

// ** PosixTCPSocket::update
void PosixTCPSocket::update( void )
{
    if( m_isServer ) {
        updateServerSocket();
    } else {
		pullDataFromStream( m_parent );
    }
}

// ** PosixTCPSocket::pullDataFromStream
void PosixTCPSocket::pullDataFromStream( TCPSocket* socket )
{
	s32				 result = 0;
	TCPStream*		 stream = socket->stream();
	TCPStream::State state  = TCPStream::Idle;

	while( (state = stream->pull()) ) {
		if( state == TCPStream::Closed ) {
			close();
			break;
		}

		m_delegate->handleReceivedData( m_parent, socket, stream );
		stream->flush();

		if( !isValid() ) {
			break;
		}
	}
}

// ** PosixTCPSocket::updateServerSocket
void PosixTCPSocket::updateServerSocket( void )
{
    int result = 0;

    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    fd_set readSet;

    do {
        FD_ZERO( &readSet );
        FD_SET( m_socket, &readSet );

        for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
            FD_SET( i->m_socket->descriptor(), &readSet );
        }

//      result = select( 0, &readSet, NULL, NULL, &timeout ); // ** Win32
        result = select( 16, &readSet, NULL, NULL, &timeout );
    } while( result == -1 );

	if( result < 0 ) {
		return;
	}

    // ** Accept connections
    if( FD_ISSET( m_socket, &readSet ) ) {
		ClientConnection accepted = acceptConnection();
        m_clientSockets.push_back( accepted );

		m_delegate->handleConnectionAccepted( m_parent, accepted.m_socket.get() );
    }

    // ** Process connections
    for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
        ClientConnection&		clientConnection = *i;

		if( FD_ISSET( clientConnection.m_socket->descriptor(), &readSet ) ) {
			pullDataFromStream( clientConnection.m_socket.get() );
        }
    }

    // ** Remove closed connections
    for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ) {
		if( i->m_closed ) {
            i = m_clientSockets.erase( i );
        } else {
            ++i;
        }
    }

    DC_BREAK_IF( result < 0 );
}

} // namespace net

DC_END_DREEMCHEST
