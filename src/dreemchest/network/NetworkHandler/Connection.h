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

#ifndef __Network_Connection_H__
#define __Network_Connection_H__

#include "RemoteCallHandler.h"
#include "Packets.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Remote connection interface.
	class Connection : public RefCounted {
	EmbedUserData
	friend class NetworkHandler;
	public:

		//! Returns parent network handler.
		NetworkHandler*			networkHandler( void ) const;

		//! Returns the total amount of bytes received.
		u32						totalBytesReceived( void ) const;

		//! Returns the total amount of bytes sent.
		u32						totalBytesSent( void ) const;

		//! Returns a connection TCP socket.
		const  TCPSocketPtr&	socket( void ) const;
		TCPSocketPtr&			socket( void );

		//! Returns a remote address of a connection.
		const NetworkAddress&	address( void ) const;

		//! Invokes a remote procedure.
		template<typename T>
		void					invoke( CString method, const T& argument );

		//! Invokes a remote procedure.
		template<typename T, typename R>
		void					invoke( CString method, const T& argument, const typename RemoteResponseHandler<R>::Callback& callback );

		//! Emits the event to this connection.
		template<typename TEvent>
		void					emit( const TEvent& e );

		//! Sends a packet over this connection.
		void					send( NetworkPacket* packet );

		//! Sends a packet of a specified type over this connection.
		template<typename T>						 void send( void );
		template<typename T, TemplateFunctionTypes1> void send( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2> void send( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3> void send( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4> void send( TemplateFunctionArgs4 );
		template<typename T, TemplateFunctionTypes5> void send( TemplateFunctionArgs5 );

	private:

								//! Constructs Connection instance.
								Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket );

		//! Updates this connection
		void					update( void );

		//! Handles a recieved remote call response.
		bool					handleResponse( const packets::RemoteCallResponse& packet );

	private:

		//! A helper struct to store a timestamp of an RPC call.
		struct PendingRemoteCall {
			String							m_name;			//!< Remote procedure name.
			UnixTime						m_timestamp;	//!< A Unix time when a call was performed.
			AutoPtr<IRemoteResponseHandler>	m_handler;		//!< Response handler.

											//! Constructs a PendingRemoteCall instance.
											PendingRemoteCall( const String& name = "", IRemoteResponseHandler* handler = NULL )
												: m_name( name ), m_handler( handler ) {}
		};

		//! A container type to store all pending remote calls.
		typedef Map< u16, PendingRemoteCall > PendingRemoteCalls;

		//! Parent network connection.
		NetworkHandler*			m_networkHandler;

		//! Connection TCP socket.
		TCPSocketPtr			m_socket;

		//! A list of pending remote calls.
		PendingRemoteCalls		m_pendingRemoteCalls;

		//! Next remote call response id.
		u16						m_nextRemoteCallId;

		//! The total amount of bytes received.
		u32						m_totalBytesReceived;

		//! The total amount of bytes sent.
		u32						m_totalBytesSent;
	};

	// ** Connection::invoke
	template<typename T>
	inline void Connection::invoke( CString method, const T& argument )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( argument );

		// ** Send an RPC request
		send<packets::RemoteCall>( 0, StringHash( method ), 0, buffer->array() );
	}

	// ** Connection::invoke
	template<typename T, typename R>
	inline void Connection::invoke( CString method, const T& argument, const typename RemoteResponseHandler<R>::Callback& callback )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( argument );

		// ** Send an RPC request
		u16     remoteCallId = m_nextRemoteCallId++;
        TypeId  returnTypeId = TypeInfo<R>::id();
        
		send<packets::RemoteCall>( remoteCallId, StringHash( method ), returnTypeId, buffer->array() );
		
		// ** Create a response handler.
		m_pendingRemoteCalls[remoteCallId] = PendingRemoteCall( method, DC_NEW RemoteResponseHandler<R>( callback ) );

	// ** Connection::emit
	template<typename TEvent>
	inline void Connection::emit( const TEvent& e )
	{
		// ** Serialize event to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( e );

		// ** Send the packet
		send<packets::Event>( TypeInfo<TEvent>::id(), buffer->array() );
	}

	// ** Connection::send
	template<typename T>
	inline void Connection::send( void )
	{
		T packet;
		send( &packet );
	}

	// ** Connection::send
	template<typename T, TemplateFunctionTypes1>
	inline void Connection::send( TemplateFunctionArgs1 )
	{
		T packet( arg0 );
		send( &packet );
	}

	// ** Connection::send
	template<typename T, TemplateFunctionTypes2>
	inline void Connection::send( TemplateFunctionArgs2 )
	{
		T packet( arg0, arg1 );
		send( &packet );
	}

	// ** Connection::send
	template<typename T, TemplateFunctionTypes3>
	inline void Connection::send( TemplateFunctionArgs3 )
	{
		T packet( arg0, arg1, arg2 );
		send( &packet );
	}

	// ** Connection::send
	template<typename T, TemplateFunctionTypes4>
	inline void Connection::send( TemplateFunctionArgs4 )
	{
		T packet( arg0, arg1, arg2, arg3 );
		send( &packet );
	}

	// ** Connection::send
	template<typename T, TemplateFunctionTypes5>
	inline void Connection::send( TemplateFunctionArgs5 )
	{
		T packet( arg0, arg1, arg2, arg3, arg4 );
		send( &packet );
	}

	//! Send a response to caller.
	template<typename T>
	inline bool Response<T>::operator()( const T& value, const Error& error )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( value );

		// ** Send an RPC response packet.
		m_connection->send<packets::RemoteCallResponse>( m_id, error, TypeInfo<T>::id(), buffer->array() );

		// ** Mark this response as sent.
		m_wasSent = true;

		return true;
	}

}

DC_END_DREEMCHEST

#endif	/*	!__Network_Connection_H__	*/