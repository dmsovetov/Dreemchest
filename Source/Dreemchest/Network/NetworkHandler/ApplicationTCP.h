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

#ifndef __DC_Network_ApplicationTCP_H__
#define __DC_Network_ApplicationTCP_H__

#include "NetworkApplication.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Base class for all network applications that use TCP as a transport protocol.
    class ApplicationTCP : public Application {
    public:

        virtual                     ~ApplicationTCP( void ) DC_DECL_OVERRIDE;

        //! Connects the TCP application to a remote host.
        static ApplicationTCPPtr    connect( const Address& address, u16 port );

        //! Launches a listening TCP application on a specified port.
        static ApplicationTCPPtr    listen( u16 port );

    private:

                                    //! Constructs ApplicationTCP instance.
                                    ApplicationTCP( TCPSocketPtr socket );

		//! Handles socket connection event (accepting remote connection for listening socket).
		void			            handleSocketConnected( const TCPSocket::Connected& e );

		//! Handles socket closed event (closing remote connection for listening socket).
		void			            handleSocketClosed( const TCPSocket::Closed& e );

		//! Receives and processes all incoming TCP data.
		virtual void			    update( u32 dt ) DC_DECL_OVERRIDE;

    private:

        //! Container type to map from a TCP socket instance to a connection.
        typedef Map<TCPSocketWPtr, ConnectionWPtr> ConnectionBySocket;

        TCPSocketPtr                m_socket;               //!< TCP socket instance.
        ConnectionBySocket          m_connectionBySocket;   //!< Socket to connection mapping.
        bool                        m_workaroundClientConn;
    };

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  !__DC_Network_ApplicationTCP_H__ */