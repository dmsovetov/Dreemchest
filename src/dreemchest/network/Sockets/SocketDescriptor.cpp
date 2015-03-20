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

#include "SocketDescriptor.h"
#include "Posix/PosixNetwork.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** SocketDescriptor::Invalid
SocketDescriptor SocketDescriptor::Invalid = INVALID_SOCKET;

// ** SocketDescriptor::ActiveCount
u32 SocketDescriptor::ActiveCount = 0;

// ** SocketDescriptor::SocketDescriptor
SocketDescriptor::SocketDescriptor( s32 socket ) : m_socket( socket )
{
	ActiveCount++;
	log::verbose( "Active socket descriptors: %d\n", ActiveCount );
}

// ** SocketDescriptor::SocketDescriptor
SocketDescriptor::SocketDescriptor( const SocketDescriptor& other )
{
	ActiveCount++;

	m_socket = other.m_socket;
	other.m_socket = INVALID_SOCKET;
}

SocketDescriptor::~SocketDescriptor( void )
{
	close();
	ActiveCount--;
	log::verbose( "Active socket descriptors: %d\n", ActiveCount );
}

// ** SocketDescriptor::operator bool
SocketDescriptor::operator s32( void ) const
{
	return m_socket;
}

// ** SocketDescriptor::operator =
const SocketDescriptor& SocketDescriptor::operator = ( const SocketDescriptor& other )
{
	m_socket = other.m_socket;
	other.m_socket = INVALID_SOCKET;
	return *this;
}

// ** SocketDescriptor::operator =
const SocketDescriptor& SocketDescriptor::operator = ( s32 socket )
{
	close();
	m_socket = socket;
	return *this;
}

// ** SocketDescriptor::operator ==
bool SocketDescriptor::operator == ( const SocketDescriptor& other ) const
{
	return m_socket == other.m_socket;
}

// ** SocketDescriptor::isValid
bool SocketDescriptor::isValid( void ) const
{
	return m_socket != INVALID_SOCKET;
}

// ** SocketDescriptor::accept
SocketDescriptor SocketDescriptor::accept( NetworkAddress& remoteAddress ) const
{
    sockaddr_in addr;
#if defined( DC_PLATFORM_WINDOWS )
    s32       size = sizeof( addr );
#else
    socklen_t size = sizeof( addr );
#endif
    SocketDescriptor socket = ::accept( m_socket, ( sockaddr* )&addr, &size );

	if( socket.isValid() ) {
		remoteAddress = addr.sin_addr.s_addr;
	}

	return socket;
}

// ** SocketDescriptor::close
void SocketDescriptor::close( void )
{
	if( m_socket == INVALID_SOCKET ) {
		return;
	}

#if defined( DC_PLATFORM_WINDOWS )
	closesocket( m_socket );
#else
    ::close( m_socket );
#endif

	m_socket = INVALID_SOCKET;
}

// ** SocketDescriptor::setNonBlocking
bool SocketDescriptor::setNonBlocking( void )
{
	s32 result = 0;

#if defined( DC_PLATFORM_WINDOWS )
	u_long noBlock = 1;
	result = ioctlsocket( m_socket, FIONBIO, &noBlock );
#else
    result = fcntl( m_socket, F_SETFL, O_NONBLOCK );
#endif

	DC_BREAK_IF( result == SOCKET_ERROR );
	return result != SOCKET_ERROR;
}

// ** SocketDescriptor::enableBroadcast
bool SocketDescriptor::enableBroadcast( void )
{
	s32 result = 0;

#if defined( DC_PLATFORM_WINDOWS )
	s8 one = 1;
#else
	s32 one = 1;
#endif

	result  = setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof( one ) );
	DC_BREAK_IF( result == SOCKET_ERROR );
	return result != SOCKET_ERROR;
}

// ** SocketDescriptor::enableAddressReuse
bool SocketDescriptor::enableAddressReuse( void )
{
#if defined( DC_PLATFORM_WINDOWS )
	s8 one = 1;
#else
	s32 one = 1;
#endif

    s32 result = setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );
	if( result == SOCKET_ERROR ) {
		log::error( "SocketDescriptor::enableAddressReuse : setsockopt failed, %d\n", PosixNetwork::lastError() );
		DC_BREAK_IF( result == SOCKET_ERROR );
	}

	return result != SOCKET_ERROR;
}

} // namespace net

DC_END_DREEMCHEST