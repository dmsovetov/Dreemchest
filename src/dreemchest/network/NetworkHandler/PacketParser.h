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

#ifndef __DC_Network_PacketParser_H__
#define __DC_Network_PacketParser_H__

#include "../Network.h"
#include "../../io/serialization/Storage.h"
#include "../../io/serialization/Serializable.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Basic network packet.
	struct NetworkPacket : public io::Serializable {
		ClassEnableTypeId( NetworkPacket )
		ClassEnableCloning( NetworkPacket )

		u32 timestamp;

		IoBeginSerializer
			IoField( timestamp )
		IoEndSerializer
	};

	//! Network packet parser.
	class PacketParser {
	public:

		//! Parses a packet from stream.
		NetworkPacket*		parseFromStream( io::ByteBufferPtr& stream ) const;

		//! Writes a packet to stream.
		u32					writeToStream( NetworkPacket* packet, io::ByteBufferPtr& stream );

		//! Returns a new packet type.
		template<typename T>
		void				registerPacketType( void );

	private:

		//! Creates a packed by type id.
		NetworkPacket*		createPacket( TypeId packetId ) const;

		//! Returns true if the specified packet type is registered.
		bool				hasPacketType( TypeId packetId ) const;

	private:

		//! Packet header struct.
		struct PacketHeader {
			TypeId	m_id;	//!< Packet identifier.
			u16		m_size;	//!< Packet size.

					//! Constructs a PacketHeader instance.
					PacketHeader( TypeId id = 0 )
						: m_id( id ), m_size( 0 ) {}

			//! Returns a packet header size.
			static s32 size( void ) { return sizeof( TypeId ) + sizeof( u16 ); }
		};

		//! Container type to store registered network packet types.
		typedef Map< TypeId, AutoPtr<NetworkPacket> > NetworkPacketTypes;

		//! Available packet types.
		NetworkPacketTypes	m_packetTypes;
	};

	// ** PacketParser::registerPacketType
	template<typename T>
	void PacketParser::registerPacketType( void ) {
		T* packet = DC_NEW T;
		m_packetTypes[packet->typeId()] = packet;
	}

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PacketParser_H__	*/