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

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

namespace packets {

	//! Packet payload type.
	typedef Array<u8> Payload;

	//! Latency test & time sync packet
	struct Time : public io::SerializableType<Time> {
		u32				timestamp;		//!< A timestamp value when the packet was sent.
		s32				roundTripTime;	//!< A time took the packet to travel to target and back.

						//! Constructs a Time instance.
						Time( u32 timestamp = 0, s32 roundTripTime = 0 )
							: timestamp( timestamp ), roundTripTime( roundTripTime ) {}

		//! Packet serializer
		IoBeginSerializer
			IoField( timestamp )
			IoField( roundTripTime )
		IoEndSerializer
	};

	//! Server detection packet.
	struct DetectServers : public io::SerializableType<DetectServers> {
		String			query;	//!< A server query.

						//! Constructs DetectServers instance.
						DetectServers( const String& query = "" )
							: query( query ) {}

		//! Packet serializer
		IoBeginSerializer
			IoField( query )
		IoEndSerializer
	};

	//! Network event packet
	struct Event : public io::SerializableType<Event> {
		TypeId			eventId;
		Payload			payload;

						//! Constructs Event instance.
						Event( server::TypeId eventId = 0, const Payload& payload = Payload() )
							: eventId( eventId ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializer
			IoField( eventId )
			IoArray( payload )
		IoEndSerializer
	};

	//! RPC call packet.
	struct RemoteCall : public io::SerializableType<RemoteCall> {
		u16			id;
		u32			method;
		TypeId		returnType;
		Payload		payload;

					//! Constructs RemoteCall instance.
					RemoteCall( u16 id = 0, u32 method = 0, TypeId returnType = 0, const Payload& payload = Payload() )
						: id( id ), method( method ), returnType( 0 ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializer
			IoField( id )
			IoField( method )
			IoField( returnType )
			IoArray( payload )
		IoEndSerializer
	};

	//! RPC call response
	struct RemoteCallResponse : public io::SerializableType<RemoteCallResponse> {
		u16			id;
		TypeId		returnType;
		Payload		payload;

					//! Constructs RemoteCallResponse instance.
					RemoteCallResponse( u16 id = 0, TypeId returnType = 0, const Payload& payload = Payload() )
						: id( id ), returnType( returnType ), payload( payload ) {}

		//! Packet serializer
		IoBeginSerializer
			IoField( id )
			IoField( returnType )
			IoArray( payload )
		IoEndSerializer
	};

} // namespace packets

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Packets_H__	*/