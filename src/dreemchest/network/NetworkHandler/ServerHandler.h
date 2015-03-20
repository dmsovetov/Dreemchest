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

#ifndef		__DC_Network_ServerHandler_H__
#define		__DC_Network_ServerHandler_H__

#include "NetworkHandler.h"
#include "../Sockets/TCPSocketListener.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Server-side network handler.
    class ServerHandler : public NetworkHandler {
	friend class ServerSocketDelegate;
    public:

        // ** NetworkHandler
        virtual void				update( void );

		//! Creates a new server network handler and binds it to specified port.
		static ServerHandlerPtr		create( u16 port );

	protected:

									//! Constructs ServerHandler instance.
									ServerHandler( const TCPSocketListenerPtr& socketListener );

		//! Processes a client connection.
		virtual void				processClientConnection( TCPSocket* socket );

		//! Processes a client disconnection.
		virtual void				processClientDisconnection( TCPSocket* socket );

	private:

		//! TCP socket listener.
		TCPSocketListenerPtr		m_socketListener;

//    protected:

//        virtual void        ProcessAcceptedConnection( const NetworkAddress& address, int connection );
//        virtual void        ProcessClosedConnection( int connection );

        // ** NetworkHandler
 //       virtual void        ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, int connection );

//    private:

//        void                OnConnectionAccepted( const dcEvent e );
//        void                OnConnectionClosed( const dcEvent e );

//    private:

   //     int                 m_time;
    };

	//! Server socket listener handler.
	class ServerSocketDelegate : public TCPSocketListenerDelegate {
	friend class ServerHandler;
	public:

		//! Handles incoming data from client.
		virtual void			handleReceivedData( TCPSocketListener* sender, TCPSocket* socket, TCPStream* stream );

		//! Handles accepted incomming connection.
		virtual void			handleConnectionAccepted( TCPSocketListener* sender, TCPSocket* socket );

		//! Handles a remote connection closed.
		virtual void			handleConnectionClosed( TCPSocketListener* sender, TCPSocket* socket );

	private:

		//! Parent network server handler.
		ServerHandler*			m_serverHandler;
	};
    
} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_ServerHandler_H__	*/