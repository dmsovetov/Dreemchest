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

#include    "UDPSocket.h"
#include	"Posix/PosixUDPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** UDPSocket::UDPSocket
UDPSocket::UDPSocket( impl::UDPSocketPrivate* impl ) : m_impl( impl )
{
    if( m_impl != NULL ) {
        m_impl->setParent( this );
    }
}

UDPSocket::~UDPSocket( void )
{

}

// ** UDPSocket::create
UDPSocketPtr UDPSocket::create( UDPSocketDelegate* delegate )
{
	return UDPSocketPtr( DC_NEW UDPSocket( DC_NEW PosixUDPSocket( delegate, false ) ) );
}

// ** UDPSocket::createBroadcast
UDPSocketPtr UDPSocket::createBroadcast( UDPSocketDelegate* delegate )
{
	return UDPSocketPtr( DC_NEW UDPSocket( DC_NEW PosixUDPSocket( delegate, true ) ) );
}

// ** UDPSocket::send
u32 UDPSocket::send( const NetworkAddress& address, u16 port, const void *buffer, u32 size )
{
    DC_CHECK_IMPL( -1 )
    return m_impl->send( address, port, buffer, size );
}

// ** UDPSocket::listen
bool UDPSocket::listen( u16 port )
{
    DC_CHECK_IMPL( false )
    return m_impl->listen( port );
}

// ** UDPSocket::update
void UDPSocket::update( void )
{
    DC_CHECK_IMPL()
    m_impl->update();
}

} // namespace net

DC_END_DREEMCHEST
