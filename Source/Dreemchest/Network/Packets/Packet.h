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

#ifndef __DC_Network_Packet_H__
#define __DC_Network_Packet_H__

#include "../Network.h"
#include "../NetworkHandler/Connection.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Base class for all network packets.
    class AbstractPacket {
    public:

        virtual                 ~AbstractPacket( void ) {}

        //! Returns the packet name.
        virtual CString         name( void ) const = 0;

        //! Returns the unique packet identifier.
        virtual PacketTypeId    id( void ) const = 0;

        //! Writes packet to a binary stream.
        virtual void            serialize( Io::StreamWPtr stream ) const = 0;

        //! Reads packet from a binary stream.
        virtual void            deserialize( Io::StreamWPtr stream ) = 0;
    };

    //! Template class to define packet types.
    template<typename TPacket>
    class Packet : public AbstractPacket {
    public:

        //! Returns the packet name.
        virtual CString         name( void ) const NIMBLE_OVERRIDE { return TypeInfo<TPacket>::name(); }

        //! Returns the unique packet identifier.
        virtual PacketTypeId    id( void ) const NIMBLE_OVERRIDE { return TypeInfo<TPacket>::id(); }

        //! Writes packet to a binary stream.
        virtual void            serialize( Io::StreamWPtr stream ) const NIMBLE_OVERRIDE {}

        //! Reads packet from a binary stream.
        virtual void            deserialize( Io::StreamWPtr stream ) NIMBLE_OVERRIDE {}
    };

    //! A binary blob type
    typedef Array<u8> BinaryBlob;

namespace Packets {

    //! RPC call packet.
    struct RemoteCall : public Packet<RemoteCall> {
        u16                id;         //!< Remote call identifier.
        u32                method;     //!< Remote call method identifier.
        TypeId          returnType;
        BinaryBlob      payload;    //!< Remote call argument.

                        //! Constructs RemoteCall instance.
                        RemoteCall( u16 id = 0, u32 method = 0, TypeId returnType = 0, const BinaryBlob& payload = BinaryBlob() )
                            : id( id ), method( method ), returnType( returnType ), payload( payload ) {}

        virtual void    serialize( Io::StreamWPtr stream ) const NIMBLE_OVERRIDE
        {
            u16 length = static_cast<u16>( payload.size() );

            stream->write( &id, sizeof id );
            stream->write( &method, sizeof method );
            stream->write( &returnType, sizeof returnType );
            stream->write( &length, sizeof length );
            stream->write( &payload[0], length );
        }

        virtual void    deserialize( Io::StreamWPtr stream ) NIMBLE_OVERRIDE
        {
            u16 length = 0;

            stream->read( &id, sizeof id );
            stream->read( &method, sizeof method );
            stream->read( &returnType, sizeof returnType );
            stream->read( &length, sizeof length );
            payload.resize( length );
            stream->read( &payload[0], length );
        }
    };

    //! RPC call response
    struct RemoteCallResponse : public Packet<RemoteCallResponse> {
        u16            id;
        Error        error;
        TypeId        returnType;
        BinaryBlob    payload;

                    //! Constructs RemoteCallResponse instance.
                    RemoteCallResponse( u16 id = 0, const Error& error = Error(), TypeId returnType = 0, const BinaryBlob& payload = BinaryBlob() )
                        : id( id ), error( error ), returnType( returnType ), payload( payload ) {}

        virtual void    serialize( Io::StreamWPtr stream ) const NIMBLE_OVERRIDE
        {
            u16 length = static_cast<u16>( payload.size() );

            stream->write( &id, sizeof id );
            stream->write( &error.code, sizeof error.code );
            stream->writeString( error.message.c_str() );
            stream->write( &returnType, sizeof returnType );
            stream->write( &length, sizeof length );
            stream->write( &payload[0], length );
        }

        virtual void    deserialize( Io::StreamWPtr stream ) NIMBLE_OVERRIDE
        {
            u16 length = 0;

            stream->read( &id, sizeof id );
            stream->read( &error.code, sizeof error.code );
            stream->readString( error.message );
            stream->read( &returnType, sizeof returnType );
            stream->read( &length, sizeof length );
            payload.resize( length );
            stream->read( &payload[0], length );
        }
    };

    //! Network event packet
    struct Event : public Packet<Event> {
        TypeId            eventId;
        BinaryBlob        payload;

                        //! Constructs Event instance.
                        Event( TypeId eventId = 0, const BinaryBlob& payload = BinaryBlob() )
                            : eventId( eventId ), payload( payload ) {}

        virtual void    serialize( Io::StreamWPtr stream ) const NIMBLE_OVERRIDE
        {
            u16 length = static_cast<u16>( payload.size() );

            stream->write( &eventId, sizeof eventId );
            stream->write( &length, sizeof length );
            stream->write( &payload[0], length );
        }

        virtual void    deserialize( Io::StreamWPtr stream ) NIMBLE_OVERRIDE
        {
            u16 length = 0;

            stream->read( &eventId, sizeof eventId );
            stream->read( &length, sizeof length );
            payload.resize( length );
            stream->read( &payload[0], length );
        }
    };

} // namespace Packets

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_Packet_H__ */
