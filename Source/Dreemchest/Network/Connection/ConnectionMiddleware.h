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

#ifndef __DC_Network_ConnectionMiddleware_H__
#define __DC_Network_ConnectionMiddleware_H__

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! A connection middleware is a piece of connection processing logic.
	class ConnectionMiddleware {
	friend class Connection_;
	public:

		virtual				~ConnectionMiddleware( void ) {}

		//! Abstract method to run a custom logic while updating a connection instance.
		virtual void		update( u32 dt ) = 0;

	private:

		//! Sets the parent connection instance.
		void				setConnection( Connection* value );

	protected:

		Connection*			m_connection;	//!< Parent connection instance.
	};

	//! Base class for all middlewares that send packets with a specified interval of time.
	class PacketSendingMiddleware : public ConnectionMiddleware {
	public:

							//! Constructs the PacketSendingMiddleware instance.
							PacketSendingMiddleware( u32 interval );

	protected:

		//! Updates the time accumulator and returns true if the packet should be sent.
		bool				accumulateTime( u32 dt );

	protected:

		u32					m_interval;		//!< The packet interval.
		u32					m_accumulator;	//!< The accumulated time.
	};

	//! Middleware to ping the connection with a specified interval of time.
	class PingInterval : public PacketSendingMiddleware {
	public:

							//! Constructs the PingInterval instance.
							PingInterval( u32 interval = 1000, s32 iterations = 1 );

	protected:

		//! Sends the Ping packet over a connection when the specified amount of time passed.
		virtual void		update( u32 dt ) NIMBLE_OVERRIDE;

	private:

		s32					m_iterations;	//!< The total number of ping iterations to perform.
	};

	//! Middleware to send the KeepAlive packet with a specified interval of time.
	class KeepAliveInterval : public PacketSendingMiddleware {
	public:

							//! Constructs the KeepAliveInterval instance.
							KeepAliveInterval( u32 interval = 1000 );

	protected:

		//! Sends the KeepAlive packet over a connection when the specified amount of time passed.
		virtual void		update( u32 dt ) NIMBLE_OVERRIDE;
	};

	//! Middleware to close the connection on time out.
	class CloseOnTimeout : public ConnectionMiddleware {
	public:

							//! Constructs the CloseOnTimeout instance.
							CloseOnTimeout( s32 interval );

	protected:

		//! Closes the connection instance if there are no packets received in a specified interval of time.
		virtual void		update( u32 dt ) NIMBLE_OVERRIDE;

	protected:

		s32					m_interval;	//!< The connection time out value.
	};

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  !__DC_Network_ConnectionMiddleware_H__ */
