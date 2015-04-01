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
    
    namespace detail {
        
        //! Event listener subscribtion
        class Listener {
        public:
            
            virtual         ~Listener( void ) {}
            
            //! Emits event to listener.
            virtual void    emit( const void* e ) = 0;
        };
        
        //! Event listener with a type.
        template<typename T>
        class EventListener : public Listener {
        public:
            
            //! Callback function type.
            typedef cClosure<void(const T&)> Callback;
            
                            //! Constructs EventListener instance.
                            EventListener( const Callback& callback )
                                : m_callback( callback ) {}

			//! Compares this listener with a callback.
			bool			operator == ( const Callback& callback ) const { return m_callback == callback; }
            
            //! Calls a callback function with a casted event.
            virtual void    emit( const void* e ) { m_callback( *reinterpret_cast<const T*>( e ) ); }
            
        private:
            
            //! Callback function.
            Callback        m_callback;
        };
    } // namespace detail

	//! Event emitter class is used for dispatching strong typed global events.
	class EventEmitter {
	public:

        virtual	~EventEmitter( void ) {}

		//! Subscribes for an event.
		template<typename T>
        void subscribe( const typename detail::EventListener<T>::Callback& callback );

		//! Removes an event listener.
		template<typename T>
		void unsubscribe( const typename detail::EventListener<T>::Callback& callback );

		//! Emits a global event.
		template<typename T>
		void emit( const T& e );

		//! Constructs and emits a new event instance.
		template<typename T>							void emit( void );
		template<typename T, TemplateFunctionTypes1>	void emit( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2>	void emit( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3>	void emit( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4>	void emit( TemplateFunctionArgs4 );

	private:

		//! Array of listeners.
		typedef Array< AutoPtr<detail::Listener> > Listeners;

		//! Listener container type.
		typedef Map<TypeIdx, Listeners> Subscribers;

		//! Array of array of listeners.
		Subscribers	m_subscribers;
	};

	// ** EventEmitter::subscribe
	template<typename T>
	inline void EventEmitter::subscribe( const typename detail::EventListener<T>::Callback& callback )
	{
		TypeIdx idx = TypeIndex<T>::idx();

		if( m_subscribers.count( idx ) == 0 ) {
			m_subscribers[idx] = Listeners();
		}

        m_subscribers[idx].push_back( DC_NEW detail::EventListener<T>( callback ) );
	}

	// ** EventEmitter::unsubscribe
	template<typename T>
	inline void EventEmitter::unsubscribe( const typename detail::EventListener<T>::Callback& callback )
	{
		TypeIdx idx = TypeIndex<T>::idx();

		if( m_subscribers.count( idx ) == 0 ) {
			return;
		}

		typedef detail::EventListener<T> EventListenerType;
		Listeners& listeners = m_subscribers[idx];

		for( Listeners::iterator i = listeners.begin(); i != listeners.end(); )
		{
			EventListenerType& listener = *static_cast<EventListenerType*>( i->get() );
			if( listener == callback ) {
				i = listeners.erase( i );
			} else {
				++i;
			}
		}
	}

	// ** EventEmitter::emit
	template<typename T>
	inline void EventEmitter::emit( const T& e )
	{
		typedef cClosure<void(const T&)> CallbackType;

		TypeIdx idx = TypeIndex<T>::idx();
		Subscribers::iterator i = m_subscribers.find( idx );

		if( i == m_subscribers.end() ) {
			return;
		}

		for( u32 j = 0, n = ( u32 )i->second.size(); j < n; j++ ) {
            i->second[j]->emit( &e );
		}
	}

	// ** EventEmitter::emit
	template<typename T>
	inline void EventEmitter::emit( void )
	{
		T e;
		emit( e );
	}

	// ** EventEmitter::emit
	template<typename T, TemplateFunctionTypes1>
	inline void EventEmitter::emit( TemplateFunctionArgs1 )
	{
		T e( arg0 );
		emit( e );
	}

	// ** EventEmitter::send
	template<typename T, TemplateFunctionTypes2>
	inline void EventEmitter::emit( TemplateFunctionArgs2 )
	{
		T e( arg0, arg1 );
		emit( e );
	}

	// ** EventEmitter::send
	template<typename T, TemplateFunctionTypes3>
	inline void EventEmitter::emit( TemplateFunctionArgs3 )
	{
		T e( arg0, arg1, arg2 );
		emit( e );
	}

	// ** EventEmitter::send
	template<typename T, TemplateFunctionTypes4>
	inline void EventEmitter::emit( TemplateFunctionArgs4 )
	{
		T e( arg0, arg1, arg2, arg3 );
		emit( e );
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