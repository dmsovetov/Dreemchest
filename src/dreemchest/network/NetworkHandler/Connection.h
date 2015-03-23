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

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Remote connection interface.
	class Connection : public RefCounted {
	EmbedUserData
	public:

		//! Returns a connection TCP socket.
		const  TCPSocketPtr&	socket( void ) const;
		TCPSocketPtr&			socket( void );

		//! Returns a remote address of a connection.
		const NetworkAddress&	address( void ) const;

		//! Sends a packet over this connection.
		void					sendPacket( NetworkPacket* packet );

		//! Sends a packet of a specified type over this connection.
		template<typename T>						 void sendPacket( void );
		template<typename T, TemplateFunctionTypes1> void sendPacket( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2> void sendPacket( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3> void sendPacket( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4> void sendPacket( TemplateFunctionArgs4 );

		//! Creates a new Connection instance.
		static ConnectionPtr	create( NetworkHandler* networkHandler, const TCPSocketPtr& socket );

	private:

								//! Constructs Connection instance.
								Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket );

	private:

		//! Parent network handler.
		NetworkHandler*			m_networkHandler;

		//! Connection TCP socket.
		TCPSocketPtr			m_socket;
	};

	// ** Connection::sendPacket
	template<typename T>
	inline void Connection::sendPacket( void )
	{
		T packet;
		sendPacket( &packet );
	}

	// ** Connection::sendPacket
	template<typename T, TemplateFunctionTypes1>
	inline void Connection::sendPacket( TemplateFunctionArgs1 )
	{
		T packet( arg0 );
		sendPacket( &packet );
	}

	// ** Connection::sendPacket
	template<typename T, TemplateFunctionTypes2>
	inline void Connection::sendPacket( TemplateFunctionArgs2 )
	{
		T packet( arg0, arg1 );
		sendPacket( &packet );
	}

	// ** Connection::sendPacket
	template<typename T, TemplateFunctionTypes3>
	inline void Connection::sendPacket( TemplateFunctionArgs3 )
	{
		T packet( arg0, arg1, arg2 );
		sendPacket( &packet );
	}

	// ** Connection::sendPacket
	template<typename T, TemplateFunctionTypes4>
	inline void Connection::sendPacket( TemplateFunctionArgs4 )
	{
		T packet( arg0, arg1, arg2, arg3 );
		sendPacket( &packet );
	}
}

DC_END_DREEMCHEST

#endif	/*	!__Network_Connection_H__	*/