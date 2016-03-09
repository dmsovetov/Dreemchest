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

#ifndef __DC_Event_EventListener_H__
#define __DC_Event_EventListener_H__

#include "Event.h"

DC_BEGIN_DREEMCHEST

namespace event {

	// ** class EventListener
	class EventListener {
	friend class EventDispatcher;

        enum {
            Native   = BIT( 0 ),
            Object   = BIT( 2 ),
        };

	public:

                                EventListener( void ) {}
                                EventListener( const EventListenerCallback& native );
                                EventListener( const dcScriptObject& script );
                                EventListener( const EventListener& other );

		bool                    operator == ( const EventListener& other ) const;

		void                    Trigger( dcContext ctx, Event *event, const char *method );
		void                    Trigger( dcContext ctx, const dcScriptObject& event, const char *method );

    private:

        EventListenerCallback   m_nativeCallback;
        dcScriptObject          m_scriptCallback;
        cFlagSet8               m_flags;
	};

} // namespace event

DC_END_DREEMCHEST

#endif	/*	!__DC_EventListener_H__	*/