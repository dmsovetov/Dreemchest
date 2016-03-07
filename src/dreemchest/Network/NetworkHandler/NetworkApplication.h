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

#ifndef __DC_Network_Application_H__
#define __DC_Network_Application_H__

#include "Connection.h"
#include "../Packets/PacketHandler.h"
#include "RemoteCallHandler.h"
#include "EventHandler.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! Basic network application class.
	class Application : public InjectEventEmitter<RefCounted> {
	friend class Connection;
	public:

								//! Constructs Application instance.
								Application( void );
        virtual                 ~Application( void ) {}

		//! Updates network application instance.
		virtual void			update( u32 dt );

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

		//! Emits a network event.
		template<typename T>
		void					emitTo( const T& e, const ConnectionList& listeners );

    #ifndef DC_CPP11_DISABLED
		//! Template functions to emit a new event.
		template<typename TEvent, typename ... TArgs>
        void                    emit( const TArgs& ... args );
    #endif  /*  DC_CPP11_DISABLED   */

		//! Connected event is emitted when a connection to server established or new client is connected to a server.
		struct Connected {
									//! Constructs Connected instance.
									Connected( ApplicationWPtr sender, ConnectionWPtr connection )
										: sender( sender ), connection( connection ) {}

            ApplicationWPtr         sender;     //!< Network application instance that emitted this event.
			ConnectionWPtr			connection;	//!< Opened connection instance.
		};

		//! Disconnected event is emitted when a connection to server was closed or client is disconnected from a server.
		struct Disconnected {
									//! Constructs Disconnected instance.
									Disconnected( ApplicationWPtr sender, ConnectionWPtr connection )
										: sender( sender ), connection( connection ) {}

            ApplicationWPtr         sender;     //!< Network application instance that emitted this event.
			ConnectionWPtr			connection;	//!< Closed connection instance.
		};

	protected:

		//! Returns a list of TCP sockets to send event to.
		virtual ConnectionList	eventListeners( void ) const;

		//! Creates a connection from socket.
		ConnectionPtr			createConnection( TCPSocketWPtr socket );

		//! Removes the connection instance from application.
		void					removeConnection( ConnectionWPtr connection );

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

		//! Handles the connection closed event.
		void					handleConnectionClosed( const Connection::Closed& e );

	protected:

		//! A container type to store all network event emitters.
		typedef Map< TypeId, AutoPtr<IEventHandler> > EventHandlers;
    
		//! A container type to store all remote call handlers.
		typedef Hash< AutoPtr<IRemoteCallHandler> > RemoteCallHandlers;

		//! Container type to store active connections.
		typedef Set<ConnectionPtr>						ConnectionSet;

        //! Container type to store a list of packet handlers.
        typedef List<AutoPtr<AbstractPacketHandler>>    PacketHandlerList;

        //! Container type to store mapping from a packet type to a handlers list.
        typedef Map<PacketTypeId, PacketHandlerList>    PacketHandlers;

        //! Network packet factory type.
        typedef AbstractFactory<AbstractPacket, PacketTypeId> PacketFactory;

		EventHandlers			m_eventHandlers;        //!< Event handlers.
		RemoteCallHandlers		m_remoteCallHandlers;   //!< Remote call handlers.
        PacketFactory           m_packetFactory;        //!< Packet factory.
        PacketHandlers          m_packetHandlers;       //!< Registered packet handlers.
		ConnectionSet			m_connections;			//!< Active connections.
        u32                     m_nextConnectionId;     //!< The next id that will be assigned to a connection.
	};

	// ** Application::registerEvent
	template<typename T>
	inline void Application::registerEvent( void )
	{
		m_eventHandlers[TypeInfo<T>::id()] = DC_NEW EventHandler<T>( &m_eventEmitter );
	}

	// ** Application::registerRemoteProcedureVoid
	template<typename TRemoteProcedure>
	inline void Application::registerRemoteProcedureVoid( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, Void>::Callback& callback )
	{
		m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, Void>( callback );
	}

	// ** NetworkHandler::registerRemoteProcedure
	template<typename TRemoteProcedure>
	inline void Application::registerRemoteProcedure( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>::Callback& callback )
	{
		m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>( callback );
	}

	// ** Application::emitTo
	template<typename T>
	inline void Application::emitTo( const T& e, const ConnectionList& listeners )
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

    // ** Application::addPacketHandler
    template<typename TPacketHandler>
    void Application::addPacketHandler( TPacketHandler* instance )
    {
        // Register the packet type inside a factory
        m_packetFactory.declare<typename TPacketHandler::Packet>( TypeInfo<typename TPacketHandler::Packet>::id() );

        // Add this packet handler instance to a list
        PacketTypeId type = instance->packetTypeId();
        m_packetHandlers[type].push_back( instance );
    }

#ifndef DC_CPP11_DISABLED
    // ** Application::addPacketHandler
    template<typename TPacketHandler, typename ... TArgs>
    void Application::addPacketHandler( const TArgs& ... args )
    {
        addPacketHandler( DC_NEW TPacketHandler( args... ) );
    }

	// ** Application::emit
	template<typename TEvent, typename ... TArgs>
	void Application::emit( const TArgs& ... args )
	{
        TEvent e( args... );
		emitTo( e, eventListeners() );
	}
#endif  /*  DC_CPP11_DISABLED   */

} // namespace Network
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Application_H__	*/