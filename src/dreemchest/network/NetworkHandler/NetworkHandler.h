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

#ifndef		__DC_Network_NetworkHandler_H__
#define		__DC_Network_NetworkHandler_H__

#include "Packets.h"
#include "PacketHandler.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Basic network handler.
	class NetworkHandler : public RefCounted {
	public:

							//! Constructs NetworkHandler instance.
							NetworkHandler( void );

		//! Updates network handler.
		virtual void		update( void );

		//! Registeres a new packet type.
		template<typename T>
		void				registerPacketHandler( typename GenericPacketHandler<T>::Callback& callback );

	protected:

		//! Sends a packet.
		void				sendPacket( TCPSocket* socket, NetworkPacket* packet );

		//! Sends a packet of a specified type.
		template<typename T>
		void sendPacket( TCPSocket* socket );

		template<typename T, typename Arg0>
		void sendPacket( TCPSocket* socket, const Arg0& arg0 );

		template<typename T, typename Arg0, typename Arg1>
		void sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1 );

		//! Processes a received data from client.
		virtual void		processReceivedData( TCPSocket* socket, TCPStream* stream );

		//! Performs a latency test with a specified amount of iterations.
		void				doLatencyTest( TCPSocket* socket, u8 iterations );

		//! Handles a ping packet and sends back a pong response.
		bool				handlePingPacket( TCPSocket* sender, const packets::Ping* packet );

		//! Handles a pong packet.
		bool				handlePongPacket( TCPSocket* sender, const packets::Pong* packet );

	private:

		//! A container type to store all registered packet handlers.
		typedef Map< TypeId, AutoPtr<PacketHandler> > PacketHandlers;

		//! Packet parser.
		PacketParser		m_packetParser;

		//! Packet handlers.
		PacketHandlers		m_packetHandlers;
	};

	// ** NetworkHandler::registerPacketHandler
	template<typename T>
	inline void NetworkHandler::registerPacketHandler( typename GenericPacketHandler<T>::Callback& callback )
	{
		m_packetParser.registerPacketType<T>();
		m_packetHandlers[T::classTypeId()] = DC_NEW GenericPacketHandler<T>( callback );
	}

	// ** NetworkHandler::sendPacket
	template<typename T>
	inline void NetworkHandler::sendPacket( TCPSocket* socket )
	{
		T packet;
		sendPacket( socket, &packet );
	}

	// ** NetworkHandler::sendPacket
	template<typename T, typename Arg0>
	inline void NetworkHandler::sendPacket( TCPSocket* socket, const Arg0& arg0 )
	{
		T packet( arg0 );
		sendPacket( socket, &packet );
	}

	// ** NetworkHandler::sendPacket
	template<typename T, typename Arg0, typename Arg1>
	inline void NetworkHandler::sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1 )
	{
		T packet( arg0, arg1 );
		sendPacket( socket, &packet );
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkHandler_H__	*/