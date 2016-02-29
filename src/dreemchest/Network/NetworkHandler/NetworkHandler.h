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

#ifndef __DC_Network_NetworkHandler_H__
#define __DC_Network_NetworkHandler_H__

#include "Connection.h"
#include "../Packets/PacketHandler.h"
#include "RemoteCallHandler.h"
#include "EventHandler.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! Basic network handler.
	class NetworkHandler : public InjectEventEmitter<RefCounted> {
	friend class Connection;
	public:

								//! Constructs NetworkHandler instance.
								NetworkHandler( void );

		//! Updates network handler.
		virtual void			update( u32 dt );

		//! Returns ping rate.
		u32						pingRate( void ) const;

		//! Sets the ping rate.
		void					setPingRate( u32 value );

		//! Returns the keep-alive time.
		s32						keepAliveTime( void ) const;

		//! Sets the keep-alive time.
		void					setKeepAliveTime( s32 value );

    #if DEV_DEPRECATED_PACKETS
		//! Registers a new packet type.
		template<typename T>
		void					registerPacketHandler( const typename PacketHandler<T>::Callback& callback );
    #else
        //! Registers a new event type.
		template<typename T>
		void					registerEvent( void );

		//! Registers a remote procedure.
		template<typename TRemoteProcedure>
		void					registerRemoteProcedureVoid( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, Void>::Callback& callback );

		//! Registers a remote procedure.
		template<typename TRemoteProcedure>
		void					registerRemoteProcedure( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>::Callback& callback );
    
        //! Registers a new packet handler.
        template<typename TPacketHandler>
        void                    addPacketHandler( TPacketHandler* instance );

    #ifndef DC_CPP11_DISABLED
        //! Creates a new instance of a packet handler and adds it to an application.
        template<typename TPacketHandler, typename ... TArgs>
        void                    addPacketHandler( const TArgs& ... args );
    #endif  /*  DC_CPP11_DISABLED   */

    #endif  /*  DEV_DEPRECATED_PACKETS  */

		//! Emits a network event.
		template<typename T>
		void					emitTo( const T& e, const ConnectionList& listeners );

    #ifndef DC_CPP11_DISABLED
		//! Template functions to emit a new event.
		template<typename TEvent, typename ... TArgs>
        void                    emit( const TArgs& ... args );
    #endif  /*  DC_CPP11_DISABLED   */

	protected:

		//! Returns a list of TCP sockets to send event to.
		virtual ConnectionList	eventListeners( void ) const;

		//! Creates a connection from socket.
		ConnectionPtr			createConnection( TCPSocketWPtr socket );

		//! Returns a connection by socket.
        ConnectionPtr			findConnectionBySocket( TCPSocketWPtr socket ) const;

		//! Removes connection by socket.
		void					removeConnection( TCPSocketWPtr socket );

    #if DEV_DEPRECATED_PACKETS
		//! Handles a ping packet.
		virtual bool			handlePingPacket( ConnectionPtr& connection, packets::Ping& packet );

		//! Handles a keep-alive message and updates the connection TTL value.
		virtual bool			handleKeepAlivePacket( ConnectionPtr& connection, packets::KeepAlive& packet );

		//! Handles a server detection packet.
		virtual bool			handleDetectServersPacket( ConnectionPtr& connection, packets::DetectServers& packet );

		//! Handles an event packet.
		bool					handleEventPacket( ConnectionPtr& connection, packets::Event& packet );

		//! Handles a remote call packet.
		bool					handleRemoteCallPacket( ConnectionPtr& connection, packets::RemoteCall& packet );

		//! Handles a response to remote call.
		bool					handleRemoteCallResponsePacket( ConnectionPtr& connection, packets::RemoteCallResponse& packet );
    #else
		//! Handles an event packet.
		void					handleEventPacket( ConnectionWPtr connection, const Packets::Event& packet );

        //! Handles a ping packet.
		void			        handlePingPacket( ConnectionWPtr connection, const Packets::Ping& packet );

		//! Handles a remote call packet.
		void					handleRemoteCallPacket( ConnectionWPtr connection, const Packets::RemoteCall& packet );

		//! Handles a response to remote call.
		void					handleRemoteCallResponsePacket( ConnectionWPtr connection, const Packets::RemoteCallResponse& packet );

        //! Handles a packet received over a connection.
        void                    handlePacketReceived( const Connection::Received& e );
    #endif

	protected:

    #if DEV_DEPRECATED_PACKETS
		//! A container type to store all registered packet handlers.
		typedef Map< TypeId, AutoPtr<IPacketHandler> > PacketHandlers;
    #endif
		//! A container type to store all network event emitters.
		typedef Map< TypeId, AutoPtr<IEventHandler> > EventHandlers;
    
		//! A container type to store all remote call handlers.
		typedef Hash< AutoPtr<IRemoteCallHandler> > RemoteCallHandlers;

		//! Container type to store socket to connection mapping.
		typedef Map<TCPSocketWPtr, ConnectionPtr>	    ConnectionBySocket;

        //! Container type to store a list of packet handlers.
        typedef List<AutoPtr<AbstractPacketHandler>>    PacketHandlerList;

        //! Container type to store mapping from a packet type to a handlers list.
        typedef Map<PacketTypeId, PacketHandlerList>    PacketHandlers;

        //! Network packet factory type.
        typedef AbstractFactory<AbstractPacket, PacketTypeId> PacketFactory;

    #if DEV_DEPRECATED_PACKETS
		//! Packet handlers.
		PacketHandlers			m_packetHandlers;
    #else
		EventHandlers			m_eventHandlers;        //!< Event handlers.
		RemoteCallHandlers		m_remoteCallHandlers;   //!< Remote call handlers.
        PacketFactory           m_packetFactory;        //!< Packet factory.
        PacketHandlers          m_packetHandlers;       //!< Registered packet handlers.
    #endif  /*  DEV_DEPRECATED_PACKETS  */

		//! Active connections.
		ConnectionBySocket		m_connections;

		//! Ping send rate.
		u32						m_pingSendRate;

		//! Ping time accumulator.
		s32						m_pingTimeLeft;

		//! Keep alive time.
		s32						m_keepAliveTime;

		//! Keep alive send rate.
		u32						m_keepAliveSendRate;
	};

