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

#ifndef __DC_Network_PacketHandler_H__
#define __DC_Network_PacketHandler_H__

#include "Packet.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Abstract packet handler to process incomming packets.
    class AbstractPacketHandler {
    public:

        virtual                 ~AbstractPacketHandler( void ) {}

        //! Abstract method to process a packet instance.
        virtual void            process( ConnectionWPtr connection, const AbstractPacket& packet ) = 0;

        //! Returns the handled packet type.
        virtual PacketTypeId    packetTypeId( void ) const = 0;
    };

    //! Template class to define packet handlers.
    template<typename TPacket>
    class PacketHandler : public AbstractPacketHandler {
    public:

        
        typedef TPacket         Packet; //!< Save the packet type handled by this class.

        //! Returns the handled packet type.
        virtual PacketTypeId    packetTypeId( void ) const NIMBLE_OVERRIDE { return TypeInfo<TPacket>::id(); }

    private:

        //! Casts the incoming abstract packet to a requested type and runs the processing function
        virtual void            process( ConnectionWPtr connection, const AbstractPacket& packet ) NIMBLE_OVERRIDE;

    protected:

        //! Abstract method to process the type-casted network packet.
        virtual void            process( ConnectionWPtr connection, const TPacket& packet ) = 0;
    };

    // ** PacketHandler::process
    template<typename TPacket>
    void PacketHandler<TPacket>::process( ConnectionWPtr connection, const AbstractPacket& packet )
    {
        process( connection, static_cast<const TPacket&>( packet ) );
    }

    //! Template class to define packet handlers that dispatch packet processing to a callback method.
    template<typename TPacket>
    class PacketHandlerCallback : public PacketHandler<TPacket> {
    public:

        //! Function type used for packet processing.
        typedef cClosure<void(ConnectionWPtr, const TPacket&)>  Callback;

                                //! Constructs the PacketHandlerCallback instance.
                                PacketHandlerCallback( const Callback& callback );

    private:

        //! This method runs a processing callback.
        virtual void            process( ConnectionWPtr connection, const TPacket& packet );

    private:

        Callback                m_callback; //!< Packet processing callback.
    };

    // ** PacketHandlerCallback::PacketHandlerCallback
    template<typename TPacket>
    PacketHandlerCallback<TPacket>::PacketHandlerCallback( const Callback& callback )
        : m_callback( callback )
    {
    
    }

    // ** PacketHandlerCallback::process
    template<typename TPacket>
    void PacketHandlerCallback<TPacket>::process( ConnectionWPtr connection, const TPacket& packet )
    {
        m_callback( connection, packet );
    }

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_PacketHandler_H__ */
