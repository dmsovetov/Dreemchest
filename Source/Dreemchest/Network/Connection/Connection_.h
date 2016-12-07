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

#ifndef __DC_Network_Connection1_H__
#define __DC_Network_Connection1_H__

#include "ConnectionMiddleware.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Remote connection interface wraps a network sockets and used for sending/receiving packets.
    class Connection_ : public InjectEventEmitter<RefCounted> {
	friend class Application;
    public:

        virtual                 ~Connection_( void ) {}

        //! Sets the connection id.
        void                    setId( u32 value );

        //! Returns the connection id.
        u32                     id( void ) const;

		//! Queues this connection for closing.
		void					closeLater( void );

		//! Returns true if the connection will be closed.
		bool					willBeClosed( void ) const;

		//! Returns the total amount of bytes received.
		s32						totalBytesReceived( void ) const;

		//! Returns the total amount of bytes sent.
		s32						totalBytesSent( void ) const;

		//! Returns current time.
		s32						time( void ) const;

		//! Returns the round trip time.
		s32						roundTripTime( void ) const;

		//! Returns current timeout value.
		s32						timeout( void ) const;

        //! Sends a packet over this connection.
        void                    send( const AbstractPacket& packet );

		//! Adds new connection middleware.
		void					addMiddleware( ConnectionMiddlewareUPtr instance );

	#if DREEMCHEST_CPP11
		//! Generic method to construct and sent the network packet over this connection.
		template<typename TPacket, typename ... TArgs>
		void					send( const TArgs& ... args );

		//! Generic method to construct and add the middleware to this connection.
		template<typename TMiddleware, typename ... TArgs>
		void					addMiddleware( const TArgs& ... args );
	#endif	/*	#if DREEMCHEST_CPP11	*/

        //! Base class for all connection events.
        struct Event {
                                //! Constructs Event instance.
                                Event( Connection_WPtr sender )
                                    : sender( sender ) {}
            Connection_WPtr     sender; //!< Connection instance that emitted this event.
        };

        //! This event is emitted when packet received over this connection.
        struct Received : public Event {
                                //! Constructs Received instance.
                                Received( Connection_WPtr sender, PacketTypeId type, s32 size, Io::ByteBufferWPtr packet )
                                    : Event( sender ), type( type ), size( size ), packet( packet ) {}
            PacketTypeId        type;   //!< Packet type identifier.
            s32                 size;   //!< Packet size.
            Io::ByteBufferWPtr  packet; //!< Received packet instance.
        };

        //! This event is emitted when a connection was closed.
        struct Closed : public Event {
                                //! Constructs Closed instance.
                                Closed( Connection_WPtr sender )
                                    : Event( sender ) {}            
        };

    protected:

		//! Network packet data header.
		struct Header {
            				    //! Constructs Header instance.
						        Header( PacketTypeId type = 0, u16 size = 0 )
							        : type( type ), size( size ) {}
			PacketTypeId	    type;	//!< Packet type.
			u16			        size;	//!< Total serialized data size.

            enum { Size = sizeof( PacketTypeId ) + sizeof( u16 ) };
		};

                                //! Constructs Connection instance.
                                Connection_( void );

        //! Tracks the specified amount of received data.
        void                    trackReceivedAmount( s32 value );

        //! Tracks the specified amount of sent data.
        void                    trackSentAmount( s32 value );

		//! Notifies about a received packet.
		void					notifyPacketReceived( PacketTypeId type, u16 size, Io::ByteBufferWPtr packet );	

		//! Sets the round trip time for this connection.
		void					setRoundTripTime( s32 value );

		//! Sets current time.
		void					setTime( s32 value );

        //! Writes the packet to a binary stream.
        s32                     writePacket( const AbstractPacket& packet, Io::ByteBufferWPtr stream ) const;

        //! Reads the packet from a binary stream to a temporary buffer and returns it's header.
        Header                  readPacket( Io::ByteBufferWPtr stream, Io::ByteBufferWPtr packet ) const;

		//! Updates this connection
		void					update( u32 dt );

        //! Sends a byte buffer over this connection.
        virtual s32             sendData( Io::ByteBufferWPtr data ) = 0;

        //! Closes this connection.
        virtual void            close( void ) = 0;

    private:

		//! Container type to store the list of connection middlewares.
		typedef List<ConnectionMiddlewareUPtr> ConnectionMiddlewares;
	
        u32                     m_id;                   //!< Connection id.
		s32						m_totalBytesReceived;   //!< The total amount of bytes received.
		s32						m_totalBytesSent;       //!< The total amount of bytes sent.
		s32						m_time;					//!< Current connection time.
		s32						m_timeout;				//!< The timeout counter reset each time a packet received.
		s32						m_roundTripTime;		//!< Current round trip time.
		bool					m_shouldClose;			//!< Indicates that a connection should be closed.
		ConnectionMiddlewares	m_middlewares;			//!< Connection middlewares added to connection.
    };

#if DREEMCHEST_CPP11
	// ** Connection::send
	template<typename TPacket, typename ... TArgs>
	void Connection_::send( const TArgs& ... args )
	{
		TPacket packet( args... );
		send( packet );
	}

	// ** Connection::addMiddleware
	template<typename TMiddleware, typename ... TArgs>
	void Connection_::addMiddleware( const TArgs& ... args )
	{
		addMiddleware( DC_NEW TMiddleware( args... ) );
	}
#endif	/*	#if DREEMCHEST_CPP11	*/

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_Connection_H__ */