#if DEV_DEPRECATED_PACKETS
	// ** NetworkHandler::registerPacketHandler
	template<typename T>
	inline void NetworkHandler::registerPacketHandler( const typename PacketHandler<T>::Callback& callback )
	{
		Io::SerializableTypes::registerType<T>();
		m_packetHandlers[TypeInfo<T>::id()] = DC_NEW PacketHandler<T>( callback );
	}
#endif  /*  DEV_DEPRECATED_PACKETS  */
	// ** NetworkHandler::registerEvent
	template<typename T>
	inline void NetworkHandler::registerEvent( void )
	{
		m_eventHandlers[TypeInfo<T>::id()] = DC_NEW EventHandler<T>( &m_eventEmitter );
	}

	// ** NetworkHandler::registerRemoteProcedureVoid
	template<typename TRemoteProcedure>
	inline void NetworkHandler::registerRemoteProcedureVoid( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, Void>::Callback& callback )
	{
		m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, Void>( callback );
	}

	// ** NetworkHandler::registerRemoteProcedure
	template<typename TRemoteProcedure>
	inline void NetworkHandler::registerRemoteProcedure( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>::Callback& callback )
	{
		m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>( callback );
	}

	// ** NetworkHandler::emitTo
	template<typename T>
	inline void NetworkHandler::emitTo( const T& e, const ConnectionList& listeners )
	{
		if( listeners.empty() ) {
			LogWarning( "rpc", "no listeners to listen for %s\n", TypeInfo<T>::name() );
			return;
		}

		// ** Serialize event to a byte buffer.
		Io::ByteBufferPtr buffer = Io::BinarySerializer::write( e );

		for( ConnectionList::const_iterator i = listeners.begin(), end = listeners.end(); i != end; ++i ) {
            const_cast<ConnectionPtr&>( *i )->send<packets::Event>( TypeInfo<T>::id(), buffer->array() );
		}
	}

    // ** NetworkHandler::addPacketHandler
    template<typename TPacketHandler>
    void NetworkHandler::addPacketHandler( TPacketHandler* instance )
    {
        // Register the packet type inside a factory
        m_packetFactory.declare<typename TPacketHandler::Packet>( TypeInfo<typename TPacketHandler::Packet>::id() );

        // Add this packet handler instance to a list
        PacketTypeId type = instance->packetTypeId();
        m_packetHandlers[type].push_back( instance );
    }

#ifndef DC_CPP11_DISABLED
    // ** NetworkHandler::addPacketHandler
    template<typename TPacketHandler, typename ... TArgs>
    void NetworkHandler::addPacketHandler( const TArgs& ... args )
    {
        addPacketHandler( DC_NEW TPacketHandler( args... ) );
    }
#endif  /*  DC_CPP11_DISABLED   */

#ifndef DC_CPP11_DISABLED
	// ** NetworkHandler::emit
	template<typename TEvent, typename ... TArgs>
	void NetworkHandler::emit( const TArgs& ... args )
	{
        TEvent e( args... );
		emitTo( e, eventListeners() );
	}
#endif  /*  DC_CPP11_DISABLED   */

} // namespace Network
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkHandler_H__	*/