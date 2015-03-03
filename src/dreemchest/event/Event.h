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

#ifndef		__DC_Event_H__
#define		__DC_Event_H__

#ifndef DC_UTILS_INCLUDED
	#include <utils/Utils.h>
#endif

DC_BEGIN_DREEMCHEST

namespace event {

	//! Event identifier type.
	typedef u32 EventId;

	//! Generates unique integer event identifiers.
	class dcInterface EventIdGenerator {
	protected:

		//! Generates a next event identifier.
		static EventId	generateEventId( void );
	};

	// ----


	typedef void (*EventCallback)( void* callback, const void* event );

	struct BaseEvent
	{
		virtual ~BaseEvent() {}
	protected:
		static size_t getNextType();
	};

	inline size_t BaseEvent::getNextType()
	{
		static size_t type_count = 0;
		return type_count++;
	}

	template <typename EventType>
	struct Event : BaseEvent
	{
		static size_t type()
		{
			static size_t t_type = BaseEvent::getNextType();
			return t_type;
		} //; You don't need this semi-colon. Remove it.
	};

	template<typename Event>
	EventCallback createCallback( cClosure<void(const Event&)> callback ) {
		typedef cClosure<void(const Event&)> Closure;

		struct X {
			static void thunk( void* callback, const void* event ) {
				const Event& e = *reinterpret_cast<const Event*>( event );
				Closure c = *reinterpret_cast<Closure*>( callback );
				c( e );
			}
		};

		return X::thunk;
	}

	struct EventHandler {
		EventCallback	callback;
		int				id;
		void*			closure;
	};

	class EventManager {
	public:

		template<typename E>
		void subscribe( cClosure<void(const E&)>& callback ) {
			EventHandler handler;
			handler.callback = createCallback<E>( callback );
			handler.id		 = Event<E>::type();
			handler.closure	 = &callback;
			m_callbacks.push_back( handler );
		}

		template<typename E>
		void emit( const E& e ) {
			int id = Event<E>::type();

			for( int i = 0; i < m_callbacks.size(); i++ ) {
				if( m_callbacks[i].id == id ) {
					m_callbacks[i].callback( m_callbacks[i].closure, &e );
				}
			}
		}

	private:

		std::vector<EventHandler> m_callbacks;
	};


} // namespace event

DC_END_DREEMCHEST

#endif	/*	!__DC_Event_H__	*/