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

	//! Ping packet
	BeginNetworkPacket( Ping )
		u8 iteration;

		//! Constructs a Ping instance.
		Ping( u8 iteration = 0 ) : iteration( iteration ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( iteration )
		IoEndSerializer
	EndNetworkPacket

	//! Pong packet
	BeginNetworkPacket( Pong )
		u8 iteration;

		//! Constructs a Pong instance.
		Pong( u8 iteration = 0 ) : iteration( iteration ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( iteration )
		IoEndSerializer
	EndNetworkPacket

	//! Network event packet
	BeginNetworkPacket( Event )
		TypeId		eventId;
		Array<u8>	payload;

		//! Constructs Event instance.
		Event( TypeId eventId = 0, const Array<u8>& payload = Array<u8>() ) : eventId( eventId ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializerSuper( NetworkPacket )
			IoField( eventId )
			IoArray( payload )
		IoEndSerializer
	EndNetworkPacket

} // namespace packets

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Packets_H__	*/