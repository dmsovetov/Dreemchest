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

#include "../Connection/ConnectionTCP.h"
#include "RemoteCallHandler.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! Remote connection interface.
	class Connection : public ConnectionTCP {
	friend class Application;
	public:

		//! A helper struct to track the traffic in kbps.
		struct Traffic {
			u32					m_lastUpdateTimestamp;	//!< The last time the tracking was updated.
			u32					m_sentBps;				//!< Last sent bits/s value.
			u32					m_receivedBps;			//!< Last received bits/s value.
			u32					m_lastSentBytes;		//!< Total sent bytes when the tacking was update.
			u32					m_lastReceivedBytes;	//!< Total received bytes when the tacking was update.
		};

		//! Returns parent network application instance.
		Application*			application( void ) const;

		//! Returns the traffic counter.
		const Traffic&			traffic( void ) const;

		//! Invokes a remote procedure.
		template<typename TRemoteProcedure>
		void					invokeVoid( const typename TRemoteProcedure::Argument& argument );

		//! Invokes a remote procedure.
		template<typename TRemoteProcedure>
		void					invoke( const typename TRemoteProcedure::Argument& argument, const typename RemoteResponseHandler<typename TRemoteProcedure::Response>::Callback& callback );

		//! Emits the event to this connection.
		template<typename TEvent>
		void					emit( const TEvent& e );

	private:

								//! Constructs Connection instance.
								Connection( Application* application, const TCPSocketPtr& socket );

		//! Updates this connection
		void					update( u32 dt );

		//! Handles a recieved remote call response.
		void					handleResponse( const Packets::RemoteCallResponse& packet );

	private:

		//! A helper struct to store a timestamp of an RPC call.
		struct PendingRemoteCall {
			String							m_name;			//!< Remote procedure name.
			s32								m_timeLeft;		//!< The time left to wait for a response to this call.
			AutoPtr<IRemoteResponseHandler>	m_handler;		//!< Response handler.

											//! Constructs a PendingRemoteCall instance.
											PendingRemoteCall( const String& name = "", IRemoteResponseHandler* handler = NULL, s32 timeLeft = 60000 )
												: m_name( name ), m_timeLeft( timeLeft ), m_handler( handler ) {}
		};

		//! A container type to store all pending remote calls.
		typedef Map< u16, PendingRemoteCall > PendingRemoteCalls;

		//! Parent network connection.
		Application*			m_application;

		//! A list of pending remote calls.
		PendingRemoteCalls		m_pendingRemoteCalls;

		//! Next remote call response id.
		u16						m_nextRemoteCallId;

		//! Traffic counter.
		Traffic					m_traffic;
	};
}

DC_END_DREEMCHEST

#endif	/*	!__Network_Connection_H__	*/
