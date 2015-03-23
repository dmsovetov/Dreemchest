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

#include "Connection.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** Connection::Connection
Connection::Connection( const TCPSocketPtr& socket ) : m_socket( socket )
{

}

// ** Connection::create
ConnectionPtr Connection::create( const TCPSocketPtr& socket )
{
	DC_BREAK_IF( socket == NULL );
	return ConnectionPtr( DC_NEW Connection( socket ) );
}

// ** Connection::address
const NetworkAddress& Connection::address( void ) const
{
	return m_socket->address();
}

// ** Connection::socket
const TCPSocketPtr& Connection::socket( void ) const
{
	return m_socket;
}

// ** Connection::socket
TCPSocketPtr& Connection::socket( void )
{
	return m_socket;
}

} // namespace net

DC_END_DREEMCHEST