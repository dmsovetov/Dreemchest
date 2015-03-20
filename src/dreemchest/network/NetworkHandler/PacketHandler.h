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

#ifndef __DC_Network_PacketHandler_H__
#define __DC_Network_PacketHandler_H__

#include "../Network.h"
#include "../../event/EventEmitter.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Packet handler interface class.
	class PacketHandler {
	public:

		virtual			~PacketHandler( void ) {}

		//! Packet handler callback.
		virtual bool	handle( TCPSocket* sender, NetworkPacket* packet ) = 0;
	};

	//! Template class that handles a strict-typed packets.
	template<typename T>
	class GenericPacketHandler : public PacketHandler {
	public:

		//! Function type to handle packets.
		typedef cClosure<bool(TCPSocket*,const T*)> Callback;

						//! Constructs GenericPacketHandler instance.
						GenericPacketHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Casts an input network packet to a specified type and runs a callback.
		virtual bool	handle( TCPSocket* sender, NetworkPacket* packet )
		{
			T* packetWithType = castTo<T>( packet );
			DC_BREAK_IF( packetWithType == NULL );
			return m_callback( sender, packetWithType );
		}

	private:

		//! Packet handler callback.
		Callback		m_callback;
	};

	//! Template class that handles an Event packet and emits the local event.
	template<typename T>
	class EventPacketHandler : public PacketHandler {
	public:

								//! Constructs EventPacketHandler instance.
								EventPacketHandler( event::EventEmitter* eventEmitter )
									: m_eventEmitter( eventEmitter ) {}

		//! Casts an input network packet to an Event packet and emits it's payload as local event.
		virtual bool handle( TCPSocket* sender, NetworkPacket* packet )
		{
			packets::Event* eventPacket = castTo<packets::Event>( packet );
			DC_BREAK_IF( eventPacket == NULL );

			io::ByteBufferPtr buffer = io::ByteBuffer::createFromData( &eventPacket->payload[0], eventPacket->payload.size() );
			io::Storage       storage( io::StorageBinary, buffer );

			T e;
			e.read( storage );

			m_eventEmitter->emit( e );
			return true;
		}

	private:

		//! Parent event emitter.
		event::EventEmitter*	m_eventEmitter;
	};

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PacketHandler_H__	*/