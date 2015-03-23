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

#ifndef	__DC_Network_RemoteCallHandler_H__
#define	__DC_Network_RemoteCallHandler_H__

#include "../Network.h"
#include "Connection.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Remote procedure call handler interface class.
	class IRemoteCallHandler {
	public:

		virtual			~IRemoteCallHandler( void ) {}

		//! Packet handler callback.
		virtual bool	handle( ConnectionPtr& connection, const packets::RemoteCall* packet ) = 0;
	};

	//! Remote call response object.
	template<typename T>
	class Response {
	public:

		//! Constructs a Response instance.
		Response( const ConnectionWPtr& connection, u16 id ) 
			: m_connection( connection ), m_id( id ), m_wasSent( false ) {}

		bool wasSent( void ) const { return m_wasSent; }

		//! Send a response to caller.
		void operator()( const T& value )
		{
			// ** Serialize argument to a byte buffer.
			io::ByteBufferPtr buffer = value.writeToByteBuffer();

			// ** Send an RPC response packet.
			m_connection->send<packets::RemoteCallResponse>( m_id, T::classTypeId(), buffer->array() );

			// ** Mark this response as sent.
			m_wasSent = true;
		}

	private:

		//! Parent connection.
		ConnectionWPtr	m_connection;

		//! Remote call identifier.
		u16				m_id;

		//! Flag indicating that a response was sent.
		bool			m_wasSent;
	};

	//! Template class that handles a RemoteCall packet and invokes a local procedure.
	template<typename T, typename R>
	class RemoteCallHandler : public IRemoteCallHandler {
	public:

		//! Response type.
		typedef Response<R> Response;

		//! Function type to handle remote calls.
		typedef cClosure<bool(ConnectionPtr&,Response&,const T&)> Callback;

						//! Constructs RemoteCallHandler instance.
						RemoteCallHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Reads a payload from an Event packet and emits it as local event.
		virtual bool	handle( ConnectionPtr& connection, const packets::RemoteCall* packet );

	private:

		//! Local procedure
		Callback		m_callback;
	};

	// ** RemoteCallHandler::handle
	template<typename T, typename R>
	inline bool RemoteCallHandler<T, R>::handle( ConnectionPtr& connection, const packets::RemoteCall* packet )
	{
		Response response( connection, packet->id );
		bool result = m_callback( connection, response, io::Serializable::readFromBytes<T>( packet->payload ) );
		DC_BREAK_IF( !response.wasSent() );
		return result;
	}

	//! Remote response handler interface.
	class IRemoteResponseHandler {
	public:

		virtual			~IRemoteResponseHandler( void ) {}

		virtual bool	handle( ConnectionPtr& connection, const packets::RemoteCallResponse* packet ) = 0;
	};

	//! Template class that handles a RemoteCallResponse packet and invokes a local procedure.
	template<typename T>
	class RemoteResponseHandler : public IRemoteResponseHandler {
	public:

		//! Function type to handle remote calls.
		typedef cClosure<bool(ConnectionPtr&,const T&)> Callback;

						//! Constructs RemoteResponseHandler instance.
						RemoteResponseHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Reads a payload from an Event packet and emits it as local event.
		virtual bool	handle( ConnectionPtr& connection, const packets::RemoteCallResponse* packet );

	private:

		//! Local procedure
		Callback		m_callback;
	};

	// ** RemoteResponseHandler::handle
	template<typename T>
	inline bool RemoteResponseHandler<T>::handle( ConnectionPtr& connection, const packets::RemoteCallResponse* packet )
	{
		return m_callback( connection, io::Serializable::readFromBytes<T>( packet->payload ) );
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_RemoteCallHandler_H__	*/