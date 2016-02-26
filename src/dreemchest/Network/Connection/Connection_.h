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

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Remote connection interface wraps a network sockets and used for sending/receiving packets.
    class Connection_ : public InjectEventEmitter<RefCounted> {
    public:

        virtual                 ~Connection_( void ) {}

		//! Returns the total amount of bytes received.
		s32						totalBytesReceived( void ) const;

		//! Returns the total amount of bytes sent.
		s32						totalBytesSent( void ) const;

		//! Returns the round trip time.
		s32						roundTripTime( void ) const;

		//! Returns the time to live for this connection.
		s32						timeToLive( void ) const;

        //! Closes this connection.
        virtual void            close( void );

        //! Sends a packet over this connection.
        virtual void            sendPacket( const NetworkPacket& packet ) = 0;

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
                                Received( Connection_WPtr sender, NetworkPacketPtr packet )
                                    : Event( sender ), packet( packet ) {}
            NetworkPacketPtr    packet; //!< Received packet instance.
        };

        //! This event is emitted when a connection was closed.
        struct Closed : public Event {
                                //! Constructs Closed instance.
                                Closed( Connection_WPtr sender )
                                    : Event( sender ) {}            
        };

    protected:

                                //! Constructs Connection instance.
                                Connection_( void );

        //! Tracks the specified amount of received data.
        void                    trackReceivedAmount( s32 value );

        //! Tracks the specified amount of sent data.
        void                    trackSentAmount( s32 value );

    private:
	
		s32						m_totalBytesReceived;   //!< The total amount of bytes received.
		s32						m_totalBytesSent;       //!< The total amount of bytes sent.
    };

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_Connection_H__ */