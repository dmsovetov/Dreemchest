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
	friend class NetworkHandler;
	public:

		//! Returns parent network handler.
		NetworkHandler*			networkHandler( void ) const;

		//! Returns a connection TCP socket.
		const  TCPSocketPtr&	socket( void ) const;
		TCPSocketPtr&			socket( void );

		//! Returns a remote address of a connection.
		const NetworkAddress&	address( void ) const;

		//! Sends a packet over this connection.
		void					send( NetworkPacket* packet );

		//! Sends a packet of a specified type over this connection.
		template<typename T>						 void send( void );
		template<typename T, TemplateFunctionTypes1> void send( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2> void send( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3> void send( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4> void send( TemplateFunctionArgs4 );

	private:

								//! Constructs Connection instance.
								Connection( NetworkHandler* networkHandler, const TCPSocketPtr& socket );

	private:

		//! Parent network connection.
		NetworkHandler*			m_networkHandler;

		//! Connection TCP socket.
		TCPSocketPtr			m_socket;
	};

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

}

DC_END_DREEMCHEST

#endif	/*	!__Network_Connection_H__	*/