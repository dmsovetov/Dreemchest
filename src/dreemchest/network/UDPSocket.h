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

#ifndef		__DC_Network_UDPSocket_H__
#define		__DC_Network_UDPSocket_H__

#include    "Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

	BeginPrivateInterface( UDPSocket )
		InterfaceMethod( u32  send( const NetworkAddress& address, u16 port, const void* buffer, u32 size ) )
		InterfaceMethod( bool listen( u16 port ) )
		InterfaceMethod( void update( void ) )
	EndPrivateInterface

    //! Datagram socket class.
    class UDPSocket : public RefCounted {
    public:

							//! Constructs a UDPSocket instance.
                            UDPSocket( impl::UDPSocketPrivate* impl = NULL );
        virtual             ~UDPSocket( void );

		//! Sends a datagram to a specified address & port.
        u32                 send( const NetworkAddress& address, u16 port, const void* buffer, u32 size );

		//! Starts listening for datagrams at a given port.
        bool                listen( u16 port );

		//! Checks if any data has been received.
        void                update( void );

		//! Creates a new UDP socket instance.
		static UDPSocketPtr	create( UDPSocketDelegate* delegate, bool broadcast );

    private:

		//! UDP socket implementation.
		impl::UDPSocketPrivatePtr	m_impl;
    };

	//! UDP socket event delegate.
	class UDPSocketDelegate : public RefCounted {
	public:

		virtual				~UDPSocketDelegate( void ) {}

		//! Handles socket listen failure.
		virtual void		handleListenFailure( UDPSocket* sender ) {}

		//! Handles socked listen success.
		virtual void		handleListenSuccess( UDPSocket* sender ) {}

		//! Handles received data.
		virtual void		handleReceivedData( UDPSocket* sender, const NetworkAddress& address, const void* data, u32 size ) {}
	};

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_UDPSocket_H__	*/