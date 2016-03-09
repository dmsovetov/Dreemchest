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

#include "EventDispatcher.h"

DC_BEGIN_DREEMCHEST

namespace event {

// ** EventDispatcher::EventDispatcher
EventDispatcher::EventDispatcher( dcContext ctx ) : cObject( ctx ), m_blockEvents( false )
{
}

// ** EventDispatcher::EventDispatcher
EventDispatcher::~EventDispatcher( void )
{
    m_listeners.clear();
}

// ** EventDispatcher::TaskDispatchEvent
void EventDispatcher::TaskDispatchEvent( dcTaskProgress progress, void *userData )
{
    dcEvent e = ( dcEvent )userData;
    DispatchEvent( e->m_eventID, e );
    e->Unref();
}

// ** EventDispatcher::IsBlockEvents
bool EventDispatcher::IsBlockEvents( void ) const
{
    return m_blockEvents;
}

// ** EventDispatcher::SetBlockEvents
void EventDispatcher::SetBlockEvents( bool value )
{
    m_blockEvents = value;
}

// ** EventDispatcher::DispatchEvent
void EventDispatcher::DispatchEvent( const char *eventId, Event *e )
{
    if( m_blockEvents ) {
        return;
    }
    
	DC_PROFILER( "EventDispatcher::DispatchEvent" );

    DC_BREAK_IF( e == NULL );
    DC_BREAK_IF( m_ctx == NULL );
    
	if( !e->m_sender ) {
		e->m_sender = this;
	}

    std::pair<EventListeners::iterator, EventListeners::iterator> range = m_listeners.equal_range( cStringHash::Djb2( eventId ) );
    u32 currentThread = thread::Thread::currentThread();

    // ** Dispatch events
    e->Ref();
    
    for( EventListeners::iterator i = range.first; i != range.second; ) {
        // ** Get listener handle
        sEventListenerHandle& handle = i->second;

        // !! Increment iterator BEFORE running a callback
        ++i;

        // ** Dispatch this event
	//	if( handle.m_threadId == currentThread ) {
			handle.m_listener.Trigger( m_ctx, e, handle.m_method.empty() ? eventId : handle.m_method.c_str() );
	//	} else {
	//		e->Ref();
	//		m_ctx->iTaskManager->RunTask( dcThisMethod( EventDispatcher::TaskDispatchEvent ), e, handle.m_threadId );
	//	}
    }

	e->Unref();
}

// ** EventDispatcher::DispatchEvent
void EventDispatcher::DispatchEvent( const char *eventId, dcScriptObject e )
{
    if( m_blockEvents ) {
        return;
    }
    
    DC_PROFILER( "EventDispatcher::DispatchEvent" );
    
    DC_BREAK_IF( m_ctx == NULL );

    if( !eventId ) {
        Error( "EventDispatcher::DispatchEvent : invalid event ID (null)\n" );
        return;
    }

    std::pair<EventListeners::iterator, EventListeners::iterator> range = m_listeners.equal_range( cStringHash::Djb2( eventId ) );

    // ** Dispatch events
    for( EventListeners::iterator i = range.first; i != range.second; ) {
        sEventListenerHandle& handle = i->second;

        ++i; // !! Increment iterator before calling a callback
        handle.m_listener.Trigger( m_ctx, e, handle.m_method.empty() ? eventId : handle.m_method.c_str() );
    }
}

// ** EventDispatcher::AttachListener
void EventDispatcher::AttachListener( const char *eventId, EventListener listener, const char *method )
{
    if( !eventId ) {
        Error( "EventDispatcher::AttachListener : invalid event ID (null)\n" );
        return;
    }
    
    strhash hash     = cStringHash::Djb2( eventId );
    u32   threadId = thread::Thread::currentThread();
    m_listeners.insert( std::pair<strhash, sEventListenerHandle>( hash, sEventListenerHandle( listener, method, threadId ) ) );
}

// ** EventDispatcher::DetachListener
void EventDispatcher::DetachListener( const char *eventId, EventListener listener, const char *method )
{
    if( !eventId ) {
        Error( "EventDispatcher::DetachListener : invalid event ID (null)\n" );
        return;
    }

    std::pair<EventListeners::iterator, EventListeners::iterator> range = m_listeners.equal_range( cStringHash::Djb2( eventId ) );

    // ** Dispatch events
    for( EventListeners::iterator i = range.first; i != range.second; ) {
        sEventListenerHandle& handle = i->second;

        if( handle.m_listener == listener && (!method || (method == handle.m_method)) ) {
            m_listeners.erase( i++ );
        } else {
            ++i;
        }
    }
}

// ** EventDispatcher::DetachAllListeners
void EventDispatcher::DetachAllListeners( const void *listener )
{
    for( EventListeners::iterator i = m_listeners.begin(), end = m_listeners.end(); i != end; ) {
        sEventListenerHandle& handle = i->second;

        if( handle.m_listener.m_nativeCallback.GetObject() == listener ) {
            m_listeners.erase( i++ );
        } else {
            ++i;
        }
    }
}

} // namespace event

DC_END_DREEMCHEST