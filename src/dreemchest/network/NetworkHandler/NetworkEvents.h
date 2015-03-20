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

#ifndef __DC_Network_NetworkEvents_H__
#define __DC_Network_NetworkEvents_H__

#include "../Network.h"
#include "Packets.h"
#include "../../io/serialization/Serializable.h"
#include "../../event/EventEmitter.h"

DC_BEGIN_DREEMCHEST

namespace net {
/*
	//! Basic network event.
	struct Event : public io::Serializable {
	};

	//! Event emitter interface.
	class EventEmitter {
	public:

		virtual				~EventEmitter( void ) {}

		//! Localy emits a received network event.
		virtual void		emit( TCPSocket* sender, Event* event ) = 0;
	};

	//! Generic event emitter.
	template<typename T>
	class GenericEventEmitter : public EventEmitter {
	public:

							//! Constructs GenericEventEmitter instance.
							GenericEventEmitter( NetworkEvents* events )
								: m_events( events ) {}

		//! Emits a casted event.
		virtual void		emit( TCPSocket* sender, Event* event )
		{
			T* eventWithType = castTo<T>( event );
			DC_BREAK_IF( eventWithType == NULL );
			m_events->emit( *eventWithType );
		}

	private:

		//! Parent network event interface.
		NetworkEvents*		m_events;
	};

	//! Network event interface.
	class NetworkEvents {
	public:

		//! Emits a network event.
		template<typename T>
		void					emit( const TCPSocketList& listeners, const T& e );

		//! Subscribes for a network event.
		template<typename T>
		void					subscribe( const cClosure<void(const T&)>& callback );

	private:

		//! A container type to store all registered event handlers.
		typedef Map< TypeId, AutoPtr<EventEmitter> > EventEmitters;

		//! Local event emitter.
		event::EventEmitter		m_eventEmitter;

		//! Registered event types.
		EventEmitters			m_eventEmitters;
	};

	// ** NetworkEvents::emit
	template<typename T>
	void NetworkEvents::emit( const TCPSocketList& listeners, const T& e )
	{
		// ** Serialize event to a byte buffer.
		io::ByteBufferPtr buffer = io::ByteBuffer::create();
		io::Storage		  storage( io::StorageBinary, buffer );
		e.write( storage );

		// ** Send to listeners
		for( TCPSocketList::iterator i = listeners.begin(), end = listeners.end(); i != end; ++i ) {
			sendPacket<packets::Event>( i->get(), T::classTypeId(), buffer->array() );
		}
	}
	*/
} // namespace event

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkEvents_H__	*/