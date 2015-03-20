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

#include "NetworkHandler.h"
#include "../Sockets/TCPSocket.h"
#include "../Sockets/TCPStream.h"
#include "../../io/streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** NetworkHandler::NetworkHandler
NetworkHandler::NetworkHandler( void )
{
	registerPacketHandler<packets::Ping> ( dcThisMethod( NetworkHandler::handlePingPacket ) );
	registerPacketHandler<packets::Pong> ( dcThisMethod( NetworkHandler::handlePongPacket ) );
	registerPacketHandler<packets::Event>( dcThisMethod( NetworkHandler::handleEventPacket ) );
}

// ** NetworkHandler::sendPacket
void NetworkHandler::sendPacket( TCPSocket* socket, NetworkPacket* packet )
{
	io::ByteBufferPtr buffer = io::ByteBuffer::create();

	// ** Set packet timestamp
	packet->timestamp = UnixTime();

	// ** Write packet to binary stream
	u32 bytesWritten = m_packetParser.writeToStream( packet, buffer );

	// ** Send binary data to socket
	s32 bytesSent = socket->sendTo( buffer->buffer(), buffer->length() );
	DC_BREAK_IF( bytesWritten != bytesSent );
}

// ** NetworkHandler::processReceivedData
void NetworkHandler::processReceivedData( TCPSocket* socket, TCPStream* stream )
{
	log::verbose( "%d bytes of data received from %s\n", stream->bytesAvailable(), socket->address().toString() );

    io::ByteBufferPtr source( stream );
	NetworkPacket*    packet = m_packetParser.parseFromStream( source );

	if( !packet ) {
		log::warn( "Failed to parse packed from data sent by %s\n", socket->address().toString() );
		return;
	}

	PacketHandlers::iterator i = m_packetHandlers.find( packet->typeId() );
	DC_BREAK_IF( i == m_packetHandlers.end() );

	log::verbose( "Packet [%s] received from %s\n", packet->typeName(), socket->address().toString() );
	if( !i->second->handle( socket, packet ) ) {
		log::warn( "NetworkHandler::processReceivedData : invalid packet of type %s received from %s\n", packet->typeName(), socket->address().toString() );
	}

	delete packet;
}

// ** NetworkHandler::eventListeners
TCPSocketList NetworkHandler::eventListeners( void ) const
{
	return TCPSocketList();
}

// ** NetworkHandler::doLatencyTest
void NetworkHandler::doLatencyTest( TCPSocket* socket, u8 iterations )
{
	sendPacket<packets::Ping>( socket, iterations );
}

// ** NetworkHandler::handlePingPacket
bool NetworkHandler::handlePingPacket( TCPSocket* sender, const packets::Ping* packet )
{
	sendPacket<packets::Pong>( sender, packet->iteration );
	return true;
}

// ** NetworkHandler::handlePongPacket
bool NetworkHandler::handlePongPacket( TCPSocket* sender, const packets::Pong* packet )
{
	if( packet->iteration == 0 ) {
		log::verbose( "To much ping packets sent\n" );
		return true;
	}

	sendPacket<packets::Ping>( sender, packet->iteration - 1 );
	return true;
}

// ** NetworkHandler::handleEventPacket
bool NetworkHandler::handleEventPacket( TCPSocket* sender, const packets::Event* packet )
{
	return false;
}

// ** NetworkHandler::update
void NetworkHandler::update( void )
{
}

} // namespace net

DC_END_DREEMCHEST

