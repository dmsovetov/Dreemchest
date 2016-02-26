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

#ifndef	__DC_Network_TCPSocket_H__
#define	__DC_Network_TCPSocket_H__

#include "TCPStream.h"

DC_BEGIN_DREEMCHEST

namespace net {

    //! TCP socket.
	class TCPSocket NIMBLE_FINAL : public InjectEventEmitter<RefCounted> {
    friend class TCPSocketListener;
    public:

        virtual						~TCPSocket( void );

        //! Returns a remote address.
		const NetworkAddress&	    address( void ) const;

        //! Returns a socket descriptor.
		const SocketDescriptor&	    descriptor( void ) const;

        //! Returns true if this socket is valid.
		bool					    isValid( void ) const;

        //! Connects to a TCP socket at a given remote address and port.
        bool					    connect( const NetworkAddress& address, u16 port );

        //! Closes a socket.
        void					    close( void );

        //! Reads all incoming data.
        void					    fetch( void );

		//! Sends data to socket.
		/*
		\param buffer Data to be sent.
		\param size Data size to be sent.
		*/
        u32						    send( const void* buffer, u32 size );

        //! Connects to a TCP socket at a given remote address and port.
        static TCPSocketPtr		    connectTo( const NetworkAddress& address, u16 port );

        //! Base class for all TCP socket events.
        struct Event {
                                    //! Constructs Event instance.
                                    Event( TCPSocketWPtr sender )
                                        : sender( sender ) {}
            TCPSocketWPtr           sender; //!< Pointer to a socket that emitted this event.      
        };

        //! This event is emitted when new data is received from a remote connection.
        struct Data : public Event {
                                    //! Constructs Data event instance.
                                    Data( TCPSocketWPtr sender, TCPStreamWPtr stream )
                                        : Event( sender ), stream( stream ) {}
            TCPStreamWPtr           stream; //!< TCP stream that contains received data.
        };

        //! This event is emitted when socket is closed.
        struct Closed : public Event {
                                    //! Constructs Closed event instance.
                                    Closed( TCPSocketWPtr sender )
                                        : Event( sender ) {}
        };

    private:

                                    //! Constructs a TCPSocket instance.
                                    TCPSocket( SocketDescriptor& descriptor = SocketDescriptor::Invalid, const NetworkAddress& address = NetworkAddress::Null );

    private:

        SocketDescriptor			m_descriptor;   //!< Socket descriptor.
		TCPStreamPtr				m_stream;       //!< Socket receiving buffer.
        NetworkAddress				m_address;      //!< Remote socket address.
    };

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_TCPSocket_H__	*/