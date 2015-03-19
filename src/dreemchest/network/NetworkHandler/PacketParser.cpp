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

#include "PacketParser.h"
#include "../Sockets/TCPStream.h"

DC_BEGIN_DREEMCHEST

namespace net {

// ** PacketParser::writeToStream
u32 PacketParser::writeToStream( NetworkPacket* packet, io::ByteBufferPtr& stream )
{
	u64 initialPosition = stream->position();

	// ** Write header
	PacketHeader header( packet->typeId() );
	stream->write( &header.m_id, sizeof( header.m_id ) );
	stream->write( &header.m_size, sizeof( header.m_size ) );

    // ** Write packet
	io::Storage storage( io::StorageBinary, stream );
	packet->write( storage );

	// ** Calculate and write packet size
	header.m_size = stream->position() - initialPosition - PacketHeader::size();
	stream->setPosition( initialPosition + sizeof( header.m_id ) );
	stream->write( &header.m_size, sizeof( header.m_size ) );

	// ** Move back
	stream->setPosition( initialPosition + header.m_size + PacketHeader::size() );

	return stream->position() - initialPosition;
}

// ** PacketParser::parseFromStream
NetworkPacket* PacketParser::parseFromStream( io::ByteBufferPtr& stream ) const
{
	// ** Save current position.
	u64 position = stream->position();

	// ** Check amount of available bytes.
	if( stream->bytesAvailable() < PacketHeader::size() ) {
		return NULL;
	}

	// ** Read packet header
	PacketHeader header;
	stream->read( &header.m_id, sizeof( header.m_id ) );
	stream->read( &header.m_size, sizeof( header.m_size ) );

	// ** Ensure we have received full packet.
	if( stream->bytesAvailable() < header.m_size ) {
		// ** Rewind back
		stream->setPosition( position );
		return NULL;
	}

	// ** Create packet by type.
	NetworkPacket* packet = createPacket( header.m_id );

	if( packet ) {
		io::Storage storage( io::StorageBinary, stream );
		packet->read( storage );
	} else {
		stream->setPosition( position + header.m_size );
	}

	return packet;
}

// ** PacketParser::createPacket
NetworkPacket* PacketParser::createPacket( TypeId packetId ) const
{
	NetworkPacketTypes::const_iterator i = m_packetTypes.find( packetId );
	return i != m_packetTypes.end() ? i->second->clone() : NULL;
}

} // namespace net

DC_END_DREEMCHEST