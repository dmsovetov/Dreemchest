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

#include "TCPSocketListener.h"
#include "Posix/PosixTCPSocketListener.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TCPSocketListener::TCPSocketListener
TCPSocketListener::TCPSocketListener( impl::TCPSocketListenerPrivate* impl ) : m_impl( impl )
{
	if( m_impl != NULL ) {
        m_impl->setParent( this );
	}
}

// ** TCPSocketListener::update
void TCPSocketListener::update( void )
{
	DC_CHECK_IMPL();
	m_impl->update();
}

// ** TCPSocketListener::close
void TCPSocketListener::close( void )
{
	DC_CHECK_IMPL();
	m_impl->close();
}

// ** TCPSocketListener::connections
const TCPSocketList& TCPSocketListener::connections( void ) const
{
	DC_CHECK_IMPL( TCPSocketList() );
	return m_impl->connections();
}

// ** TCPSocketListener::bindTo
TCPSocketListenerPtr TCPSocketListener::bindTo( u16 port, TCPSocketListenerDelegate* delegate )
{
	PosixTCPSocketListener* impl = DC_NEW PosixTCPSocketListener( delegate );
	if( !impl->bindTo( port ) ) {
		delete impl;
		return TCPSocketListenerPtr();
	}

	return TCPSocketListenerPtr( DC_NEW TCPSocketListener( impl ) );
}

} // namespace net

DC_END_DREEMCHEST