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

#ifndef	__DC_Network_Packets_H__
#define	__DC_Network_Packets_H__

#include "PacketParser.h"

DC_BEGIN_DREEMCHEST


#define BeginNetworkPacket( name )			\
	struct name : public NetworkPacket {	\
		ClassEnableTypeId( name );			\
		ClassEnableCloning( name );

#define EndNetworkPacket	};


namespace net {

namespace packets {

	//! Packet payload type.
	typedef Array<u8> Payload;

	//! Network event packet
	BeginNetworkPacket( Event )
		TypeId		eventId;
		Payload		payload;

		//! Constructs Event instance.
		Event( TypeId eventId = 0, const Payload& payload = Payload() ) : eventId( eventId ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( eventId )
			IoArray( payload )
		IoEndSerializer
	EndNetworkPacket

	//! RPC call packet.
	BeginNetworkPacket( RemoteCall )
		u16			id;
		u32			method;
		TypeId		returnType;
		Payload		payload;

		//! Constructs RemoteCall instance.
		RemoteCall( u16 id = 0, u32 method = 0, TypeId returnType = 0, const Payload& payload = Payload() )
			: id( id ), method( method ), returnType( 0 ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( id )
			IoField( method )
			IoField( returnType )
			IoArray( payload )
		IoEndSerializer
	EndNetworkPacket

	//! RPC call response
	BeginNetworkPacket( RemoteCallResponse )
		u16			id;
		TypeId		returnType;
		Payload		payload;

		//! Constructs RemoteCallResponse instance.
		RemoteCallResponse( u16 id = 0, TypeId returnType = 0, const Payload& payload = Payload() )
			: id( id ), returnType( returnType ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( id )
			IoField( returnType )
			IoArray( payload )
		IoEndSerializer
	EndNetworkPacket

} // namespace packets

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Packets_H__	*/