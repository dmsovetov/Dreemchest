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

#include "TCPStream.h"
#include "Posix/PosixNetwork.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** TCPStream::TCPStream
TCPStream::TCPStream( SocketDescriptor* socket ) : ByteBuffer( NULL, 0 ), m_socket( socket )
{
}

// ** TCPStream::descriptor
const SocketDescriptor* TCPStream::descriptor( void ) const
{
	return m_socket;
}

// ** TCPStream::pull
TCPStream::State TCPStream::pull( void )
{
	DC_BREAK_IF( !m_socket->isValid() );
	DC_BREAK_IF( bytesAvailable() );

	s8  chunk[1];
	s32 received = 0;

	while( (received = recv( *m_socket, chunk, sizeof( chunk ), 0 )) != -1 ) {
		if( received == 0 ) {
			return Closed;
		}

		ByteBuffer::write( chunk, received );
	}

	setPosition( 0 );
	return bytesAvailable() ? Received : Idle;
}

// ** TCPStream::flush
void TCPStream::flush( void )
{
	DC_BREAK
	trimFromLeft( length() );
}

// ** TCPStream::write
s32 TCPStream::write( const void* buffer, s32 size )
{
    DC_BREAK_IF( !m_socket->isValid() );

    s32       bytesSent = 0;
    const u8* data      = ( u8* )buffer;

    while( bytesSent < size ) {
        s32 result = send( *m_socket, ( CString )(data + bytesSent), size - bytesSent, 0 );

        if( result == -1 ) {
		#ifdef DC_PLATFORM_WINDOWS
			if( WSAGetLastError() == WSAEWOULDBLOCK ) continue;
			log::error( "PosixTCPSocket::sendTo : %d\n", PosixNetwork::lastError() );
		#else
			if( errno == EAGAIN ) continue;
			log::error( "PosixTCPSocket::sendTo : %s\n", strerror( errno ) );
		#endif

            return -1;
        }

        bytesSent += result;
    }

    return bytesSent;
}

} // namespace net

DC_END_DREEMCHEST