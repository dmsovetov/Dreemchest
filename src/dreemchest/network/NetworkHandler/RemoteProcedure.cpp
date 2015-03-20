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

#include	"RemoteProcedure.h"
#include    "NetworkHandler.h"

DC_BEGIN_DREEMCHEST

namespace net {
/*
// ** RemoteProcedure::RemoteProcedure
RemoteProcedure::RemoteProcedure( dcContext ctx, NetworkHandler *networkHandler ) : m_ctx( ctx ), m_networkHandler( networkHandler ), m_callbackId( 0 )
{
//    m_networkHandler->AttachListener( NetworkHandlerEvent::PacketReceived, dcThisMethod( RemoteProcedure::OnPacketReceived ) );
}

RemoteProcedure::~RemoteProcedure( void )
{
	DC_BREAK
//    m_networkHandler->DetachListener( NetworkHandlerEvent::PacketReceived, dcThisMethod( RemoteProcedure::OnPacketReceived ) );
}

// ** RemoteProcedure::RegisterRemoteProcedure
bool RemoteProcedure::RegisterRemoteProcedure( const char *name, const RemoteProcedureCallback& callback )
{
    strhash                     hash = cStringHash::Djb2( name );
    RemoteProcedures::iterator  i    = m_procedures.find( hash );
    if( i != m_procedures.end() ) {
        return false;
    }

    m_procedures[hash] = callback;
    return true;
}

// ** RemoteProcedure::InvokeRemoteProcedure
bool RemoteProcedure::InvokeRemoteProcedure( const char *name, const cValue *args, int count )
{
    return SendCallPacket( name, args, count );
}

// ** RemoteProcedure::InvokeRemoteProcedure
bool RemoteProcedure::InvokeRemoteProcedure( const char *name, const dreemchest::cValue *args, int count, RemoteProcedureResponseCallback callback )
{
    m_callbackId++;
    
    SendCallPacket( name, args, count, m_callbackId );
    m_callbacks[m_callbackId] = callback;
    
    return true;
}

// ** RemoteProcedure::InvokeLocalProcedure
bool RemoteProcedure::InvokeLocalProcedure( const char *name, const cValue *args, int count, int identifier, int caller )
{
    RemoteProcedures::const_iterator  i = m_procedures.find( cStringHash::Djb2( name ) );
    if( i == m_procedures.end() ) {
        return false;
    }

    // ** Run a callback
    cValue result;
    i->second( result, args, count );

    // ** Send results back to a caller
    if( identifier >= 0 ) {
        SendResponsePacket( result, identifier, caller );
    }

    return true;
}

// ** RemoteProcedure::InvokeLocalResponse
bool RemoteProcedure::InvokeLocalResponse( int identifier, const cValue& value )
{
    RemoteProcedureCallbacks::iterator i = m_callbacks.find( identifier );
    if( i == m_callbacks.end() ) {
        return false;
    }

    i->second( value );
    m_callbacks.erase( i );

    return true;
}

// ** RemoteProcedure::SendCallPacket
bool RemoteProcedure::SendCallPacket( const char *name, const cValue *args, int count, int identifier )
{
    sRemoteCallPacket rpc;
    rpc.name        = name;
    rpc.identifier  = identifier;
    for( int i = 0; i < count; i++ ) {
        rpc.args.push_back( args[i] );
    }

    return m_networkHandler->SendPacket( sRemoteCallPacket::PacketId, &rpc );
}

// ** RemoteProcedure::SendResponsePacket
bool RemoteProcedure::SendResponsePacket( const cValue& value, int identifier, int caller )
{
    sRemoteResponsePacket response;
    response.identifier = identifier;
    response.value      = value;

    return m_networkHandler->SendPacket( sRemoteResponsePacket::PacketId, &response, caller );
}

// ** RemoteProcedure::OnPacketReceived
void RemoteProcedure::OnPacketReceived( const dcEvent e )
{
    DC_ENSURE_TYPE( e, NetworkHandlerEvent );
    dcNetworkHandlerEvent ne = ( dcNetworkHandlerEvent )e;

    switch( ne->m_packetId ) {
    case sRemoteCallPacket::PacketId:       {
                                                DC_ENSURE_TYPE( ne->m_packet, sRemoteCallPacket )
                                                sRemoteCallPacket *rpc = ( sRemoteCallPacket* )ne->m_packet;

                                                if( !InvokeLocalProcedure( rpc->name.c_str(), &rpc->args[0], rpc->args.size(), rpc->identifier, ne->m_connection ) ) {
                                                    m_ctx->iLog->Warning( "RemoteProcedure::OnPacketReceived : no such method %s\n", rpc->name.c_str() );
                                                }
                                            }
                                            break;

    case sRemoteResponsePacket::PacketId:   {
                                                DC_ENSURE_TYPE( ne->m_packet, sRemoteResponsePacket )
                                                sRemoteResponsePacket *response = ( sRemoteResponsePacket* )ne->m_packet;

                                                if( !InvokeLocalResponse( response->identifier, response->value ) ) {
                                                    m_ctx->iLog->Warning( "RemoteProcedure::OnPacketReceived : no such response receiver %d\n", response->identifier );
                                                }
                                            }
                                            break;
    }
}
*/
} // namespace net

DC_END_DREEMCHEST
