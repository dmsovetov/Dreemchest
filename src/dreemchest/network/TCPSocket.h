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

#ifndef		__DC_Network_TCPSocket_H__
#define		__DC_Network_TCPSocket_H__

#include    "Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

	namespace impl {
		class TCPSocketPrivate;
	} // namespace impl

    //! TCP socket class.
    class TCPSocket : public RefCounted {
    public:

								//! Constructs a TCPSocket instance.
								TCPSocket( impl::TCPSocketPrivate* impl = NULL );
		virtual					~TCPSocket( void ) {}

								//! Returns true if this socket is valid.
								operator bool() const;

		//! Returns a socket handle.
		Socket					socket( void ) const;

		//! Returns true if this is a server socket.
		bool					isServer( void ) const;

		//! Connects to a TCP socket at a given remote address, if address is 0 then starts listening for incomming connections.
        bool					connectTo( const NetworkAddress& address, u16 port );

		//! Closes a socket. If performed for server socket - closes a client connection.
        void					disconnect( Socket connection = -1 );

		//! Reads all incoming data.
        void					update( void );

		//! Returns a remote address.
		const NetworkAddress&	address( void ) const;

		//! Sends data to socket.
		/*
		\param buffer Data to be sent.
		\param size Data size to be sent.
		\param sendTo Connection to send data to (valid only for server sockets).
		*/
        u32						sendTo( const void* buffer, u32 size, Socket sendTo = -1 ) const;

		//! Creates a new TCP socket.
		static TCPSocketPtr		create( TCPSocketDelegate* delegate = NULL );
        
    private:

		//! Socket implementation.
		StrongPtr<impl::TCPSocketPrivate>	m_impl;
    };

	//! TCP socket event delegate.
	class TCPSocketDelegate : public RefCounted {
	public:

		virtual					~TCPSocketDelegate( void ) {}

		//! Handles a successfull socket connection.
		virtual void			handleConnected( TCPSocket* sender, const NetworkAddress& address, Socket socket ) {}

		//! Handles a socket connection failure.
		virtual void			handleConnectionFailed( TCPSocket* sender, Socket socket ) {}

		//! Handles a socket disconnection.
		virtual void			handleDisconnected( TCPSocket* sender, Socket socket ) {}

		//! Handles a received data.
		virtual void			handleReceivedData( TCPSocket* sender, TCPSocket* socket, const u8 *data, u32 size ) {}

		//! Handles accepted incomming connection.
		virtual void			handleConnectionAccepted( TCPSocket* sender, TCPSocket* socket ) {}

		//! Handles a remote connection closed.
		virtual void			handleConnectionClosed( TCPSocket* sender, TCPSocket* socket ) {}
	};

	namespace impl {

		// ** class TCPSocketPrivate
		class TCPSocketPrivate : public RefCounted {
		friend class TCPSocket;
		public:

			virtual							~TCPSocketPrivate( void ) {}

			virtual const NetworkAddress&	address( void ) const = 0;
			virtual Socket					socket( void ) const = 0;
			virtual bool					isServer( void ) const = 0;
			virtual bool					isValid( void ) const = 0;
			virtual bool					connectTo( const NetworkAddress& address, u16 port ) = 0;
			virtual void					disconnect( Socket connection = -1 ) = 0;
			virtual void					update( void ) = 0;
			virtual u32						sendTo( const void* buffer, u32 size, Socket sendTo = -1 ) const = 0;

		protected:

			class TCPSocket*			m_parent;
		};
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_TCPSocket_H__	*/