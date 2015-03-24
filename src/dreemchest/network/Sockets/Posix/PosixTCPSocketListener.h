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

#ifndef __DC_Network_PosixTCPSocketListener_H__
#define __DC_Network_PosixTCPSocketListener_H__

#include "../TCPSocketListener.h"
#include "PosixNetwork.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Berkley TCP socket listener implementation.
	class PosixTCPSocketListener : public impl::TCPSocketListenerPrivate {
	public:

										PosixTCPSocketListener( TCPSocketListenerDelegate* delegate );

		// ** TCPSocketListenerPrivate
		virtual void					update( void );
		virtual void					close( void );
        virtual u16                     port( void ) const;
		virtual const TCPSocketList&	connections( void ) const;

		//! Binds a socket to a specified port.
		bool							bindTo( u16 port );

	private:

		//! Accepst incoming connection.
		TCPSocketPtr					acceptConnection( void );

		//! Setups FD sets used in select call.
		void							setupFDSets( fd_set& read, fd_set& write,  fd_set& except, SocketDescriptor& listener );

	private:

		//! TCP socket listener delegate.
		TCPSocketListenerDelegatePtr	m_delegate;

		//! Socket descriptor.
		SocketDescriptor				m_socket;
        
        //! Port used.
        u16                             m_port;

		//! List of client connections.
        TCPSocketList					m_clientSockets;

		//! A client connection delegate to dispatch client events to parent delegate.
		class TCPClientDelegate : public TCPSocketDelegate {
		public:
                                //! Constructs TCPClientDelegate instance.
                                TCPClientDelegate( PosixTCPSocketListener* listener );

            //! Throws a received data to TCP listener delegate.
            virtual void		handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream );
            virtual void        handleClosed( TCPSocket* sender );

		private:

			//! Parent socket listener.
			WeakPtr<PosixTCPSocketListener>	m_listener;
		};
	};

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PosixTCPSocketListener_H__	*/