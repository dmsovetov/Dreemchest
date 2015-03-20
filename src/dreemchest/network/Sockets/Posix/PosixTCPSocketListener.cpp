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

#include "PosixTCPSocketListener.h"
#include "PosixTCPSocket.h"
#include "PosixNetwork.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ----------------------------------PosixTCPSocketListener::TCPClientDelegate ------------------------------ //

// ** PosixTCPSocketListener::TCPClientDelegate::TCPClientDelegate
PosixTCPSocketListener::TCPClientDelegate::TCPClientDelegate( PosixTCPSocketListener* listener ) : m_listener( listener )
{
}

// ** PosixTCPSocketListener::TCPClientDelegate::handleReceivedData
void PosixTCPSocketListener::TCPClientDelegate::handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream )
{
	m_listener->m_delegate->handleReceivedData( m_listener->m_parent, socket, stream );
}

// ------------------------------------------PosixTCPSocketListener ---------------------------------------- //

// ** PosixTCPSocketListener::PosixTCPSocketListener
PosixTCPSocketListener::PosixTCPSocketListener( TCPSocketListenerDelegate* delegate )
{
	m_delegate = TCPSocketListenerDelegatePtr( delegate ? delegate : DC_NEW TCPSocketListenerDelegate );
}

// ** PosixTCPSocketListener::update
void PosixTCPSocketListener::update( void )
{
    int result = 0;

    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    fd_set readSet;

    do {
        FD_ZERO( &readSet );
        FD_SET( m_socket, &readSet );

        for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
            FD_SET( (*i)->descriptor(), &readSet );
        }

//      result = select( 0, &readSet, NULL, NULL, &timeout ); // ** Win32
        result = select( 16, &readSet, NULL, NULL, &timeout );
    } while( result == -1 );

	if( result < 0 ) {
		return;
	}

    // ** Accept connections
    if( FD_ISSET( m_socket, &readSet ) ) {
		TCPSocketPtr accepted = acceptConnection();
        m_clientSockets.push_back( accepted );

		m_delegate->handleConnectionAccepted( m_parent, accepted.get() );
    }

    // ** Process connections
    for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ++i ) {
        TCPSocketPtr& socket = *i;

		if( FD_ISSET( socket->descriptor(), &readSet ) ) {
			socket->update();
        }
    }

    // ** Remove closed connections
    for( TCPSocketList::iterator i = m_clientSockets.begin(), end = m_clientSockets.end(); i != end; ) {
		if( !(*i)->isValid() ) {
            i = m_clientSockets.erase( i );
        } else {
            ++i;
        }
    }

    DC_BREAK_IF( result < 0 );
}

// ** PosixTCPSocketListener::close
void PosixTCPSocketListener::close( void )
{
	m_socket.close();
	m_clientSockets.clear();
}

// ** PosixTCPSocketListener::connections
const TCPSocketList& PosixTCPSocketListener::connections( void ) const
{
	return m_clientSockets;
}

// ** PosixTCPSocketListener::acceptConnection
TCPSocketPtr PosixTCPSocketListener::acceptConnection( void )
{
	NetworkAddress	 address;
	SocketDescriptor socket = m_socket.accept( address );

	if( !socket.isValid() ) {
		return TCPSocketPtr();
	}

	return TCPSocketPtr( DC_NEW TCPSocket( DC_NEW PosixTCPSocket( DC_NEW TCPClientDelegate( this ), socket, address ) ) );
}

// ** PosixTCPSocketListener::bindTo
bool PosixTCPSocketListener::bindTo( u16 port )
{
    m_socket = socket( PF_INET, SOCK_STREAM, 0 );

	if( !m_socket.isValid() ) {
		log::error( "PosixTCPSocketListener::bindTo : failed to create socket, %d\n", PosixNetwork::lastError() );
		return false;
	}

	sockaddr_in addr = PosixNetwork::toSockaddr( NetworkAddress::Null, port );
	s32 result = bind( m_socket, ( const sockaddr* )&addr, sizeof( addr ) );

	if( result == SOCKET_ERROR ) {
		log::error( "PosixTCPSocketListener::bindTo : bind failed, %d\n", PosixNetwork::lastError() );
		return false;
	}

	// ** Set non blocking mode
	m_socket.setNonBlocking();

	// ** Listen for connections
    result = listen( m_socket, 16 );
   
	if( result == SOCKET_ERROR ) {
		log::error( "PosixTCPSocketListener::bindTo : listen failed, %d\n", PosixNetwork::lastError() );
		return false;
	}

    return true;
}

} // namespace net

DC_END_DREEMCHEST