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
#include "EventHandler.h"
#include "RemoteCallHandler.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Basic network handler.
	class NetworkHandler : public RefCounted {
	public:

								//! Constructs NetworkHandler instance.
								NetworkHandler( void );

		//! Updates network handler.
		virtual void			update( void );

		//! Registers a new packet type.
		template<typename T>
		void					registerPacketHandler( const typename PacketHandler<T>::Callback& callback );

		//! Registers a new event type.
		template<typename T>
		void					registerEvent( void );

		//! Registers a remote procedure.
		template<typename T>
		void					registerRemoteProcedure( CString name, const typename RemoteCallHandler<T>::Callback& callback );

		//! Invokes a remote procedure.
		template<typename T>
		void					invoke( TCPSocket* socket, CString method, const T& argument );

		//! Invokes a remote procedure.
		template<typename T, typename R>
		void					invoke( TCPSocket* socket, CString method, const T& argument, const typename RemoteResponseHandler<R>::Callback& callback );

		//! Responds to a remote procedure call.
		template<typename T>
		void					respond( TCPSocket* socket, u16 id, const T& value );

		//! Emits a network event.
		template<typename T>
		void					emit( const T& e );

		//! Subscribes for a network event of a specified type.
		template<typename T>
		void					subscribe( const cClosure<void(const T&)>& callback );

	protected:

		//! Sends a packet.
		void					sendPacket( TCPSocket* socket, NetworkPacket* packet );

		//! Sends a packet of a specified type.
		template<typename T>
		void sendPacket( TCPSocket* socket );

		template<typename T, typename Arg0>
		void sendPacket( TCPSocket* socket, const Arg0& arg0 );

		template<typename T, typename Arg0, typename Arg1>
		void sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1 );

		template<typename T, typename Arg0, typename Arg1, typename Arg2>
		void sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1, const Arg2& arg2 );

		template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
		void sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3 );

		//! Returns a list of TCP sockets to send event to.
		virtual TCPSocketList	eventListeners( void ) const;

		//! Processes a received data from client.
		virtual void			processReceivedData( TCPSocket* socket, TCPStream* stream );

		//! Performs a latency test with a specified amount of iterations.
		void					doLatencyTest( TCPSocket* socket, u8 iterations );

		//! Handles a ping packet and sends back a pong response.
		bool					handlePingPacket( TCPSocket* sender, const packets::Ping* packet );

		//! Handles a pong packet.
		bool					handlePongPacket( TCPSocket* sender, const packets::Pong* packet );

		//! Handles an event packet.
		bool					handleEventPacket( TCPSocket* sender, const packets::Event* packet );

		//! Handles a remote call packet.
		bool					handleRemoteCallPacket( TCPSocket* sender, const packets::RemoteCall* packet );

		//! Handles a response to remote call.
		bool					handleRemoteCallResponsePacket( TCPSocket* sender, const packets::RemoteCallResponse* packet );

	protected:

		//! A helper struct to store a timestamp of an RPC call.
		struct PendingRemoteCall {
			String							m_name;			//!< Remote procedure name.
			UnixTime						m_timestamp;	//!< A Unix time when a call was performed.
			AutoPtr<IRemoteResponseHandler>	m_handler;		//!< Response handler.

											//! Constructs a PendingRemoteCall instance.
											PendingRemoteCall( const String& name = "", IRemoteResponseHandler* handler = NULL )
												: m_name( name ), m_handler( handler ) {}
		};

		//! A container type to store all registered packet handlers.
		typedef Map< TypeId, AutoPtr<IPacketHandler> > PacketHandlers;

		//! A container type to store all network event emitters.
		typedef Map< TypeId, AutoPtr<IEventHandler> > EventHandlers;

		//! A container type to store all remote call handlers.
		typedef Hash< AutoPtr<IRemoteCallHandler> > RemoteCallHandlers;

		//! A container type to store all pending remote calls.
		typedef Map< u16, PendingRemoteCall > PendingRemoteCalls;

		//! Packet parser.
		PacketParser			m_packetParser;

		//! Packet handlers.
		PacketHandlers			m_packetHandlers;

		//! Event handlers.
		EventHandlers			m_eventHandlers;

		//! Remote call handlers.
		RemoteCallHandlers		m_remoteCallHandlers;

		//! A list of pending remote calls.
		PendingRemoteCalls		m_pendingRemoteCalls;

		//! Next remote call response id.
		u16						m_nextRemoteCallId;

		//! Network event emitter.
		event::EventEmitter		m_eventEmitter;
	};

	// ** NetworkHandler::registerPacketHandler
	template<typename T>
	inline void NetworkHandler::registerPacketHandler( const typename PacketHandler<T>::Callback& callback )
	{
		m_packetParser.registerPacketType<T>();
		m_packetHandlers[T::classTypeId()] = DC_NEW PacketHandler<T>( callback );
	}

	// ** NetworkHandler::registerEvent
	template<typename T>
	inline void NetworkHandler::registerEvent( void )
	{
		m_eventHandlers[T::classTypeId()] = DC_NEW EventHandler<T>( &m_eventEmitter );
	}

	// ** NetworkHandler::registerRemoteProcedure
	template<typename T>
	inline void NetworkHandler::registerRemoteProcedure( CString name, const typename RemoteCallHandler<T>::Callback& callback )
	{
		m_remoteCallHandlers[StringHash( name )] = DC_NEW RemoteCallHandler<T>( callback );
	}

	// ** NetworkHandler::subscribe
	template<typename T>
	inline void NetworkHandler::subscribe( const cClosure<void(const T&)>& callback )
	{
		m_eventEmitter.subscribe<T>( callback );
	}

	// ** NetworkHandler::emit
	template<typename T>
	inline void NetworkHandler::emit( const T& e )
	{
		// ** Get the list of potential listeners
		TCPSocketList listeners = eventListeners();

		if( listeners.empty() ) {
			log::warn( "NetworkHandler::emit : no listeners to listen for %s\n", T::classTypeName() );
			return;
		}

		// ** Serialize event to a byte buffer.
		io::ByteBufferPtr buffer = io::ByteBuffer::create();
		io::Storage		  storage( io::StorageBinary, buffer );
		e.write( storage );

		for( TCPSocketList::iterator i = listeners.begin(), end = listeners.end(); i != end; ++i ) {
			sendPacket<packets::Event>( i->get(), T::classTypeId(), buffer->array() );
		}
	}

	// ** NetworkHandler::respond
	template<typename T>
	inline void NetworkHandler::respond( TCPSocket* socket, u16 id, const T& value )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::ByteBuffer::create();
		io::Storage		  storage( io::StorageBinary, buffer );
		value.write( storage );

		// ** Send an RPC response packet.
		sendPacket<packets::RemoteCallResponse>( socket, id, T::classTypeId(), buffer->array() );
	}

	// ** NetworkHandler::invoke
	template<typename T>
	inline void NetworkHandler::invoke( TCPSocket* socket, CString method, const T& argument )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::ByteBuffer::create();
		io::Storage		  storage( io::StorageBinary, buffer );
		argument.write( storage );

		// ** Send an RPC request
		sendPacket<packets::RemoteCall>( socket, 0, StringHash( method ), 0, buffer->array() );
	}

	// ** NetworkHandler::invoke
	template<typename T, typename R>
	inline void NetworkHandler::invoke( TCPSocket* socket, CString method, const T& argument, const typename RemoteResponseHandler<R>::Callback& callback )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::ByteBuffer::create();
		io::Storage		  storage( io::StorageBinary, buffer );
		argument.write( storage );

		// ** Send an RPC request
		u16 remoteCallId = m_nextRemoteCallId++;
		sendPacket<packets::RemoteCall>( socket, remoteCallId, StringHash( method ), R::classTypeId(), buffer->array() );
		
		// ** Create a response handler.
		m_pendingRemoteCalls[remoteCallId] = PendingRemoteCall( method, DC_NEW RemoteResponseHandler<R>( callback ) );
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

	// ** NetworkHandler::sendPacket
	template<typename T, typename Arg0, typename Arg1, typename Arg2>
	inline void NetworkHandler::sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1, const Arg2& arg2 )
	{
		T packet( arg0, arg1, arg2 );
		sendPacket( socket, &packet );
	}

	// ** NetworkHandler::sendPacket
	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	inline void NetworkHandler::sendPacket( TCPSocket* socket, const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3 )
	{
		T packet( arg0, arg1, arg2, arg3 );
		sendPacket( socket, &packet );
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkHandler_H__	*/