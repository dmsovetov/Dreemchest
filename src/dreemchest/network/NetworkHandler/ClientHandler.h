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

#ifndef		__DC_Network_ClientHandler_H__
#define		__DC_Network_ClientHandler_H__

#include "NetworkHandler.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class ClientHandler
    class ClientHandler : public NetworkHandler {
	friend class ClientSocketDelegate;
    public:

                            

  //      int                 GetServerTime( void ) const;

   //     bool                Connect( const NetworkAddress& address, u16 port );

		//! Return current connection.
		const ConnectionPtr&	connection( void ) const;
		ConnectionPtr&			connection( void );

		//! Creates a new NetworkClientHandler instance and connects to server.
		static ClientHandlerPtr	create( const NetworkAddress& address, u16 port );

        // ** NetworkHandler
        virtual void			update( void );
		virtual TCPSocketList	eventListeners( void ) const;

	protected:

								//! Constructs ClientHandler instance.
								ClientHandler( const TCPSocketPtr& socket );

	private:

		//! Client connection.
		ConnectionPtr			m_connection;

//    protected:

        // ** NetworkHandler
    //    virtual void        ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, int connection );
   //     virtual void        ProcessConnection( const NetworkAddress& address );

 //   private:

 //       TimeSync            m_timeSync;
 //       int                 m_serverTime;
    };

	//! Client socket handler.
	class ClientSocketDelegate : public TCPSocketDelegate {
	friend class ClientHandler;
	public:

		//! Handles a socket disconnection.
		virtual void		handleClosed( TCPSocket* sender );

		//! Handles a received data.
		virtual void		handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream );

	private:

		//! Parent client handler.
		ClientHandler*		m_clientHandler;
	};
    
} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkClientHandler_H__	*/