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

#ifndef __DC_Network_TCPStream_H__
#define __DC_Network_TCPStream_H__

#include "SocketDescriptor.h"
#include "../../io/streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! A TCP stream class.
	class TCPStream : public Io::ByteBuffer {
	public:

		//! Socket pull result
		enum State { Idle, Received, Closed };

								//! Constructs a TCPStream instance.
								TCPStream( SocketDescriptor* descriptor );

		//! Returns a TCP socket descriptor.
		const SocketDescriptor*	descriptor( void ) const;

		//! Pulls incoming data from TCP stream.
		State					pull( void );

		//! Flushes all received data.
		void					flush( void );

        //! Writes data from stream.
        virtual s32				write( const void* buffer, s32 size );

	private:

		//! TCP stream socket.
		SocketDescriptor*	    m_descriptor;
	};

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_TCPStream_H__	*/