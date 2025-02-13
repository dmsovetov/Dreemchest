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

#ifndef __DC_Network_TCPSocketListener_H__
#define __DC_Network_TCPSocketListener_H__

#include "TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Berkley TCP socket listener implementation.
    class TCPSocketListener NIMBLE_FINAL : public TCPSocket {
    public:

        //! Checks for incoming connections & updates existing.
        virtual void                    recv( void ) NIMBLE_OVERRIDE;

        //! Closes a socket listener.
        virtual void                    close( void ) NIMBLE_OVERRIDE;
        
        //! Returns a port that listener is bound to.
        u16                             port( void ) const;

        //! Returns a list of active connections.
        const TCPSocketList&            connections( void ) const;

        //! Creates and binds a new socket listener to a specified port.
        static TCPSocketListenerPtr        bindTo( u16 port );

        //! Binds a socket to a specified port.
        bool                            bind( u16 port );

    private:

                                        //! Constructs TCPSocketListener instance.
                                        TCPSocketListener( void );

        //! Accepst incoming connection.
        TCPSocketPtr                    acceptConnection( void );

        //! Setups FD sets used in select call.
        s32                             setupFDSets( fd_set& read, fd_set& write,  fd_set& except, SocketDescriptor& listener );

        //! Removes closed connections.
        void                            removeClosedConnections( void );

        //! Handles the socket closed event.
        void                            handleSocketClosed( const TCPSocket::Closed& e );

    private:

        u16                             m_port;             //!< Port this listener is bound to.
        TCPSocketList                    m_clientSockets;    //!< List of client connections.
    };

} // namespace Network

DC_END_DREEMCHEST

#endif    /*    !__DC_Network_TCPSocketListener_H__    */
