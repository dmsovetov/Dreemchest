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

#ifndef		__DC_Event_EventDispatcher_H__
#define		__DC_Event_EventDispatcher_H__

#include	"EventListener.h"

DC_BEGIN_DREEMCHEST

namespace event {

	// ** class EventDispatcher
	class dcInterface EventDispatcher {

        // ** struct sEventListenerHandle
        struct sEventListenerHandle {
            EventListener   m_listener;
            std::string     m_method;
            u32           m_threadId;

                            sEventListenerHandle( const EventListener& listener, const char *method, u32 threadId )
                                : m_listener( listener ), m_method( method ? method : "" ), m_threadId( threadId ) {}
        };

        typedef std::multimap<strhash, sEventListenerHandle> EventListeners;

	public:

                                    EventDispatcher( dcContext ctx );
                                    ~EventDispatcher( void );

        bool                        IsBlockEvents( void ) const;
        void                        SetBlockEvents( bool value );

		void						AttachListener( const char *eventId, EventListener listener, const char *method = NULL );
		void						DetachListener( const char *eventId, EventListener listener, const char *method = NULL );
        void						DispatchEvent( const char *eventId, dcScriptObject event );
        void                        DispatchEvent( const char *eventId, Event *e );
		void                        DetachAllListeners( const void *listener );

    private:

        void                        TaskDispatchEvent( dcTaskProgress progress, void *userData );

    private:

        EventListeners              m_listeners;
        EventListeners::iterator    m_iterator;
        bool                        m_wasRemoved;
        bool                        m_blockEvents;
	};

} // namespace event

DC_END_DREEMCHEST

#endif	/*	!__DC_Event_EventDispatcher_H__	*/