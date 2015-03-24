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

#ifndef __DC_Network_SocketDescriptor_H__
#define __DC_Network_SocketDescriptor_H__

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Socket descriptor.
	class SocketDescriptor {
	public:

								//! Constructs a new socket descritptor
								SocketDescriptor( s32 socket = -1 );
								SocketDescriptor( const SocketDescriptor& other );
								~SocketDescriptor( void );

								//! Converts to raw socket handle.
								operator s32( void ) const;

		//! Invalid socket descriptor;
		static SocketDescriptor Invalid;

		//! Open sockets counter.
		static u32				ActiveCount;

		//! Compares two socket descriptors.
		bool					operator == ( const SocketDescriptor& other ) const;

		//! Copies a socket descriptor.
		const SocketDescriptor&	operator = ( const SocketDescriptor& other );

		//! Sets a socket descriptor from 32-bit integer
		const SocketDescriptor&	operator = ( s32 socket );

		//! Returns true if this socket descriptor is valid.
		bool					isValid( void ) const;

		//! Closes a socket.
		void					close( void );

		//! Returns an error code on this socket.
		s32						error( void ) const;

		//! Accepts incoming connection to this socket.
		SocketDescriptor		accept( NetworkAddress& remoteAddress ) const;			

		//! Enables socket address reuse.
		bool					enableAddressReuse( void );

		//! Sets this socket to non-blocking mode.
		bool					setNonBlocking( void );

		//! Enables broadcasts for this socket.
		bool					enableBroadcast( void );

	private:

		//! Actual socket descriptor.
		mutable s32				m_socket;
	};

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_SocketDescriptor_H__	*/