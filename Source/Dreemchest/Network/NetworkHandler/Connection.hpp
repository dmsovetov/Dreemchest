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

#ifndef __Network_Connection_Hpp__
#define __Network_Connection_Hpp__

DC_BEGIN_DREEMCHEST

namespace Network {

namespace Private {
    
    //! Writes a network argument value to a binary stream
    struct PrimitiveValueWriter {
        template<typename TValue>
        static Io::ByteBufferPtr write( const TValue& value )
        {
            NIMBLE_STATIC_ASSERT( false, "the specified type could not be sent over a network" );
            return Io::ByteBufferPtr();
        }
        
        template<typename TValue>
        static TValue read( Io::StreamPtr stream )
        {
            NIMBLE_STATIC_ASSERT( false, "the specified type could not be sent over a network" );
            return TValue();
        }
        
        template<>
        static Io::ByteBufferPtr write( const KeyValue& value )
        {
            Io::ByteBufferPtr buffer = Io::ByteBuffer::create();
            Io::BinaryVariantStream stream( buffer );
            stream.write( Variant::fromValue( value ) );
            return buffer;
        }
        
        template<>
        static KeyValue read( Io::StreamPtr stream )
        {
            Io::BinaryVariantStream reader( stream );
            Variant kv = Variant::fromValue( KeyValue() );
            reader.read( kv );
            return kv.as<KeyValue>();
        }
    };
    
    struct MetaObjectWriter {
        template<typename T>
        static Io::ByteBufferPtr write( const T& value )
        {
            Reflection::Serializer serializer;
            KeyValue ar;
            serializer.serialize( value.metaInstance(), ar );
            return PrimitiveValueWriter::write( ar );
        }
        
        template<typename T>
        static T read( Io::StreamPtr stream )
        {
            Reflection::Serializer serializer;
            KeyValue kv = PrimitiveValueWriter::read<KeyValue>( stream );
            T v;
            serializer.deserialize( v.metaInstance(), kv );
            return v;
        }
    };
    
    struct StreamableWriter {
        template<typename T>
        static Io::ByteBufferPtr write( const T& value )
        {
            Io::ByteBufferPtr stream = Io::ByteBuffer::create();
            value.serialize( stream );
            return stream;
        }
        
        template<typename T>
        static T read( Io::StreamPtr stream )
        {
            T v;
            v.deserialize( stream );
            return v;
        }
    };
    
    template<typename TValue>
    Io::ByteBufferPtr writeToStream( const TValue& value )
    {
        return TypeSelector<Reflection::Has_staticMetaObject<TValue>::value, MetaObjectWriter, TypeSelector<IsBaseOf<Io::Streamable, TValue>::value, StreamableWriter, PrimitiveValueWriter>::type>::type::write<TValue>( value );
    }
    
    template<typename TValue>
    TValue readFromStream( Io::StreamPtr stream )
    {
        return TypeSelector<Reflection::Has_staticMetaObject<TValue>::value, MetaObjectWriter, TypeSelector<IsBaseOf<Io::Streamable, TValue>::value, StreamableWriter, PrimitiveValueWriter>::type>::type::read<TValue>( stream );
    }
    
} // namespace Private

// ** Connection::invokeVoid
template<typename TRemoteProcedure>
void Connection::invokeVoid( const typename TRemoteProcedure::Argument& argument )
{
    Io::ByteBufferPtr buffer = Private::writeToStream( argument );
    send<Packets::RemoteCall>( 0, TRemoteProcedure::id(), 0, buffer->array() );
}

// ** Connection::invoke
template<typename TRemoteProcedure>
void Connection::invoke( const typename TRemoteProcedure::Argument& argument, const typename RemoteResponseHandler<typename TRemoteProcedure::Response>::Callback& callback )
{
    Io::ByteBufferPtr buffer = Private::writeToStream( argument );
    
    // Send an RPC request
    u16     remoteCallId = m_nextRemoteCallId++;
    TypeId  returnTypeId = TypeInfo<typename TRemoteProcedure::Response>::id();
    
    send<Packets::RemoteCall>( remoteCallId, TRemoteProcedure::id(), returnTypeId, buffer->array() );
    
    // Create a response handler.
    m_pendingRemoteCalls[remoteCallId] = PendingRemoteCall( TRemoteProcedure::name(), DC_NEW RemoteResponseHandler<typename TRemoteProcedure::Response>( callback ) );
}

// ** Connection::emit
template<typename TEvent>
void Connection::emit( const TEvent& e )
{
    // Serialize event to a byte buffer.
    Io::ByteBufferPtr buffer = Private::writeToStream( e );
    
    // Send the packet
    send<Packets::Event>( TypeInfo<TEvent>::id(), buffer->array() );
}

//! Send a response to caller.
template<typename T>
inline bool Response<T>::operator()( const T& value, const Error& error )
{
    // Serialize argument to a byte buffer.
    Io::ByteBufferPtr buffer = Private::writeToStream( value );
    
    // Send an RPC response packet.
    m_connection->send<Packets::RemoteCallResponse>( m_id, error, TypeInfo<T>::id(), buffer->array() );
    
    // Mark this response as sent.
    m_wasSent = true;
    
    return true;
}
    
} // namespace Network

DC_END_DREEMCHEST

#endif    /*    !__Network_Connection_Hpp__    */
