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

#ifndef __DC_Event_EventEmitter_H__
#define __DC_Event_EventEmitter_H__

#include "Event.h"

DC_BEGIN_DREEMCHEST

namespace event {

	//! Event emitter class is used for dispatching strong typed global events.
	class EventEmitter {
	public:

		virtual	~EventEmitter( void );

		//! Subscribes for an event.
		template<typename T>
		void subscribe( const cClosure<void(const T&)>& callback );

		//! Emits a global event.
		template<typename T>
		void emit( const T& e );

	private:

		//! Array of listeners.
		typedef Array<const void*> Listeners;

		//! Listener container type.
		typedef Map<TypeIdx, Listeners> Subscribers;

		//! Array of array of listeners.
		Subscribers	m_subscribers;
	};

	// ** EventEmitter::~EventEmitter
	inline EventEmitter::~EventEmitter( void )
	{
		for( Subscribers::iterator i = m_subscribers.begin(), end = m_subscribers.end(); i != end; ++i ) {
			for( u32 j = 0, n = i->second.size(); j < n; j++ ) {
				delete i->second[j];
			}
		}
	}

	// ** EventEmitter::subscribe
	template<typename T>
	inline void EventEmitter::subscribe( const cClosure<void(const T&)>& callback )
	{
		typedef cClosure<void(const T&)> CallbackType;

		TypeIdx idx = TypeIndex<T>::idx();

		if( m_subscribers.count( idx ) == 0 ) {
			m_subscribers[idx] = Listeners();
		}

		m_subscribers[idx].push_back( new CallbackType( callback ) );
	}

	// ** EventEmitter::emit
	template<typename T>
	inline void EventEmitter::emit( const T& e )
	{
		typedef cClosure<void(const T&)> CallbackType;

		TypeIdx idx = TypeIndex<T>::idx();
		Subscribers::const_iterator i = m_subscribers.find( idx );

		if( i == m_subscribers.end() ) {
			return;
		}

		for( u32 j = 0, n = ( u32 )i->second.size(); j < n; j++ ) {
			(*reinterpret_cast<const CallbackType*>( i->second[j] ))( e );
		}
	}

	namespace detail {

		//! Event interface
		class IQueuedEvent {
		public:

			virtual			~IQueuedEvent( void ) {}
			virtual void	emit( EventEmitter& emitter ) = 0;
		};

		//! Typed event.
		template<typename T>
		class QueuedEvent : public IQueuedEvent {
		public:

							//! Constructs a QueuedEvent instance.
							QueuedEvent( const T& e )
								: m_event( e ) {}

			//! Emits an event.
			virtual void	emit( EventEmitter& emitter ) { emitter.emit( m_event ); }

		private:

			//! Stored event data.
			T				m_event;
		};

		//! Event queue is used for cross-thread event emitting.
		class EventQueue {
		public:

			//! Queues a new event.
			template<typename T>
			void			queue( const T& e );

			//! Emits all queued events.
			void			emit( EventEmitter& emitter );

		private:

			//! Queued events.
			Array<IQueuedEvent*>	m_events;
		};

		// ** EventQueue::queue
		template<typename T>
		inline void EventQueue::queue( const T& e )
		{
		//	m_events.push_back( new QueuedEvent( e ) );
		}

		// ** EventQueue::emit
		inline void EventQueue::emit( EventEmitter& emitter )
		{
			for( u32 i = 0, n = m_events.size(); i < n; i++ ) {
				m_events[i]->emit( emitter );
				delete m_events[i];
			}

			m_events.clear();
		}

	} // namespace detail

} // namespace event

DC_END_DREEMCHEST

#endif	/*	!__DC_Event_EventEmitter_H__	*/