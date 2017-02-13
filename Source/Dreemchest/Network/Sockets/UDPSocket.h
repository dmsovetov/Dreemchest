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

#ifndef __DC_Network_UDPSocket_H__
#define __DC_Network_UDPSocket_H__

#include "Socket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Datagram socket class.
    class UDPSocket NIMBLE_FINAL : public Socket {
    public:

        //! Sends a datagram to a specified address & port.
        u32                     send( const Address& address, u16 port, const void* buffer, u32 size );

        //! Starts listening for datagrams at a given port.
        bool                    listen( u16 port );

        //! Checks if any data has been received.
        void                    recv( void );

        //! Creates a new UDP socket instance.
        static UDPSocketPtr        create( void );

        //! Creates a new broadcast UDP socket instance.
        static UDPSocketPtr        createBroadcast( void );

        //! This event is emitted when packet was received.
        struct Data {
                                //! Constructs Data instance.
                                Data( UDPSocketWPtr sender, const Address& address, Io::ByteBufferWPtr data )
                                    : sender( sender ), address( address ), data( data ) {}

            UDPSocketWPtr       sender;     //!< Socket instance that received packet.
            Address             address;    //!< Sender remote address.
            Io::ByteBufferWPtr  data;       //!< Received data.
        };

    private:

                                //! Constructs a UDPSocket instance.
                                UDPSocket( bool broadcast );
    };
    
} // namespace Network

DC_END_DREEMCHEST

#endif    /*    !__DC_Network_UDPSocket_H__    */
