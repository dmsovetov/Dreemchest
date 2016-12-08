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

#ifndef __DC_Network_ConnectionTCP_H__
#define __DC_Network_ConnectionTCP_H__

#include "Connection_.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Reliable network connection that uses a TCP socket as a data transmit protocol.
    class ConnectionTCP : public Connection_ {
    public:

                            //! Cleans up the socket event subscribtions.
        virtual             ~ConnectionTCP( void );

        //! Returns the connection TCP socket.
        TCPSocketWPtr       socket( void ) const;

        //! Returns a remote address of a connection.
        const Address&        address( void ) const;

        //! Closes this TCP connection.
        virtual void        close( void ) NIMBLE_OVERRIDE;

    protected:

                            //! Constructs ConnectionTCP instance.
                            ConnectionTCP( TCPSocketPtr socket );

        //! Sends a byte buffer over TCP connection.
        virtual s32         sendData( Io::ByteBufferWPtr stream ) NIMBLE_OVERRIDE;

        //! Splits the received data into packets and emits notifications.
        void                handleSocketData( const TCPSocket::Data& e );

        //! Closes this connection after a socket closed event.
        void                handleSocketClosed( const TCPSocket::Closed& e );

    private:

        TCPSocketPtr        m_socket;   //!< TCP socket instance.
        Io::ByteBufferPtr   m_packet;   //!< Packet temporary read buffer.
    };

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  !__DC_Network_ConnectionTCP_H__ */
