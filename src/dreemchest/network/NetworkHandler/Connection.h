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

		//! A helper struct to track the traffic in kbps.
		struct Traffic {
			u32					m_lastUpdateTimestamp;	//!< The last time the tracking was updated.
			u32					m_sentBps;				//!< Last sent bits/s value.
			u32					m_receivedBps;			//!< Last received bits/s value.
			u32					m_lastSentBytes;		//!< Total sent bytes when the tacking was update.
			u32					m_lastReceivedBytes;	//!< Total received bytes when the tacking was update.
		};

		//! Returns parent network handler.
		NetworkHandler*			networkHandler( void ) const;

		//! Returns the total amount of bytes received.
		u32						totalBytesReceived( void ) const;

		//! Returns the total amount of bytes sent.
		u32						totalBytesSent( void ) const;

		//! Returns current time.
		u32						time( void ) const;

		//! Returns the round trip time.
		u32						roundTripTime( void ) const;

		//! Returns the time to live for this connection.
		s32						timeToLive( void ) const;

		//! Returns the traffic counter.
		const Traffic&			traffic( void ) const;

		//! Returns a connection TCP socket.
		const  TCPSocketPtr&	socket( void ) const;
		TCPSocketPtr&			socket( void );

		//! Returns a remote address of a connection.
		const NetworkAddress&	address( void ) const;

		//! Invokes a remote procedure.
		template<typename TRemoteProcedure>
		void					invokeVoid( const typename TRemoteProcedure::Argument& argument );

		//! Invokes a remote procedure.
		template<typename TRemoteProcedure>
		void					invoke( const typename TRemoteProcedure::Argument& argument, const typename RemoteResponseHandler<typename TRemoteProcedure::Response>::Callback& callback );

		//! Emits the event to this connection.
		template<typename TEvent>
		void					emit( const TEvent& e );

		//! Sends a packet over this connection.
		void					send( NetworkPacket* packet );

	#ifndef DC_CPP11_DISABLED
		//! Generic method to construct and sent the network packet over this connection.
		template<typename TPacket, typename ... Args>
		void					send( const Args& ... args );
	#endif	/*	!DC_CPP11_DISABLED	*/

	private:

								//! Constructs Connection instance.
								Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket );

		//! Updates this connection
		void					update( u32 dt );

		//! Sets the round trip time for this connection.
		void					setRoundTripTime( u32 value );

		//! Sets current time.
		void					setTime( u32 value );

		//! Sets the time to live for this connection.
		void					setTimeToLive( s32 value );

		//! Sets the keep-alive timestamp.
		void					setKeepAliveTimestamp( u32 value );

		//! Returns the keep-alive timestamp.
		u32						keepAliveTimestamp( void ) const;

		//! Handles a recieved remote call response.
		bool					handleResponse( const packets::RemoteCallResponse& packet );

	private:

		//! A helper struct to store a timestamp of an RPC call.
		struct PendingRemoteCall {
			String							m_name;			//!< Remote procedure name.
			u32								m_timestamp;	//!< The time in milliseconds when a call was performed.
			AutoPtr<IRemoteResponseHandler>	m_handler;		//!< Response handler.

											//! Constructs a PendingRemoteCall instance.
											PendingRemoteCall( const String& name = "", IRemoteResponseHandler* handler = NULL )
												: m_name( name ), m_handler( handler ), m_timestamp( 0 ) {}
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

		//! Current connection time.
		u32						m_time;

		//! Current round trip time.
		u32						m_roundTripTime;

		//! Time to live.
		s32						m_timeToLive;

		//! Last keep-alive timestamp.
		u32						m_keepAliveTimestamp;

		//! Traffic counter.
		Traffic					m_traffic;
	};

	// ** Connection::invokeVoid
	template<typename TRemoteProcedure>
	void Connection::invokeVoid( const typename TRemoteProcedure::Argument& argument )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( argument );

		// ** Send an RPC request
		send<packets::RemoteCall>( 0, TRemoteProcedure::id(), 0, buffer->array() );
	}

	// ** Connection::invoke
	template<typename TRemoteProcedure>
	void Connection::invoke( const typename TRemoteProcedure::Argument& argument, const typename RemoteResponseHandler<typename TRemoteProcedure::Response>::Callback& callback )
	{
		// ** Serialize argument to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( argument );

		// ** Send an RPC request
		u16     remoteCallId = m_nextRemoteCallId++;
        TypeId  returnTypeId = TypeInfo<typename TRemoteProcedure::Response>::id();
        
		send<packets::RemoteCall>( remoteCallId, TRemoteProcedure::id(), returnTypeId, buffer->array() );
		
		// ** Create a response handler.
		m_pendingRemoteCalls[remoteCallId] = PendingRemoteCall( TRemoteProcedure::name(), DC_NEW RemoteResponseHandler<typename TRemoteProcedure::Response>( callback ) );
	}

	// ** Connection::emit
	template<typename TEvent>
	void Connection::emit( const TEvent& e )
	{
		// ** Serialize event to a byte buffer.
		io::ByteBufferPtr buffer = io::BinarySerializer::write( e );

		// ** Send the packet
		send<packets::Event>( TypeInfo<TEvent>::id(), buffer->array() );
	}

#ifndef DC_CPP11_DISABLED
	//! Generic method to construct and sent the network packet over this connection.
	template<typename TPacket, typename ... Args>
	void Connection::send( const Args& ... args )
	{
		TPacket packet( args... );
		send( &packet );
	}
#endif	/*	!DC_CPP11_DISABLED	*/

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