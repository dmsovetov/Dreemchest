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
#include "Posix/PosixTCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TCPSocket::TCPSocket
TCPSocket::TCPSocket( impl::TCPSocketPrivate* impl ) : m_impl( impl )
{
    if( m_impl != NULL ) {
        m_impl->m_parent = this;

		if( !m_impl->isValid() ) {
			m_impl = NULL;
		}
    }
}

// ** TCPSocket::create
TCPSocket TCPSocket::create( TCPSocketDelegate* delegate )
{
	return TCPSocket( DC_NEW PosixTCPSocket( delegate ) );
}

// ** TCPSocket::operator bool
TCPSocket::operator bool( void ) const
{
	return m_impl != NULL;
}

// ** TCPSocket::isServer
bool TCPSocket::isServer( void ) const
{
	DC_CHECK_IMPL( false );
	return m_impl->isServer();
}

// ** TCPSocket::address
const NetworkAddress& TCPSocket::address( void ) const
{
	DC_CHECK_IMPL( NetworkAddress::Null );
	return m_impl->address();
}

// ** TCPSocket::socket
Socket TCPSocket::socket( void ) const
{
	DC_CHECK_IMPL( INVALID_SOCKET );
	return m_impl->socket();
}

// ** TCPSocket::connectTo
bool TCPSocket::connectTo( const NetworkAddress& address, u16 port )
{
    DC_CHECK_IMPL( false );
    return m_impl->connectTo( address, port );
}

// ** TCPSocket::disconnect
void TCPSocket::disconnect( Socket connection )
{
    DC_CHECK_IMPL();
    m_impl->disconnect( connection );
}

// ** TCPSocket::update
void TCPSocket::update( void )
{
    DC_CHECK_IMPL();
    m_impl->update();
}

// ** TCPSocket::sendTo
u32 TCPSocket::sendTo( const void* buffer, u32 size, Socket connection ) const
{
    DC_CHECK_IMPL( -1 );
    return m_impl->sendTo( buffer, size, connection );
}

} // namespace net

DC_END_DREEMCHEST
