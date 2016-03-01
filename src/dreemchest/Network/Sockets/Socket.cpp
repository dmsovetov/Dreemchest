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

#include "Socket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Socket::Socket
Socket::Socket( SocketDescriptor& descriptor ) : m_descriptor( descriptor )
{
    // Create the socket receiving buffer
    m_data = Io::ByteBuffer::create();
}

// ** Socket::isValid
bool Socket::isValid( void ) const
{
	return m_descriptor.isValid();
}

// ** Socket::descriptor
const SocketDescriptor& Socket::descriptor( void ) const
{
	return m_descriptor;
}

// ** Socket::close
void Socket::close( void )
{
	m_descriptor.close();
}

} // namespace Network

DC_END_DREEMCHEST