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

#include	"PacketFormatter.h"
#include    "NetworkHandler.h"

DC_BEGIN_DREEMCHEST

namespace net {
/*
// ** PacketFormatter::PacketFormatter
PacketFormatter::PacketFormatter( int magic ) : m_magic( magic )
{

}

PacketFormatter::~PacketFormatter( void )
{
    for( PacketTypes::iterator i = m_packets.begin(), end = m_packets.end(); i != end; ++i ) {
        delete i->second;
    }
}

// ** PacketFormatter::WritePacketToStream
int PacketFormatter::WritePacketToStream( int packetId, const INetworkPacket *packet, dcMemoryStream stream ) const
{
    int offset = 0, packetSize = 0;

    int bytesWritten = stream->position();

    // ** Write header
    stream->write( &m_magic, sizeof( m_magic ) );
    stream->write( &packetId, sizeof( packetId ) );
    stream->write( &packetSize, sizeof( packetSize ) );

    // ** Write packet
    offset = stream->position();
    packet->write( stream );
    packetSize = stream->position() - offset;

    // ** Save total size
    bytesWritten = stream->position() - bytesWritten;

    // ** Write packet size
    stream->setPosition( offset - 4 );
    stream->write( &packetSize, sizeof( packetSize ) );

    return bytesWritten;
}

// ** PacketFormatter::ParsePacketFromStream
bool PacketFormatter::ParsePacketFromStream( INetworkPacket **packet, int& packetId, dcMemoryStream stream ) const
{
    int index = FindPacket( stream );

    if( index == -1 ) {
        return false;
    }

    stream->setPosition( index );
    bool parsed = ParsePacket( packet, packetId, stream );

    return parsed;
}

// ** PacketFormatter::ParsePacketFromMemory
bool PacketFormatter::ParsePacketFromMemory( INetworkPacket **packet, int& packetId, const u8 *buffer, int size ) const
{
    AutoPtr<io::MemoryStream> stream = DC_NEW io::MemoryStream( const_cast<u8*>( buffer ), size, false );
    stream->write( buffer, size );
    stream->setPosition( 0 );
    
    bool parsed = ParsePacket( packet, packetId, stream.Get() );

    return parsed;
}

// ** PacketFormatter::FindPacket
int PacketFormatter::FindPacket( dcMemoryStream stream ) const
{
    const u8 *data = stream->buffer();

    for( int i = stream->position(); i < stream->length(); i++ ) {
        if( data[i] == m_magic ) {
            return i;
        }
    }

    return -1;
}

// ** PacketFormatter::ParsePacket
bool PacketFormatter::ParsePacket( INetworkPacket **packet, int& packetId, dcMemoryStream stream ) const
{
    const int   headerSize  = 12;
    int         index       = stream->position();
    int         packetSize  = 0;
    int         magic;

    *packet  = NULL;
    packetId = 0;

    if( stream->bytesAvailable() < headerSize ) {
        return false;
    }

    stream->read( &magic, sizeof( int ) );
    stream->read( &packetId, sizeof( int ) );
    stream->read( &packetSize, sizeof( int ) );

    DC_BREAK_IF( magic != m_magic )

    if( stream->bytesAvailable() < packetSize ) {
        stream->setPosition( index );
        return false;
    }

    // ** Create and read packet from stream
    *packet = CreatePacket( packetId );
    
    if( *packet ) {
        (*packet)->read( stream );
    }
    
    return true;
}

// ** PacketFormatter::CreatePacket
INetworkPacket* PacketFormatter::CreatePacket( int packetId ) const
{
    PacketTypes::const_iterator i = m_packets.find( packetId );
    if( i != m_packets.end() ) {
        return i->second->clone();
    }

    return NULL;
}

// ** PacketFormatter::RegisterPacket
bool PacketFormatter::RegisterPacket( int packetId, const INetworkPacket *packet )
{
    if( m_packets.find( packetId ) != m_packets.end() ) {
        return false;
    }
    
    m_packets[packetId] = packet;
    return true;
}*/

} // namespace net

DC_END_DREEMCHEST
