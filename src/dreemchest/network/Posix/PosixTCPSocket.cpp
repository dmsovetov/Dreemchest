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

#include	"PosixTCPSocket.h"

#include    "../TCPSocket.h"
#include	"../../io/streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** PosixTCPSocket::PosixTCPSocket
PosixTCPSocket::PosixTCPSocket( TCPSocketDelegate* delegate, Socket socket, const NetworkAddress& address )
	: m_delegate( delegate ), m_socket( socket ), m_isServer( false ), m_buffer( NULL ), m_address( address )
{
	m_parent = NULL;
	m_buffer = io::ByteBuffer::create( 4096 );

	if( m_delegate == NULL ) {
		m_delegate = new TCPSocketDelegate;
	}

    if( m_socket != INVALID_SOCKET ) {
        return;
    }

    m_socket = ::socket( PF_INET, SOCK_STREAM, 0 );
	if( m_socket == INVALID_SOCKET ) {
		log::error( "PosixTCPSocket::PosixTCPSocket : failed to create socket, %d\n", PosixNetwork::lastError() );
		return;
	}

	PosixNetwork::setSocketAddressReuse( m_socket );
}

PosixTCPSocket::~PosixTCPSocket( void )
{
    disconnect( -1 );
}

// ** PosixTCPSocket::isValid
bool PosixTCPSocket::isValid( void ) const
{
	return m_socket != INVALID_SOCKET;
}

// ** PosixTCPSocket::isServer
bool PosixTCPSocket::isServer( void ) const
{
	return m_isServer;
}

// ** PosixTCPSocket::socket
Socket PosixTCPSocket::socket( void ) const
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
    if( !m_socket ) {
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
		m_delegate->handleConnected( m_parent, m_address, m_socket );
    } else {
		m_delegate->handleConnectionFailed( m_parent, m_socket );
        return false;
    }

	// ** Set non blocking mode
	PosixNetwork::setSocketNonBlocking( m_socket );

    if( m_isServer ) {
        listenConnections();
    }

	return true;
}

// ** PosixTCPSocket::disconnect
void PosixTCPSocket::disconnect( Socket connection )
{
    int socket = connection == -1 ? m_socket : connection;
    
    if( socket == INVALID_SOCKET ) {
        return;
    }

    if( socket == m_socket && m_isServer ) {
        for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
			disconnect( i->m_socket->socket() );
        }

        m_clientSockets.clear();
    }

    if( socket == m_socket && m_parent ) {
		m_delegate->handleDisconnected( m_parent, m_socket );
    }

	PosixNetwork::closeSocket( socket );
}

// ** PosixTCPSocket::toSockaddr
sockaddr_in PosixTCPSocket::toSockaddr( const NetworkAddress& address, u16 port ) const
{
    sockaddr_in addr;

    addr.sin_addr.s_addr = address ? ( u32 )address : INADDR_ANY;
	addr.sin_port        = htons( (short)port );
	addr.sin_family      = AF_INET;

    return addr;
}

// ** PosixTCPSocket::sendTo
u32 PosixTCPSocket::sendTo( const void* buffer, u32 size, Socket connection = -1 ) const
{
    DC_BREAK_IF( m_socket == INVALID_SOCKET );

    u32       bytesSent = 0;
    const u8* data      = ( u8* )buffer;

    while( bytesSent < size ) {
        s32 result = send( connection == -1 ? m_socket : connection, ( CString )(data + bytesSent), size - bytesSent, 0 );

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

    return bytesSent;
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
    Socket socket  = accept( m_socket, ( sockaddr* )&addr, &size );

    if( socket == INVALID_SOCKET ) {
        DC_BREAK;
        return ClientConnection();
    }

	ClientConnection connection;
	connection.m_buffer = io::ByteBuffer::create( 4096 );
	connection.m_socket = dcTCPSocketStrong( DC_NEW TCPSocket( DC_NEW PosixTCPSocket( NULL, socket, addr.sin_addr.s_addr ) ) );
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
        updateClientSocket();
    }
}

// ** PosixTCPSocket::updateClientSocket
void PosixTCPSocket::updateClientSocket( void )
{
    DC_BREAK_IF( m_socket == INVALID_SOCKET );
    
	u32 length = 0;

	while( (length = recv( m_socket, ( s8* )m_buffer->buffer(), m_buffer->length() - 1, 0 )) != -1 ) {
		if( length == 0 ) {
            disconnect( m_socket );
			m_delegate->handleDisconnected( m_parent, m_socket );
			break;
		}

		m_delegate->handleReceivedData( m_parent, m_parent, m_buffer->buffer(), length );
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
            FD_SET( i->m_socket->socket(), &readSet );
        }

//      result = select( 0, &readSet, NULL, NULL, &timeout ); // ** Win32
        result = select( 16, &readSet, NULL, NULL, &timeout );
    } while( result == -1 );

    if( result > 0 ) {
        // ** Accept connections
        if( FD_ISSET( m_socket, &readSet ) ) {
			ClientConnection accepted = acceptConnection();
            m_clientSockets.push_back( accepted );

			m_delegate->handleConnectionAccepted( m_parent, accepted.m_socket.get() );
        }

        // ** Process connections
        for( ClientSocketsList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
            ClientConnection&	clientConnection = *i;
			TCPSocket*			clientSocket = clientConnection.m_socket.get();
			Socket				clientHandle = clientSocket->socket();

            if( FD_ISSET( clientHandle, &readSet ) ) {
                u32 length = 0;

				while( (length = recv( clientHandle, ( s8* )clientConnection.m_buffer->buffer(), clientConnection.m_buffer->length() - 1, 0 )) != -1 ) {
                    if( length == 0 ) {
						m_delegate->handleConnectionClosed( m_parent, clientSocket );
						closeConnection( clientConnection );
                        break;
                    }

					m_delegate->handleReceivedData( m_parent, clientSocket, clientConnection.m_buffer->buffer(), length );
                }
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
    }

    DC_BREAK_IF( result < 0 );
}

} // namespace net

DC_END_DREEMCHEST
