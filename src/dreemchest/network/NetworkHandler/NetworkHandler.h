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
	friend class Connection;
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
		void					registerRemoteProcedure( CString name, const typename RemoteCallHandler<T, Void>::Callback& callback );

		//! Registers a remote procedure.
		template<typename T, typename R>
		void					registerRemoteProcedure( CString name, const typename RemoteCallHandler<T, R>::Callback& callback );

		//! Emits a network event.
		template<typename T>
		void					emitTo( const T& e, ConnectionList& listeners );

		//! Template functions to emit a new event.
		template<typename T>						 void emit( void );
		template<typename T, TemplateFunctionTypes1> void emit( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2> void emit( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3> void emit( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4> void emit( TemplateFunctionArgs4 );

		//! Subscribes for a network event of a specified type.
		template<typename T>
		void					subscribe( const cClosure<void(const T&)>& callback );

	protected:

		//! Returns a list of TCP sockets to send event to.
		virtual ConnectionList	eventListeners( void ) const;

		//! Processes a received data from client.
		virtual void			processReceivedData( TCPSocket* socket, TCPStream* stream );

		//! Creates a connection from socket.
		ConnectionPtr			createConnection( TCPSocket* socket );

		//! Returns a connection by socket.
        ConnectionPtr			findConnectionBySocket( TCPSocket* socket ) const;

		//! Removes connection by socket.
		void					removeConnection( TCPSocket* socket );

		//! Handles an event packet.
		bool					handleEventPacket( ConnectionPtr& connection, const packets::Event* packet );

		//! Handles a remote call packet.
		bool					handleRemoteCallPacket( ConnectionPtr& connection, const packets::RemoteCall* packet );

		//! Handles a response to remote call.
		bool					handleRemoteCallResponsePacket( ConnectionPtr& connection, const packets::RemoteCallResponse* packet );

	protected:

		//! A container type to store all registered packet handlers.
		typedef Map< TypeId, AutoPtr<IPacketHandler> > PacketHandlers;

		//! A container type to store all network event emitters.
		typedef Map< TypeId, AutoPtr<IEventHandler> > EventHandlers;

		//! A container type to store all remote call handlers.
		typedef Hash< AutoPtr<IRemoteCallHandler> > RemoteCallHandlers;

		//! Container type to store socket to connection mapping
		typedef Map<TCPSocket*, ConnectionPtr>	ConnectionBySocket;

		//! Packet handlers.
		PacketHandlers			m_packetHandlers;

		//! Event handlers.
		EventHandlers			m_eventHandlers;

		//! Remote call handlers.
		RemoteCallHandlers		m_remoteCallHandlers;

		//! Active connections.
		ConnectionBySocket		m_connections;

		//! Network event emitter.
		event::EventEmitter		m_eventEmitter;
	};

	// ** NetworkHandler::registerPacketHandler
	template<typename T>
	inline void NetworkHandler::registerPacketHandler( const typename PacketHandler<T>::Callback& callback )
	{
		io::BinarySerializer::registerType<T>();
		m_packetHandlers[TypeInfo<T>::id()] = DC_NEW PacketHandler<T>( callback );
	}

	// ** NetworkHandler::registerEvent
	template<typename T>
	inline void NetworkHandler::registerEvent( void )
	{
		m_eventHandlers[TypeInfo<T>::id()] = DC_NEW EventHandler<T>( &m_eventEmitter );
	}

	// ** NetworkHandler::registerRemoteProcedure
	template<typename T>
	inline void NetworkHandler::registerRemoteProcedure( CString name, const typename RemoteCallHandler<T, Void>::Callback& callback )
	{
		m_remoteCallHandlers[StringHash( name )] = DC_NEW RemoteCallHandler<T, Void>( callback );
	}

	// ** NetworkHandler::registerRemoteProcedure
	template<typename T, typename R>
	inline void NetworkHandler::registerRemoteProcedure( CString name, const typename RemoteCallHandler<T, R>::Callback& callback )
	{
		m_remoteCallHandlers[StringHash( name )] = DC_NEW RemoteCallHandler<T, R>( callback );
	}

	// ** NetworkHandler::subscribe
	template<typename T>
	inline void NetworkHandler::subscribe( const cClosure<void(const T&)>& callback )
	{
		m_eventEmitter.subscribe<T>( callback );
	}

	// ** NetworkHandler::emitTo
	template<typename T>
	inline void NetworkHandler::emitTo( const T& e, ConnectionList& listeners )
	{
		if( listeners.empty() ) {
			log::warn( "NetworkHandler::emit : no listeners to listen for %s\n", TypeInfo<T>::name() );
			return;
		}

		// ** Serialize event to a byte buffer.
		io::ByteBufferPtr buffer = e.writeToByteBuffer();

		for( ConnectionList::iterator i = listeners.begin(), end = listeners.end(); i != end; ++i ) {
            (*i)->send<packets::Event>( TypeInfo<T>::id(), buffer->array() );
		}
	}

	// ** NetworkHandler::emit
	template<typename T>
	inline void NetworkHandler::emit( void )
	{
		emitTo( T(), eventListeners() );
	}

	// ** NetworkHandler::emit
	template<typename T, TemplateFunctionTypes1>
	inline void NetworkHandler::emit( TemplateFunctionArgs1 )
	{
		emitTo( T( arg0 ), eventListeners() );
	}

	// ** NetworkHandler::send
	template<typename T, TemplateFunctionTypes2>
	inline void NetworkHandler::emit( TemplateFunctionArgs2 )
	{
		emitTo( T( arg0, arg1 ), eventListeners() );
	}

	// ** NetworkHandler::send
	template<typename T, TemplateFunctionTypes3>
	inline void NetworkHandler::emit( TemplateFunctionArgs3 )
	{
		emitTo( T( arg0, arg1, arg2 ), eventListeners() );
	}

	// ** NetworkHandler::send
	template<typename T, TemplateFunctionTypes4>
	inline void NetworkHandler::emit( TemplateFunctionArgs4 )
	{
		emitTo( T( arg0, arg1, arg2, arg3 ), eventListeners() );
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkHandler_H__	*/