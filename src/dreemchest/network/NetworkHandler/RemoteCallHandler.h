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

DC_BEGIN_DREEMCHEST

namespace net {

	//! Remote procedure call handler interface class.
	class IRemoteCallHandler {
	public:

		virtual			~IRemoteCallHandler( void ) {}

		//! Packet handler callback.
		virtual bool	handle( TCPSocket* sender, const packets::RemoteCall* packet ) = 0;
	};

	//! Template class that handles a RemoteCall packet and invokes a local procedure.
	template<typename T>
	class RemoteCallHandler : public IRemoteCallHandler {
	public:

		//! Function type to handle remote calls.
		typedef cClosure<bool(TCPSocket*,u16,const T*)> Callback;

						//! Constructs RemoteCallHandler instance.
						RemoteCallHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Reads a payload from an Event packet and emits it as local event.
		virtual bool	handle( TCPSocket* sender, const packets::RemoteCall* packet );

	private:

		//! Local procedure
		Callback		m_callback;
	};

	// ** RemoteCallHandler::handle
	template<typename T>
	inline bool RemoteCallHandler<T>::handle( TCPSocket* sender, const packets::RemoteCall* packet )
	{
		T argument;

		if( packet->payload.size() ) {
			io::ByteBufferPtr buffer = io::ByteBuffer::createFromData( &packet->payload[0], packet->payload.size() );
			io::Storage       storage( io::StorageBinary, buffer );
			argument.read( storage );
		}

		return m_callback( sender, packet->id, &argument );
	}

	//! Remote response handler interface.
	class IRemoteResponseHandler {
	public:

		virtual			~IRemoteResponseHandler( void ) {}

		virtual bool	handle( TCPSocket* socket, const packets::RemoteCallResponse* packet ) = 0;
	};

	//! Template class that handles a RemoteCallResponse packet and invokes a local procedure.
	template<typename T>
	class RemoteResponseHandler : public IRemoteResponseHandler {
	public:

		//! Function type to handle remote calls.
		typedef cClosure<bool(TCPSocket*,const T*)> Callback;

						//! Constructs RemoteResponseHandler instance.
						RemoteResponseHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Reads a payload from an Event packet and emits it as local event.
		virtual bool	handle( TCPSocket* sender, const packets::RemoteCallResponse* packet );

	private:

		//! Local procedure
		Callback		m_callback;
	};

	// ** RemoteResponseHandler::handle
	template<typename T>
	inline bool RemoteResponseHandler<T>::handle( TCPSocket* sender, const packets::RemoteCallResponse* packet )
	{
		T value;

		if( packet->payload.size() ) {
			io::ByteBufferPtr buffer = io::ByteBuffer::createFromData( &packet->payload[0], packet->payload.size() );
			io::Storage       storage( io::StorageBinary, buffer );
			value.read( storage );
		}

		return m_callback( sender, &value );
	}

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_RemoteCallHandler_H__	*/