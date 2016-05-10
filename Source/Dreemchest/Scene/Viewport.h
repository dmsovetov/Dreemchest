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

#ifndef __DC_Scene_Viewport_H__
#define __DC_Scene_Viewport_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene viewport interface.
	class AbstractViewport : public InjectEventEmitter<RefCounted> {
	public:

		virtual						~AbstractViewport( void ) {}

		//! Returns the render target rect.
		Rect						rect( void ) const;

		//! Returns the render target width.
		virtual s32					width( void ) const NIMBLE_ABSTRACT;

		//! Returns the render target height.
		virtual s32					height( void ) const NIMBLE_ABSTRACT;

        //! A base class for all viewport event types.
        struct Event {
                                    //! Constructs an Event instance.
                                    Event( ViewportWPtr sender )
                                        : sender( sender ) {}
            ViewportWPtr            sender; //!< An event sender instance.
        };

        //! A base class for all touch events.
        struct TouchEvent : public Event {
                                    //! Constructs a TouchEvent instance.
                                    TouchEvent( ViewportPtr sender, s32 id, s32 x, s32 y )
                                        : Event( sender ), id( id ), x( x ), y( y ) {}
            s32                     id;     //!< A touch integer identifier.
            s32                     x;      //!< A touch X coordinate.
            s32                     y;      //!< A touch Y coordinate.
        };

        //! This event is emitted when a user touch begins.
        struct TouchBegan : public TouchEvent {
                                    //! Constructs a TouchBegan instance.
                                    TouchBegan( ViewportPtr sender, s32 id, s32 x, s32 y )
                                        : TouchEvent( sender, id, x, y ) {}
        };

        //! This event is emitted when a user touch ends.
        struct TouchEnded : public TouchEvent {
                                    //! Constructs a TouchEnded instance.
                                    TouchEnded( ViewportPtr sender, s32 id, s32 x, s32 y )
                                        : TouchEvent( sender, id, x, y ) {}
        };

        //! This event is emitted when a user touch was moved.
        struct TouchMoved : public TouchEvent {
                                    //! Constructs a TouchMoved instance.
                                    TouchMoved( ViewportPtr sender, s32 id, s32 x, s32 y, s32 dx, s32 dy )
                                        : TouchEvent( sender, id, x, y ), dx( dx ), dy( dy ) {}
            s32                     dx;     //!< A change of a touch X coordinate.
            s32                     dy;     //!< A change of a touch Y coordinate.
        };
	};

	//! WindowViewport is used for attaching a scene viewport to a window instance.
	class WindowViewport : public AbstractViewport {
	public:

        virtual                     ~WindowViewport( void );

		//! Returns the window width.
		virtual s32					width( void ) const NIMBLE_OVERRIDE;

		//! Returns the window height.
		virtual s32					height( void ) const NIMBLE_OVERRIDE;

		//! Creates the WindowView instance.
		static WindowViewportPtr    create( const Platform::WindowWPtr& window );

	private:

									//! Constructs the WindowViewport instance.
									WindowViewport( const Platform::WindowWPtr& window );

        //! Handles a TouchBegan event from a window instance.
        void                        handleTouchBegan( const Platform::Window::TouchBegan& e );

        //! Handles a TouchMoved event from a window instance.
        void                        handleTouchMoved( const Platform::Window::TouchMoved& e );

        //! Handles a TouchEnded event from a window instance.
        void                        handleTouchEnded( const Platform::Window::TouchEnded& e );

	private:

		Platform::WindowWPtr		m_window;	//!< The output window.
        s32                         m_lastX;    //!< A last known cursor X coordinate.
        s32                         m_lastY;    //!< A last known cursor Y coordinate.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Viewport_H__    */