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

#include	"EventListener.h"

DC_BEGIN_DREEMCHEST

namespace event {

// ** EventListener::EventListener
EventListener::EventListener( const EventListenerCallback& native )
{
    m_nativeCallback = native;
    m_flags.On( Native );
}

EventListener::EventListener( const dcScriptObject& script )
{
    m_scriptCallback = script;
    m_flags.Set( Object, script.IsFunction() == false );
}

EventListener::EventListener( const EventListener& other )
{
    m_flags          = other.m_flags;
    m_nativeCallback = other.m_nativeCallback;
    m_scriptCallback = other.m_scriptCallback;
}

// ** EventListener::Trigger
void EventListener::Trigger( dcContext ctx, Event *e, const char *method )
{
	DC_BREAK_IF( ctx == NULL );

    // ** Native callback
    if( m_nativeCallback ) {
        m_nativeCallback( e );
        return;
    }

    // ** Scripted events are disabled
    if( (ctx->GetEventMask() & EVENT_SCRIPT) == 0 ) {
        return;
    }

    // ** Call a script callback
    if( m_flags.Is( Object ) ) {
        ctx->iAssembly->Invoke( &m_scriptCallback, method, e );
    }
    else {
        ctx->iAssembly->Invoke( m_scriptCallback, e );
    }

    // ** workaround
    if( e ) {
        delete e->GetScriptInstance();
        e->SetScriptInstance( NULL );
    }
}

// ** EventListener::Trigger
void EventListener::Trigger( dcContext ctx, const dcScriptObject& e, const char *method )
{
    if( m_flags.Is( Native ) ) {
		m_nativeCallback( NULL );
        return;
    }

    // ** Scripted events are disabled
	if( (ctx->GetEventMask() & EVENT_SCRIPT) == 0 ) {
		return;
	}

    // ** Call a script callback
    if( m_flags.Is( Object ) ) {
        ctx->iAssembly->Invoke( &m_scriptCallback, method, e );
    }
    else {
        ctx->iAssembly->Invoke( m_scriptCallback, e );
    }
}

// ** EventListener::operator ==
bool EventListener::operator == ( const EventListener& other ) const
{
    if( m_flags.Is( Native ) ) {
        return other.m_nativeCallback == m_nativeCallback;
    }

    return other.m_scriptCallback == m_scriptCallback;
}

} // namespace event

DC_END_DREEMCHEST