/*
#include "NetworkHandler.h"
//#include    "PacketFormatter.h"
//#include    "RemoteProcedure.h"

//#include "../../io/streams/ByteBuffer.h"
//#include "../TCPSocket.h"
//#include "../UDPSocket.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** NetworkHandler::NetworkHandler
NetworkHandler::NetworkHandler( dcContext ctx ) : EventDispatcher( ctx )
{
    m_netTCP            = TCPSocket::create();
    m_netBroadcast      = UDPSocket::create( NULL, true );
    m_netDatagram       = UDPSocket::create( NULL, false );
    m_packetParser      = DC_NEW PacketFormatter( PacketMagic );
    m_outputStream      = DC_NEW io::ByteBuffer::create( MaxPacketBufferSize );
    m_remoteProcedure   = DC_NEW RemoteProcedure( m_ctx, this );

    // ** Register built-in packets
//    RegisterPacket( sTimeSyncPacket::PacketId,       DC_NEW sTimeSyncPacket );
//    RegisterPacket( sRemoteCallPacket::PacketId,     DC_NEW sRemoteCallPacket );
//    RegisterPacket( sRemoteResponsePacket::PacketId, DC_NEW sRemoteResponsePacket );
//    RegisterPacket( sSendFile::PacketId,             DC_NEW sSendFile );
//    RegisterPacket( sFileChunk::PacketId,            DC_NEW sFileChunk );

    // ** TCP socket listeners
//    m_netTCP->AttachListener( TCPSocketEvent::Data,         dcThisMethod( NetworkHandler::OnDataReceived ) );
//    m_netTCP->AttachListener( TCPSocketEvent::Disconnected, dcThisMethod( NetworkHandler::OnDisconnected ) );

    // ** UDP socket listeners
//    m_netDatagram->AttachListener( UDPSocketEvent::Data, dcThisMethod( NetworkHandler::OnPacketUDP ) );
//    m_netBroadcast->AttachListener( UDPSocketEvent::Data, dcThisMethod( NetworkHandler::OnPacketUDP ) );
}
    
NetworkHandler::~NetworkHandler( void )
{
//    for( InputStreams::iterator i = m_inputStreams.begin(), end = m_inputStreams.end(); i != end; ++i ) {
//        delete i->second;
//    }

    DC_DELETE( m_remoteProcedure )
    DC_DELETE( m_outputStream )
    DC_DELETE( m_packetParser )
}

// ** NetworkHandler::ProcessConnection
void NetworkHandler::ProcessConnection( const NetworkAddress& address )
{
    m_flags.On( Connected );
	DC_BREAK;
//    DispatchEvent( NetworkHandlerEvent::Connected, NetworkHandlerEvent::OnConnected( address ) );
}

// ** NetworkHandler::ProcessDisconnection
void NetworkHandler::ProcessDisconnection( void )
{
    m_netTCP->Disconnect();
    
//  DC_DELETE( m_netTCP )
//  DC_DELETE( m_netUDP )
    Warning( "NetworkHandler::ProcessDisconnection sockets are not cleaned up [dev]\n" );
    
    m_flags.Off( Connected );
	DC_BREAK;
//    DispatchEvent( NetworkHandlerEvent::Disconnected, NetworkHandlerEvent::OnDisconnected() );
}

// ** NetworkHandler::ProcessFailure
void NetworkHandler::ProcessFailure( void )
{
	DC_BREAK
//    DispatchEvent( NetworkHandlerEvent::Failure, NetworkHandlerEvent::OnFailure() );
}

// ** NetworkHandler::ProcessReceivedPacket
void NetworkHandler::ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, Socket connection )
{
    switch( packetId ) {
    case sSendFile::PacketId:   {
                                    DC_ENSURE_TYPE( packet, sSendFile )
                                    sSendFile *file = ( sSendFile* )packet;
                                    DispatchEvent( NetworkHandlerEvent::FileAccepted, NetworkHandlerEvent::OnFileAccepted( file->fileName.c_str(), file->length ) );
                                }
                                break;

    case sFileChunk::PacketId:  {
                                    DC_ENSURE_TYPE( packet, sFileChunk )
                                    sFileChunk *chunk = ( sFileChunk* )packet;
                                    DispatchEvent( NetworkHandlerEvent::FileChunk, NetworkHandlerEvent::OnFileChunk( &chunk->data[0], chunk->data.size() ) );
                                }
                                break;

    default:                    DispatchEvent( NetworkHandlerEvent::PacketReceived, NetworkHandlerEvent::OnPacketReceived( packetId, packet, connection, address ) );
                                break;
    }
}

// ** NetworkHandler::ListenDatagrams
bool NetworkHandler::ListenDatagrams( u16 port )
{
    DC_BREAK_IF( IsListeningDatagrams() )
    
    bool listening = m_netDatagram->Listen( port );
    m_flags.Set( ListeningDatagrams, listening );

    return listening;
}

// ** NetworkHandler::ListenBroadcast
bool NetworkHandler::ListenBroadcast( u16 port )
{
    DC_BREAK_IF( IsListeningBroadcast() )
    
    bool listening = m_netBroadcast->Listen( port );
    m_flags.Set( ListeningBroadcast, listening );

    return listening;
}

// ** NetworkHandler::Disconnect
void NetworkHandler::Disconnect( void )
{
    m_flags.Off( Connected );
}

// ** NetworkHandler::Update
bool NetworkHandler::Update( int dt )
{
    if( IsConnected() ) {
        m_netTCP->Update();
    }

    if( IsListeningDatagrams() ) {
        m_netDatagram->Update();
    }

    if( IsListeningBroadcast() ) {
        m_netBroadcast->Update();
    }

    return IsConnected();
}

// ** NetworkHandler::SendPacket
bool NetworkHandler::SendPacket( int packetId, const INetworkPacket *packet, Socket connection )
{
    DC_BREAK_IF( !IsConnected() );

    // ** Write packet to stream
    m_outputStream->setPosition( 0 );
    int bytesWritten = m_packetParser->WritePacketToStream( packetId, packet, m_outputStream );
    DC_BREAK_IF( bytesWritten <= 0 );

    // ** Send stream to receiver
    int bytesSent = m_netTCP->Send( m_outputStream->buffer(), bytesWritten, connection );
    DC_BREAK_IF( bytesWritten != bytesSent );

    return bytesSent == bytesWritten;
}

// ** NetworkHandler::SendFile
bool NetworkHandler::SendFile( const io::StreamPtr& file, const char *fileName, Socket connection )
{
    DC_BREAK_IF( file == NULL );
    DC_BREAK_IF( fileName == NULL );

    // ** Send file request
    sSendFile sendFile;
    sendFile.fileName = fileName;
    sendFile.length   = file->length();

    if( !SendPacket( sSendFile::PacketId, &sendFile, connection ) ) {
        return false;
    }

    // ** Send file chunks
    sFileChunk chunk;
    
    while( file->hasDataLeft() ) {
        int bytesToSend = std::min( ( int )MaxFileChunkSize, int( sendFile.length - file->position() ) );
        if( bytesToSend == 0 ) {
            break;
        }
        
        if( chunk.data.size() != bytesToSend ) {
            chunk.data.resize( bytesToSend );
        }

        file->read( &chunk.data[0], bytesToSend );
        if( !SendPacket( sFileChunk::PacketId, &chunk, connection ) ) {
            return false;
        }
    }
    
    return true;
}

// ** NetworkHandler::SendUDP
bool NetworkHandler::SendUDP( UDPSocket* socket, int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port )
{
    m_outputStream->setPosition( 0 );
    int bytesWritten = m_packetParser->WritePacketToStream( packetId, packet, m_outputStream );
    DC_BREAK_IF( bytesWritten <= 0 );

    int bytesSent = socket->Send( address, port, m_outputStream->buffer(), bytesWritten );
    DC_BREAK_IF( bytesWritten != bytesSent );
    
    return bytesSent == bytesWritten;
}

// ** NetworkHandler::SendDatagram
bool NetworkHandler::SendDatagram( int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port )
{
    return SendUDP( m_netDatagram, packetId, packet, address, port );
}

// ** NetworkHandler::SendDatagram
bool NetworkHandler::SendBroadcast( int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port )
{
    return SendUDP( m_netBroadcast, packetId, packet, address, port );
}

// ** NetworkHandler::GetOrCreateInputStream
io::ByteBufferPtr NetworkHandler::GetOrCreateInputStream( Socket connection )
{
    InputStreams::iterator i = m_inputStreams.find( connection );
    if( i != m_inputStreams.end() ) {
        return i->second;
    }

    io::ByteBufferPtr stream = io::ByteBuffer::create( MaxPacketBufferSize );
    m_inputStreams[connection] = stream;

    return stream;
}

// ** NetworkHandler::RegisterPacket
bool NetworkHandler::RegisterPacket( int packetId, const INetworkPacket *packet )
{
    return m_packetParser->RegisterPacket( packetId, packet );
}

// ** NetworkHandler::RegisterRemoteProcedure
//bool NetworkHandler::RegisterRemoteProcedure( const char *name, const RemoteProcedureCallback& callback )
//{
//    return m_remoteProcedure->RegisterRemoteProcedure( name, callback );
//}

// ** NetworkHandler::InvokeRemoteProcedure
//bool NetworkHandler::InvokeRemoteProcedure( const char *name, const dreemchest::cValue *args, int count )
//{
//    return m_remoteProcedure->InvokeRemoteProcedure( name, args, count );
//}

// ** NetworkHandler::InvokeRemoteProcedure
//bool NetworkHandler::InvokeRemoteProcedure( const char *name, const cValue *args, int count, const RemoteProcedureResponseCallback& callback )
//{
//    return m_remoteProcedure->InvokeRemoteProcedure( name, args, count, callback );
//}

// ** NetworkHandler::IsConnected
bool NetworkHandler::IsConnected( void ) const
{
    return m_flags.Is( Connected );
}

// ** NetworkHandler::IsListeningDatagrams
bool NetworkHandler::IsListeningDatagrams( void ) const
{
    return m_flags.Is( ListeningDatagrams );
}

// ** NetworkHandler::IsListeningBroadcast
bool NetworkHandler::IsListeningBroadcast( void ) const
{
    return m_flags.Is( ListeningBroadcast );
}

// ** NetworkHandler::GetMaxPacketPayload
int NetworkHandler::GetMaxPacketPayload( void ) const
{
    return MaxPacketBufferSize - 12;
}

// ** NetworkHandler::OnDataReceived
void NetworkHandler::OnDataReceived( const dcEvent e )
{
    dcTCPSocketEvent se = ( dcTCPSocketEvent )e;

    // ** Get stream for connection
    io::ByteBufferPtr stream = GetOrCreateInputStream( se->m_connection );
    DC_BREAK_IF( stream == NULL );
    if( stream == NULL ) {
        return;
    }

    int bytesParsed = 0;

    while( bytesParsed < se->m_size ) {
        int maxLength   = stream->maxSize() - stream->bytesAvailable();
        int bytesToCopy = std::min( maxLength, se->m_size - bytesParsed );

        stream->setPosition( stream->bytesAvailable() );
        stream->write( se->m_data + bytesParsed, bytesToCopy );
        stream->setPosition( 0 );

        ParseReceivedPackets( stream, se->m_address, se->m_connection );

        bytesParsed += bytesToCopy;
    }
}

// ** NetworkHandler::ParseReceivedPackets
void NetworkHandler::ParseReceivedPackets( io::ByteBufferPtr& stream, const NetworkAddress& address, int connection )
{
    int              packetId    = 0;
    INetworkPacket  *packet      = NULL;
    
    // ** Parse packets
    while( m_packetParser->ParsePacketFromStream( &packet, packetId, stream ) ) {
        if( !packet ) {
            DC_BREAK
            Warning( "NetworkHandler::ParseReceivedPackets : unknown packet %d received\n", packetId );
            continue;
        }

        ProcessReceivedPacket( packetId, packet, address, connection );
    }

    // ** Trim processed bytes
    stream->trimFromLeft( stream->position() );
    stream->setPosition( 0 );
}

// ** NetworkHandler::OnDisconnected
void NetworkHandler::OnDisconnected( const dcEvent e )
{
    ProcessDisconnection();
}

// ** NetworkHandler::OnPacketUDP
void NetworkHandler::OnPacketUDP( const dcEvent e )
{
    dcUDPSocketEvent ue = ( dcUDPSocketEvent )e;

    INetworkPacket *packet   = NULL;
    int             packetId = 0;
    bool            parsed   = m_packetParser->ParsePacketFromMemory( &packet, packetId, ue->m_data, ue->m_size );

    if( !parsed ) {
        return;
    }

    if( !packet ) {
        Warning( "NetworkHandler::OnPacketUDP : unknown packet %d received\n", packetId );
        return;
    }

    ProcessReceivedPacket( packetId, packet, ue->m_address, -1 );
}
 
} // namespace net

DC_END_DREEMCHEST
*/