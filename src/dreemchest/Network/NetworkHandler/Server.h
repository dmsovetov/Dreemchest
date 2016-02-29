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

#ifndef __DC_Network_Server_H__
#define __DC_Network_Server_H__

#include "NetworkApplication.h"
#include "../Sockets/TCPSocketListener.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! Base class for all server-side applications
    class Server : public Application {
    public:

		//! ClientConnected event is emitted when a new client is connected to a server.
		struct ClientConnected {
									//! Constructs ClientConnected instance.
									ClientConnected( const ConnectionPtr& connection )
										: m_connection( connection ) {}

			ConnectionPtr			m_connection;	//!< Opened connection instance.
		};

		//! ClientDisconnected event is emitted when a new client is disconnected from a server.
		struct ClientDisconnected {
									//! Constructs ClientDisconnected instance.
									ClientDisconnected( const ConnectionPtr& connection )
										: m_connection( connection ) {}

			ConnectionPtr			m_connection;	//!< Closed connection instance.
		};

        // ** NetworkHandler
        virtual void				update( u32 dt );
		virtual ConnectionList		eventListeners( void ) const;

		//! Creates a new server network handler and binds it to specified port.
		static ServerPtr			create( u16 port );

	protected:

									//! Constructs Server instance.
									Server( TCPSocketListenerPtr socketListener );

    private:

		//! Handles accepted incomming connection.
		void			            handleConnectionAccepted( const TCPSocketListener::Accepted& e );

		//! Handles a remote connection closed.
		void			            handleConnectionClosed( const TCPSocketListener::Closed& e );

	private:

		//! TCP socket listener.
		TCPSocketListenerPtr		m_socketListener;
    };
    
} // namespace Network

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Server_H__	*/